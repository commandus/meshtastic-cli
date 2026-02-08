#include "MeshtasticDeviceBLE-win.h"
#include "MeshtasticBLETransport-win.h"
#include "MeshtasticString.h"

#include <cinttypes>

#include <winrt/windows.devices.bluetooth.advertisement.h>

MeshtasticDeviceBLE::MeshtasticDeviceBLE(MeshtasticTransport *aTransport)
    : MeshtasticDevice(aTransport), addr(0), device(nullptr), session(nullptr), service(nullptr),
    characteristics{ nullptr, nullptr, nullptr, nullptr }
{

}

MeshtasticDeviceBLE::MeshtasticDeviceBLE(
    MeshtasticTransport *aTransport,
    uint64_t aAddr
)
    : MeshtasticDevice(aTransport), addr(aAddr), device(nullptr), session(nullptr), service(nullptr), characteristics{ nullptr, nullptr, nullptr, nullptr }
{
    winrt::Windows::Devices::Bluetooth::BluetoothLEDevice dev = winrt::Windows::Devices::Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(addr).get();
    if (!MeshtasticBLETransport::checkBLECharacteristicsExists(&dev)) {

    }
}

MeshtasticDeviceBLE::MeshtasticDeviceBLE(
    MeshtasticBLETransport *aTransport,
    uint64_t aAddr,
    const std::string &aName
)
    : MeshtasticDevice(aTransport, aName), addr(aAddr), device(nullptr), session(nullptr), service(nullptr), characteristics{ nullptr, nullptr, nullptr, nullptr }
{
    winrt::Windows::Devices::Bluetooth::BluetoothLEDevice dev = winrt::Windows::Devices::Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(addr).get();
    if (!MeshtasticBLETransport::checkBLECharacteristicsExists(&dev)) {

    }
}

bool MeshtasticDeviceBLE::equals(
    MeshtasticDevice *cmp
) const
{
    return MeshtasticDevice::equals(cmp) && addr == ((MeshtasticDeviceBLE *) cmp)->addr;
}

uint64_t MeshtasticDeviceBLE::hash() const
{
    return addr;
}

bool MeshtasticDeviceBLE::isValid() const {
    return true;
}

std::string MeshtasticDeviceBLE::addressAsString() const {
    return MeshtasticString::macAddress2string(addr);
}

MeshtasticDeviceBLE::~MeshtasticDeviceBLE() {

}

