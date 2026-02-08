#ifndef MESHTASTICDEVICEBLE_WIN_H
#define MESHTASTICDEVICEBLE_WIN_H

#include <winrt/windows.devices.bluetooth.h>

#include "MeshtasticTypes.h"
#include "MeshtasticDevice.h"
#include "MeshtasticBLETransport-win.h"

class MeshtasticDeviceBLE : public MeshtasticDevice {
public:
    uint64_t addr;
    winrt::Windows::Devices::Bluetooth::BluetoothLEDevice device;
    winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSession session;
    // BLE service
    winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService *service;
    // BLE characteristics
    winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic characteristics[4];
    MeshtasticDeviceBLE(MeshtasticTransport *aTransport);
    MeshtasticDeviceBLE(MeshtasticTransport *aTransport, uint64_t addr);
    MeshtasticDeviceBLE(MeshtasticBLETransport *aTransport, uint64_t addr, const std::string &name);
    virtual ~MeshtasticDeviceBLE();
    bool equals(MeshtasticDevice *cmp) const override;
    bool isValid() const override;
    uint64_t hash() const override;
    std::string addressAsString() const override;
};

#endif
