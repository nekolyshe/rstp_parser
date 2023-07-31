#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <cstdint>
#include <QList>
#include <QTime>
#include <QJsonObject>

#define INDEX_INVALID -1

class ParsedMessageBase
{
public:
    ParsedMessageBase(const QByteArray &rawData);
    ~ParsedMessageBase();

    enum Isvalid {
        VALID,
        NOT_VALID,
        EMPTY
    };

    enum Direcion {
        RX = 0,
        TX,
        NONE
    };

    enum AddDescriptionStatus {
        OK = 0,
        PATH_DOEST_EXIST
    };

    struct Item {
        Item() : isvalid(EMPTY), index(INDEX_INVALID), size(0) {}
        Item(Isvalid isvalid, int index, int size) : isvalid(isvalid), index(index) , size(size){}

        Isvalid isvalid;
        QString name;
        QString description;

        int index;
        int size;
    };

    typedef QList<Item*> Parsed;

    QByteArray GetSof()       const;
    QByteArray GetFrameType() const;
    QByteArray GetLenght()    const;
    QByteArray GetChannelId() const;
    QByteArray GetSeqNum()    const;
    QByteArray GetMessageId() const;
    QByteArray GetDirection() const;
    QByteArray GetRawData()   const;
    const Parsed getDescriptions();

    static AddDescriptionStatus AddDescriptions(QString descriptionFolder);

private:
    enum ItemType {
        TYPE_SOF,
        TYPE_FRAME_TYPE,
        TYPE_CHANNEL,
        TYPE_LENGHT,
        TYPE_SEQ_NUM,
        TYPE_MSG_ID,
        TYPE_MSG_LEN,
        TYPE_MSG_PARAM,
        TYPE_CHRST_DATA,
        TYPE_INITDATA,
        TYPE_CRC,

        TYPE_UNKNOWM
    };

    enum FrameType {
        FT_INIT        = 0x00,// Init frame with boot parameters
        FT_INIT_ACK    = 0x01,// Acknowledge for init frame
        FT_CHRST       = 0x02,// Reset Channel Sequence Numbers
        FT_CHRST_ACK   = 0x03,// Acknowledge for CHRST
        FT_DATA        = 0x10, //Data with acknowledge
        FT_DATAN       = 0x20,// Data without acknowledge
        FT_DATANC      = 0x21,// Data without acknowledge and without CRC 1
        FT_SDATA       = 0x14,// Segmented Data with acknowledge
        FT_SDATAN      = 0x15,// Segmented Data without acknowledge 1
        FT_ACK         = 0x11,// Data Acknowledge
        FT_NACK        = 0x12,// Data No acknowledge
    };

    enum PayloadItemType {
        IT_FIXED_8     = 0x00,
        IT_FIXED_16    = 0x01,
        IT_FIXED_32    = 0x02,
        IT_FIXED_64    = 0x03,
        IT_NOT_FIXED   = 0x04
    };

    struct ItemTyped {
        ItemType type;
        Item item;
    };

    typedef QList<ItemTyped*> IndexedMsg;

    const QByteArray GetItemData (ItemType type) const;
    const Item GetItem (ItemType type) const;

    int GetItemIndex (ItemType type) const;

    ItemTyped* AddItem(int index, int size, ItemType type);
    ItemTyped* AddIndexedItem(ItemType type, int size);
    void AddItemSof();
    void AddItemFrameType();
    void AddItemInitData();
    void AddItemLen();
    void AddItemChannel();
    void AddItemSeqNum();
    void AddItemMsgId();
    void AddItemMsgLen();
    void AddChrstData();
    void AddItemsPayload();
    void AddItemsCrc();

    int GetPayloadParamSize(int index);
    int GetPayloadParamOffSet(int index);

    Isvalid IsItemValid(int index, int size) const;

    QByteArray mRawData;
    IndexedMsg mIndexedMsg;
};

#endif // MESSAGEPARSER_H
