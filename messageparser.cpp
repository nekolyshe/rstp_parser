#include "messageparser.h"

#define MIN_MSG_SIZE 6

ParsedMessageBase::ParsedMessageBase(const QByteArray &rawData) : mIsValid(true), mData(rawData)
{
    if (mData.size() < MIN_MSG_SIZE) { //TODO: add additional check
        mIsValid = false;
    }
}

QString ParsedMessageBase::GetSofTexted() const
{
    return GetTextedValWithIndex(INDEX_SOF);
}

QString ParsedMessageBase::GetFrameTypeTexted() const
{
    return GetTextedValWithIndex(INDEX_FRAME_TYPE);
}

QString ParsedMessageBase::GetLenghtTexted() const
{
    return GetTextedValWithIndex(INDEX_LEN);
}

QString ParsedMessageBase::GetChannelIdTexted() const
{
    return GetTextedValWithIndex(INDEX_CHANNEL_ID);
}

QString ParsedMessageBase::GetSeqNumTexted() const
{
    return GetTextedValWithIndex(INDEX_SEQ_NUM);
}

QString ParsedMessageBase::GetMessageIdTexted() const
{
    return GetTextedValWithIndex(INDEX_MSG_ID);
}

QString ParsedMessageBase::GetPayloadTexted() const
{
    QString text;
    uint8_t value;

    if (mData.count() < INDEX_PAYLOAD + CRC_SIZE_BYTES) {
        return EmptyValText();
    }

    const unsigned int lastIndex = mData.count() - CRC_SIZE_BYTES;

    for (unsigned int i = INDEX_PAYLOAD; i < lastIndex; i++) {
        if (GetValWithIndex(i, value)) {
            text += QString("0x%1 ").arg(value, 2, 16, QLatin1Char( '0' ));
        }
    }

    return text;
}

bool ParsedMessageBase::GetChannelId(uint8_t &channelId) const
{
    return GetValWithIndex(INDEX_CHANNEL_ID, channelId);
}

bool ParsedMessageBase::IsValid() const
{
    return mIsValid;
}

QString ParsedMessageBase::U8ToText(const uint8_t val) const
{
    return QString("0x%1").arg(val, 2, 16, QLatin1Char( '0' ));
}

const QString ParsedMessageBase::EmptyValText() const
{
    return QString("(empty)");
}

bool ParsedMessageBase::GetValWithIndex(const uint8_t index, uint8_t &value) const
{
    bool status = false;

    if (index < mData.size()) {
        value = mData[index];
        status = true;
    }

    return status;
}

QString ParsedMessageBase::GetTextedValWithIndex(const uint8_t index) const
{
    uint8_t value = 0;

    if (GetValWithIndex(index, value)) {
        return U8ToText(value);
    }

    return EmptyValText();
}



