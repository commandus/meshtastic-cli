#ifndef MESHTASTICDEVICEINTERFACE_H
#define MESHTASTICDEVICEINTERFACE_H

#include <vector>

#include "MeshtasticTransport.h"

#include "meshtastic.h"
#include "MeshtasticDeviceContext.h"

class MeshtasticEnvironment;

/**
 * Device metadata: name, address(if exists).
 * Can contain device state.
 * There are no r/w operations- transport is responsible to do so.
 */
class MeshtasticDevice {
private:
    // listener
    std::mutex mutexListener;
    std::condition_variable cvListenerState;
    std::thread *listenerThread;
    void listenerRun();
public:
    MeshtasticListenerState state;
    MeshtasticTransport *transport;
    std::string name;   ///< assigned by transport

    uint32_t messageReadCount;
    uint32_t messageWriteCount;

    MeshtasticDeviceContext context;

    MeshtasticDevice(MeshtasticTransport *aTransport);
    MeshtasticDevice(MeshtasticTransport *aTransport, const std::string &name);
    MeshtasticDevice(const MeshtasticDevice &device);
    virtual ~MeshtasticDevice();

    bool equals(uint64_t hash) const;
    virtual bool equals(MeshtasticDevice *cmp) const;
    virtual bool isValid() const = 0;
    virtual uint64_t hash() const;
    virtual std::string addressAsString() const = 0;

    uint32_t tag() const;
    void requestDeviceContext();

    MeshtasticMessageType readMessage(google::protobuf::Message **retVal, int timeOutMs = 0);

    void startListener();
    void stopListener(int seconds = 10);

    bool waitState(MeshtasticListenerState state = MLS_RUN, uint32_t milliSeconds = 12000);
};

#endif
