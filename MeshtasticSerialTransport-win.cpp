#include "MeshtasticSerialTransport-win.h"

#include <chrono>

#include "MeshtasticTypes.h"
#include "MeshtasticDeviceSerial-win.h"
#include "MeshtasticEnvironmentEvent.h"

#define COM_PORT_NAME_PREFIX    "COM"

void MeshtasticSerialTransport::listPorts(
    std::vector<SerialPortId> &ports
) {
    int bufferSize = 4096;
    bool bWantStop = false;
    while (bufferSize && !bWantStop) {
        std::string devices;
        devices.resize(bufferSize);
        auto chars= QueryDosDeviceA(nullptr, (LPSTR) devices.c_str(), bufferSize);
        if (chars == 0) {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
                bufferSize *= 2;    // Expand the buffer
                continue;
            } else
                return;    // fatal error
        }
        devices.resize(chars);
        bWantStop = true;
        const char* device = devices.c_str();
        while (*device) {
            auto len = strlen(device);
            if (strncmp(device, COM_PORT_NAME_PREFIX, 3) == 0) {
                ports.push_back(atoi(device + 3));
            }
            device += len + 1;
        }
    }
    // sort ascending
    std::sort(ports.begin(), ports.end());
}

std::string MeshtasticSerialTransport::portName(
    SerialPortId port
)
{
    return "COM" + std::to_string(port);
}

bool MeshtasticSerialTransport::probeIsPortMeshtastic(
    const SerialPortId &port
) {
    MeshtasticDeviceSerial device(this, port);
    int r = openDevice(&device);
    if (r)
        return false;
    // try to write smth and wait response
    bool c = ping(device);
    r = closeDevice(&device);
    return (c && r == 0);
}

MeshtasticSerialTransport::MeshtasticSerialTransport(
    MeshtasticEnvironment *env
)
    : MeshtasticTransport(env, MTT_SERIAL)
{

}

int MeshtasticSerialTransport::startTransport() {
    int r = MeshtasticTransport::startTransport();
    if (r)
        return r;
    return startDiscovery();
}

void MeshtasticSerialTransport::stopTransport() {
    stopDiscovery();
    MeshtasticTransport::stopTransport();
}

int MeshtasticSerialTransport::startDiscovery() {
    int r = MeshtasticTransport::startDiscovery();
    int found = 0;
    if (r == 0) {
        std::vector<uint8_t> ports;
        listPorts(ports);
        // send event to inform
        for (auto port : ports) {
            MeshtasticDiscoveryEvent de(nullptr, portName(port), port, 0, false);

            MeshtasticDeviceSerial *d = new MeshtasticDeviceSerial(this, port, de.deviceName);

            if (!env->filterDevice.deviceAllowed(d)) {
                delete d;
                continue;
            }

            if (openDevice(d) < 0)
                continue;
            env->fireEvent(MeshtasticEnvironmentEvent(env, ET_DEVICE_DISCOVERED, &de, this, nullptr, nullptr));

            d->startListener();
            if (!d->context.configComplete) {
                d->requestDeviceContext();
                if (!d->waitState(MLS_WAIT_CONFIG, 1000)) {
                    delete d;
                    continue;
                }
                if (!d->waitState(MLS_RUN, 15000)) {
                    delete d;
                    continue;
                }
            }
            if (!env->addDevice(d)) {
                delete d;
                continue;
            }

            found++;
            de.isMeshtasticDevice = true;
            env->fireEvent(MeshtasticEnvironmentEvent(env, ET_DEVICE_ADD, &de, this, nullptr, nullptr));
        }
    }
    stopDiscovery();
    return found;
}

void MeshtasticSerialTransport::stopDiscovery(
    int seconds
)
{
    MeshtasticTransport::stopDiscovery(seconds);
    env->fireEvent(MeshtasticEnvironmentEvent(env, ET_DISCOVERY_OFF, nullptr, this, nullptr, nullptr));
}

int MeshtasticSerialTransport::openDevice(
    MeshtasticDevice *device
) {
    MeshtasticDeviceSerial* sd = (MeshtasticDeviceSerial*) device;
    if (sd->handle != INVALID_HANDLE_VALUE)
        return 0;   // already opened
    sd->handle = CreateFile(sd->deviceName().c_str(),GENERIC_READ | GENERIC_WRITE,0, nullptr, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,nullptr);
    if (sd->handle == INVALID_HANDLE_VALUE)
        return -1;
    COMMTIMEOUTS cto;
    GetCommTimeouts(sd->handle, &cto);
    // Set the timeouts
    cto.ReadIntervalTimeout = 50;          // Max time between two bytes (ms)
    cto.ReadTotalTimeoutMultiplier = 10;   // Multiplier per byte (ms)
    cto.ReadTotalTimeoutConstant = 100;    // Constant time-out (ms)
    cto.WriteTotalTimeoutMultiplier = 10;  // Multiplier per byte (ms)
    cto.WriteTotalTimeoutConstant = 50;    // Constant time-out (ms)
    SetCommTimeouts(sd->handle, &cto);

    return 0;
}

int MeshtasticSerialTransport::closeDevice(
    MeshtasticDevice *device
) {
    MeshtasticDeviceSerial* sd = (MeshtasticDeviceSerial*) device;
    if (sd == INVALID_HANDLE_VALUE)
        return 0;   // already closed
    CloseHandle(sd->handle);
    sd->handle = nullptr;
    return 0;
}

int MeshtasticSerialTransport::write(
    const MeshtasticDevice *device,
    const google::protobuf::Message *msg
) {
    const std::string m = msg->SerializeAsString();
    auto sz = m.size();
    DWORD written;

    MESHTASTIC_HEADER h(sz);
    if (!WriteFile(((MeshtasticDeviceSerial*) device)->handle, &h.v.c, MESHTASTIC_HEADER_SIZE, &written, nullptr))
        return -1;
    if (written != MESHTASTIC_HEADER_SIZE)
        return -1;

    if (!WriteFile(((MeshtasticDeviceSerial*) device)->handle, m.c_str(), sz, &written, nullptr))
        return -1;
    return 0;
}

#define MESHTASTIC_MAGIC_NUMBER_1 0x94
#define MESHTASTIC_MAGIC_NUMBER_2 0xC3

/**
 * Read until header has been found, skip header, try read packet. If fail, read header again etc.
 * @param device
 * @param buffer  at least 512 bytes long
 * @param bufferSize at least 512 bytes
 * @param timeOutMs
 * @return <0 - error, >=0- count of read bytes
 */
int MeshtasticSerialTransport::read(
    const MeshtasticDevice *device,
    void *buffer,
    uint16_t bufferSize,
    int timeOutMs
) {
    uint8_t *p = (uint8_t*) buffer;
    DWORD cnt;
    auto start = std::chrono::system_clock::now();
    while (true) {
        // read first magic number
        uint8_t b;
        if (!ReadFile(((MeshtasticDeviceSerial *) device)->handle, &b, 1, &cnt, nullptr))
            return -1;

        // check timeout
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::system_clock::now() - start);
        if (elapsed.count() > timeOutMs)
            return -1;

        if (cnt == 0 || b != MESHTASTIC_MAGIC_NUMBER_1)
            continue;
        if (!ReadFile(((MeshtasticDeviceSerial *) device)->handle, &b, 1, &cnt, nullptr))
            return -1;
        if (cnt == 0 || b != MESHTASTIC_MAGIC_NUMBER_2)
            continue;

        uint16_t len;
        if (!ReadFile(((MeshtasticDeviceSerial *) device)->handle, &len, sizeof(uint16_t), &cnt, nullptr))
            return -1;
        if (cnt != sizeof(uint16_t))
            continue;
#if IS_BIG_ENDIAN
#else
        len = SWAP_BYTES_2(len);
#endif
        if (len > bufferSize)
            continue;   // invalid size in the header, ignore
        if (len == 0)
            return 0;   // empty message
        // try read message
        if (!ReadFile(((MeshtasticDeviceSerial *) device)->handle, p, len, &cnt, nullptr))
            return -1;
        if (cnt == len)
            return len; // successfully read
        // buffer now can contain valid header and message.
        // It happens if device log output contains magic numbers.
    }
}

void MeshtasticSerialTransport::debugLog(
    const void *buffer,
    uint16_t size,
    const MeshtasticDevice *device
) {
    std::string s((const char *) buffer, (size_t) size);
    auto p = s.find('\n');
    if (p != std::string::npos) {
        while (p != std::string::npos) {
            std::string ss = debugLogBuffer[device] + s.substr(0, p);
            env->debugLog(3, ss, device);
            s = s.substr(p);
            while (!s.empty())
                if (s[0] < 32)
                    s.erase(0);
            debugLogBuffer[device] = s;
            p = s.find('\n');
        }
    }
    if (!s.empty())
        debugLogBuffer[device] += s;
}
