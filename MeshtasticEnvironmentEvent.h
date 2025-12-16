#ifndef MESHTASTICENVIRONMENTEVENT_H
#define MESHTASTICENVIRONMENTEVENT_H

#include <chrono>

#include "MeshtasticEnvironment.h"
#include "MeshtasticDiscoveryEvent.h"

enum MeshtasticEnvironmentEventType {
    ET_DISCOVERY_OFF,
    ET_DISCOVERY_ON,
    ET_DEVICE_DISCOVERED,
    ET_DEVICE_ADD,
    ET_DEVICE_RM,
    ET_DEVICE_FROM_RADIO,
    ET_DEVICE_TO_RADIO,
    ET_DEVICE_LOG
};

typedef std::chrono::time_point<std::chrono::system_clock> EVENT_TIME;

class MeshtasticEnvironment;
class MeshtasticDiscoveryEvent;
class MeshtasticTransport;
class MeshtasticDevice;

class MeshtasticEnvironmentEvent {
public:
    MeshtasticEnvironment *env;
    EVENT_TIME dt;
    enum MeshtasticEnvironmentEventType eventType;
    MeshtasticDiscoveryEvent *de;
    MeshtasticTransport* transport;
    MeshtasticDevice* device;
    google::protobuf::Message *msg;
    MeshtasticEnvironmentEvent(
        MeshtasticEnvironment *env,
        MeshtasticEnvironmentEventType eventType,
        MeshtasticDiscoveryEvent *de,
        MeshtasticTransport* transport,
        MeshtasticDevice* device,
        google::protobuf::Message *msg
    );
};

#endif
