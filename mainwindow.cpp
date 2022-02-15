#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
      QMainWindow(parent),
      mShowSof(true),
      mShowFrameType(true),
      mShowLenght(true),
      mShowChannelId(true),
      mShowSeqNum(true),
      mShowMessageId(true),
      mShowPayload(true),
      ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mRstpData = new RstpData();

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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Log"), nullptr, "*.csv");
    mRstpData->GetFromFile(fileName);
}

void MainWindow::on_updateList(const RstpData::RstpDataList &dataList)
{
    mMessage.clear();

    for(const auto &msg: dataList) {
        mMessage << ParsedMessageBase(msg.data);
    }

    UpdateListItems();
    UpdateListview();
}

void MainWindow::initSlots()
{
    connect(mRstpData, &RstpData::ListUpdated, this, &MainWindow::on_updateList);
}

void MainWindow::initFilterText()
{
    QRegExp rx ("[0-9A-Fa-f ]+");
    ui->leFilterChannels->setValidator (new QRegExpValidator (rx, this));
    ui->leFilterMsgId->setValidator (new QRegExpValidator (rx, this));
}

void MainWindow::UpdateCheckBoxes()
{
    ui->checkBox_sof->setChecked(mShowSof);
    ui->checkBox_frameType->setChecked(mShowFrameType);
    ui->checkBox_lenght->setChecked(mShowLenght);
    ui->checkBox_channelId->setChecked(mShowChannelId);
    ui->checkBox_seqNum->setChecked(mShowSeqNum);
    ui->checkBox_msgId->setChecked(mShowMessageId);
}

void MainWindow::UpdateListItems()
{
    unsigned int row = 0;

    ui->twMessages->setRowCount(0);

    for (const auto &message : qAsConst(mMessage)) {
        ui->twMessages->insertRow(row);

        // the sequence should be kept
        ui->twMessages->setItem(row, COLLUMN_NUM,        new QTableWidgetItem(QStringLiteral("%1")
                                                                              .arg(ui->twMessages->rowCount(), 7, 10, QLatin1Char(' '))));
        ui->twMessages->setItem(row, COLLUMN_TIME_STAMP, new QTableWidgetItem("time"));
        ui->twMessages->setItem(row, COLLUMN_SOF,        new QTableWidgetItem(message.GetSofTexted()));
        ui->twMessages->setItem(row, COLLUMN_FRAME_TYPE, new QTableWidgetItem(message.GetFrameTypeTexted()));
        ui->twMessages->setItem(row, COLLUMN_LENGHT,     new QTableWidgetItem(message.GetLenghtTexted()));
        ui->twMessages->setItem(row, COLLUMN_CHANNEL_ID, new QTableWidgetItem(message.GetChannelIdTexted()));
        ui->twMessages->setItem(row, COLLUMN_SEQ_NUM,    new QTableWidgetItem(message.GetSeqNumTexted()));
        ui->twMessages->setItem(row, COLLUMN_MESSAGE_ID, new QTableWidgetItem(message.GetMessageIdTexted()));
        ui->twMessages->setItem(row, COLLUMN_PAYLOAD,    new QTableWidgetItem(message.GetPayloadTexted()));

        row++;
    }
}

void MainWindow::UpdateListview()
{
    ui->twMessages->setColumnHidden(COLLUMN_SOF,        !mShowSof);
    ui->twMessages->setColumnHidden(COLLUMN_FRAME_TYPE, !mShowFrameType);
    ui->twMessages->setColumnHidden(COLLUMN_LENGHT,     !mShowLenght);
    ui->twMessages->setColumnHidden(COLLUMN_CHANNEL_ID, !mShowChannelId);
    ui->twMessages->setColumnHidden(COLLUMN_SEQ_NUM,    !mShowSeqNum);
    ui->twMessages->setColumnHidden(COLLUMN_MESSAGE_ID, !mShowMessageId);
    ui->twMessages->setColumnHidden(COLLUMN_PAYLOAD,    !mShowPayload);

    const unsigned int rows = ui->twMessages->rowCount();

    for (unsigned int i = 0; i < rows; i++) {

        bool showByMessageId = isFilteredPersist(ui->twMessages->item(i, COLLUMN_MESSAGE_ID), mMsgFilter);
        bool showByChannel = isFilteredPersist(ui->twMessages->item(i, COLLUMN_CHANNEL_ID), mChannelFilter);

        ui->twMessages->setRowHidden(i, !(showByChannel && showByMessageId));
    }
}

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

/*
    const int row = mUi->TwRawData->rowCount();
    int collum = 0;

    mUi->TwRawData->insertRow(row);

    mUi->TwRawData->setItem(row, collum++, new QTableWidgetItem(QTime::currentTime().toString("hh:mm:ss.zzz ")));
    mUi->TwRawData->setItem(row, collum++, new QTableWidgetItem(directionText[direction]));
    mUi->TwRawData->setItem(row, collum++, new QTableWidgetItem(data));

    for (int i = 0; i < collum; i++ ) {
        mUi->TwRawData->item(row, i)->setForeground(directionColor[direction]);
    }

*
*/

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
    mMsgFilter = ui->leFilterMsgId->text().split(" ", QString::SkipEmptyParts);
    mChannelFilter = ui->leFilterChannels->text().split(" ", QString::SkipEmptyParts);

    UpdateListview();
}

