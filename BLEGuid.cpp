#include <iomanip>
#include <sstream>

#include "BLEGuid.h"
#include "MeshtasticTypes.h"

static const char ADDR_DELIMITER = ':';
static const char UUID_DELIMITER = '-';
static const char NUMBER_FILL = '0';

// Prefix for 16bit Bluetooth address
static const winrt::guid bluetoothBaseUUID {0, 0, 0x1000, { 0x80, 0x00, 0x00, 0x80, 0x5f, 0x9b, 0x34, 0xfb } };

std::string BLEGuid::ServiceUUIDToString(
    const winrt::guid &uuid
)
{
    if (uuid.Data2 == 0 && uuid.Data3 == 0x1000
        && uuid.Data4[0] == 0x80 && uuid.Data4[1] == 0x00 && uuid.Data4[2] == 0x00 && uuid.Data4[3] == 0x80
        && uuid.Data4[4] == 0x5f && uuid.Data4[5] == 0x9b && uuid.Data4[6] == 0x34 && uuid.Data4[7] == 0xfb)
        return UUID16ToString(uuid.Data1);
    return UUIDToString(uuid);
}

std::string BLEGuid::UUIDToString(
    const winrt::guid &uuid
)
{
    std::stringstream ss;
    ss << std::hex << std::setfill(NUMBER_FILL)
       << std::setw(8) << uuid.Data1 << UUID_DELIMITER
       << std::setw(4) << uuid.Data2 << UUID_DELIMITER
       << std::setw(4) << uuid.Data3 << UUID_DELIMITER
       << std::setw(2) << (int) uuid.Data4[0]
       << std::setw(2) << (int) uuid.Data4[1] << UUID_DELIMITER
       << std::setw(2) << (int) uuid.Data4[2]
       << std::setw(2) << (int) uuid.Data4[3]
       << std::setw(2) << (int) uuid.Data4[4]
       << std::setw(2) << (int) uuid.Data4[5]
       << std::setw(2) << (int) uuid.Data4[6]
       << std::setw(2) << (int) uuid.Data4[7];
    return ss.str();
}

std::string BLEGuid::UUID16ToString(
    uint32_t val
) {
    std::stringstream ss;
    ss << std::hex << std::setfill(NUMBER_FILL)
       << std::setw(4) << val;
    return ss.str();
}

void BLEGuid::set(
    winrt::guid &uuid,
    void *buffer,
    size_t size
) {
    switch (size) {
        case 2:
            uuid = bluetoothBaseUUID;
            memmove(&uuid.Data1, buffer, 2);
#if IS_BIG_ENDIAN
#else
            uuid.Data1 = SWAP_BYTES_2(uuid.Data1);
#endif
            break;
        case 4:
            uuid = bluetoothBaseUUID;
            memmove(&uuid.Data1, buffer, 2);
#if IS_BIG_ENDIAN
            uuid.Data1 = SWAP_BYTES_4(uuid.Data1);
#else
#endif
            break;
        case 16: {
            char *c = (char *) buffer;
            memmove(&uuid.Data1, c + 12, 4);
            memmove(&uuid.Data2, c + 10, 2);
            memmove(&uuid.Data3, c + 8, 2);
            memmove(&uuid.Data4, c, 8);
#if IS_BIG_ENDIAN
            uuid.Data1 = SWAP_BYTES_4(uuid.Data1);
            uuid.Data2 = SWAP_BYTES_2(uuid.Data2);
            uuid.Data3 = SWAP_BYTES_2(uuid.Data3);
#else
#endif
            (*((uint64_t*) &uuid.Data4)) = SWAP_BYTES_8(*((uint64_t*) &uuid.Data4));
        }
            break;
        default:
            break;
    }
}
