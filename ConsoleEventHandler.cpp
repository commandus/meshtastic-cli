#include <iostream>
#include "ConsoleEventHandler.h"

bool ConsoleEventHandler::processEvent(
    const MeshtasticEnvironmentEvent &event
)
{
    switch (event.eventType) {
        case ET_DISCOVERY_OFF:
            std::cout << "Discovery off" << std::endl;
            break;
        case ET_DISCOVERY_ON:
            std::cout << "Discovery on" << std::endl;
            break;
        case ET_DEVICE_DISCOVERED: {
            std::string d = event.de->isMeshtasticDevice ? "Meshtastic " : "";
            std::cout << d << "BLE device discovered " << event.de->deviceAddressString() << ' ' << event.de->deviceName
                << ' ' << event.de->signalStrength << "dBm" << std::endl;
        }
            break;
        case ET_DEVICE_FROM_RADIO:
            std::cout << "From radio  " << event.de->deviceAddressString() << ' ' << event.de->deviceName
                      << ' ' << event.de->signalStrength << std::endl;
            break;
        case ET_DEVICE_TO_RADIO:
            std::cout << "To radio  " << event.de->deviceAddressString() << ' ' << event.de->deviceName
                      << ' ' << event.de->signalStrength << std::endl;
            break;
        case ET_DEVICE_ADD:
            break;
        case ET_DEVICE_RM:
            break;
        case ET_DEVICE_LOG:
            break;
    }
    return false;
}
