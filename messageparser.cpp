#include "messageparser.h"

#define MIN_MSG_SIZE 6

const QString kEmptyValText("(empty)");

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

bool ParsedMessageBase::IsValid() const
{
    return mIsValid;
}

QString ParsedMessageBase::U8ToText(const uint8_t val) const
{
    return QString("0x%1").arg(val, 2, 16, QLatin1Char( '0' ));
}

QString ParsedMessageBase::GetTextedValWithIndex(const uint8_t index) const
{
    if (index < mData.size()) {
        return U8ToText(mData[index]);
    }

    return kEmptyValText;
}



