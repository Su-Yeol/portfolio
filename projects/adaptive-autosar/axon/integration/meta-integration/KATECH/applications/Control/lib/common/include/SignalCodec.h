#ifndef KATECH_CONTROL_SIGNAL_CODEC_H
#define KATECH_CONTROL_SIGNAL_CODEC_H

#include <cstddef>
#include <cstdint>

#include <linux/can.h>

namespace katech_common {
inline std::uint8_t ReadByte(const std::uint8_t* data, std::size_t size, std::size_t idx) {
    return idx < size ? data[idx] : 0U;
}

inline std::uint16_t JoinU16Be(const std::uint8_t* data, std::size_t size, std::size_t hi_idx, std::size_t lo_idx) {
    const auto hi = static_cast<std::uint16_t>(ReadByte(data, size, hi_idx));
    const auto lo = static_cast<std::uint16_t>(ReadByte(data, size, lo_idx));
    return static_cast<std::uint16_t>((hi << 8U) | lo);
}

inline float DecodeScaledSignedU16(const canfd_frame& frame,
                                   std::size_t hi_idx,
                                   std::size_t lo_idx,
                                   double scale,
                                   double signed_threshold,
                                   double signed_raw_multiplier,
                                   double signed_offset,
                                   double signed_result_scale) {
    double value = static_cast<double>(JoinU16Be(frame.data, sizeof(frame.data), hi_idx, lo_idx)) * scale;
    if (value > signed_threshold) {
        value = ((value * signed_raw_multiplier) - signed_offset) * signed_result_scale;
    }
    return static_cast<float>(value);
}
}  // namespace katech_common

#endif
