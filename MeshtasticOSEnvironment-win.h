#ifndef MESHTASTICOSENVIRONMENT_WIN_H
#define MESHTASTICOSENVIRONMENT_WIN_H

#include "MeshtasticEnvironment.h"

/**
 * initializes the thread for Windows Runtime (WinRT) operations.
 * Sets up a multi-threaded apartment (MTA) and initializes COM, making it essential for using WinRT APIs.
 */
class MeshtasticOSEnvironment : public MeshtasticEnvironment{
public:
    MeshtasticOSEnvironment();
    virtual ~MeshtasticOSEnvironment();
};

#endif
