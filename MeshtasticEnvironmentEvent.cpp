#include "MeshtasticEnvironmentEvent.h"

MeshtasticEnvironmentEvent::MeshtasticEnvironmentEvent(
    MeshtasticEnvironment *aEnv,
    enum MeshtasticEnvironmentEventType aEventType,
    MeshtasticDiscoveryEvent *aDe,
    MeshtasticTransport* aTransport,
    MeshtasticDevice* aDevice,
    google::protobuf::Message *aMsg
)
    : env(aEnv), dt(std::chrono::system_clock::now()), eventType(aEventType), de(aDe), transport(aTransport), device(aDevice),
    msg(aMsg)
{

}
