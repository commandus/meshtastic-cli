#ifndef MESHTASTICENVIRONMENTEVENTHANDLER_H
#define MESHTASTICENVIRONMENTEVENTHANDLER_H

#include "MeshtasticEnvironmentEvent.h"

class MeshtasticEnvironmentEvent;

/**
 * Transport of the environment fire event.
 * Environment pass event to registered event handlers one by one in the loop.
 * Event handler can interrupt loop by returning true. That means no more event handlers required.
 */
class MeshtasticEnvironmentEventHandler {
public:
    /**
     *
     * @param event
     * @return false - continue, pass event to the next event handler (if exists). True- stop queue of event handlers.
     */
    virtual bool processEvent(const MeshtasticEnvironmentEvent &event) = 0;
};

#endif
