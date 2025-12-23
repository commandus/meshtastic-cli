#ifndef MESHTASTICCLIENT_H
#define MESHTASTICCLIENT_H


#include "MeshtasticOSEnvironment.h"
#include "MeshtasticBLETransport.h"

class MeshtasticClient {
public:
    MeshtasticOSEnvironment env;
    // MeshtasticBLETransport * transport[2];
    MeshtasticClient();
    int start();
    int stop();
};

#endif
