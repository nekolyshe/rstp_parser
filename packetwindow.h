#ifndef PACKETWINDOW_H
#define PACKETWINDOW_H

#include "messageparser.h"

#include <QDialog>

namespace Ui {
class packetWindow;
}

class packetWindow : public QDialog
{
    Q_OBJECT

public:
    explicit packetWindow(QWidget *parent = nullptr);
    ~packetWindow();

    void AddWindowData(const ParsedMessageBase::Parsed &info, const QByteArray &rawData);
    void AddTextData(int selectedIndex, int selectedSize);
private slots:

    void on_twData_cellClicked(int row, int column);

private:
    enum {
        ROW_NAME = 0,
        ROW_DESCRIPTION
    };

    QByteArray mRawData;
    ParsedMessageBase::Parsed mInfo;
    Ui::packetWindow *ui;
};

#endif // PACKETWINDOW_H
