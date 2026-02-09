#include <iostream>
#include <sstream>

#include "MeshtasticEnvironment.h"

#include "MeshtasticEnvironmentEventHandler.h"
#include "MeshtasticBLETransport-win.h"

MeshtasticEnvironment::MeshtasticEnvironment()
    : logStream(nullptr), verbosity(0), pinCodeProvider(nullptr)
{
}

MeshtasticEnvironment::~MeshtasticEnvironment() {
    setDebugLog(0, nullptr);
    clearEventHandlers();
    clearDevices();
    clearTransport();
}

void MeshtasticEnvironment::startTransport(
    enum MeshtasticTransportType typ,
    const std::string &name
)
{
    switch (typ) {
        case MTT_BLE: {
            auto t = new MeshtasticBLETransport(this);
            addTransport(t);
            t->startTransport();
        }
            break;
        case MTT_SERIAL:
            break;
        case MTT_WIFI:
            break;
        default:
            break;
    }
}

void MeshtasticEnvironment::stopTransport(
    enum MeshtasticTransportType typ,
    const std::string &name
)
{
    switch (typ) {
        case MTT_ANY:
            rmTransport(MTT_BLE);
            break;
        case MTT_BLE:
            rmTransport(typ);
            break;
        case MTT_SERIAL:
            break;
        case MTT_WIFI:
            break;
        default:
            break;
    }
}

MeshtasticDevice *MeshtasticEnvironment::get(size_t index) {
    MeshtasticDevice *r = nullptr;
    mutexDevice.lock();
    if (index < devices.size())
        r = devices[index];
    mutexDevice.unlock();
    return r;
}

MeshtasticDevice *MeshtasticEnvironment::find(
    MeshtasticTransport *aTransport
)
{
    MeshtasticDevice *r = nullptr;
    mutexDevice.lock();
    const auto it = std::find_if(devices.begin(), devices.end(), [aTransport](auto d) {
        return d->transport == aTransport;
    });
    if (it != devices.end())
        r = *it;
    mutexDevice.unlock();
    return r;
}

MeshtasticDevice *MeshtasticEnvironment::find(
    MeshtasticDevice *device
) {
    if (!device)
        return nullptr;
    return find(device->transport->transportType, device->hash());
}

MeshtasticDevice *MeshtasticEnvironment::find(
    enum MeshtasticTransportType aTyp
) {
    MeshtasticDevice *r = nullptr;
    mutexDevice.lock();
    const auto it = std::find_if(devices.begin(), devices.end(), [aTyp](auto d) {
        return ((d->transport->transportType == aTyp) || aTyp == MTT_ANY);
    });
    if (it != devices.end())
        r = *it;
    mutexDevice.unlock();
    return r;
}

MeshtasticDevice *MeshtasticEnvironment::find(
    enum MeshtasticTransportType aTyp,
    uint64_t aHash
) {
    MeshtasticDevice *r = nullptr;
    mutexDevice.lock();
    const auto it = std::find_if(devices.begin(), devices.end(), [aHash, aTyp](auto d) {
        return ((d->transport->transportType == aTyp) || aTyp == MTT_ANY) && (aHash == 0 || d->hash() == aHash);
    });
    if (it != devices.end())
        r = *it;
    mutexDevice.unlock();
    return r;
}

MeshtasticDevice *MeshtasticEnvironment::find(
    enum MeshtasticTransportType aTyp,
    const std::string &aName
) {
    MeshtasticDevice *r = nullptr;
    mutexDevice.lock();
    const auto it = std::find_if(devices.begin(), devices.end(), [aName, aTyp](auto d) {
        return ((d->transport->transportType == aTyp) || aTyp == MTT_ANY) && ((d->name == aName) || aName == "");
    });
    if (it != devices.end())
        r = *it;
    mutexDevice.unlock();
    return r;
}

size_t MeshtasticEnvironment::count() {
    mutexDevice.lock();
    auto r = devices.size();
    mutexDevice.unlock();
    return r;
}

bool MeshtasticEnvironment::addDevice(
    MeshtasticDevice *device
) {
    auto d = find(device);
    mutexDevice.lock();
    bool r = false;
    if (!d) {
        if (device->transport->openDevice(device) == 0) {
            devices.push_back(device);
            r = true;
        }
    }
    mutexDevice.unlock();
    if (!device->context.configComplete)
        device->requestDeviceContext();
    return true;
}

void MeshtasticEnvironment::rmDevice(
    MeshtasticDevice *device
) {
    const auto it = std::find(devices.begin(), devices.end(), device);
    mutexDevice.lock();
    if (it == devices.end())
        return;
    delete device;
    devices.erase(it);
    mutexDevice.unlock();
}

void MeshtasticEnvironment::rmDevice(
    size_t index
) {
    mutexDevice.lock();
    if (index >= devices.size())
        return;
    delete devices[index];
    devices.erase(devices.begin() + index);
    mutexDevice.unlock();
}

void MeshtasticEnvironment::clearDevices() {
    mutexDevice.lock();
    for (auto &d : devices) {
        delete d;
    }
    devices.clear();
    mutexDevice.unlock();
}

void MeshtasticEnvironment::addTransport(
    MeshtasticTransport *transport
) {
    transport->env = this;
    transports.push_back(transport);
    transport->startTransport();
}

MeshtasticTransport *MeshtasticEnvironment::getTransport(
    size_t index
) {
    if (index >= transports.size())
        return nullptr;
    return transports[index];
}

size_t MeshtasticEnvironment::transportCount() {
    return transports.size();
}

void MeshtasticEnvironment::rmTransport(
    size_t index
) {
    if (index >= transports.size())
        return;
    delete transports[index];
    transports.erase(transports.begin() + index);
}

void MeshtasticEnvironment::rmTransport(
    MeshtasticTransport *transport
) {
    const auto it = std::find(transports.begin(), transports.end(), transport);
    if (it == transports.end())
        return;
    delete transport;
    transports.erase(it);
}

void MeshtasticEnvironment::rmTransport(
    enum MeshtasticTransportType typ
) {
    auto it = std::find_if(transports.begin(), transports.end(), [typ] (MeshtasticTransport *t) {
        return t->transportType == typ || typ == MTT_ANY;
    });
    while (it != transports.end()) {
        MeshtasticTransport *tt = *it;
        delete tt;
        transports.erase(it++);
    }
}

void MeshtasticEnvironment::clearTransport() {
    transports.clear();
}

MeshtasticTransport *MeshtasticEnvironment::findFirstTransport(
    enum MeshtasticTransportType typ,
    const std::string &name
) {
    const auto it = std::find_if(transports.begin(), transports.end(), [typ, name](MeshtasticTransport *t) {
        return typ == MTT_ANY || t->transportType == typ;
    });
    if (it == transports.end())
        return nullptr;
    return *it;
}

void MeshtasticEnvironment::addEventHandler(
    MeshtasticEnvironmentEventHandler *handler
) {
    mutexHandler.lock();
    eventHandlers.push_back(handler);
    mutexHandler.unlock();
}

void MeshtasticEnvironment::rmEventHandler(
    MeshtasticEnvironmentEventHandler *handler
)
{
    auto it = std::find(eventHandlers.begin(), eventHandlers.end(), handler);
    mutexHandler.lock();
    if (it != eventHandlers.end()) {
        eventHandlers.erase(it);
    }
    mutexHandler.unlock();
}

void MeshtasticEnvironment::clearEventHandlers()
{
    mutexHandler.lock();
    eventHandlers.clear();
    mutexHandler.unlock();
}

size_t MeshtasticEnvironment::fireEvent(
    const MeshtasticEnvironmentEvent &event
) {
    mutexHandler.lock();
    size_t sz = 0;
    for (auto h : eventHandlers) {
        if (h->processEvent(event))
            break;
        sz++;
    }
    mutexHandler.unlock();

    std::unique_lock<std::mutex> lck2(mutexEvent);
    lck2.unlock();
    cvEvent.notify_all();
    return sz;
}

uint32_t MeshtasticEnvironment::waitDeviceConfigured(
    int seconds
)
{
    std::unique_lock<std::mutex> lock(mutexEvent);
    cvEvent.wait_for(lock, std::chrono::seconds(seconds), [this] {
        auto c = discoveredDeviceCount();
        return c > 0 && configuredRunningDeviceCount() == c;
    });
    return configuredRunningDeviceCount();
}

bool MeshtasticEnvironment::isDebugEnabled(
    int verbose
) {
    // std::cout << "isDebugEnabled " << verbose << " " << tag() << std::endl;
    return (verbosity >= verbose && logStream);
}

void MeshtasticEnvironment::debugLog(
    int verbose,
    const std::string &line,
    const MeshtasticDevice *device
) {
    if (logStream) {
        if (device) {
            if (device->name.empty())
                *logStream << device->addressAsString();
            else
                *logStream << device->name;
            *logStream << ": ";
        }
        *logStream << line << std::endl;
    }
}

void MeshtasticEnvironment::setDebugLog(
    int aVerbosity,
    std::ostream *strm
) {
    logStream = strm;
    if (!logStream)
        verbosity = 0;  // none
    else
        verbosity = aVerbosity;
}

int MeshtasticEnvironment::startDiscovery(
    enum MeshtasticTransportType typ,
    const std::string &name
) {
    auto t = findFirstTransport(typ, name);
    if (t)
        return t->startDiscovery();
    return -1;
}

void MeshtasticEnvironment::stopDiscovery(
    enum MeshtasticTransportType typ,
    const std::string &name
) {
    auto t = findFirstTransport(typ, name);
    if (t)
        t->stopDiscovery();
    // wait until discovery has been stopped
}

uint32_t MeshtasticEnvironment::sendString(
    const std::vector<uint32_t > &nodeNums,
    const std::string &value,
    const MeshtasticDevice* device
)
{
    uint32_t r = 0;
    for (auto &n : nodeNums) {
        r = sendString(n, value, device);
    }
    return r;   // last message id
}

uint32_t MeshtasticEnvironment::sendString(
    uint32_t nodeNum,
    const std::string &value,
    const MeshtasticDevice* device
)
{
    const MeshtasticDevice *d = (device ? device : find(MTT_ANY));
    if (!d)
        return 0;
    google::protobuf::Message *msg;
    auto itNode = std::find_if(d->context.nodes.begin(), d->context.nodes.end(), [nodeNum] (const std::pair<uint32_t, const meshtastic::NodeInfo&> &pair) {
        return pair.second.num() == nodeNum;
    });
    if (itNode == d->context.nodes.end())
        return 0;
    uint32_t id = MeshtasticMessage::msgSendText(&msg, value, itNode->second.num(), meshtastic::TEXT_MESSAGE_APP);
    int r = d->transport->write(d, msg);
    delete msg;
    if (r < 0)
        return 0;
    return id;
}

int MeshtasticEnvironment::waitMessage(
    MeshtasticDevice *device,
    google::protobuf::Message **retMsg,
    uint32_t nodeNumFrom,
    uint32_t nodeNumTo,
    uint32_t id,
    meshtastic::PortNum port,
    int timeoutMs
) {
    bool k = true;
    while (k) {
        MeshtasticMessageType t = device->readMessage(retMsg, timeoutMs);
        if (t == MMT_UNKNOWN)
            return -1;  // timeout
        if (t == MMT_FROM_RADIO) {
            auto *f = (meshtastic::FromRadio *) *retMsg;
            // check from, to, port numbers
            if ((nodeNumFrom == 0 || nodeNumFrom == f->packet().from())
                && (nodeNumTo == 0 || nodeNumTo == f->packet().to())
                && (port == meshtastic::UNKNOWN_APP || port == f->packet().decoded().portnum())
                && (id == 0 || id == f->packet().decoded().request_id())) {
                return 0;
            }
        }
        delete *retMsg;
    }
    return -1;
}

void MeshtasticEnvironment::deviceConfigured(
    const MeshtasticDevice *device
) {
    if (isDebugEnabled(LOG_INFO)) {
        std::stringstream ss;
        ss << _("Device ") << device->name << " successfully configured.";
        debugLog(LOG_INFO, ss.str(), device);
    }
}

uint32_t MeshtasticEnvironment::configuredRunningDeviceCount() {
    uint32_t r = 0;
    for (auto &d : devices) {
        if (d->state == MLS_RUN)
            r++;
    }
    return r;
}

uint32_t MeshtasticEnvironment::discoveredDeviceCount() {
    return (uint32_t) devices.size();
}

uint64_t MeshtasticEnvironment::hash() const {
    return (uint64_t) this;
}

uint32_t MeshtasticEnvironment::tag() const {
    HASH_TYPE u64_2;
    u64_2.v = hash();
    return u64_2.l;
}
