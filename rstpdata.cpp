#include "rstpdata.h"
#include "qdebug.h"

#include <QFile>
#include <QString>
#include <QTextStream>
#include <QRegularExpression>
#include <QElapsedTimer>

#define VALUE_EMPTY 0xFFFFFFFF

RstpData::RstpData()
{

}

RstpData::~RstpData()
{

}

static uint32_t myToInt(QString& strValue) {//TODO: rename
    bool succes = true;
    uint32_t intValue = strValue.toInt(&succes);

    if (succes) {
        return intValue;
    } else {
        return VALUE_EMPTY;
    }
}

static void StringToU8(const QString &str, QByteArray &payload)
{
    QStringList listValues = str.split(QRegularExpression ("[^0-9A-F]"), Qt::SkipEmptyParts);

    payload.clear();

    for (auto &val: qAsConst(listValues)) {
        payload.append(val.toUShort(nullptr, 16));
    }
}

static void ParseTimestamp(const QString &timestamp, QTime &timeDate)
{
    QStringList listValues = timestamp.split(QRegularExpression("[^0-9]"), Qt::SkipEmptyParts);
    QStringList listNames = timestamp.split(QRegularExpression("[^A-zµ]"), Qt::SkipEmptyParts);

    int hour    = 0,
        min     = 0,
        sec     = 0,
        msec    = 0;

    for(int i = 0; i < listNames.count(); i++) { //TODO: somethind if data isn`t correct
        if(listNames[i] == QString("h")) {
            hour = myToInt(listValues[i]);
        } else if(listNames[i] == QString("m")) {
            min = myToInt(listValues[i]);
        } else if(listNames[i] == QString("s")) {
            sec = myToInt(listValues[i]);
        } else if(listNames[i] == QString("ms")) {
            msec = myToInt(listValues[i]);
        }
    }

    timeDate = QTime(hour, min, sec, msec);
}

void RstpData::GetFromFile(const QString &filename)
{
    enum {
        TIMESTAMP_INDEX = 0,
        PAYLOAD_INDEX   = 1,
        DURATION_INDEX  = 2,
        DIRECTION_INDEX = 3
    };

    ClearList();

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    RstpPacket rstpPacket;
    QElapsedTimer timer;

    timer.start();
    qDebug()<< "read file start " << QTime::currentTime();

    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList list = line.split(QLatin1Char(','), Qt::SkipEmptyParts); // TODO: check results

        if (list.count() > TIMESTAMP_INDEX) {
            ParseTimestamp(list[TIMESTAMP_INDEX], rstpPacket.timestamp);
        }
        if (list.count() > PAYLOAD_INDEX) {
            StringToU8(list[PAYLOAD_INDEX], rstpPacket.data);
        }
        if (list.count() > DIRECTION_INDEX) {
            rstpPacket.diretion =
                static_cast<uint8_t>(myToInt(list[DIRECTION_INDEX]));
        }

        AddItemToList(rstpPacket);
    }

    qDebug()<< "read file end " << QTime::currentTime();
    qDebug()<< "total " << timer.elapsed();
    file.close();
    emit ListUpdated(mRstpDataList);
}

void RstpData::AddItemToList(RstpPacket &packet)
{
    mRstpDataList.append(packet);
}

void RstpData::ClearList()
{
    mRstpDataList.clear();
}
