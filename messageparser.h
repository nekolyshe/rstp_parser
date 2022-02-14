#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <cstdint>
#include <QList>


class ParsedMessageBase
{
public:
    ParsedMessageBase(const QByteArray &rawData);

    uint8_t GetSof() const;
    uint8_t GetFrameType() const;
    uint8_t GetLenght() const;
    uint8_t GetChannelId() const;
    uint8_t GetSeqNum() const;
    uint8_t GetMessageId() const;

    QString GetSofTexted() const;
    QString GetFrameTypeTexted() const;
    QString GetLenghtTexted() const;
    QString GetChannelIdTexted() const;
    QString GetSeqNumTexted() const;
    QString GetMessageIdTexted() const;

    bool IsValid() const;

private:
    enum {
        INDEX_SOF        = 0,
        INDEX_FRAME_TYPE = 0,
        INDEX_LEN        = 0,
        INDEX_CHANNEL_ID = 0,
        INDEX_SEQ_NUM    = 0,
        INDEX_MSG_ID     = 0,
        INDEX_PAYLOAD    = 0,
        INDEX_MSG_LEN    = 0,
    };

    bool mIsValid;
    QByteArray mData;
};

#endif // MESSAGEPARSER_H
