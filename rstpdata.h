#ifndef RSTPDATA_H
#define RSTPDATA_H

#include <QTime>
#include <QObject>


class RstpData : public QObject
{
    Q_OBJECT

public:
    RstpData();
    ~RstpData();

    struct RstpPacket {
        QTime timestamp;
        QByteArray data;
        std::uint8_t diretion;
    };

    typedef QList<RstpPacket> RstpDataList;

    void GetFromFile(const QString &filename);

signals:
    void ListUpdated(const RstpDataList &dataList);

private:

    RstpDataList mRstpDataList;

protected:
    void AddItemToList(RstpPacket& packet);
    void ClearList();
};

#endif // RSTPDATA_H
