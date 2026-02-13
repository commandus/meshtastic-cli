#include <sstream>
#include "MeshtasticTransport.h"
#include "MeshtasticDevice.h"
#include "MeshtasticEnvironmentEvent.h"
#include "MeshtasticString.h"

MeshtasticTransport::MeshtasticTransport()
    : discoveryOn(false), state(MTS_OFF),  env(nullptr), transportType(MTT_ANY)
{

}

MeshtasticTransport::MeshtasticTransport(
    MeshtasticEnvironment *aEnv
)
    : discoveryOn(false), state(MTS_OFF), env(aEnv), transportType(MTT_ANY)
{

}

MeshtasticTransport::MeshtasticTransport(
    MeshtasticEnvironment *aEnv,
    MeshtasticTransportType aTransportType
)
    : discoveryOn(false), state(MTS_OFF), env(aEnv), transportType(aTransportType)
{

}

int MeshtasticTransport::startTransport() {
    state = MTS_ON;
    env->fireEvent(MeshtasticEnvironmentEvent(env, ET_DISCOVERY_ON, nullptr, this, nullptr, nullptr));
    if (env->isDebugEnabled(LOG_INFO)) {
        std::stringstream ss;
        ss << MeshtasticString::transportType2String(this->transportType) << _(" starts discover device(s)");
        env->debugLog(LOG_INFO, ss.str());
    }
    return 0;
}

void MeshtasticTransport::stopTransport() {
    if (env) {
        while (true) {
            // remove all devices uses this transport
            auto d = env->find(this);
            if (!d)
                break;
            env->rmDevice(d);
        }
    }
    state = MTS_OFF;
    env->fireEvent(MeshtasticEnvironmentEvent(env, ET_DISCOVERY_OFF, nullptr, this, nullptr, nullptr));
}

int MeshtasticTransport::startDiscovery() {
    discoveryOn = true;
    return 0;
}

void MeshtasticTransport::stopDiscovery(
    int seconds
) {
    discoveryOn = false;
}

bool MeshtasticTransport::ping(
    MeshtasticDevice &device
) {
    google::protobuf::Message *msg;
    uint32_t id = MeshtasticMessage::msgPing(&msg);
    if (!id)
        return false;
    int r = write(&device, msg);
    if (r < 0) {
        if (env->isDebugEnabled(LOG_INFO)) {
            std::stringstream ss;
            ss << device.addressAsString() << _(" write on discovery probe with error code ") << r;
            env->debugLog(LOG_INFO, ss.str());
        }
    }

    delete msg;
    if (r < 0)
        return false;
    msg = nullptr;
    auto t = device.readMessage(&msg, 1000);
    if (t != MMT_FROM_RADIO) {
        if (env->isDebugEnabled(LOG_INFO)) {
            std::stringstream ss;
            ss << device.addressAsString() << _(" is not meshtastic device");
            env->debugLog(LOG_INFO, ss.str());
        }
        return false;
    }
    if (env->isDebugEnabled(LOG_INFO)) {
        std::stringstream ss;
        ss << device.addressAsString() << _(" is meshtastic device");
        env->debugLog(LOG_INFO, ss.str());
    }
    delete msg;
    return true;
}

MeshtasticTransport::~MeshtasticTransport()
{
    if (env->isDebugEnabled(LOG_DEBUG)) {
        std::stringstream ss;
        env->debugLog(LOG_DEBUG, _("Transport closed"));
    }
}
