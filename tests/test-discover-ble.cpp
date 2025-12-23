/**
 *  ./test-discover-ble
 */

#include <iostream>

#include "MeshtasticOSEnvironment.h"
#include "MeshtasticDeviceBLE.h"
#include "MeshtasticBLETransport.h"
#include "ConsolePinCodeProvider.h"

int main(int argc, char **argv) {
    MeshtasticOSEnvironment env;
    MeshtasticBLETransport bleTransport(&env);

    // env.setDebugLog(3, &std::cout);

    ConsolePinCodeProvider consolePinCodeProvider;
    env.pinCodeProvider = &consolePinCodeProvider;

    env.filterDevice.transportNDeviceNames.emplace_back(MTT_BLE, "9c:13:9e:a0:b7:5d");

    env.addTransport(&bleTransport);
    assert(env.transportCount() == 1);

    uint32_t devicesRunning = env.waitDeviceConfigured(20);
    if (devicesRunning == 0) {
        std::cerr << "Device not found" << std::endl;
        return 0;
    }

    env.stopDiscovery(MTT_ANY, "");
    for (int i = 0; i < env.count(); i++) {
        auto d = env.get(i);
        std::cout << "Device " << i + 1 << " " << d->name << " " << ((MeshtasticDeviceBLE* ) d)->addressAsString() << std::endl;
        for (auto &n: d->context.nodes) {
            if (n.second.has_user())
                std::cout << std::hex << n.second.num() << ' '
                    << n.second.user().short_name() << " ("
                    << n.second.user().long_name() << ")\n";
        }
        std::cout << std::endl;
    }
    return 0;
}
