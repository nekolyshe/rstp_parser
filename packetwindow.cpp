#include "packetwindow.h"
#include "ui_packetwindow.h"

packetWindow::packetWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::packetWindow)
{
    ui->setupUi(this);
}

packetWindow::~packetWindow()
{
    delete ui;
}

static QString ConvertRawToText(const QByteArray &arr)
{
    QString text("");

    if(arr.size() > 0) {
        for (const uint8_t item : arr) {
            text += QString("%1 ").arg(item, 2, 16, QLatin1Char( '0' ));
        }
    }

    return text;
}

void packetWindow::AddWindowData(const ParsedMessageBase::Parsed &info, const QByteArray &rawData)
{
    mRawData = rawData;
    mInfo = info;

    unsigned int column = 0;

    ui->twData->setColumnCount(column);

    for(const auto &item : info) {
        ui->twData->insertColumn(column);

        ui->twData->setItem(ROW_NAME, column, new QTableWidgetItem(item->name));
        ui->twData->setItem(ROW_DESCRIPTION, column, new QTableWidgetItem(item->description));
        column++;
    }

    AddTextData(0,0);
}

void packetWindow::AddTextData(int selectedIndex, int selectedSize)
{
    ui->lbData1->setText(ConvertRawToText(mRawData.mid(0, selectedIndex)));
    ui->lbData2Bold->setText(ConvertRawToText(mRawData.mid(selectedIndex, selectedSize)));
    ui->lbData3->setText(ConvertRawToText(mRawData.mid(selectedIndex + selectedSize)));
}

void packetWindow::on_tableWidget_cellClicked(int row, int column)
{

}

void packetWindow::on_twData_cellClicked(int row, int column)
{
    (void)row;
    AddTextData(mInfo[column]->index, mInfo[column]->size);
}

