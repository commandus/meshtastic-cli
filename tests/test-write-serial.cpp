/**
 *  ./test-write-serial
 */

#include <iostream>
#include <sstream>
#include "MeshtasticDevice.h"
#include "MeshtasticOSEnvironment.h"
#include "MeshtasticSerialTransport.h"
#include "MeshtasticString.h"

int main(int argc, char **argv) {
    MeshtasticOSEnvironment env;
    env.setDebugLog(3, &std::cerr);
    MeshtasticSerialTransport serialTransport(&env);

    env.filterDevice.transportNDeviceNames.push_back({MTT_SERIAL, "COM4"});
    uint32_t a = 0x9aa9259d;

    env.addTransport(&serialTransport);
    assert(env.transportCount() == 1);
    int deviceCount = env.waitDeviceConfigured(20);
    env.stopDiscovery(MTT_ANY, "");

    if (deviceCount <= 0) {
        std::cerr << " No device found" << std::endl;
        return 0;
    }
    for (int i = 0; i < env.count(); i++) {
        auto d = env.get(i);
        std::stringstream ss;
        ss << "Device " << i + 1 << " " << d->name << " " << MeshtasticString::currentTimeStamp();
        uint32_t id = env.sendString(a, ss.str(), d);
        uint32_t deviceNode = d->context.myNodeInfo.my_node_num();
        std::cout << "Message #" << id << " sent from " << deviceNode << " to " << a << std::endl;
        google::protobuf::Message *ackNak;
        // wait until message has been sent
        int r = env.waitMessage(d, &ackNak, a, deviceNode, id, meshtastic::ROUTING_APP, 30000);
        if (r == 0) {
            meshtastic::FromRadio *f = (meshtastic::FromRadio *) ackNak;
            std::cout << "Message #" << id << " delivered to " << a
                << " port " << f->packet().decoded().portnum()
                << std::endl;
            meshtastic::Routing *rr = new meshtastic::Routing;
            if (!rr->ParseFromString(f->packet().decoded().payload())) {
                std::cerr << "Error: wrong response" << std::endl;
            } else {
                std::cout << "Sent error: " << MeshtasticString::routingError2string(rr->error_reason()) << std::endl;
            }
            delete ackNak;
        }
    }
    return 0;
}
