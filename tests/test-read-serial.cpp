/**
 *  ./test-read-serial
 */

#include <iostream>
#include <sstream>
#include "MeshtasticDevice.h"
#include "MeshtasticOSEnvironment.h"
#include "MeshtasticSerialTransport.h"
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
    MeshtasticSerialTransport serialTransport(&env);

    env.filterDevice.transportNDeviceNames.emplace_back(MTT_SERIAL, "COM4");

    env.addTransport(&serialTransport);
    assert(env.transportCount() == 1);
    uint32_t deviceCount = env.waitDeviceConfigured(20);
    env.stopDiscovery(MTT_ANY, "");

    if (deviceCount <= 0) {
        std::cerr << " No device found" << std::endl;
        return 0;
    }

    std::cout << _("Press any key to stop read messages") << std::endl;
    auto c = std::cin.get();
}
