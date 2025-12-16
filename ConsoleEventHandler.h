#ifndef CONSOLEEVENTHANDLER_H
#define CONSOLEEVENTHANDLER_H

#include <cinttypes>
#include <string>

#include "MeshtasticEnvironmentEventHandler.h"

/**
 * Just log events to the console output.
 */
class ConsoleEventHandler : public MeshtasticEnvironmentEventHandler {
public:
    bool processEvent(const MeshtasticEnvironmentEvent &event) override;
};


#endif
