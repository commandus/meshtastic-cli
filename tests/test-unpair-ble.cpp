/**
 *  ./test-unpair-ble
 */

#include <iostream>
#include <sstream>
#include "MeshtasticOSEnvironment.h"
#include "MeshtasticDeviceBLE.h"
#include "MeshtasticBLETransport.h"
#include "MeshtasticEnvironmentEventHandler.h"
#include "MeshtasticString.h"

class MyEventHandler : public MeshtasticEnvironmentEventHandler {
public:
    bool processEvent(
            const MeshtasticEnvironmentEvent &event
    ) override
    {
        switch (event.eventType) {
            case ET_DISCOVERY_OFF:
                std::cout << _("Discovery off") << std::endl;
                break;
            case ET_DISCOVERY_ON:
                std::cout << _("Discovery on") << std::endl;
                break;
            case ET_DEVICE_DISCOVERED: {
                std::string d = event.de->isMeshtasticDevice ? _("Meshtastic ") : "";
                std::cout << d << _("Device discovered ") << event.de->deviceAddressString() << ' ' << event.de->deviceName
                          << ' ' << event.de->signalStrength << "dBm" << std::endl;
                if (event.de->isMeshtasticDevice) {
                }
            }
                break;
            case ET_DEVICE_FROM_RADIO:
                std::cout << _("From radio  ") << event.de->deviceAddressString() << ' ' << event.de->deviceName
                          << ' ' << event.de->signalStrength << std::endl;
                if (event.msg) {
                    auto *f = (meshtastic::FromRadio *) event.msg;
                    if (f->has_packet()) {
                        if (f->packet().has_decoded()) {
                            auto p = f->packet().decoded().portnum();
                            if (p == meshtastic::TEXT_MESSAGE_APP) {
                                std::cout << f->packet().decoded().payload() << std::endl;
                            }
                        }
                    }
                }
                break;
            case ET_DEVICE_TO_RADIO:
                std::cout << _("To radio  ") << event.de->deviceAddressString() << ' ' << event.de->deviceName
                          << ' ' << event.de->signalStrength << std::endl;
                break;
            case ET_DEVICE_ADD:
            {
                std::string d = event.de->isMeshtasticDevice ? _("Meshtastic ") : "";
                std::cout << d << _("Device configured ") << event.de->deviceAddressString() << ' ' << event.de->deviceName
                          << ' ' << event.de->signalStrength << "dBm" << std::endl;
                // stop discovery if necessary
                // env.stopDiscovery(MTT_BLE, "");
            }
                break;
            case ET_DEVICE_RM:
                break;
            case ET_DEVICE_LOG:
                break;
        }
        return false;
    }
};

int main(int argc, char **argv) {
    MeshtasticOSEnvironment env;
    MyEventHandler eh;
    env.addEventHandler(&eh);
    // env.setDebugLog(3, &std::cerr);
    MeshtasticBLETransport bleTransport(&env);

    std::string a("9c:13:9e:a0:b7:5d");
    env.filterDevice.transportNDeviceNames.emplace_back(MTT_BLE, a);

    env.addTransport(&bleTransport);
    assert(env.transportCount() == 1);

    auto d = (MeshtasticDeviceBLE *) env.find(MTT_BLE, 0x9c139ea0b75d);
    if (!d) {
        std::cerr << _("No device ") << a << _(" found") << std::endl;
        return 0;
    }

    uint32_t devicesRunning = env.waitDeviceConfigured(20);
    if (devicesRunning == 0) {
        std::cout << _("Device ") << a << _(" not found") << std::endl;
        return 0;
    }

    env.stopDiscovery(MTT_ANY, "");

    auto t = (MeshtasticBLETransport *) d->transport;
    int r = t->unpair(d);
    if (r) {
        std::cerr << _("Unpairing error ") << r << std::endl;
        return r;
    }
    std::cout << _("Device ") << d->name << _(" has been successfully unpaired") << std::endl;
    env.stopTransport(MTT_ANY);
}
