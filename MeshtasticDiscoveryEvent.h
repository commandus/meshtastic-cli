#ifndef MESHTASTICDISCOVERYEVENT_H
#define MESHTASTICDISCOVERYEVENT_H


#include <string>
#include "MeshtasticDevice.h"

class MeshtasticDevice;

class MeshtasticDiscoveryEvent {
public:
    MeshtasticDevice *device;   ///< optional, can be NULL
    std::string deviceName;
    uint64_t addr;
    std::string deviceAddressString() const;
    int16_t signalStrength;     ///< dBm
    bool isMeshtasticDevice;
    std::string manufacturerSpecificData;

    MeshtasticDiscoveryEvent(
        MeshtasticDevice *device,
        const std::string &deviceName,
        uint64_t deviceAddress,
        int16_t signalStrength,
        bool isMeshtasticDevice
    );
};


#endif
