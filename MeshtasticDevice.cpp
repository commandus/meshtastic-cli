#include <sstream>
#include "MeshtasticDevice.h"
#include "MeshtasticEnvironmentEvent.h"

MeshtasticDevice::MeshtasticDevice(
    MeshtasticTransport *aTransport
)
    : state(MLS_STOPPED), listenerThread(nullptr), transport(aTransport), messageReadCount(0), messageWriteCount(0)
{
}

MeshtasticDevice::MeshtasticDevice(
    MeshtasticTransport *aTransport,
    const std::string &aName
)
    : listenerThread(nullptr), state(MLS_STOPPED), transport(aTransport), name(aName),
        messageReadCount(0), messageWriteCount(0)
{
}


MeshtasticDevice::MeshtasticDevice(
    const MeshtasticDevice &device
)
    : listenerThread(device.listenerThread),
        state(device.state), transport(device.transport),
        name(device.name), messageReadCount(device.messageReadCount),
        messageWriteCount(device.messageWriteCount),
        context(device.context)
{

}

bool MeshtasticDevice::equals(MeshtasticDevice *cmp) const {
    return cmp != nullptr && transport == cmp->transport && hash() == cmp->hash();
}

bool MeshtasticDevice::equals(
    uint64_t aHash
) const
{
    return hash() == aHash;
}

uint64_t MeshtasticDevice::hash() const {
    return (uint64_t) this;
}

void MeshtasticDevice::requestDeviceContext()
{
    context.clear();
    google::protobuf::Message *m = nullptr;
    context.configId = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    MeshtasticMessage::msgRequestConfig(&m, context.configId);
    messageWriteCount++;
    transport->write(this, m);

    delete m;
}

uint32_t MeshtasticDevice::tag() const {
    union {
        uint64_t v;
        struct {
            uint32_t h;
            uint32_t l;
        };
    } u64_2;
    u64_2.v = hash();
    return u64_2.l;
}

MeshtasticMessageType MeshtasticDevice::readMessage(
    google::protobuf::Message **retVal,
    int timeOutMs
)
{
    // repeatedly read until get empty packet
    char buffer[MTU_SIZE];
    int sz = transport->read(this, &buffer, sizeof(buffer), timeOutMs);
    if (sz <= 0)
        return MMT_UNKNOWN;

    *retVal = nullptr;
    MeshtasticMessageType mt = MeshtasticMessage::parse(retVal, &buffer, sz, MMT_FROM_RADIO);
    if (*retVal) {
        if (mt == MMT_UNKNOWN) {
            delete *retVal;
        }
    }
    messageReadCount++;
    return mt;
}

void MeshtasticDevice::startListener() {
    if (transport->env->isDebugEnabled(LOG_INFO)) {
        transport->env->debugLog(LOG_INFO, "Device listener start", this);
    }
    listenerThread = new std::thread(&MeshtasticDevice::listenerRun, this);
    listenerThread->detach();
}

void MeshtasticDevice::stopListener(
    int seconds
) {
    if (state == MLS_STOPPED)
        return;
    if (transport->env->isDebugEnabled(LOG_INFO)) {
        std::stringstream ss;
        ss << "Device " << name << " listener stop in " << seconds << "s.";
        transport->env->debugLog(LOG_INFO, ss.str(), this);
    }
    std::unique_lock<std::mutex> lckListener(mutexListener);
    state = MLS_STOP;
    lckListener.unlock();

    if (listenerThread) {
        std::unique_lock<std::mutex> lock(mutexListener);
        if (cvListenerState.wait_for(lock, std::chrono::seconds(seconds), [this] {
            return state == MLS_STOPPED;
        })) {
            if (transport->env->isDebugEnabled(LOG_INFO)) {
                std::stringstream ss;
                ss << "Device " << name << " listener successfully stopped.";
                transport->env->debugLog(LOG_INFO, ss.str(), this);
            }
            listenerThread = nullptr;
            return;
        }
        // kill detached thread here ...
        if (transport->env->isDebugEnabled(LOG_ERR)) {
            std::stringstream ss;
            ss << "Device " << name << " listener stop failed..";
            transport->env->debugLog(LOG_ERR, ss.str(), this);
        }
        listenerThread = nullptr;
    }
}

void MeshtasticDevice::listenerRun()
{
    if (transport->env->isDebugEnabled(LOG_INFO)) {
        std::stringstream ss;
        ss << _("Device ") << name << _(" listener started");
        transport->env->debugLog(LOG_INFO, ss.str(), this);
    }
    std::unique_lock<std::mutex> lckListener(mutexListener);
    state = MLS_WAIT_ANY_MSG;
    lckListener.unlock();
    while (state != MLS_STOP) {
        // repeatedly read until stop requested
        char buffer[MTU_SIZE];
        int sz = transport->read(this, &buffer, sizeof(buffer), 100);
        if (sz <= 0)
            continue;
        messageReadCount++;

        google::protobuf::Message *m = nullptr;
        MeshtasticMessageType mt = MeshtasticMessage::parse(&m, &buffer, sz, MMT_FROM_RADIO);

        if (m) {
            if (mt == MMT_FROM_RADIO) {
                if (state == MLS_WAIT_ANY_MSG) {
                    std::unique_lock<std::mutex> lckListener2(mutexListener);
                    state = MLS_WAIT_CONFIG;
                    lckListener2.unlock();
                    cvListenerState.notify_all();
                }
                if (state == MLS_WAIT_CONFIG) {
                    // config device
                    context.mergeFrom(m);
                    context.configComplete |= ((meshtastic::FromRadio *) m)->has_config_complete_id();
                    if (transport->env->isDebugEnabled(LOG_DEBUG)) {
                        std::stringstream ss;
                        ss << _("Device ") << name << _(" configuration received ") << m->DebugString();
                        transport->env->debugLog(LOG_DEBUG, ss.str(), this);
                    }
                    if (context.configComplete) {
                        std::unique_lock<std::mutex> lckListener2(mutexListener);
                        state = MLS_RUN;
                        lckListener2.unlock();
                        cvListenerState.notify_all();
                        if (transport->env->isDebugEnabled(LOG_DEBUG)) {
                            std::stringstream ss;
                            ss << _("Device ") << name << _(" end of configuration received");
                            transport->env->debugLog(LOG_DEBUG, ss.str(), this);
                        }
                        transport->env->deviceConfigured(this);
                    }
                } else {
                    if (transport->env->isDebugEnabled(LOG_INFO)) {
                        std::stringstream ss;
                        ss << _("Device read message ") << name << " " << m->DebugString();
                        transport->env->debugLog(LOG_INFO, ss.str(), this);
                    }
                    MeshtasticDiscoveryEvent de(this, name, this->hash(), 0, false);
                    transport->env->fireEvent(MeshtasticEnvironmentEvent(transport->env, ET_DEVICE_FROM_RADIO, &de, transport, this, m));
                }
            }
            delete m;
        }
    }

    if (transport->env->isDebugEnabled(LOG_INFO)) {
        std::stringstream ss;
        ss << _("Device ") << name << _(" listener stopped");
        transport->env->debugLog(LOG_INFO, ss.str(), this);
    }

    std::unique_lock<std::mutex> lckListener2(mutexListener);
    state = MLS_STOPPED;
    lckListener2.unlock();
    cvListenerState.notify_all();
}

MeshtasticDevice::~MeshtasticDevice()
{
    stopListener(); // stop and wait until it is stopped
    // do not destroy object until listener is stopped
    /*
    std::unique_lock<std::mutex> lock(mutexListener);
    cvListenerState.wait_for(lock, std::chrono::seconds(60), [this] {
        return state == MLS_STOPPED;
    });
     */
}

bool MeshtasticDevice::waitState(
    MeshtasticListenerState state,
    uint32_t milliSeconds
) {
    std::unique_lock<std::mutex> lock(mutexListener);
    return (cvListenerState.wait_for(lock, std::chrono::milliseconds(milliSeconds), [state, this] {
        return this->state == state;
    }));
}
