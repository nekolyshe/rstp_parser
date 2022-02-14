#include "messageparser.h"

#define MIN_MSG_SIZE 6

ParsedMessageBase::ParsedMessageBase(const QByteArray &rawData) : mData(rawData), mIsValid(true)
{
    if (mData.size() < MIN_MSG_SIZE) { //TODO: add additional check
        mIsValid = false;
    }
}

uint8_t ParsedMessageBase::GetSof() const
{
    if (!IsValid()) {
        return 0;
    }

    return mData[INDEX_SOF];
}

uint8_t ParsedMessageBase::GetFrameType() const
{
    if (!IsValid()) {
        return 0;
    }
    return mData[INDEX_FRAME_TYPE];
}

uint8_t ParsedMessageBase::GetLenght() const
{
    if (!IsValid()) {
        return 0;
    }
    return mData[INDEX_LEN];
}

uint8_t ParsedMessageBase::GetChannelId() const
{
    if (!IsValid()) {
        return 0;
    }
    return mData[INDEX_CHANNEL_ID];
}

uint8_t ParsedMessageBase::GetSeqNum() const
{
    if (!IsValid()) {
        return 0;
    }
    return mData[INDEX_SEQ_NUM];
}

uint8_t ParsedMessageBase::GetMessageId() const
{
    if (!IsValid()) {
        return 0;
    }
    return mData[INDEX_MSG_ID];
}

bool ParsedMessageBase::IsValid() const
{
    return mIsValid;
}
