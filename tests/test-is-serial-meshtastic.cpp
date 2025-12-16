/**
 *  ./test-is-serial-meshtastic
 */

#include <iostream>
#include "MeshtasticDevice.h"
#include "MeshtasticOSEnvironment.h"
#include "MeshtasticSerialTransport.h"

int main(int argc, char **argv) {
    MeshtasticOSEnvironment env;

    MeshtasticSerialTransport serialTransport(&env);
    std::vector <SerialPortId> ports;
    MeshtasticSerialTransport::listPorts(ports);
    for (auto p : ports) {
        std::cout << MeshtasticSerialTransport::portName(p) << " "
            << (serialTransport.probeIsPortMeshtastic(p) ? _("meshtastic") : "-")
            << std::endl;
    }
    return 0;
}
