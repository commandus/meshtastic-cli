//
// Created by andrei on 18.12.2025.
//

#include "MeshtasticClient.h"

int MeshtasticClient::start() {
    MeshtasticBLETransport bleTransport(&env);

    // env.setDebugLog(3, &std::cout);

    /*
    ConsolePinCodeProvider consolePinCodeProvider;
    env.pinCodeProvider = &consolePinCodeProvider;

    // env.filterDevice.transportNDeviceNames.emplace_back(MTT_BLE, "9c:13:9e:a0:b7:5d");

    env.addTransport(&bleTransport);
    assert(env.transportCount() == 1);

    uint32_t devicesRunning = env.waitDeviceConfigured(20);
    if (devicesRunning == 0) {
        std::cerr << "Device not found" << std::endl;
        return 0;
    }

    env.stopDiscovery(MTT_ANY, "");
     */
    return 0;
}
