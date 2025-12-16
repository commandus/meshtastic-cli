#ifndef MESHTASTICDEVICESERIAL_H
#define MESHTASTICDEVICESERIAL_H

#include "Windows.h"

#include "MeshtasticDevice.h"
#include "MeshtasticSerialTransport-win.h"

class MeshtasticDeviceSerial : public MeshtasticDevice {
public:
    uint8_t portNumber;
    HANDLE handle;

    std::string deviceName() const;

    MeshtasticDeviceSerial(MeshtasticTransport *aTransport);
    MeshtasticDeviceSerial(MeshtasticTransport *aTransport, uint8_t port);
    MeshtasticDeviceSerial(MeshtasticSerialTransport *aTransport, uint8_t port, const std::string &name);
    MeshtasticDeviceSerial(const MeshtasticDeviceSerial &value);
    virtual ~MeshtasticDeviceSerial();
    bool equals(MeshtasticDevice *cmp) const override;
    bool isValid() const override;
    uint64_t hash() const override;
    std::string addressAsString() const override;
};

#endif
