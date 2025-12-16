#ifndef MESHTASTIC_H
#define MESHTASTIC_H

#if defined(_MSC_VER) || defined(__MINGW32__)
#undef BAUD_110
#undef BAUD_300
#undef BAUD_600
#undef BAUD_1200
#undef BAUD_2400
#undef BAUD_4800
#undef BAUD_9600
#undef BAUD_19200
#undef BAUD_38400
#undef BAUD_57600
#undef BAUD_115200
#endif

#include "meshtastic/mesh.pb.h"

// i18n
// #include <libintl.h>
// #define _(String) gettext (String)
#define _(String) (String)

#define STR_BOOL(b) (b ? "true" : "false")

#define LOG_ERR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_DEBUG 4

enum MeshtasticListenerState {
    MLS_STOPPED,
    MLS_WAIT_ANY_MSG,
    MLS_WAIT_CONFIG,
    MLS_RUN,
    MLS_STOP
};

#endif
