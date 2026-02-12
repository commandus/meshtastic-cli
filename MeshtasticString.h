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

namespace MeshtasticString {
    std::string macAddress2string(uint64_t addr);
    // check is valid MAC address e.g. 9c:13:9e:a0:b7:5d
    bool isMacAddressString(const std::string &str);
    uint64_t string2macAddress(const std::string &str, bool *retValid = nullptr);
    // check is valid device file name e.g. COM4
    bool isDeviceFileNameString(const std::string &str);
    std::string transportType2String(MeshtasticTransportType type);
    std::string advertisementDataType2String(uint8_t dataType);
    std::string hex(const std::string &str);
    std::string hex(void *buffer, size_t size);
    std::string meshtasticMessageType2String(MeshtasticMessageType typ);
    std::string channelRole2String(meshtastic::Channel_Role role);
    std::string psk2String(const std::string &psk);
    std::string routingError2string(meshtastic::Routing_Error error);
    std::string port2string(meshtastic::PortNum portNum);

    std::string hstring2string(const winrt::hstring &value);
    std::string characteristic2String(const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic &characteristic);
    std::string characteristicProperties2String(const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic &characteristic);
    std::string devicePairingStatus2string(winrt::Windows::Devices::Enumeration::DevicePairingResultStatus status);
    std::string gattCommunicationStatus2string(const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus &status);
    std::string sessionStatus2string(const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSessionStatus status);
    std::string asyncStatus2string(const winrt::Windows::Foundation::AsyncStatus status);
    std::string currentTimeStamp();
};

#endif
