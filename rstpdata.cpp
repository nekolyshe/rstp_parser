#include "rstpdata.h"

#include <QFile>
#include <QTextStream>

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
        return 0;
    }
}

static void StringToU8(const QString &str, QByteArray &payload)
{
    QStringList listValues = str.split(QRegExp("[^0-9A-F]"), QString::SkipEmptyParts);

    payload.clear();

    for (auto &val: qAsConst(listValues)) {
        payload.append(val.toUShort(nullptr, 16));
    }
}

static void ParceTimestamp(const QString &timestamp, QTime &timeDate)
{
    QStringList listValues = timestamp.split(QRegExp("[^0-9]"), QString::SkipEmptyParts);
    QStringList listNames = timestamp.split(QRegExp("[^A-zµ]"), QString::SkipEmptyParts);

    int hour    = 0,
        min     = 0,
        sec     = 0,
        msec    = 0;

    for(int i = 0; i < listNames.size(); i++) { //TODO: somethind if data isn`t correct
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
        PAYLOAD_INDEX = 1
    };

    ClearList();

    QFile file(filename);
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
         return;
     }

     RstpPacket rstpPacket;

     QTextStream in(&file);
     while (!in.atEnd()) {
         QString line = in.readLine();
         QStringList list = line.split(QLatin1Char(','), QString::SkipEmptyParts); //TODO: check results
         ParceTimestamp(list[TIMESTAMP_INDEX], rstpPacket.timestamp);
         StringToU8(list[PAYLOAD_INDEX], rstpPacket.data);

         AddItemToList(rstpPacket);
     }

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
