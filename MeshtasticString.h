#ifndef MESHTASTICSTRING_H
#define MESHTASTICSTRING_H

#include <cinttypes>
#include <string>

#ifdef _MSC_VER
#include <winrt/base.h>
#include <winrt/windows.devices.bluetooth.genericattributeprofile.h>
#endif

#include "MeshtasticTransport.h"
#include "meshtastic/channel.pb.h"

class MeshtasticString {
public:
    static std::string macAddress2string(uint64_t addr);
    // check is valid MAC address e.g. 9c:13:9e:a0:b7:5d
    static bool isMacAddressString(const std::string &str);
    static uint64_t MeshtasticString::string2macAddress(const std::string &str, bool *retValid = nullptr);
    // check is valid device file name e.g. COM4
    static bool isDeviceFileNameString(const std::string &str);
    static std::string transportType2String(MeshtasticTransportType type);
    static std::string advertisementDataType2String(uint8_t dataType);
    static std::string hex(const std::string &str);
    static std::string hex(void *buffer, size_t size);
    static std::string meshtasticMessageType2String(MeshtasticMessageType typ);
    static std::string channelRole2String(meshtastic::Channel_Role role);
    static std::string psk2String(const std::string &psk);
    static std::string routingError2string(meshtastic::Routing_Error error);
    static std::string port2string(meshtastic::PortNum portNum);

#ifdef _MSC_VER
    static std::string hstring2string(const winrt::hstring &value);
    static std::string characteristic2String(const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic &characteristic);
    static std::string characteristicProperties2String(const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic &characteristic);
    static std::string devicePairingStatus2string(winrt::Windows::Devices::Enumeration::DevicePairingResultStatus status);
    static std::string gattCommunicationStatus2string(const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus &status);
    static std::string sessionStatus2string(const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSessionStatus status);
    static std::string asyncStatus2string(const winrt::Windows::Foundation::AsyncStatus status);
    static std::string currentTimeStamp();
#endif

};

#endif
