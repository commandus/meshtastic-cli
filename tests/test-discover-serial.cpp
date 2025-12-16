/**
 *  ./test-discover-serial
 */

#include <iostream>
#include "MeshtasticDevice.h"
#include "MeshtasticOSEnvironment.h"
#include "MeshtasticSerialTransport.h"

int main(int argc, char **argv) {
    MeshtasticOSEnvironment env;
    MeshtasticSerialTransport serialTransport(&env);

    //
    // env.setDebugLog(3, &std::cout);
    // env.filterDevice.transportNDeviceNames.push_back({MTT_SERIAL, "COM3"});
    // env.filterDevice.transportNDeviceNames.push_back({MTT_SERIAL, "COM4"});

    env.addTransport(&serialTransport);
    assert(env.transportCount() == 1);

    uint32_t devicesRunning = env.waitDeviceConfigured(20);
    if (devicesRunning == 0) {
        std::cerr << "Device not found" << std::endl;
        return 0;
    }
    env.stopDiscovery(MTT_ANY, "");
    std::cout << "Found " << env.count() << " device(s)" << std::endl;
    for (int i = 0; i < env.count(); i++) {
        auto d = env.get(i);
        std::cout << "Device " << i + 1 << " " << d->name << std::endl;
        for (auto &n: d->context.nodes) {
            if (n.second.has_user())
                std::cout << std::hex << n.second.num() << ' '
                    << n.second.user().short_name() << " ("
                    << n.second.user().long_name() << ")"
                    << std::endl;
        }
    }
    return 0;
}
