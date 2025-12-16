#ifndef MESHTASTICSERIALTRANSPORT_WIN_H
#define MESHTASTICSERIALTRANSPORT_WIN_H

#include "MeshtasticTransport.h"

typedef uint8_t SerialPortId;

class MeshtasticSerialTransport : public MeshtasticTransport {
private:
    std::map<const MeshtasticDevice*, std::string> debugLogBuffer;
    void debugLog(const void* buffer, uint16_t size, const MeshtasticDevice *device = nullptr);
public:
    explicit MeshtasticSerialTransport(MeshtasticEnvironment *env);

    int startTransport() override;
    void stopTransport() override;
    int startDiscovery() override;
    void stopDiscovery(int seconds = 10) override;

    int openDevice(MeshtasticDevice *device) override;
    int closeDevice(MeshtasticDevice *device) override;

    int write(const MeshtasticDevice *device, const google::protobuf::Message *msg) override;
    int read(const MeshtasticDevice *device, void *buffer, uint16_t size, int timeOutMs) override;

    static std::string portName(SerialPortId port);
    static void listPorts(std::vector<SerialPortId> &ports);
    bool probeIsPortMeshtastic(const SerialPortId& port);
};

#endif
