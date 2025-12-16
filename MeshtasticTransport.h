#ifndef MESHTASTICCONNECTION_H
#define MESHTASTICCONNECTION_H

#include "MeshtasticEnvironment.h"

#define MTU_SIZE 512

/**
 * Client can connect to the device by Bluetooth LE, USB COM port and over Wi-Fi hotspot.
 */
enum MeshtasticTransportType {
    MTT_ANY,
    MTT_BLE,
    MTT_SERIAL,
    MTT_WIFI
};

enum MeshtasticTransportState {
    MTS_OFF = 0,
    MTS_ON
};

class MeshtasticDevice;
class MeshtasticEnvironment;

class MeshtasticTransportBuffer {
public:
    uint16_t size;
    uint8_t data[MTU_SIZE];
};

/**
 * Transport is responsible for all I/O operations such
 * - discover devices- fire addDiscoveredDevice()
 * - read
 * - write
 */
class MeshtasticTransport {
public:
    bool discoveryOn;
    enum MeshtasticTransportState state;

    MeshtasticEnvironment *env;
    MeshtasticTransportType transportType;  ///< BLE, Serial, WiFi but not any

    MeshtasticTransport();
    MeshtasticTransport(MeshtasticEnvironment *env);
    MeshtasticTransport(MeshtasticEnvironment *env, MeshtasticTransportType transportType);
    virtual ~MeshtasticTransport();

    virtual int startTransport();
    virtual void stopTransport();
    virtual int startDiscovery();
    virtual void stopDiscovery(int seconds = 10);
    virtual int openDevice(MeshtasticDevice *device) = 0;
    virtual int closeDevice(MeshtasticDevice *device) = 0;
    virtual int write(const MeshtasticDevice *device, const google::protobuf::Message *msg) = 0;
    virtual int read(const MeshtasticDevice *device, void *buffer, uint16_t size, int timeOutMs) = 0;

    bool ping(MeshtasticDevice& device);
};

#endif
