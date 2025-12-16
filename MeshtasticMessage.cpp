#include "MeshtasticMessage.h"
#include "meshtastic/mesh.pb.h"

#include "meshtastic/storeforward.pb.h"

static std::atomic<uint32_t> messageId = (uint32_t) time(nullptr);

enum MeshtasticMessageType MeshtasticMessage::parse(
    google::protobuf::Message **retVal,
    void *buffer,
    size_t size,
    enum MeshtasticMessageType hintMessageType
) {
    google::protobuf::Message *r;
    switch (hintMessageType) {
        case MMT_FROM_RADIO:
            r = new meshtastic::FromRadio;
            break;
        case MMT_LOG:
            r = new meshtastic::LogRecord;
            break;
        default:
            hintMessageType = MMT_FROM_RADIO;
            r = new meshtastic::FromRadio;
            break;
    }
    if (!r->ParseFromArray(buffer, size)) {
        delete r;
        // try another type except ToRadio
        switch (hintMessageType) {
            case MMT_FROM_RADIO:
                r = new meshtastic::LogRecord;
                break;
            default:
                r = new meshtastic::FromRadio;
                break;
        }
        if (!r->ParseFromArray(buffer, size)) {
            delete r;
            r = nullptr;
            hintMessageType = MMT_UNKNOWN;
        }
    }
    if (retVal)
        *retVal = r;
    return hintMessageType;
}

void MeshtasticMessage::msgRequestConfig(
    google::protobuf::Message **retVal,
    uint32_t configId
) {
    if (!retVal)
        return;
    auto r = new meshtastic::ToRadio;
    r->set_want_config_id(configId);
    *retVal = r;
}

uint32_t MeshtasticMessage::msgSendText(
    google::protobuf::Message **retVal,
    const std::string &val,
    uint32_t nodeNum,
    meshtastic::PortNum port,
    uint32_t channel,
    meshtastic::MeshPacket_Priority priority,
    bool pkiEncrypted,
    bool wantResponse,
    bool wantAck,
    uint32_t replyId,
    uint32_t hopLimit,
    const void *publicKey,
    uint8_t publicKeySize
)
{
    if (!retVal)
        return 0;
    auto r = new meshtastic::ToRadio;
    auto p = r->mutable_packet();

    p->set_id(messageId);
    messageId++;

    p->set_to(nodeNum);
    p->set_want_ack(wantAck);
    if (hopLimit == 0)
        hopLimit = DEFAULT_HOP_LIMIT;
    p->set_hop_limit(hopLimit);
    p->set_pki_encrypted(pkiEncrypted);
    if ((publicKey != nullptr) && (publicKeySize > 0)) {
        p->set_public_key(publicKey, publicKeySize);
    }

    if (priority != meshtastic::MeshPacket_Priority_UNSET)
        p->set_priority(priority);

    p->set_channel(channel);
    auto dp = p->mutable_decoded();
    dp->set_payload(val);
    dp->set_portnum(port);
    dp->set_want_response(wantResponse);
    if (replyId)
        dp->set_reply_id(replyId);

    *retVal = r;
    return p->id();
}

uint32_t MeshtasticMessage::msgPing(google::protobuf::Message **retVal) {
    if (!retVal)
        return 0;
    auto r = new meshtastic::ToRadio;
    uint32_t id = time(nullptr);
    r->set_want_config_id(id);
    *retVal = r;
    return id;
}
