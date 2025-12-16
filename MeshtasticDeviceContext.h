#ifndef MESHTASTICDEVICECONTEXT_H
#define MESHTASTICDEVICECONTEXT_H


#include "meshtastic.h"

#include "meshtastic/clientonly.pb.h"

class MeshtasticDeviceContext {
protected:
    static bool hasField(
        const google::protobuf::Message *message,
        const std::string &fieldName
    );

public:
    bool configComplete;
    bool rebooted;
    uint32_t configId;
    meshtastic::MyNodeInfo myNodeInfo;
    meshtastic::DeviceMetadata deviceMetadata;
    meshtastic::NodeInfo localNode;
    std::map<uint32_t, meshtastic::NodeInfo> nodes;
    std::vector<meshtastic::FileInfo> fileInfos;

    // meshtastic::Config config is union :(
    meshtastic::Config::DeviceConfig device;
    meshtastic::Config::PositionConfig position;
    meshtastic::Config::PowerConfig power;
    meshtastic::Config::NetworkConfig network;
    meshtastic::Config::DisplayConfig display;
    meshtastic::Config::LoRaConfig lora;
    meshtastic::Config::BluetoothConfig bluetooth;
    meshtastic::Config::SecurityConfig security;
    meshtastic::Config::SessionkeyConfig sessionKey;

    meshtastic::ModuleConfig::MQTTConfig mqtt;
    meshtastic::ModuleConfig::SerialConfig serial;
    meshtastic::ModuleConfig::ExternalNotificationConfig external_notification;
    meshtastic::ModuleConfig::StoreForwardConfig store_forward;
    meshtastic::ModuleConfig::RangeTestConfig range_test;
    meshtastic::ModuleConfig::TelemetryConfig telemetry;
    meshtastic::ModuleConfig::CannedMessageConfig canned_message;
    meshtastic::ModuleConfig::AudioConfig audio;
    meshtastic::ModuleConfig::RemoteHardwareConfig remote_hardware;
    meshtastic::ModuleConfig::NeighborInfoConfig neighbor_info;
    meshtastic::ModuleConfig::AmbientLightingConfig ambient_lighting;
    meshtastic::ModuleConfig::DetectionSensorConfig detection_sensor;
    meshtastic::ModuleConfig::PaxcounterConfig paxcounter;

    std::vector<meshtastic::Channel> channels;

    MeshtasticDeviceContext();
    MeshtasticDeviceContext(const MeshtasticDeviceContext &value);
    void mergeFrom(const google::protobuf::Message *msg);
    void clear();
    std::string toString() const;

};

#endif
