#include "MeshtasticDeviceSerial-win.h"

MeshtasticDeviceSerial::MeshtasticDeviceSerial(
    MeshtasticTransport *aTransport
)
    : MeshtasticDevice(aTransport), portNumber(0), handle(INVALID_HANDLE_VALUE)
{

}

MeshtasticDeviceSerial::MeshtasticDeviceSerial(
    MeshtasticTransport *aTransport,
    uint8_t port
)
    : MeshtasticDevice(aTransport), portNumber(port), handle(INVALID_HANDLE_VALUE)
{

}

MeshtasticDeviceSerial::MeshtasticDeviceSerial(
    MeshtasticSerialTransport *aTransport,
    uint8_t port,
    const std::string &name
)
    : MeshtasticDevice(aTransport, name), portNumber(port), handle(INVALID_HANDLE_VALUE)
{

}

bool MeshtasticDeviceSerial::equals(MeshtasticDevice *cmp) const {
    return MeshtasticDevice::equals(cmp) && portNumber == ((MeshtasticDeviceSerial *) cmp)->portNumber;
}

bool MeshtasticDeviceSerial::isValid() const
{
    return true;
}

uint64_t MeshtasticDeviceSerial::hash() const
{
    return portNumber;
}

std::string MeshtasticDeviceSerial::deviceName() const
{
    return addressAsString();
}

std::string MeshtasticDeviceSerial::addressAsString() const {
    return "COM" + std::to_string(portNumber);
}

MeshtasticDeviceSerial::MeshtasticDeviceSerial(
    const MeshtasticDeviceSerial &value
)
    : MeshtasticDevice(value), portNumber(value.portNumber), handle(value.handle)
{

}

MeshtasticDeviceSerial::~MeshtasticDeviceSerial()
{
}

