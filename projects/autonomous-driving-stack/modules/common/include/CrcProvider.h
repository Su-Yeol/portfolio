#ifndef AUTODRIVE_CRC_PROVIDER_H
#define AUTODRIVE_CRC_PROVIDER_H

#include <cstddef>
#include <cstdint>

class ICrcProvider
{
public:
    virtual ~ICrcProvider() = default;
    virtual std::uint16_t ComputeCrc16CcittWithDataId(
        const std::uint8_t *payload,
        std::size_t payload_len,
        std::uint16_t seed,
        std::uint8_t data_id_l,
        std::uint8_t data_id_h) const = 0;
};

const ICrcProvider &GetCrcProvider();
const ICrcProvider &GetPrivateCrcProvider();

#endif
