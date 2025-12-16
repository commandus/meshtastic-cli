#ifndef MESHTASTICOSTRANSPORT_WIN_H
#define MESHTASTICOSTRANSPORT_WIN_H

#include <winrt/windows.foundation.h>
#include <winrt/windows.foundation.collections.h>
#include <winrt/windows.devices.bluetooth.h>
#include <winrt/windows.devices.enumeration.h>
#include <winrt/windows.devices.bluetooth.advertisement.h>
#include <winrt/windows.devices.bluetooth.genericattributeprofile.h>
#include <winrt/windows.storage.streams.h>

#include "MeshtasticTypes.h"
#include "MeshtasticTransport.h"

enum IdxGattProperty {
    MESHTASTIC_BLE_PROPERTY_FROMRADIO = 0,
    MESHTASTIC_BLE_PROPERTY_TORADIO = 1,
    MESHTASTIC_BLE_PROPERTY_FROMNUM = 2,
    MESHTASTIC_BLE_PROPERTY_LOG = 3,
    // others
    MESHTASTIC_BLE_PROPERTY_UNKNOWN = 4
};

class MeshtasticBLETransport : public MeshtasticTransport {
private:
    winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher advWatcher;
    winrt::event_token discoveryToken;
    winrt::event_token stoppedDiscoveryToken;
    std::mutex mutexDiscoveryState;
    std::condition_variable cvDiscoveryState;

    int pair(MeshtasticDevice *device);
    int subscribe(MeshtasticDevice *device);
    // notifications
    void characteristicLogValueChanged(winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic const& sender, const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs &args) ;
    void characteristicFromNumValueChanged(winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic const& sender, const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs &args) ;
    void pairingRequestedHandler(
        const winrt::Windows::Devices::Enumeration::DeviceInformationCustomPairing &devicePairing,
        const winrt::Windows::Devices::Enumeration::DevicePairingRequestedEventArgs &eventArgs
    );

public:
    static const winrt::guid &meshtasticBluetoothServiceUUID();
    static const winrt::guid &meshtasticProperty(enum IdxGattProperty prop);
    static enum IdxGattProperty getPropertyIdx(const winrt::guid &properetyUuid);

    explicit MeshtasticBLETransport(MeshtasticEnvironment *env);

    int startTransport() override;
    void stopTransport() override;
    int startDiscovery() override;
    void stopDiscovery(int seconds = 10) override;

    int openDevice(MeshtasticDevice *device) override;
    int closeDevice(MeshtasticDevice *device) override;

    int write(const MeshtasticDevice *device, const google::protobuf::Message *msg) override;
    int read(const MeshtasticDevice *device, void *buffer, uint16_t size, int timeOutMs) override;

    static bool isServiceUUIDMeshtastic(const winrt::guid *serviceUuid);

    static bool checkBLECharacteristicsExists(uint64_t addr);
    static bool checkBLECharacteristicsExists(winrt::Windows::Devices::Bluetooth::BluetoothLEDevice *device);
    static bool printCharacteristics(std::ostream &strm, uint64_t addr);
    static bool printCharacteristics(std::ostream &strm, winrt::Windows::Devices::Bluetooth::BluetoothLEDevice *device);

    static std::string getDeviceName(uint64_t deviceAddr);
    static std::string getDeviceName(winrt::Windows::Devices::Bluetooth::BluetoothLEDevice *device);

    int unpair(MeshtasticDevice *device);
};

#endif
