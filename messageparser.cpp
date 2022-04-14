#include "messageparser.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

#define ITEM_DEFAULT_SIZE_BYTES sizeof (uint8_t)

static QMap<int, QJsonObject> &getChannelDescriptions()
{
    static QMap<int, QJsonObject> channelDescriptions;
    return channelDescriptions;
}

ParsedMessageBase::ParsedMessageBase(const QByteArray &rawData) : mRawData(rawData)
{

    AddItemSof();
    if (GetItem(TYPE_SOF).isvalid != VALID) return;

    AddItemFrameType();
    if (GetItem(TYPE_FRAME_TYPE).isvalid != VALID) return;

    switch (GetItemData(TYPE_FRAME_TYPE).toHex().toInt(nullptr, 16)) {
    case FT_INIT:
        AddItemInitData();
        break;
    case FT_INIT_ACK:
        break;
    case FT_CHRST:
    case FT_CHRST_ACK:
        AddItemLen();
        AddChrstData();
        break;
    case FT_DATA:
    case FT_DATAN:
    case FT_SDATA:
    case FT_SDATAN:
        AddItemLen();
        AddItemChannel();
        AddItemSeqNum();
        AddItemMsgId();
        AddItemMsgLen();
        AddItemsPayload();
        //get data
        break;
    case FT_ACK:
    case FT_NACK:
        //get ack
        break;
    case FT_DATANC:
    default:
        //get uncknown
        break;
    }

    AddItemsCrc();
}

ParsedMessageBase::~ParsedMessageBase()
{
    for (int i  = 0; i < mIndexedMsg.size(); i++) {
        //delete mIndexedMsg[i]; ???
    }
}

QByteArray ParsedMessageBase::GetSof() const
{
    return GetItemData(TYPE_SOF);
}

QByteArray ParsedMessageBase::GetFrameType() const
{
    return GetItemData(TYPE_FRAME_TYPE);
}

QByteArray ParsedMessageBase::GetLenght() const
{
    return GetItemData(TYPE_LENGHT);
}

QByteArray ParsedMessageBase::GetChannelId() const
{
    return GetItemData(TYPE_CHANNEL);
}

QByteArray ParsedMessageBase::GetSeqNum() const
{
    return GetItemData(TYPE_SEQ_NUM);
}

QByteArray ParsedMessageBase::GetMessageId() const
{
    return GetItemData(TYPE_MSG_ID);
}

QByteArray ParsedMessageBase::GetRawData() const
{
    return mRawData;
}

const ParsedMessageBase::Parsed ParsedMessageBase::getDescriptions()
{
    Parsed list;

    for (const auto &i : qAsConst(mIndexedMsg)) {
        list.append(&i->item);
    }
    return  list;
}

bool ParsedMessageBase::AddDescriptions(QString descriptionFolder)
{
    QDir directory(descriptionFolder);
    QStringList files = directory.entryList(QStringList() << "*.json" << "*.JSON",QDir::Files);

    for(const QString &filename: files) {

        QFile file(descriptionFolder + filename);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString jsoneFile = file.readAll();
            QJsonDocument jsonDocument = QJsonDocument::fromJson(jsoneFile.toUtf8());

            if(jsonDocument.isObject() == false) {
                qDebug() << "It is not a JSON object";
            } else {
                QJsonObject object = jsonDocument.object();
                if (object.contains("Interface_Channel")){
                    getChannelDescriptions().insert(object["Interface_Channel"].toInt(), jsonDocument.object());
                }
            }
            file.close();
        }
    }

    return true;
}

const QByteArray ParsedMessageBase::GetItemData(ItemType type) const
{
    QByteArray arr;
    for (const auto &item : mIndexedMsg) {
        if (item->type == type) {
            arr = mRawData.mid(item->item.index, item->item.size);
            break;
        }
    }

    return arr;
}

const ParsedMessageBase::Item ParsedMessageBase::GetItem(ItemType type) const
{
    Item it;
    for (const auto &item : mIndexedMsg) {
        if (item->type == type) {
            it = item->item;
            break;
        }
    }

    return it;
}

int ParsedMessageBase::GetItemIndex(ItemType type) const
{
    static const QMap<ItemType, int> indexes = {
        {TYPE_SOF,          0x00},
        {TYPE_FRAME_TYPE,   0x01},
        {TYPE_LENGHT,       0x02},
        {TYPE_CHANNEL,      0x03},
        {TYPE_SEQ_NUM,      0x04},
        {TYPE_MSG_ID,       0x05},
        {TYPE_MSG_LEN,      0x06},
        {TYPE_CHRST_DATA,   0x03},
        {TYPE_INITDATA,     0x02},
    };

    if (indexes.contains(type)) {
        return indexes[type];
    }

    return INDEX_INVALID;
}

ParsedMessageBase::ItemTyped* ParsedMessageBase::AddItem(int index, int size, ItemType type)
{
    if (index == INDEX_INVALID) {
        return nullptr;
    }

    ItemTyped *item = new ItemTyped;

    item->type = type;
    item->item.isvalid = IsItemValid(index, size);
    item->item.index = index;

    if (item->item.isvalid == VALID) {
        item->item.size = size;
    } else {
        item->item.size = mRawData.size() - type;
    }

    mIndexedMsg << item;
    return item;
}

ParsedMessageBase::ItemTyped* ParsedMessageBase::AddIndexedItem(ItemType type, int size = ITEM_DEFAULT_SIZE_BYTES)
{
    return AddItem(GetItemIndex(type), size, type);
}

void ParsedMessageBase::AddItemSof()
{
    ItemTyped *pItem = AddIndexedItem(TYPE_SOF);
    if (pItem) {
        pItem->item.name = "Sof";
    }
}

void ParsedMessageBase::AddItemFrameType()
{
    static QMap<ParsedMessageBase::FrameType, QString> frameNames = {
        {ParsedMessageBase::FT_INIT ,       "INIT"},
        {ParsedMessageBase::FT_INIT_ACK ,   "INIT_ACK"},
        {ParsedMessageBase::FT_CHRST ,      "CHRST"},
        {ParsedMessageBase::FT_CHRST_ACK ,  "CHRST_ACK"},
        {ParsedMessageBase::FT_DATA ,       "DATA"},
        {ParsedMessageBase::FT_DATAN ,      "DATAN"},
        {ParsedMessageBase::FT_DATANC ,     "DATANC"},
        {ParsedMessageBase::FT_SDATA ,      "SDATA"},
        {ParsedMessageBase::FT_SDATAN ,     "SDATAN"},
        {ParsedMessageBase::FT_ACK ,        "ACK"},
        {ParsedMessageBase::FT_NACK ,       "NACK"},
    };

    ItemTyped *pItem = AddIndexedItem(TYPE_FRAME_TYPE);

    if (pItem) {
        FrameType frameType = static_cast<FrameType>(GetItemData(TYPE_FRAME_TYPE).toHex().toInt(nullptr, 16));
        pItem->item.name = QString("Frame Type: ") + frameNames[frameType];
        pItem->item.description = QString("0x00 INIT Init frame with boot parameters"
                                          "0x01 INIT_ACK Acknowledge for init frame"
                                          "0x02 CHRST Reset Channel Sequence Numbers"
                                          "0x03 CHRST_ACK Acknowledge for CHRST"
                                          "0x10 DATA Data with acknowledge"
                                          "0x20 DATAN Data without acknowledge"
                                          "0x21 DATANC Data without acknowledge and without CRC"
                                          "0x14 SDATA Segmented Data with acknowledge"
                                          "0x15 SDATAN Segmented Data without acknowledge"
                                          "0x11 ACK Data Acknowledge"
                                          "0x12 NACK Data No acknowledge");
    }
}

void ParsedMessageBase::AddItemInitData()
{
    const int kInitDataSize = 4;

    ItemTyped *pItem = AddIndexedItem(TYPE_INITDATA, kInitDataSize);

    if (pItem) {
        pItem->item.name = QString("Initialization data");
    }
}

void ParsedMessageBase::AddItemLen()
{
    ItemTyped *pItem = AddIndexedItem(TYPE_LENGHT);

    if (pItem) {
        pItem->item.name = QString("Data lenght: ") + QString::number(GetItemData(TYPE_LENGHT).toHex().toInt(nullptr, 16));
    }
}

void ParsedMessageBase::AddItemChannel()
{
    ItemTyped *pItem = AddIndexedItem(TYPE_CHANNEL);
    QJsonObject channel= getChannelDescriptions()[GetItemData(TYPE_CHANNEL).toHex().toInt(nullptr, 16)];

    pItem->item.name = QString("Channel id: ") + QString::number(GetItemData(TYPE_CHANNEL).toHex().toInt(nullptr, 16));

    if (pItem && !channel.empty()) {
        pItem->item.name += " :" + channel["Interface_Name"].toString();
        pItem->item.description = QString("Interface Version: ") + channel["Interface_Version"].toString()
                + " /" + channel["FrameWork_OR_Thread"].toString() + "\n"
                + channel["Interface_Description"].toString() + "\n";
    }
}

void ParsedMessageBase::AddItemSeqNum()
{
    ItemTyped *pItem = AddIndexedItem(TYPE_SEQ_NUM);

    if (pItem) {
        pItem->item.name = QString("Sequence Number: ") + QString::number(GetItemData(TYPE_SEQ_NUM).toHex().toInt(nullptr, 16));
    }
}

static QString getTextFromJsonValue(const QJsonValue &value)
{
    QString text = "";

    if (value.isString())
    {
        text = value.toString();
    } else if (value.isArray()) {
        int i = 0;
        while (!value[i].isNull()) {
            if(!value[i].isString()) {
                break;
            }

            text +=value[i].toString() + "\n";
            i++;
        }
    }

    return text;
}

static QJsonValue GetMessageDescriptionJson (int messageId, int channelId) {
    QJsonObject channel;
    QJsonValue msgObj;

    if (getChannelDescriptions().contains(channelId))
    {
        channel = getChannelDescriptions()[channelId];
    }

    if(!channel.isEmpty()) {
        QJsonValue messages = channel.value(QString("Interface_Messages"));
        int i = 0;

        while(messages[i] != QJsonValue::Undefined) {
            if ((messages[i])["Msg_ID"].toInt() == messageId) {
                msgObj = messages[i];
                break;
            }
            i++;
        }
    }

    return msgObj;
}

void ParsedMessageBase::AddItemMsgId()
{
    ItemTyped *pItem = AddIndexedItem(TYPE_MSG_ID);

    int channelId = GetItemData(TYPE_CHANNEL).toHex().toInt(nullptr, 16);
    int messageId = GetItemData(TYPE_MSG_ID).toHex().toInt(nullptr, 16);

    QJsonValue msg = GetMessageDescriptionJson(messageId, channelId);

    if (pItem != nullptr) {
        pItem->item.name = QString("Message id: ") + QString::number(messageId);

        if (!msg.isNull()) {
            pItem->item.name += getTextFromJsonValue(msg["Msg_Name"]);
            pItem->item.description = getTextFromJsonValue(msg["Msg_Description"]);
        }
    }
}

void ParsedMessageBase::AddItemMsgLen()
{
    const int kBitsToShift = 6;
    const int kLenSizeBitMask = 0x03;
    const int kIndex = GetItemIndex(TYPE_MSG_LEN);
    uint8_t lenSize = 0;

    ItemTyped *pItem = nullptr;

    if (kIndex < mRawData.size() && kIndex >= 0) {
        lenSize = (mRawData[kIndex] >> kBitsToShift) & kLenSizeBitMask;

        pItem = AddIndexedItem(TYPE_MSG_LEN, lenSize + 1); // 00 means 1 byte
    }

    if (pItem) {
        pItem->item.name = QString("Message lenght: ") + QString::number( GetItemData(TYPE_MSG_LEN).toHex().toInt(nullptr, 16));
    }
}

void ParsedMessageBase::AddChrstData()
{
    if (GetItem(TYPE_LENGHT).isvalid != VALID) return;

    int dataLen = (GetItemData(TYPE_LENGHT).toHex().toInt(nullptr, 16));//TODO: add additional validation

    ItemTyped *pItem = AddIndexedItem(TYPE_CHRST_DATA, dataLen);

    if (pItem) {
        pItem->item.name = QString("CHRST");
        pItem->item.description = QString("Channels that should be reseted");
    }
}

void ParsedMessageBase::AddItemsPayload()
{
    const int kBytesToShift = 3;
    bool hasDescription = false;

    int channelId = GetItemData(TYPE_CHANNEL).toHex().toInt(nullptr, 16);
    int messageId = GetItemData(TYPE_MSG_ID).toHex().toInt(nullptr, 16);

    int payloadIndex = GetItemIndex(TYPE_MSG_LEN) + GetItem(TYPE_MSG_LEN).size;
    int payloadSize = GetItemData(TYPE_LENGHT).toHex().toInt(nullptr, 16)
            - GetItem(TYPE_CHANNEL).size
            - GetItem(TYPE_SEQ_NUM).size
            - GetItem(TYPE_MSG_ID).size
            - GetItem(TYPE_MSG_LEN).size;

    QJsonValue msg = GetMessageDescriptionJson(messageId, channelId);
    QJsonValue msgObj;

    if (!msg.isNull()) {
        if(!msg["Msg_Parameters"].isNull()){
            msgObj = msg["Msg_Parameters"];
            hasDescription = true;
        }
    }

    while (payloadSize > 0) {
        ItemTyped *pItem = AddItem(payloadIndex, GetPayloadParamSize(payloadIndex), TYPE_MSG_PARAM);
        int itemSize = pItem->item.size;

        if (itemSize == 0) {
            qDebug() << "Broken param size";
            break;
        }

        if (pItem != nullptr) {
            pItem->item.name = "Payload";
        }

        if (hasDescription && pItem != nullptr) {
            int itemId = mRawData[pItem->item.index] >> kBytesToShift;
            int i = 0;

            while (msgObj[i] != QJsonValue::Undefined) {
                if (itemId == (msgObj[i])["Param_ID"].toInt()) {
                    int payloadParamOffSet = GetPayloadParamOffSet(payloadIndex);
                    QByteArray itemRawData = mRawData.mid(i + payloadIndex + payloadParamOffSet, pItem->item.size - payloadParamOffSet);

                    pItem->item.name += " : Id = " + QString::number(itemId) + " (" + getTextFromJsonValue((msgObj[i])["Param_Name"]) + ")\n";
                    pItem->item.name += "Data : \n";

                    for (const auto &i: itemRawData) {
                        pItem->item.name += QString("%1 ").arg(i, 2, 16, QLatin1Char( '0' ));
                    }

                    pItem->item.name += QString("\n (") + itemRawData + QString(")");

                    pItem->item.description = getTextFromJsonValue((msgObj[i])["Param_Description"]);
                    break;
                }
                i++;
            }
        }

        payloadIndex += itemSize;
        payloadSize -= itemSize;
    }
}

void ParsedMessageBase::AddItemsCrc()
{
    const int kCrcSize = sizeof (uint16_t);
    const int kIndex = GetItemIndex(TYPE_LENGHT) + GetItem(TYPE_LENGHT).size + GetItemData(TYPE_LENGHT).toHex().toInt(nullptr, 16);

    ItemTyped *pItem = AddItem(kIndex, kCrcSize, TYPE_CRC);

    if (pItem) {
        pItem->item.name = QString("Crc");
    }
}

int ParsedMessageBase::GetPayloadParamSize(int index)
{
    static const QMap<int, int> frameSizes = {
        {IT_FIXED_8,   2}, //Fixed 8 Bit Fixed types of 8 bit size (uint8, int8, bool, char)
        {IT_FIXED_16,  3}, //Fixed 16 Bit Fixed types of 16 bit size (uint16, int16)
        {IT_FIXED_32,  5}, //Fixed32 Bit Fixed types of 32 bit size (uint32, int32, float)
        {IT_FIXED_64,  9}, //Fixed 64 Bit Fixed types of 64 bit size (uint64, int64, double)
        {IT_NOT_FIXED, 0}, //Length-delimited String, arrays, structures, sub messages
    };

    const int kSizeMask = 0x07;
    int size = 0;

    if (IsItemValid(index, sizeof (uint8_t)) == VALID) {
        int frameSizeType = mRawData[index] & kSizeMask;

        if (frameSizes.contains(frameSizeType)){
            if (frameSizes[frameSizeType] > 0) {
                size = frameSizes[frameSizeType];
            } else if (IsItemValid(index + 1, sizeof (uint8_t)) == VALID) {
                size = mRawData[index + 1] + 2; //TODO: make no magic
            }
        }
    }

    return size;
}

int ParsedMessageBase::GetPayloadParamOffSet(int index)
{
    const int kSizeMask = 0x07;
    const int offsetFixed = 0x01;
    const int offsetNotFixed = 0x02;
    int offset = 0;

    if (IsItemValid(index, sizeof (uint8_t)) == VALID) {
        int frameSizeType = mRawData[index] & kSizeMask;

        if (frameSizeType == IT_NOT_FIXED) {
            offset = offsetNotFixed;
        } else {
            offset = offsetFixed;
        }
    }

    return offset;
}

ParsedMessageBase::Isvalid ParsedMessageBase::IsItemValid(int index, int size) const
{
    Isvalid valid = EMPTY;
    if (index + size <= mRawData.size() && index >= 0 && size > 0) {
        valid = VALID;
    } else {
        valid = NOT_VALID;
    }

    return valid;
}
