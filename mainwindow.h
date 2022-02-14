#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "rstpdata.h"
#include "messageparser.h"

#include <QMainWindow>

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

private:
    void initSlots();

    bool mShowSof;
    bool mShowFrameType;
    bool mShowLenght;
    bool mShowChannelId;
    bool mShowSeqNum;
    bool mShowMessageId;
    bool mShowPayload;

    enum {
        COLLUMN_TIME_STAMP,
        COLLUMN_SOF,
        COLLUMN_FRAME_TYPE,
        COLLUMN_LENGHT,
        COLLUMN_CHANNEL_ID,
        COLLUMN_SEQ_NUM,
        COLLUMN_MESSAGE_ID,
        COLLUMN_PAYLOAD,

        COLLUMN_COINT
    };

    Ui::MainWindow *ui;
    RstpData *mRstpData;
    QList<ParsedMessageBase> mMessage;

    void UpdateCheckBoxes();
    void UpdateListItems();
    void UpdateListview();

};
#endif // MAINWINDOW_H
