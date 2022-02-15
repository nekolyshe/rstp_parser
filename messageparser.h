#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <cstdint>
#include <QList>


class ParsedMessageBase
{
public:
    ParsedMessageBase(const QByteArray &rawData);

    QString GetSofTexted()       const;
    QString GetFrameTypeTexted() const;
    QString GetLenghtTexted()    const;
    QString GetChannelIdTexted() const;
    QString GetSeqNumTexted()    const;
    QString GetMessageIdTexted() const;
    QString GetPayloadTexted()   const;


    bool IsValid() const;

private:

    enum {
        INDEX_SOF        = 0,
        INDEX_FRAME_TYPE = 1,
        INDEX_LEN        = 2,
        INDEX_CHANNEL_ID = 3,
        INDEX_SEQ_NUM    = 4,
        INDEX_MSG_ID     = 5,
        INDEX_MSG_LEN    = 6,

        INDEX_PAYLOAD    = 7,

        CRC_SIZE_BYTES   = 2
    };

    QString U8ToText(const uint8_t val) const;
    const QString EmptyValText() const;
    bool GetValWithIndex(const uint8_t index, uint8_t &value) const;
    QString GetTextedValWithIndex(const uint8_t index) const;
    bool GetChannelId(uint8_t &channelId) const;

    bool mIsValid;
    QByteArray mData;
};

#endif // MESSAGEPARSER_H
