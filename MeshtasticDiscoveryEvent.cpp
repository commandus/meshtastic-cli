#include "MeshtasticDiscoveryEvent.h"
#include "MeshtasticString.h"

MeshtasticDiscoveryEvent::MeshtasticDiscoveryEvent(
    MeshtasticDevice *aDevice,
    const std::string &aDeviceName,
    uint64_t aAddr,
    int16_t aSignalStrength,
    bool aIsMeshtasticDevice
)
    : device(aDevice), deviceName(aDeviceName), addr(aAddr), signalStrength(aSignalStrength),
        isMeshtasticDevice(aIsMeshtasticDevice)
{

}

std::string MeshtasticDiscoveryEvent::deviceAddressString() const {
    return MeshtasticString::macAddress2string(addr);
}
