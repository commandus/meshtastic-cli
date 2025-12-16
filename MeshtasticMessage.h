#ifndef MESHTASTICMESSAGE_H
#define MESHTASTICMESSAGE_H

#include <google/protobuf/message.h>
#include "meshtastic.h"

#define DEFAULT_HOP_LIMIT   3

enum MeshtasticMessageType {
    MMT_UNKNOWN,
    MMT_LOG,
    MMT_FROM_RADIO,
    MMT_TO_RADIO
};

class MeshtasticMessage {
public:
    static enum MeshtasticMessageType parse(
        google::protobuf::Message **retVal,
        void *buffer,
        size_t size,
        enum MeshtasticMessageType hintMessageType
    );
    static void msgRequestConfig(
        google::protobuf::Message **retVal,
        uint32_t configId
    );

    static uint32_t msgSendText(
        google::protobuf::Message **retVal,
        const std::string &val,
        uint32_t nodeNum,
        meshtastic::PortNum port = meshtastic::TEXT_MESSAGE_APP,
        uint32_t channel = 0,
        meshtastic::MeshPacket_Priority priority = meshtastic::MeshPacket_Priority_RELIABLE,
        bool pkiEncrypted = false,
        bool wantResponse = false,
        bool wantAck = true,
        uint32_t replyId = 0,
        uint32_t hopLimit = DEFAULT_HOP_LIMIT,
        const void *publicKey = nullptr,
        uint8_t publicKeySize = 0
    );

    /**
     * "Ping" serial port to detect meshtastic device
     * @param retVal
     * @return message id, 0- failed
     */
    static uint32_t msgPing(google::protobuf::Message **retVal);
};

#endif // MESHTASTICMESSAGE_H
