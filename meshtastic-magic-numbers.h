#ifndef MESHTASTIC_MAGIC_NUMBERS_H
#define MESHTASTIC_MAGIC_NUMBERS_H

#include "MeshtasticTypes.h"

#if IS_BIG_ENDIAN
#define MESHTASTIC_MAGIC_NUMBER 0x94c3
#else
#define MESHTASTIC_MAGIC_NUMBER 0xc394
#endif

// Special "default" channel 128 bit (16 bytes long) PSK key
#define DEF_PSK_1 = { 0xd4, 0xf1, 0xbb, 0x3a, 0x20, 0x29, 0x07, 0x59, 0xf0, 0xbc, 0xff, 0xab, 0xcf, 0x4e, 0x69, 0x01 };
#define DEF_PSK_STR_1 = "d4f1bb3a20290759f0bcffabcf4e6901";

#endif
