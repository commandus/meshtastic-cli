#include <sstream>
#include "MeshtasticDeviceContext.h"

bool MeshtasticDeviceContext::hasField(
    const google::protobuf::Message *message,
    const std::string &fieldName
)
{
    const google::protobuf::Descriptor *descriptor = message->GetDescriptor();
    const google::protobuf::Reflection *reflection = message->GetReflection();
    const google::protobuf::FieldDescriptor* fField = descriptor->FindFieldByName(fieldName);
    if (!fField)
        return false;
    return reflection->HasField(*message, fField);
}

void MeshtasticDeviceContext::mergeFrom(
    const google::protobuf::Message *message
) {
    if (!message)
        return;

    meshtastic::FromRadio *msg = (meshtastic::FromRadio*) message;
    if (msg->has_my_info()) {
        myNodeInfo.MergeFrom(msg->my_info());
    }

    if (msg->has_metadata()) {
        deviceMetadata.MergeFrom(msg->metadata());
    }

    if (msg->has_node_info()) {
        auto v = msg->node_info();
        uint32_t nodeNumber = v.num();
        nodes[nodeNumber] = v;
        // TODO fix position
        // TODO expand user properties
    }

    if (msg->config_complete_id() == configId)
        configComplete = true;

    if (msg->has_channel()) {
        channels.push_back(msg->channel());
    }

    if (msg->has_packet()) {
        auto v = msg->packet();
        // handle packet
    }

    if (msg->has_log_record()) {
        auto v = msg->log_record();
        // handle log_record
    }

    if (msg->has_queuestatus()) {
        auto v = msg->queuestatus();
        // handle queueStatus
    }

    if (msg->has_clientnotification()) {
        auto v = msg->clientnotification();
        // handle clientnotification
    }

    if (msg->has_mqttclientproxymessage()) {
        auto v = msg->mqttclientproxymessage();
        // handle mqttClientProxyMessage
    }

    if (msg->has_xmodempacket()) {
        auto v = msg->xmodempacket();
        // handle xmodemPacket
    }

    if (msg->has_rebooted()) {
        rebooted = msg->rebooted();
    }

    if (msg->has_fileinfo()) {
        fileInfos.push_back(msg->fileinfo());
    }

    if (msg->has_config()) {
        if (msg->config().has_device())
            device.MergeFrom(msg->config().device());
        if (msg->config().has_position())
            position.MergeFrom(msg->config().position());
        if (msg->config().has_power())
            power.MergeFrom(msg->config().power());
        if (msg->config().has_network())
            network.MergeFrom(msg->config().network());
        if (msg->config().has_display())
            display.MergeFrom(msg->config().display());
        if (msg->config().has_lora())
            lora.MergeFrom(msg->config().lora());
        if (msg->config().has_bluetooth())
            bluetooth.MergeFrom(msg->config().bluetooth());
        if (msg->config().has_security())
            security.MergeFrom(msg->config().security());
        if (msg->config().has_sessionkey())
            sessionKey.MergeFrom(msg->config().sessionkey());
    }
    if (msg->has_moduleconfig()) {
        if (msg->moduleconfig().has_mqtt())
            mqtt.MergeFrom(msg->moduleconfig().mqtt());
        if (msg->moduleconfig().has_serial())
            serial.MergeFrom(msg->moduleconfig().serial());
        if (msg->moduleconfig().has_external_notification())
            external_notification.MergeFrom(msg->moduleconfig().external_notification());
        if (msg->moduleconfig().has_store_forward())
            store_forward.MergeFrom(msg->moduleconfig().store_forward());
        if (msg->moduleconfig().has_range_test())
            range_test.MergeFrom(msg->moduleconfig().range_test());
        if (msg->moduleconfig().has_telemetry())
            telemetry.MergeFrom(msg->moduleconfig().telemetry());
        if (msg->moduleconfig().has_canned_message())
            canned_message.MergeFrom(msg->moduleconfig().canned_message());
        if (msg->moduleconfig().has_audio())
            audio.MergeFrom(msg->moduleconfig().audio());
        if (msg->moduleconfig().has_remote_hardware())
            remote_hardware.MergeFrom(msg->moduleconfig().remote_hardware());
        if (msg->moduleconfig().has_neighbor_info())
            neighbor_info.MergeFrom(msg->moduleconfig().neighbor_info());
        if (msg->moduleconfig().has_ambient_lighting())
            ambient_lighting.MergeFrom(msg->moduleconfig().ambient_lighting());
        if (msg->moduleconfig().has_detection_sensor())
            detection_sensor.MergeFrom(msg->moduleconfig().detection_sensor());
        if (msg->moduleconfig().has_paxcounter())
            paxcounter.MergeFrom(msg->moduleconfig().paxcounter());
    }
}

MeshtasticDeviceContext::MeshtasticDeviceContext()
    : configComplete(false), rebooted(false), configId(0)
{
}

void MeshtasticDeviceContext::clear() {
    configComplete = false;
    rebooted = false;
    // configId = 0;
    myNodeInfo.Clear();
    deviceMetadata.Clear();
    nodes.clear();
    fileInfos.clear();

    device.Clear();
    position.Clear();
    power.Clear();
    network.Clear();
    display.Clear();
    lora.Clear();
    bluetooth.Clear();
    security.Clear();
    sessionKey.Clear();

    mqtt.Clear();
    serial.Clear();
    external_notification.Clear();
    store_forward.Clear();
    range_test.Clear();
    telemetry.Clear();
    canned_message.Clear();
    audio.Clear();
    remote_hardware.Clear();
    neighbor_info.Clear();
    ambient_lighting.Clear();
    detection_sensor.Clear();
    paxcounter.Clear();

    channels.clear();
}

std::string MeshtasticDeviceContext::toString() const {
    std::stringstream ss;
    ss << "complete: " << STR_BOOL(configComplete) << "\n"
        << "rebooted: " << STR_BOOL(rebooted) << "\n"
        << "myNodeInfo: " << myNodeInfo.DebugString() << "\n"
        << "deviceMetadata: " << deviceMetadata.DebugString() << "\n"
        << "device: " << device.DebugString() << "\n"
        << "position: " << position.DebugString() << "\n"
        << "power: " <<  power.DebugString() << "\n"
        << "network: " << network.DebugString() << "\n"
        << "display: " << display.DebugString() << "\n"
        << "lora: " << lora.DebugString() << "\n"
        << "bluetooth: " << bluetooth.DebugString() << "\n"
        << "security: " << security.DebugString() << "\n"
        << "sessionKey: " << sessionKey.DebugString() << "\n"
        << "mqtt: " << mqtt.DebugString() << "\n"
        << "serial: " << serial.DebugString() << "\n"
        << "external_notification: " << external_notification.DebugString() << "\n"
        << "store_forward: " << store_forward.DebugString() << "\n"
        << "range_test: " << range_test.DebugString() << "\n"
        << "telemetry: " << telemetry.DebugString() << "\n"
        << "canned_message: " << canned_message.DebugString() << "\n"
        << "audio: " << audio.DebugString() << "\n"
        << "remote_hardware: " << remote_hardware.DebugString() << "\n"
        << "neighbor_info: " << neighbor_info.DebugString() << "\n"
        << "ambient_lighting: " << ambient_lighting.DebugString() << "\n"
        << "detection_sensor: " << detection_sensor.DebugString() << "\n"
        << "paxcounter: " << paxcounter.DebugString() << "\n";


    for (auto fileInfo : fileInfos)
        ss << "fileInfo " << fileInfo.DebugString() << "\n";
    for (auto n : nodes)
        ss << "node " << n.first << ": " << n.second.DebugString() << "\n";
    for (auto c : channels)
        ss << "channel " << c.index() << ": " << c.DebugString() << "\n";
    return ss.str();
}

MeshtasticDeviceContext::MeshtasticDeviceContext(
    const MeshtasticDeviceContext &value
)
    : configComplete(value.configComplete), rebooted(value.rebooted), configId(value.configId),
    myNodeInfo(value.myNodeInfo), deviceMetadata(value.deviceMetadata), localNode(value.localNode),
    nodes(value.nodes), fileInfos(value.fileInfos), device(value.device), position(value.position),
    power(value.power), network(value.network), display(value.display), lora(value.lora), bluetooth(value.bluetooth),
    security(value.security), sessionKey(value.sessionKey), mqtt(value.mqtt), serial(value.serial),
    external_notification(value.external_notification), store_forward(value.store_forward), range_test(value.range_test),
    telemetry(value.telemetry), canned_message(value.canned_message), audio(value.audio), remote_hardware(value.remote_hardware),
    neighbor_info(value.neighbor_info), ambient_lighting(value.ambient_lighting), detection_sensor(value.detection_sensor),
    paxcounter(value.paxcounter), channels(value.channels)
{

}
