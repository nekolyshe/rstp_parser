#include "mainwindow.h"
#include "qdebug.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
      QMainWindow(parent),
      mShowDirection(false),
      mShowSof(false),
      mShowFrameType(true),
      mShowLenght(false),
      mShowChannelId(true),
      mShowSeqNum(false),
      mShowMessageId(true),
      mShowPayload(true),
      mShowDirectionRx(true),
      mShowDirectionTx(true),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mRstpData = new RstpData();
    mPacketWindow = new packetWindow();

    initSlots();
    initFilterText();

    UpdateCheckBoxes();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete mRstpData;
}

void MainWindow::on_pbOpenFile_clicked()
{
    qDebug()<< "on_pbOpenFile_clicked " << QTime::currentTime();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log"), nullptr, "*.csv");
    this->setWindowTitle(fileName);
    mRstpData->GetFromFile(fileName);
}

void MainWindow::on_updateList(const RstpData::RstpDataList &dataList)
{
    mMessage.clear();

    qDebug()<< "Update dataList start " << QTime::currentTime();
    for(const auto &msg: dataList) {
        mMessage << Message(msg.data, msg.timestamp, msg.diretion);
    }
    qDebug()<< "Update dataList end " << QTime::currentTime();

    qDebug()<< "UpdateListItems start " << QTime::currentTime();
    UpdateListItems();
    qDebug()<< "UpdateListItems end " << QTime::currentTime();

    qDebug()<< "UpdateListview start " << QTime::currentTime();
    UpdateListview();
    qDebug()<< "UpdateListview end " << QTime::currentTime();
}

void MainWindow::initSlots()
{
    connect(mRstpData, &RstpData::ListUpdated, this, &MainWindow::on_updateList);
}

void MainWindow::initFilterText()
{
    QRegularExpression rx ("[0-9A-Fa-f ]+");
    ui->leFilterChannels->setValidator (new QRegularExpressionValidator (rx, this));
    ui->leFilterMsgId->setValidator (new QRegularExpressionValidator (rx, this));
}

void MainWindow::UpdateCheckBoxes()
{
    ui->checkBox_Direction->setChecked(mShowDirection);
    ui->checkBox_sof->setChecked(mShowSof);
    ui->checkBox_frameType->setChecked(mShowFrameType);
    ui->checkBox_lenght->setChecked(mShowLenght);
    ui->checkBox_channelId->setChecked(mShowChannelId);
    ui->checkBox_seqNum->setChecked(mShowSeqNum);
    ui->checkBox_msgId->setChecked(mShowMessageId);
}

void MainWindow::AddListItem(unsigned int row, unsigned int column, const QString &text)
{
    QTableWidgetItem *item = new QTableWidgetItem(text);
    item->setTextAlignment(Qt::AlignHCenter);

    ui->twMessages->setItem(row, column, item);
}

const QString MainWindow::ConvertMsgToText(const QByteArray &arr)
{
    if(arr.size() == 0) {
        return textEmpty;
    }

    QString text("");

    for (const uint8_t item : arr) {
        text += QString("%1 ").arg(item, 2, 16, QLatin1Char( '0' ));
    }

    return text;
}

void MainWindow::UpdateListItems()
{
    unsigned int row = 0;

    ui->twMessages->setRowCount(0);

    setUpdatesEnabled(false);
    for (const auto &message : qAsConst(mMessage)) {
        ui->twMessages->insertRow(row);

        AddListItem(row, COLUMN_NUM, (QStringLiteral("%1").arg(ui->twMessages->rowCount(), 7, 10, QLatin1Char(' '))));
        AddListItem(row, COLUMN_TIME_STAMP, message.timestamp.toString("hh:mm:ss.zzz"));
        AddListItem(row, COLUMN_DIRECTION,  ((message.direction == ParsedMessageBase::RX) ? "RX" : (message.direction == ParsedMessageBase::TX ? "TX" : "NONE")));
        AddListItem(row, COLUMN_SOF,        ConvertMsgToText(message.msg.GetSof()));
        AddListItem(row, COLUMN_FRAME_TYPE, ConvertMsgToText(message.msg.GetFrameType()));
        AddListItem(row, COLUMN_LENGHT,     ConvertMsgToText(message.msg.GetLenght()));
        AddListItem(row, COLUMN_CHANNEL_ID, ConvertMsgToText(message.msg.GetChannelId()));
        AddListItem(row, COLUMN_SEQ_NUM,    ConvertMsgToText(message.msg.GetSeqNum()));
        AddListItem(row, COLUMN_MESSAGE_ID, ConvertMsgToText(message.msg.GetMessageId()));
        AddListItem(row, COLUMN_PAYLOAD,    ConvertMsgToText(message.msg.GetRawData()));

        row++;
    }
    setUpdatesEnabled(true);
}

void MainWindow::UpdateListview()
{
    ui->twMessages->setColumnHidden(COLUMN_DIRECTION,  !mShowDirection);
    ui->twMessages->setColumnHidden(COLUMN_SOF,        !mShowSof);
    ui->twMessages->setColumnHidden(COLUMN_FRAME_TYPE, !mShowFrameType);
    ui->twMessages->setColumnHidden(COLUMN_LENGHT,     !mShowLenght);
    ui->twMessages->setColumnHidden(COLUMN_CHANNEL_ID, !mShowChannelId);
    ui->twMessages->setColumnHidden(COLUMN_SEQ_NUM,    !mShowSeqNum);
    ui->twMessages->setColumnHidden(COLUMN_MESSAGE_ID, !mShowMessageId);
    ui->twMessages->setColumnHidden(COLUMN_PAYLOAD,    !mShowPayload);

    const unsigned int rows = ui->twMessages->rowCount();

    setUpdatesEnabled(false);
    for (unsigned int i = 0; i < rows; i++) {

        bool showByMessageId = isFilteredPersist(ui->twMessages->item(i, COLUMN_MESSAGE_ID), mMsgFilter);
        bool showByChannel = isFilteredPersist(ui->twMessages->item(i, COLUMN_CHANNEL_ID), mChannelFilter);
         bool showByDirection = false;

        if(mShowDirectionRx && mShowDirectionTx)
        {
            showByDirection = true;
        }
        else
        {
            showByDirection = (ui->twMessages->item(i, COLUMN_DIRECTION)->text().contains("RX") && mShowDirectionRx)
                    || (ui->twMessages->item(i, COLUMN_DIRECTION)->text().contains("TX") && mShowDirectionTx);
        }

        ui->twMessages->setRowHidden(i, !(showByChannel && showByMessageId && showByDirection));
    }
    setUpdatesEnabled(true);
}

const QString MainWindow::textRX("RX");
const QString MainWindow::textTX("TX");
const QString MainWindow::textEmpty("(empty)");

bool MainWindow::isFilteredPersist(const QTableWidgetItem *item, const QStringList &filter) // TODO: looks like a piece of shit
{
    if (filter.count() == 0) {
        return true;
    }

    if (item == nullptr) {
        return false;
    }

    for (const auto &str : filter) {
        if (item->text().contains(str)) {
            return true;
        }
    }

    return false;
}

void MainWindow::on_checkBox_sof_stateChanged(int arg1)
{
    mShowSof = (arg1 != 0);
    UpdateListview();
}

void MainWindow::on_checkBox_channelId_stateChanged(int arg1)
{
    mShowChannelId = (arg1 != 0);
    UpdateListview();
}

void MainWindow::on_checkBox_frameType_stateChanged(int arg1)
{
    mShowFrameType = (arg1 != 0);
    UpdateListview();
}

void MainWindow::on_checkBox_lenght_stateChanged(int arg1)
{
    mShowLenght = (arg1 != 0);
    UpdateListview();
}

void MainWindow::on_checkBox_seqNum_stateChanged(int arg1)
{
    mShowSeqNum = (arg1 != 0);
    UpdateListview();
}

void MainWindow::on_checkBox_msgId_stateChanged(int arg1)
{
    mShowMessageId = (arg1 != 0);
    UpdateListview();
}

void MainWindow::on_checkBox_payload_stateChanged(int arg1)
{
    mShowPayload = (arg1 != 0);
    UpdateListview();
}

void MainWindow::on_pbAdjustFilter_clicked()
{
    mMsgFilter = ui->leFilterMsgId->text().split(" ", Qt::SkipEmptyParts);
    mChannelFilter = ui->leFilterChannels->text().split(" ", Qt::SkipEmptyParts);

    UpdateListview();
}

void MainWindow::on_twMessages_cellDoubleClicked(int row, int column)
{
    (void)column;

    mPacketWindow->close();
    int position = ui->twMessages->item(row, COLUMN_NUM)->text().toInt() - 1;
    mPacketWindow->AddWindowData(mMessage[position].msg.getDescriptions(), mMessage[position].msg.GetRawData());
    mPacketWindow->show();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        mShowDirectionRx=true;
        mShowDirectionTx=true;
        break;
    case 1:
        mShowDirectionRx=true;
        mShowDirectionTx=false;
        break;
    case 2:
        mShowDirectionRx=false;
        mShowDirectionTx=true;
        break;
    default:
        break;
    }

    UpdateListview();
}

void MainWindow::on_checkBox_Direction_stateChanged(int arg1)
{
    mShowDirection = (arg1 != 0);
    UpdateListview();
}


void MainWindow::on_pbOpenJsonFolder_clicked()
{
    QString folderName = QFileDialog::getExistingDirectory(); //this, tr("Open folder with JSONS"), nullptr, "*"
    qDebug()<< "JSONS folder: " << folderName;
    ParsedMessageBase::AddDescriptions(folderName + "/");
}

