#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "rstpdata.h"
#include "messageparser.h"
#include "packetwindow.h"

#include <QMainWindow>
#include <QTableWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pbOpenFile_clicked();
    void on_updateList(const RstpData::RstpDataList &dataList);
    void on_checkBox_channelId_stateChanged(int arg1);
    void on_checkBox_sof_stateChanged(int arg1);
    void on_checkBox_frameType_stateChanged(int arg1);
    void on_checkBox_lenght_stateChanged(int arg1);
    void on_checkBox_seqNum_stateChanged(int arg1);
    void on_checkBox_msgId_stateChanged(int arg1);
    void on_checkBox_payload_stateChanged(int arg1);

    void on_pbAdjustFilter_clicked();

    void on_twMessages_itemDoubleClicked(QTableWidgetItem *item);

    void on_twMessages_cellDoubleClicked(int row, int column);

private:

    bool mShowSof;
    bool mShowFrameType;
    bool mShowLenght;
    bool mShowChannelId;
    bool mShowSeqNum;
    bool mShowMessageId;
    bool mShowPayload;

    enum {
        COLUMN_NUM,
        COLUMN_TIME_STAMP,
        COLUMN_SOF,
        COLUMN_FRAME_TYPE,
        COLUMN_LENGHT,
        COLUMN_CHANNEL_ID,
        COLUMN_SEQ_NUM,
        COLUMN_MESSAGE_ID,
        COLUMN_PAYLOAD,

        COLUMN_COINT
    };

    struct Message {
        Message(ParsedMessageBase msg, QTime time) : msg(msg), timestamp(time)
        {}

        ParsedMessageBase msg;
        QTime timestamp;
    };

    Ui::MainWindow *ui;
    RstpData *mRstpData;
    QList<Message> mMessage;
    QStringList mChannelFilter;
    QStringList mMsgFilter;
    packetWindow *mPacketWindow;

    void initSlots();
    void initFilterText();
    void UpdateCheckBoxes();
    void AddListItem(unsigned int row, unsigned int column, const QString &text);
    const QString ConvertMsgToText(const QByteArray &arr);
    void UpdateListItems();
    void UpdateListview();

    const QString EmptyValText() const;

    bool isFilteredPersist(const QTableWidgetItem *item, const QStringList &filter);
};
#endif // MAINWINDOW_H
