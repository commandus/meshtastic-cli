#ifndef MESHTASTICENVIRONMENT_H
#define MESHTASTICENVIRONMENT_H

#include <mutex>
#include <string>

#include <google/protobuf/message.h>

#include "MeshtasticMessage.h"
#include "PinCodeProvider.h"
#include "TransportNDeviceName.h"

class MeshtasticDevice;
class MeshtasticTransport;
class MeshtasticEnvironmentEvent;
class MeshtasticEnvironmentEventHandler;
class PinCodeProvider;

/**
 * Keep devices, their transport together in the "environment".
 * "Environment" is responsible to manage devices.
 * Transport is responsible to detect and open devices.
 * Events fired up by the transport, then environment pass fired event to the event handlers.
 */
class MeshtasticEnvironment {
private:
    std::ostream *logStream;
    int verbosity;

    // running state
    std::mutex mutexEvent;
    std::condition_variable cvEvent;
protected:
    std::mutex mutexDevice;
    std::mutex mutexHandler;
    std::vector <MeshtasticDevice *> devices;
    std::vector <MeshtasticTransport *> transports;
    std::vector <MeshtasticEnvironmentEventHandler*> eventHandlers;

public:
    TransportNDeviceNames filterDevice; // collection of filters grouped by transport type and device name e.g. "COM4"

    MeshtasticDevice *find(MeshtasticTransport *transport);   ///< find first device uses this transport
    MeshtasticDevice *find(enum MeshtasticTransportType typ);   ///< find first device uses this type of transport
    MeshtasticDevice *find(enum MeshtasticTransportType typ, uint64_t hash);   ///< find first device with specified BLE address or Windows COM port number
    MeshtasticDevice *find(enum MeshtasticTransportType typ, const std::string &name);   ///< find first device with assigned device name
    MeshtasticDevice *MeshtasticEnvironment::find(MeshtasticDevice *device);
    MeshtasticDevice *get(size_t index);
    size_t count(); ///< device count

    PinCodeProvider *pinCodeProvider;

    bool addDevice(MeshtasticDevice *device);
    void rmDevice(MeshtasticDevice *device);
    void rmDevice(size_t index);
    void clearDevices();

    MeshtasticTransport *getTransport(size_t index);
    MeshtasticTransport *findFirstTransport(enum MeshtasticTransportType typ, const std::string &name);
    size_t transportCount();
    void addTransport(MeshtasticTransport *transport);
    void rmTransport(MeshtasticTransport *transport);
    void rmTransport(size_t index);
    void rmTransport(enum MeshtasticTransportType typ);
    void clearTransport();

    void addEventHandler(MeshtasticEnvironmentEventHandler* handler);
    void rmEventHandler(MeshtasticEnvironmentEventHandler* handler);
    void clearEventHandlers();
    size_t fireEvent(const MeshtasticEnvironmentEvent &event);
    /**
     * Await until device has been discovered
     * @param seconds seconds to discover.
     * @return 0 count of discovered devices, -1- timeout
     */
    uint32_t waitDeviceConfigured(int seconds = 10);

    MeshtasticEnvironment();
    virtual ~MeshtasticEnvironment();

    void startTransport(enum MeshtasticTransportType typ, const std::string &name = "");
    void stopTransport(enum MeshtasticTransportType typ, const std::string &name = "");

    int startDiscovery(enum MeshtasticTransportType typ, const std::string &name = "");
    void stopDiscovery(enum MeshtasticTransportType typ, const std::string &name = "");
    bool isDebugEnabled(int verbosity);
    void debugLog(int verbosity, const std::string &line, const MeshtasticDevice *device = nullptr);
    void setDebugLog(int verbosity, std::ostream *strm);

    uint32_t sendString(uint32_t nodeNum, const std::string &value, const MeshtasticDevice* device = nullptr);
    uint32_t sendString(const std::vector<uint32_t> &nodeNums, const std::string &value, const MeshtasticDevice* device = nullptr);
    static int waitMessage(
        MeshtasticDevice *device,
        google::protobuf::Message **retMsg,
        uint32_t nodeNumFrom,
        uint32_t nodeNumTo,
        uint32_t id,
        meshtastic::PortNum port,
        int timeoutMs
    );
    void deviceConfigured(const MeshtasticDevice *device);
    uint32_t configuredRunningDeviceCount();
    uint32_t discoveredDeviceCount();
    uint64_t hash() const;
    uint32_t tag() const;
};

#endif
