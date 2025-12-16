#ifndef BLEGUID_H
#define BLEGUID_H

#include <string>

#ifdef _MSC_VER
#include <winrt/base.h>
#else
#endif


class BLEGuid {
public:
    static std::string ServiceUUIDToString(
        const winrt::guid &uuid
    );
    static std::string UUIDToString(
        const winrt::guid &uuid
    );
    static std::string UUID16ToString(
        uint32_t val
    );
    static void set(
        winrt::guid &uuid,
        void *buffer,
        size_t size
    );

};

#endif
