#ifndef TRANSPORTNDEVICENAME_H
#define TRANSPORTNDEVICENAME_H

#include <string>
#include <vector>

class MeshtasticDevice;

// #include "MeshtasticTransport.h"
enum MeshtasticTransportType;

class TransportNDeviceName {
public:
    MeshtasticTransportType typ;
    std::string deviceAddressOrName;
    TransportNDeviceName();
    TransportNDeviceName(MeshtasticTransportType typ, const std::string &deviceAddressOrName);
};

class TransportNDeviceNames {
public:
    std::vector<TransportNDeviceName> transportNDeviceNames;
    bool deviceAllowed(MeshtasticTransportType aType, const std::string &deviceAddressString, const std::string &deviceName) const;
    bool deviceAllowed(const MeshtasticDevice *device) const;

    bool hasAnyFor(MeshtasticTransportType transportType);
    bool add(const std::string &macAddressOrDeviceName);
};

#endif
