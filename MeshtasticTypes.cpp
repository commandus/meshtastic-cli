#include "MeshtasticTypes.h"
#include "meshtastic-magic-numbers.h"

MESHTASTIC_HEADER::MESHTASTIC_HEADER()
    : v( { MESHTASTIC_MAGIC_NUMBER, 0 })
{

}

MESHTASTIC_HEADER::MESHTASTIC_HEADER(
    uint16_t size
)
#if IS_BIG_ENDIAN
    : v( { MESHTASTIC_MAGIC_NUMBER, size })
#else
    : v( { MESHTASTIC_MAGIC_NUMBER, SWAP_BYTES_2(size) })
#endif
{

}

MESHTASTIC_HEADER::MESHTASTIC_HEADER(
    const MESHTASTIC_HEADER &value
)
    : v( { value.v.m, value.v.len })
{

}

MESHTASTIC_HEADER::MESHTASTIC_HEADER(
    uint8_t *buffer,
    uint32_t size
)
{
    uint16_t sz = size;
    if (sz > MESHTASTIC_HEADER_SIZE)
        sz = MESHTASTIC_HEADER_SIZE;
    memmove(&v.c, buffer, sz);
}

uint16_t MESHTASTIC_HEADER::length()
{
#if IS_BIG_ENDIAN
    return v.len;
#else
    return SWAP_BYTES_2(v.len);
#endif
}

void MESHTASTIC_HEADER::setLength(
    uint16_t val
)
{

#if IS_BIG_ENDIAN
    v.len = val;
#else
    v.len = SWAP_BYTES_2(val);
#endif
}

bool MESHTASTIC_HEADER::isValid(
    const MESHTASTIC_HEADER &val
)
{
    return val.v.m == MESHTASTIC_MAGIC_NUMBER;
}

bool MESHTASTIC_HEADER::isValid() const
{
    return v.m == MESHTASTIC_MAGIC_NUMBER;
}

bool MESHTASTIC_HEADER::hasValidHeader(
    void *buffer, uint32_t size
)
{
    if (size < 4)
        return false;
    MESHTASTIC_HEADER *h = (MESHTASTIC_HEADER *) buffer;
    if (!isValid(*h))
        return false;
    return h->length() == size - 4;
}

bool MESHTASTIC_HEADER::hasValidHeader(
    const std::string &value
)
{
    return MESHTASTIC_HEADER::hasValidHeader((uint8_t *) value.c_str(), value.size());
}
