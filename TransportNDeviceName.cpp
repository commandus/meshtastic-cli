#include "TransportNDeviceName.h"

#include "MeshtasticTransport.h"
#include "MeshtasticDevice.h"

TransportNDeviceName::TransportNDeviceName()
    : typ(MTT_ANY)
{

}

TransportNDeviceName::TransportNDeviceName(
    MeshtasticTransportType aType,
    const std::string &aDeviceAddressOrName
)
    : typ(aType), deviceAddressOrName(aDeviceAddressOrName)
{

}

bool TransportNDeviceNames::deviceAllowed(
    MeshtasticTransportType aType,
    const std::string &deviceAddressString,
    const std::string &deviceName
) const {
    std::string deviceAddressStringUpperCase;
    std::transform(deviceAddressString.begin(), deviceAddressString.end(), std::back_inserter(deviceAddressStringUpperCase), ::toupper);
    auto it = std::find_if(transportNDeviceNames.begin(), transportNDeviceNames.end(), [aType, deviceAddressStringUpperCase, deviceName](const TransportNDeviceName& v) {
        std::string addressUpperCase;
        std::transform(v.deviceAddressOrName.begin(), v.deviceAddressOrName.end(), std::back_inserter(addressUpperCase), ::toupper);
        return (v.typ == aType || aType == MTT_ANY) && ((deviceName == v.deviceAddressOrName) || (deviceAddressStringUpperCase == addressUpperCase));
    });
    if (it != transportNDeviceNames.end())
        return true;
    // if no filter of the transport type has been specified, allow
    auto itTransportHasAny = std::find_if(transportNDeviceNames.begin(), transportNDeviceNames.end(), [aType](const TransportNDeviceName& v) {
        return (v.typ == aType || aType == MTT_ANY);
    });
    if (itTransportHasAny == transportNDeviceNames.end())
        return true;
    return false;
}

bool TransportNDeviceNames::deviceAllowed(
    const MeshtasticDevice *device
) const {
    return device && deviceAllowed(device->transport->transportType, device->addressAsString(), device->name);
}

bool TransportNDeviceNames::hasAnyFor(
    MeshtasticTransportType transportType
) {
    auto it = std::find_if(transportNDeviceNames.begin(), transportNDeviceNames.end(), [transportType](const TransportNDeviceName& v) {
        return (v.typ == transportType || transportType == MTT_ANY);
    });
    return it != transportNDeviceNames.end();
}
