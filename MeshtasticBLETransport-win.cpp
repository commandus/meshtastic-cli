#include <sstream>
#include <winrt/windows.storage.streams.h>
#include "MeshtasticBLETransport-win.h"

#include "MeshtasticDeviceBLE-win.h"
#include "MeshtasticString.h"
#include "BLEGuid.h"
#include "MeshtasticDiscoveryEvent.h"
#include "MeshtasticEnvironmentEvent.h"

// Meshtastic's BLE GATT service
static const winrt::guid meshtasticUUID {0x6ba1b218, 0x15a8, 0x461f, {0x9f, 0xa8, 0x5d, 0xca, 0xe2, 0x73, 0xea, 0xfd } };

// Meshtastic's BLE GATT Characteristics
static const winrt::guid mMeshtasticBLEProperty[] {
    // 2c55e69e-4993-11ed-b878-0242ac120002 read fromRadio: newly received FromRadio packet
    { 0x2c55e69e, 0x4993, 0x11ed, { 0xb8, 0x78, 0x02, 0x42, 0xac, 0x12, 0x00, 0x02 }},
    // f75c76d2-129e-4dad-a1dd-7866124401e7 write toRadio - write ToRadio protobufs to this characteristic to send them (up to MAXPACKET len)
    { 0xf75c76d2, 0x129e, 0x4dad, { 0xa1, 0xdd, 0x78, 0x66, 0x12, 0x44, 0x01, 0xe7 }},
    // ed9da18c-a800-4f66-a670-aa7547e34453 read,notify,write fromNum - the current packet # in the message waiting inside fromRadio
    { 0xed9da18c, 0xa800, 0x4f66, { 0xa6, 0x70, 0xaa, 0x75, 0x47, 0xe3, 0x44, 0x53 }},
    // 5a3d6e49-06e6-4423-9944-e9de8cdf9547 notify A log message as LogRecord protobuf
    { 0x5a3d6e49, 0x06e6, 0x4423, { 0x99, 0x44, 0xe9, 0xde, 0x8c, 0xdf, 0x95, 0x47 }}
};

MeshtasticBLETransport::MeshtasticBLETransport(
    MeshtasticEnvironment *env
)
    : MeshtasticTransport(env, MTT_BLE)
{
}

/**
 * Read device name by MAC address from 0x2a00 characteristic. If no 0x2a00 characteristic provided return empty string
 * @param addr MAC address
 * @return device name
 */
std::string MeshtasticBLETransport::getDeviceName(
    uint64_t addr
) {
    winrt::Windows::Devices::Bluetooth::BluetoothLEDevice dev = winrt::Windows::Devices::Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(addr).get();
    if (dev == nullptr)
        return "";
    return getDeviceName(&dev);
}

/**
 * Read device name by MAC address from 0x2a00 characteristic. If no 0x2a00 characteristic provided return empty string
 * @param addr BLE device pointer
 * @return device name
 */
std::string MeshtasticBLETransport::getDeviceName(
    winrt::Windows::Devices::Bluetooth::BluetoothLEDevice *device
) {
    // get service
    winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult gapServicesResult = device->GetGattServicesForUuidAsync(winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattServiceUuids::GenericAccess(), winrt::Windows::Devices::Bluetooth::BluetoothCacheMode::Uncached).get();
    if (gapServicesResult.Status() != winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success)
        return "";
    winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService> gapServices = gapServicesResult.Services();
    if (gapServices.Size() == 0)
        return "";
    // get device name characteristic, number 00002a00 (UUID: 00002a00-0000-1000-8000-00805f9b34fb)
    auto s0 = gapServices.GetAt(0);
    winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic> gapDeviceNameChrs = s0.GetCharacteristics(winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristicUuids::GapDeviceName());
    if (gapDeviceNameChrs.Size() == 0)
        return "";
    auto readRes = gapDeviceNameChrs.GetAt(0).ReadValueAsync().get();
    if (readRes.Status() != winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success)
        return "";
    // read attribute
    winrt::Windows::Storage::Streams::DataReader reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(readRes.Value());
    return MeshtasticString::hstring2string(reader.ReadString(reader.UnconsumedBufferLength()).c_str());
}

/**
 * start discovery BLE devices provides Meshtastic service
 * @return 0- success
 */
int MeshtasticBLETransport::startTransport()
{
    int r = MeshtasticTransport::startTransport();
    if (r)
        return r;
    return startDiscovery();
}

/**
 * Stop discovery if it is on.
 */
void MeshtasticBLETransport::stopTransport()
{
    stopDiscovery();
    MeshtasticTransport::stopTransport();
}

bool MeshtasticBLETransport::isServiceUUIDMeshtastic(
    const winrt::guid *serviceUuid
) {
    return serviceUuid && *serviceUuid == meshtasticUUID;
}

const winrt::guid &MeshtasticBLETransport::meshtasticBluetoothServiceUUID() {
    return meshtasticUUID;
}

const winrt::guid &MeshtasticBLETransport::meshtasticProperty(
    enum IdxGattProperty propertyIndex
) {
    return mMeshtasticBLEProperty[(int) propertyIndex];
}

enum IdxGattProperty MeshtasticBLETransport::getPropertyIdx(
        const winrt::guid &properetyUuid
) {
    for (int i = 0; i < 4; i++) {
        if (mMeshtasticBLEProperty[i] == properetyUuid)
            return (enum IdxGattProperty) i;
    }
    return IdxGattProperty::MESHTASTIC_BLE_PROPERTY_UNKNOWN;
}

bool MeshtasticBLETransport::checkBLECharacteristicsExists(
    uint64_t addr
) {
    winrt::Windows::Devices::Bluetooth::BluetoothLEDevice dev = winrt::Windows::Devices::Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(addr).get();
    return checkBLECharacteristicsExists(&dev);
}

/**
 * Read BLE GATT characteristics and check does it have required characteristics.
 * Please note if device is not paired not all characteristics are visible
 * @param device BLE device pointer
 * @return true if all characteristics are present in the BLE GATT
 *
 * Services:
 *  00001800-0000-1000-8000-00805f9b34fb Generic access service
 *    Characteristic 2a00 Device name
 *    Characteristic 2a01 Appearance
 *    Characteristic 2a04 Peripheral Preferred Connection Parameters
 *    Characteristic 2aa6 Alert Level
 *  00001801-0000-1000-8000-00805f9b34fb Generic attribute service
 *    Characteristic 2a05: Service Changed
 *  00001530-1212-efde-1523-785feabcd123
 *  0000180a-0000-1000-8000-00805f9b34fb Device information service
 *  0000180f-0000-1000-8000-00805f9b34fb Battery service
 *  6ba1b218-15a8-461f-9fa8-5dcae273eafd Meshtastic service
 */
bool MeshtasticBLETransport::checkBLECharacteristicsExists(
    winrt::Windows::Devices::Bluetooth::BluetoothLEDevice *device
) {
    if (!device)
        return false;
    winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult result = device->GetGattServicesForUuidAsync(MeshtasticBLETransport::meshtasticBluetoothServiceUUID()).get();
    if (result.Status() != winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success)
        return false;

    bool hasProperty[4] { false, false, false, false };
    winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService> services = result.Services();
    for (auto &&service : services) {
        winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic> gapDeviceNameChrs = service.GetAllCharacteristics();
        for (auto &&characteristic : gapDeviceNameChrs) {
            auto idx = MeshtasticBLETransport::getPropertyIdx(characteristic.Uuid());
            if (idx != IdxGattProperty::MESHTASTIC_BLE_PROPERTY_UNKNOWN)
                hasProperty[(int) idx] = true;
        }
    }
    return hasProperty[0] && hasProperty[1] && hasProperty[2] && hasProperty[3];
}

/**
 * Print out characteristics list
 * @param strm stream out
 * @param addr BLE device MAC address
 * @return true - success, false- communication error
 */
bool MeshtasticBLETransport::printCharacteristics(
    std::ostream &strm,
    uint64_t addr
) {
    winrt::Windows::Devices::Bluetooth::BluetoothLEDevice dev = winrt::Windows::Devices::Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(addr).get();
    return printCharacteristics(strm, &dev);
}

bool MeshtasticBLETransport::printCharacteristics(
    std::ostream &strm,
    winrt::Windows::Devices::Bluetooth::BluetoothLEDevice *device
) {
    winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult result = device->GetGattServicesAsync().get();
    if (result.Status() != winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success)
        return false;

    winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService> services = result.Services();
    for (auto &&service : services) {
        strm << "Service " << BLEGuid::UUIDToString(service.Uuid()) << std::endl;
        winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic> gapDeviceNameChrs = service.GetAllCharacteristics();
        for (winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic &&characteristic : gapDeviceNameChrs) {
            strm << "  Characteristic " << BLEGuid::ServiceUUIDToString(characteristic.Uuid())
                << " (" << MeshtasticString::characteristic2String(characteristic)
                << "): " << MeshtasticString::characteristicProperties2String(characteristic) << std::endl;
        }
    }
    return true;
}

void MeshtasticBLETransport::pairingRequestedHandler(
    const winrt::Windows::Devices::Enumeration::DeviceInformationCustomPairing &devicePairing,
    const winrt::Windows::Devices::Enumeration::DevicePairingRequestedEventArgs &eventArgs
)
{
    switch (eventArgs.PairingKind()) {
        case winrt::Windows::Devices::Enumeration::DevicePairingKinds::ConfirmOnly:
            if (env->isDebugEnabled(LOG_INFO)) {
                std::stringstream ss;
                ss << _("BLE device ") << MeshtasticString::hstring2string(eventArgs.DeviceInformation().Name()) << _(" pairing: confirm only");
                env->debugLog(LOG_INFO, ss.str());
            }
            eventArgs.Accept();
            return;

        case winrt::Windows::Devices::Enumeration::DevicePairingKinds::ProvidePin: {
            if (env->isDebugEnabled(LOG_INFO)) {
                std::stringstream ss;
                ss << _("BLE device ") << MeshtasticString::hstring2string(eventArgs.DeviceInformation().Name()) << _(" pairing: provide PIN");
                env->debugLog(LOG_INFO, ss.str());
            }
            std::string pin;
            if (env->pinCodeProvider) {
                std::string dn = MeshtasticString::hstring2string(eventArgs.DeviceInformation().Name());
                pin = env->pinCodeProvider->enterPinCode(dn);
            } else {
                if (env->isDebugEnabled(LOG_ERR)) {
                    std::stringstream ss;
                    ss << _("BLE device ") << MeshtasticString::hstring2string(eventArgs.DeviceInformation().Name()) << _(" is requesting a PIN code, but the PIN code provider is not set");
                    env->debugLog(LOG_ERR, ss.str());
                }
            }
            winrt::hstring hPin(std::wstring(pin.begin(), pin.end()));
            eventArgs.Accept(hPin);
        }
            return;
        case winrt::Windows::Devices::Enumeration::DevicePairingKinds::ConfirmPinMatch:
            if (env->isDebugEnabled(LOG_INFO)) {
                std::stringstream ss;
                ss << _("BLE device ") << MeshtasticString::hstring2string(eventArgs.DeviceInformation().Name()) << _(" pairing: confirm PIN match");
                env->debugLog(LOG_INFO, ss.str());
            }
            return;
        default:
            if (env->isDebugEnabled(LOG_ERR)) {
                std::stringstream ss;
                ss << _("BLE device ") << MeshtasticString::hstring2string(eventArgs.DeviceInformation().Name()) << _(" unexpected pairing type");
                env->debugLog(LOG_INFO, ss.str());
            }
            break;
    }
}

/**
 * Open device- pair device (if not already paired)
 * @param aDevice Meshtastic BLE device
 * @return 0- success
 */
int MeshtasticBLETransport::openDevice(
    MeshtasticDevice *aDevice
)
{
    MeshtasticDeviceBLE *dev = (MeshtasticDeviceBLE *) aDevice;

    // get device
    dev->device = winrt::Windows::Devices::Bluetooth::BluetoothLEDevice::FromBluetoothAddressAsync(dev->addr).get();

    // session
    dev->session = winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSession::FromDeviceIdAsync(dev->device.BluetoothDeviceId()).get();
    if (!dev->session.CanMaintainConnection())
        return - 1;

    dev->session.MaintainConnection(true);

    auto maxPduChangedToken = dev->session.MaxPduSizeChanged([this](const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSession &session, const winrt::Windows::Foundation::IInspectable &status) {
        if (env->isDebugEnabled(LOG_INFO)) {
            std::stringstream ss;
            ss << _("BLE device ") << MeshtasticString::hstring2string(session.DeviceId().Id()) << _(" PDU size changed to ") << session.MaxPduSize();
            env->debugLog(LOG_INFO, ss.str());
        }
    });

    auto statusChangedToken = dev->session.SessionStatusChanged([this](const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattSession &session, const winrt::Windows::Foundation::IInspectable &status) {
        if (env->isDebugEnabled(LOG_INFO)) {
            std::stringstream ss;
            ss << _("BLE device ") << MeshtasticString::hstring2string(session.DeviceId().Id())
                << _(" session status changed to ") << MeshtasticString::sessionStatus2string(session.SessionStatus());
            env->debugLog(LOG_INFO, ss.str());
        }
    });

    if (env->isDebugEnabled(LOG_INFO)) {
        std::stringstream ss;
        ss << _("BLE device ") << dev->name << _(" PDU size ") << dev->session.MaxPduSize()
            << " " << (dev->session.MaintainConnection() ? "" : _("no")) <<  _(" maintain connection")
            << _(" session status ") << MeshtasticString::sessionStatus2string(dev->session.SessionStatus());
        env->debugLog(LOG_INFO, ss.str());
    }

    // get service
    winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceServicesResult result = dev->device.GetGattServicesForUuidAsync(MeshtasticBLETransport::meshtasticBluetoothServiceUUID()).get();
    if (result.Status() != winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success) {
        std::cerr << "Error "
                  << MeshtasticString::gattCommunicationStatus2string(result.Status())
                  << " getting service" << std::endl;
        return -1;
    }
    // get characteristics
    winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattDeviceService> services = result.Services();
    try {
        for (int i = 0; i < 4; i++) {
            auto characteristics = services.GetAt(0).GetCharacteristicsForUuidAsync(MeshtasticBLETransport::meshtasticProperty((enum IdxGattProperty) i)).get();
            if (characteristics.Status() != winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success)
                return -1;
            if (characteristics.Characteristics().Size() < 1)
                return -1;
            dev->characteristics[i] = characteristics.Characteristics().GetAt(0);
        }
    } catch (winrt::hresult_error const &ex) {
        return -1;
    }

    int pr = pair(aDevice);
    if (pr != 0)
        return pr;
    return subscribe(aDevice);
}

/**
 * Close Meshtastic BLE device
 * @param device pointer to the Meshtastic BLE device
 * @return 0- success
 */
int MeshtasticBLETransport::closeDevice(
    MeshtasticDevice *device
) {
    return 0;
}

int MeshtasticBLETransport::subscribe(
    MeshtasticDevice *aDevice
) {
    MeshtasticDeviceBLE *device = (MeshtasticDeviceBLE *) aDevice;
    // initialize status
    auto cccdValue = winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattClientCharacteristicConfigurationDescriptorValue::Notify;
    try {
        // log
        winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattWriteResult writeResult = device->characteristics[MESHTASTIC_BLE_PROPERTY_LOG].WriteClientCharacteristicConfigurationDescriptorWithResultAsync(cccdValue).get();
        if (writeResult.Status() != winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success) {
            if (env->isDebugEnabled(LOG_ERR)) {
                std::stringstream ss;
                ss << _("WriteClientCharacteristicConfigurationDescriptorWithResultAsync error ")
                   << MeshtasticString::gattCommunicationStatus2string(writeResult.Status());
                env->debugLog(LOG_ERR, ss.str());
            }
        }
        auto logToken = device->characteristics[MESHTASTIC_BLE_PROPERTY_LOG].ValueChanged({ this, &MeshtasticBLETransport::characteristicLogValueChanged });
        // fromNum
        writeResult = device->characteristics[MESHTASTIC_BLE_PROPERTY_FROMNUM].WriteClientCharacteristicConfigurationDescriptorWithResultAsync(cccdValue).get();
        if (writeResult.Status() != winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success) {
            if (env->isDebugEnabled(LOG_ERR)) {
                std::stringstream ss;
                ss << _("WriteClientCharacteristicConfigurationDescriptorWithResultAsync error ")
                   << MeshtasticString::gattCommunicationStatus2string(writeResult.Status());
                env->debugLog(LOG_ERR, ss.str());
            }
        }
        auto fromNumToken = device->characteristics[MESHTASTIC_BLE_PROPERTY_FROMNUM].ValueChanged({ this, &MeshtasticBLETransport::characteristicFromNumValueChanged });
    } catch (winrt::hresult_error const &ex) {
        return -1;
    }
    return 0;
}

void MeshtasticBLETransport::characteristicLogValueChanged(
    const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic &sender,
    const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs &valueChangedValue
) {
    google::protobuf::Message *m = nullptr;
    enum MeshtasticMessageType t = MeshtasticMessage::parse(&m, valueChangedValue.CharacteristicValue().data(), valueChangedValue.CharacteristicValue().Length(), MMT_LOG);
    if (t != MMT_LOG) {
        if (env->isDebugEnabled(LOG_WARN)) {
            std::stringstream ss;
            ss << _("Wrong log message type  from device ") << BLEGuid::UUIDToString(sender.Uuid());
            env->debugLog(LOG_WARN, ss.str());
        }
    }
    if (env->isDebugEnabled(LOG_INFO)) {
        std::stringstream ss;
        ss << _("Device ") << BLEGuid::UUIDToString(sender.Uuid()) << _(" log message ") << m->DebugString();
        env->debugLog(LOG_INFO, ss.str());
    }
    if (m)
        delete m;
}

void MeshtasticBLETransport::characteristicFromNumValueChanged(
    const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCharacteristic &sender,
    const winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattValueChangedEventArgs &valueChangedValue
) {
    if (valueChangedValue.CharacteristicValue().Length() != 4) {
        if (env->isDebugEnabled(LOG_ERR)) {
            std::stringstream ss;
            ss << _("Device ") << BLEGuid::UUIDToString(sender.Uuid()) << _(" fromNum message has wrong size: ") << valueChangedValue.CharacteristicValue().Length();
            env->debugLog(LOG_INFO, ss.str());
        }
        return;
    }
    uint32_t val = *((uint32_t *) valueChangedValue.CharacteristicValue().data());
#ifdef IS_BIG_ENDIAN
    val = SWAP_BYTES_4(val);
#endif
    if (env->isDebugEnabled(LOG_INFO)) {
        std::stringstream ss;
        ss << _("Device ") << BLEGuid::UUIDToString(sender.Uuid()) << _(" fromNum message ") << val;
        env->debugLog(LOG_INFO, ss.str());
    }
}

int MeshtasticBLETransport::pair(MeshtasticDevice *aDevice) {
    // get DeviceInformation
    MeshtasticDeviceBLE *dev = (MeshtasticDeviceBLE *) aDevice;
    auto di = dev->device.DeviceInformation();
    if (di.Pairing().IsPaired()) {
        if (env->isDebugEnabled(LOG_INFO)) {
            env->debugLog(LOG_INFO, _("Device is paired"));
        }

    } else {
        if (!di.Pairing().CanPair())
            return -1;
        // pair
        di.Pairing().Custom().PairingRequested({this, &MeshtasticBLETransport::pairingRequestedHandler});
        di.Pairing().Custom().PairAsync(
            winrt::Windows::Devices::Enumeration::DevicePairingKinds::ConfirmOnly |
            winrt::Windows::Devices::Enumeration::DevicePairingKinds::ProvidePin |
            winrt::Windows::Devices::Enumeration::DevicePairingKinds::ConfirmPinMatch,
            winrt::Windows::Devices::Enumeration::DevicePairingProtectionLevel::None)
            .Completed(
            [this](auto &&op, auto &&status) {
                if (env->isDebugEnabled(LOG_INFO)) {
                    std::stringstream ss;
                    ss << _("Pairing ") << MeshtasticString::asyncStatus2string(status);
                    env->debugLog(LOG_WARN, ss.str());
                }
                return 0;
            });
    }
    return 0;
}

int MeshtasticBLETransport::unpair(MeshtasticDevice *aDevice) {
    // get DeviceInformation
    MeshtasticDeviceBLE *dev = (MeshtasticDeviceBLE *) aDevice;
    auto di = dev->device.DeviceInformation();
    if (!di.Pairing().CanPair())
        return -1;
    if (!di.Pairing().IsPaired()) {
        if (env->isDebugEnabled(LOG_INFO)) {
            std::stringstream ss;
            ss << _("Device is not paired");
            env->debugLog(LOG_WARN, ss.str());
        }
        return 0;
    }
    // pair
    di.Pairing().UnpairAsync().Completed(
        [this](auto &&op, auto &&status) {
            if (env->isDebugEnabled(LOG_INFO)) {
                std::stringstream ss;
                ss << _("Unpairing ") << MeshtasticString::asyncStatus2string(status);
                env->debugLog(LOG_WARN, ss.str());
            }
            return 0;
        });
    return 0;
}

int MeshtasticBLETransport::read(
    const MeshtasticDevice *device,
    void *buffer,
    uint16_t size,
    int timeOutMs
)
{
    if (!device || !device->transport || !device->transport->env || !((MeshtasticDeviceBLE*) device)->characteristics[MESHTASTIC_BLE_PROPERTY_FROMRADIO])
        return -1;
    if (env->isDebugEnabled(LOG_DEBUG)) {
        std::stringstream ss;
        ss << _("Characteristic ") << MeshtasticString::characteristic2String(((MeshtasticDeviceBLE*) device)->characteristics[MESHTASTIC_BLE_PROPERTY_FROMRADIO]);
        ss << _(" read BLE ") << MeshtasticString::macAddress2string(device->hash())
            << " <" << (device->name.empty() ? "no name" : device->name) << ">";
        env->debugLog(LOG_DEBUG, ss.str());
    }

    try {
        // read buffer
        auto result =
                ((MeshtasticDeviceBLE*) device)->characteristics[MESHTASTIC_BLE_PROPERTY_FROMRADIO].ReadValueAsync(winrt::Windows::Devices::Bluetooth::BluetoothCacheMode::Uncached).get();
        if (result.Status() == winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success) {
            uint8_t *p = result.Value().data();
            uint32_t sz = result.Value().Length();
            if (sz)
                memmove(buffer, p, sz);
            return (int) sz;
        } else {
            return -1;
        }
    }
    catch (winrt::hresult_error const& ex) {
        if (ex.code() == RO_E_CLOSED) {
            // Server is no longer available.
        }
        return -1;
    }
}

int MeshtasticBLETransport::write(
    const MeshtasticDevice *device,
    const google::protobuf::Message *msg
)
{
    if (env->isDebugEnabled(LOG_DEBUG)) {
        std::stringstream ss;
        ss << _("Write to BLE device ") << MeshtasticString::macAddress2string(device->hash()) << " (" << device->name << "): " << msg->Utf8DebugString();
        env->debugLog(LOG_DEBUG, ss.str());
    }

    try {
        // BT_Code: Writes the value from the buffer to the characteristic.
        const std::string m = msg->SerializeAsString();
        auto sz = m.size();
        winrt::Windows::Storage::Streams::Buffer buffer(sz);
        memcpy(buffer.data(), m.c_str(), sz);
        buffer.Length(sz);

        // write buffer
        if (!((MeshtasticDeviceBLE*) device)->characteristics[MESHTASTIC_BLE_PROPERTY_TORADIO])
            return -1;
        auto result =
            ((MeshtasticDeviceBLE*) device)->characteristics[MESHTASTIC_BLE_PROPERTY_TORADIO].WriteValueWithResultAsync(buffer).get();
        if (result.Status() == winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success) {
            if (env->isDebugEnabled(LOG_DEBUG)) {
                std::stringstream ss;
                ss << _("Write to BLE device ") << device->name
                    << _(" service ")
                    << BLEGuid::ServiceUUIDToString(((MeshtasticDeviceBLE*) device)->characteristics[MESHTASTIC_BLE_PROPERTY_TORADIO].Service().Uuid())
                    << _(" characteristic ")
                    << BLEGuid::ServiceUUIDToString(((MeshtasticDeviceBLE*) device)->characteristics[MESHTASTIC_BLE_PROPERTY_TORADIO].Uuid())
                    << _(" successful ");
                env->debugLog(LOG_DEBUG, ss.str());
            }
            if (!((MeshtasticDeviceBLE*) device)->characteristics[MESHTASTIC_BLE_PROPERTY_FROMRADIO])
                return -1;
            auto rvc = ((MeshtasticDeviceBLE*) device)->characteristics[MESHTASTIC_BLE_PROPERTY_FROMRADIO].ReadValueAsync().get();
            if (rvc.Status() != winrt::Windows::Devices::Bluetooth::GenericAttributeProfile::GattCommunicationStatus::Success) {
                if (env->isDebugEnabled(LOG_ERR)) {
                    std::stringstream ss;
                    ss << _("Read after write to BLE device ") << device->name << " failed";
                    env->debugLog(LOG_INFO, ss.str());
                }
            }
            return 0;
        } else {
            if (env->isDebugEnabled(LOG_ERR)) {
                std::stringstream ss;
                ss << _("Error write to BLE device ") << device->name << " " << MeshtasticString::gattCommunicationStatus2string(result.Status());
                env->debugLog(LOG_INFO, ss.str());
            }
            return -1;
        }
    }
    catch (winrt::hresult_error const& ex) {
        if (ex.code() == RO_E_CLOSED) {
            // Server is no longer available.
        }
        return -1;
    }
}

int MeshtasticBLETransport::startDiscovery() {
    if (discoveryOn)
        return 0;

    // indicate discovery off
    std::unique_lock<std::mutex> lck(mutexDiscoveryState);
    MeshtasticTransport::startDiscovery();
    lck.unlock();

    discoveryToken = advWatcher.Received([this](const winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher &watcher, const winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementReceivedEventArgs &eventArgs) {
        uint64_t addr = eventArgs.BluetoothAddress();
        MeshtasticDiscoveryEvent de(nullptr, "", addr, eventArgs.RawSignalStrengthInDBm(), false);
        // Bluetooth Base UUID
        std::vector<winrt::guid> serviceUuids;

        for (winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataSection dataSection : eventArgs.Advertisement().DataSections()) {
            unsigned char *c = dataSection.Data().data();
            uint32_t len = dataSection.Data().Length();

            if (dataSection.DataType() == winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataTypes::CompleteService128BitUuids()
                || dataSection.DataType() == winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataTypes::IncompleteService128BitUuids()
            ) {
                size_t sz = len / 16;
                for (int i = 0; i < sz; i++) {
                    winrt::guid g;
                    BLEGuid::set(g, c, 16);
                    serviceUuids.push_back(g);
                    c += 16;
                }
            }
            if (dataSection.DataType() == winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataTypes::CompleteService32BitUuids()
                || dataSection.DataType() == winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataTypes::IncompleteService32BitUuids()) {
                size_t sz = len / 4;
                for (int i = 0; i < sz; i++) {
                    winrt::guid g;
                    BLEGuid::set(g, c, 4);
                    serviceUuids.push_back(g);
                    c += 4;
                }
            }
            if (dataSection.DataType() == winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataTypes::CompleteService16BitUuids()
                || dataSection.DataType() == winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataTypes::IncompleteService16BitUuids()) {
                size_t sz = len / 2;
                for (int i = 0; i < sz; i++) {
                    winrt::guid g;
                    BLEGuid::set(g, c, 2);
                    serviceUuids.push_back(g);
                    c += 2;
                }
            }
            if (dataSection.DataType() == winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataTypes::CompleteLocalName()) {
                c = dataSection.Data().data();
                len = dataSection.Data().Length();
                if (!len)
                    return 0;
                de.deviceName = std::string((const char *) c, len);
            }
            if (dataSection.DataType() == winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementDataTypes::ManufacturerSpecificData()) {
                c = dataSection.Data().data();
                len = dataSection.Data().Length();
                if (!len)
                    return 0;
                de.manufacturerSpecificData = std::string((const char *) c, len);
            }
        }

        // check does device already discovered.
        auto foundDevice = env->find(MTT_BLE, addr);
        if (foundDevice) {
            // copy device name from already discovered device
            de.deviceName = foundDevice->name;
            de.isMeshtasticDevice = true;
        } else {
            // check has device meshtastic service
            for (auto &uuid: serviceUuids) {
                de.isMeshtasticDevice = isServiceUUIDMeshtastic(&uuid);
                if (de.isMeshtasticDevice)
                    break;
            }
            // read name if is not provided in discovery
            if (de.deviceName.empty()) {
                // get device name from the common service
                de.deviceName = getDeviceName(addr);
            }
        }

        // log info
        if (env->isDebugEnabled(LOG_INFO)) {
            std::stringstream ss;
            ss << _("BLE device discovered ") << de.deviceAddressString() << " " << (de.deviceName.empty() ? _("<no name>") : de.deviceName);
            if (env->isDebugEnabled(LOG_DEBUG)) {
                for (auto &uuid: serviceUuids) {
                    ss << " " << BLEGuid::ServiceUUIDToString(uuid);
                }
            }
            ss << " " << std::dec << de.signalStrength << "dBm";
            env->debugLog(LOG_INFO, ss.str());
        }

        // Check service UUID
        if (!de.isMeshtasticDevice)
            return 0;

        if (!foundDevice) {
            if (env->isDebugEnabled(LOG_DEBUG)) {
                std::stringstream ss;
                printCharacteristics(ss, addr);
                env->debugLog(LOG_DEBUG, ss.str());
            }

            MeshtasticDeviceBLE *d = new MeshtasticDeviceBLE(this, addr, de.deviceName);
            if (!env->filterDevice.deviceAllowed(d)) {
                delete d;
                return 0;
            }
            if (!env->addDevice(d)) {
                delete d;
                return -1;
            }
            d->startListener();
            d->requestDeviceContext();
            // send event to inform
            env->fireEvent(MeshtasticEnvironmentEvent(env, ET_DEVICE_ADD, &de, this, nullptr, nullptr));
        }
        // send event to inform
        env->fireEvent(MeshtasticEnvironmentEvent(env, ET_DEVICE_DISCOVERED, &de, this, nullptr, nullptr));
        return 0;
    });

    stoppedDiscoveryToken = advWatcher.Stopped([this](const winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcher &watcher, const winrt::Windows::Devices::Bluetooth::Advertisement::BluetoothLEAdvertisementWatcherStoppedEventArgs &eventArgs) {
        // remove event handlers
        advWatcher.Received(discoveryToken);
        advWatcher.Stopped(stoppedDiscoveryToken);
        if (env->isDebugEnabled(LOG_INFO)) {
            env->debugLog(LOG_INFO, _("BLE discovery stopped"));
        }

        std::unique_lock<std::mutex> lck(mutexDiscoveryState);
        // indicate discovery off
        discoveryOn = false;
        lck.unlock();
        cvDiscoveryState.notify_all();
    });

    // winrt::Windows::Devices::Bluetooth::BluetoothAdapter adapter = winrt::Windows::Devices::Bluetooth::BluetoothAdapter::GetDefaultAsync().get();
    advWatcher.Start();

    return 0;
}

void MeshtasticBLETransport::stopDiscovery(
    int seconds
) {
    if (!discoveryOn)
        return;
    advWatcher.Stop();

    // wait
    std::unique_lock<std::mutex> lock(mutexDiscoveryState);
    cvDiscoveryState.wait_for(lock, std::chrono::seconds(seconds), [this] {
        return !discoveryOn;
    });
    MeshtasticTransport::stopDiscovery(seconds);
}
