#ifndef AUTODRIVE_SIGNAL_CODEC_H
#define AUTODRIVE_SIGNAL_CODEC_H

#include <cstddef>
#include <cstdint>

namespace signal_codec
{

    inline std::uint8_t PackField(std::uint8_t value, std::uint8_t mask, std::uint8_t left_shift)
    {
        return static_cast<std::uint8_t>((static_cast<std::uint8_t>(value << left_shift)) & mask);
    }

    inline std::uint8_t Pack2(std::uint8_t v1, std::uint8_t m1, std::uint8_t s1,
                              std::uint8_t v2, std::uint8_t m2, std::uint8_t s2)
    {
        return static_cast<std::uint8_t>(PackField(v1, m1, s1) | PackField(v2, m2, s2));
    }

    inline std::uint8_t Pack3(std::uint8_t v1, std::uint8_t m1, std::uint8_t s1,
                              std::uint8_t v2, std::uint8_t m2, std::uint8_t s2,
                              std::uint8_t v3, std::uint8_t m3, std::uint8_t s3)
    {
        return static_cast<std::uint8_t>(
            PackField(v1, m1, s1) | PackField(v2, m2, s2) | PackField(v3, m3, s3));
    }

    inline std::uint8_t ExtractField(std::uint8_t value, std::uint8_t mask, std::uint8_t right_shift)
    {
        return static_cast<std::uint8_t>((value & mask) >> right_shift);
    }

    inline std::uint16_t JoinU16Le(std::uint8_t low, std::uint8_t high)
    {
        return static_cast<std::uint16_t>(
            static_cast<std::uint16_t>(low) | (static_cast<std::uint16_t>(high) << 8));
    }

    inline std::uint32_t JoinU32Le(std::uint8_t b0, std::uint8_t b1, std::uint8_t b2, std::uint8_t b3)
    {
        return static_cast<std::uint32_t>(b0) |
               (static_cast<std::uint32_t>(b1) << 8) |
               (static_cast<std::uint32_t>(b2) << 16) |
               (static_cast<std::uint32_t>(b3) << 24);
    }

    inline std::int32_t JoinI32Le(std::uint8_t b0, std::uint8_t b1, std::uint8_t b2, std::uint8_t b3)
    {
        return static_cast<std::int32_t>(JoinU32Le(b0, b1, b2, b3));
    }

    inline std::uint16_t ComposeU16(std::uint8_t left_value, std::uint8_t left_mask, std::uint8_t left_shift,
                                    std::uint8_t right_value, std::uint8_t right_mask, std::uint8_t right_shift)
    {
        return static_cast<std::uint16_t>(
            (static_cast<std::uint16_t>(left_value & left_mask) << left_shift) |
            (static_cast<std::uint16_t>(right_value & right_mask) >> right_shift));
    }

    inline std::uint8_t Byte0(std::uint16_t value)
    {
        return static_cast<std::uint8_t>(value & 0xFF);
    }

    inline std::uint8_t Byte1(std::uint16_t value)
    {
        return static_cast<std::uint8_t>((value >> 8) & 0xFF);
    }

    inline std::uint8_t Byte0(std::uint32_t value)
    {
        return static_cast<std::uint8_t>(value & 0xFF);
    }

    inline std::uint8_t Byte1(std::uint32_t value)
    {
        return static_cast<std::uint8_t>((value >> 8) & 0xFF);
    }

    inline std::uint8_t Byte2(std::uint32_t value)
    {
        return static_cast<std::uint8_t>((value >> 16) & 0xFF);
    }

    inline std::uint8_t Byte3(std::uint32_t value)
    {
        return static_cast<std::uint8_t>((value >> 24) & 0xFF);
    }

    inline void WriteU16Le(std::uint8_t *buffer, std::size_t offset, std::uint16_t value)
    {
        buffer[offset] = Byte0(value);
        buffer[offset + 1] = Byte1(value);
    }

    inline void WriteU32Le(std::uint8_t *buffer, std::size_t offset, std::uint32_t value)
    {
        buffer[offset] = Byte0(value);
        buffer[offset + 1] = Byte1(value);
        buffer[offset + 2] = Byte2(value);
        buffer[offset + 3] = Byte3(value);
    }

}

#endif
