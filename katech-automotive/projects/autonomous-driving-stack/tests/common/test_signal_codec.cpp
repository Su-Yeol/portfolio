/**
 * @file test_signal_codec.cpp
 * @brief Unit tests for signal encoding/decoding utilities (ASIL D)
 * @ref SW-CMN-REQ-SIG-001 ~ SIG-005, SW-CMN-UT-001
 *
 * ISO 26262-6 Clause 9: Unit Testing
 * Coverage target: Statement 100%, Branch 100%, MC/DC >= 95%
 */

#include <gtest/gtest.h>
#include "SignalCodec.h"

#include <array>
#include <cstdint>
#include <limits>

using namespace signal_codec;

// ==========================================================================
// UT-SIG-001: PackField
// ==========================================================================

TEST(SignalCodecPackField, BasicPacking)
{
    // value=0x03, mask=0x0C, shift=2 -> (0x03 << 2) & 0x0C = 0x0C
    EXPECT_EQ(PackField(0x03, 0x0C, 2), 0x0C);
}

TEST(SignalCodecPackField, ZeroValue)
{
    EXPECT_EQ(PackField(0x00, 0xFF, 0), 0x00);
}

TEST(SignalCodecPackField, FullByte)
{
    EXPECT_EQ(PackField(0xFF, 0xFF, 0), 0xFF);
}

TEST(SignalCodecPackField, MaskClearsOverflow)
{
    // value=0xFF, mask=0x0F, shift=0 -> 0xFF & 0x0F = 0x0F
    EXPECT_EQ(PackField(0xFF, 0x0F, 0), 0x0F);
}

// ==========================================================================
// UT-SIG-002: Pack2 / Pack3
// ==========================================================================

TEST(SignalCodecPack2, CombinesTwoFields)
{
    // Field1: value=1, mask=0x01, shift=0 -> 0x01
    // Field2: value=1, mask=0x02, shift=1 -> 0x02
    // Result: 0x01 | 0x02 = 0x03
    EXPECT_EQ(Pack2(1, 0x01, 0, 1, 0x02, 1), 0x03);
}

TEST(SignalCodecPack3, CombinesThreeFields)
{
    auto result = Pack3(
        0x01, 0x03, 0,  // bits [1:0]
        0x01, 0x0C, 2,  // bits [3:2]
        0x01, 0xF0, 4   // bits [7:4]
    );
    EXPECT_EQ(result, 0x15);  // 0x01 | 0x04 | 0x10
}

// ==========================================================================
// UT-SIG-003: ExtractField (inverse of PackField)
// ==========================================================================

TEST(SignalCodecExtractField, BasicExtraction)
{
    // value=0x0C, mask=0x0C, shift=2 -> (0x0C & 0x0C) >> 2 = 0x03
    EXPECT_EQ(ExtractField(0x0C, 0x0C, 2), 0x03);
}

TEST(SignalCodecExtractField, RoundtripWithPackField)
{
    const std::uint8_t original = 0x05;
    const std::uint8_t mask = 0x3C;
    const std::uint8_t shift = 2;

    auto packed = PackField(original, mask, shift);
    auto extracted = ExtractField(packed, mask, shift);
    EXPECT_EQ(extracted, original);
}

TEST(SignalCodecExtractField, AllZeros)
{
    EXPECT_EQ(ExtractField(0x00, 0xFF, 0), 0x00);
}

// ==========================================================================
// UT-SIG-004: JoinU16Le
// ==========================================================================

TEST(SignalCodecJoinU16, BasicJoin)
{
    EXPECT_EQ(JoinU16Le(0x34, 0x12), static_cast<std::uint16_t>(0x1234));
}

TEST(SignalCodecJoinU16, ZeroBytes)
{
    EXPECT_EQ(JoinU16Le(0x00, 0x00), static_cast<std::uint16_t>(0x0000));
}

TEST(SignalCodecJoinU16, MaxValue)
{
    EXPECT_EQ(JoinU16Le(0xFF, 0xFF), static_cast<std::uint16_t>(0xFFFF));
}

TEST(SignalCodecJoinU16, RoundtripWithByte0Byte1)
{
    const std::uint16_t original = 0xABCD;
    auto low = Byte0(original);
    auto high = Byte1(original);
    EXPECT_EQ(JoinU16Le(low, high), original);
}

// ==========================================================================
// UT-SIG-005: JoinU32Le / JoinI32Le
// ==========================================================================

TEST(SignalCodecJoinU32, BasicJoin)
{
    EXPECT_EQ(JoinU32Le(0x78, 0x56, 0x34, 0x12), static_cast<std::uint32_t>(0x12345678));
}

TEST(SignalCodecJoinU32, MaxValue)
{
    EXPECT_EQ(JoinU32Le(0xFF, 0xFF, 0xFF, 0xFF), std::numeric_limits<std::uint32_t>::max());
}

TEST(SignalCodecJoinI32, NegativeValue)
{
    // 0xFFFFFFFF in two's complement = -1
    EXPECT_EQ(JoinI32Le(0xFF, 0xFF, 0xFF, 0xFF), -1);
}

TEST(SignalCodecJoinU32, RoundtripWithByteAccessors)
{
    const std::uint32_t original = 0xDEADBEEF;
    auto result = JoinU32Le(Byte0(original), Byte1(original), Byte2(original), Byte3(original));
    EXPECT_EQ(result, original);
}

// ==========================================================================
// UT-SIG-006: ComposeU16
// ==========================================================================

TEST(SignalCodecComposeU16, BasicComposition)
{
    // left: (0x0F & 0x0F) << 8 = 0x0F00
    // right: (0xAB & 0xFF) >> 0 = 0x00AB
    // Result: 0x0FAB
    EXPECT_EQ(ComposeU16(0x0F, 0x0F, 8, 0xAB, 0xFF, 0), static_cast<std::uint16_t>(0x0FAB));
}

// ==========================================================================
// UT-SIG-007: Byte0~3 accessors (uint16_t and uint32_t overloads)
// ==========================================================================

TEST(SignalCodecByte, U16Accessors)
{
    const std::uint16_t val = 0xABCD;
    EXPECT_EQ(Byte0(val), 0xCD);
    EXPECT_EQ(Byte1(val), 0xAB);
}

TEST(SignalCodecByte, U32Accessors)
{
    const std::uint32_t val = 0x12345678;
    EXPECT_EQ(Byte0(val), 0x78);
    EXPECT_EQ(Byte1(val), 0x56);
    EXPECT_EQ(Byte2(val), 0x34);
    EXPECT_EQ(Byte3(val), 0x12);
}

TEST(SignalCodecByte, ZeroValue)
{
    EXPECT_EQ(Byte0(static_cast<std::uint32_t>(0)), 0x00);
    EXPECT_EQ(Byte3(static_cast<std::uint32_t>(0)), 0x00);
}

// ==========================================================================
// UT-SIG-008: WriteU16Le / WriteU32Le
// ==========================================================================

TEST(SignalCodecWrite, WriteU16Le)
{
    std::array<std::uint8_t, 8> buffer = {};
    WriteU16Le(buffer.data(), 2, 0xABCD);
    EXPECT_EQ(buffer[2], 0xCD);  // low byte
    EXPECT_EQ(buffer[3], 0xAB);  // high byte
    EXPECT_EQ(buffer[0], 0x00);  // untouched
    EXPECT_EQ(buffer[1], 0x00);  // untouched
}

TEST(SignalCodecWrite, WriteU32Le)
{
    std::array<std::uint8_t, 8> buffer = {};
    WriteU32Le(buffer.data(), 0, 0x12345678);
    EXPECT_EQ(buffer[0], 0x78);
    EXPECT_EQ(buffer[1], 0x56);
    EXPECT_EQ(buffer[2], 0x34);
    EXPECT_EQ(buffer[3], 0x12);
}

// ==========================================================================
// UT-SIG-009: WriteU16LeSafe / WriteU32LeSafe (bounds-checked, ASIL D)
// ==========================================================================

TEST(SignalCodecWriteSafe, WriteU16LeSafe_ValidOffset)
{
    std::array<std::uint8_t, 8> buffer = {};
    EXPECT_TRUE(WriteU16LeSafe(buffer.data(), buffer.size(), 6, 0x1234));
    EXPECT_EQ(buffer[6], 0x34);
    EXPECT_EQ(buffer[7], 0x12);
}

TEST(SignalCodecWriteSafe, WriteU16LeSafe_OverflowRejected)
{
    std::array<std::uint8_t, 4> buffer = {};
    EXPECT_FALSE(WriteU16LeSafe(buffer.data(), buffer.size(), 3, 0x1234))
        << "offset+2 > buffer_size must return false";
    // Buffer must be untouched
    EXPECT_EQ(buffer[3], 0x00);
}

TEST(SignalCodecWriteSafe, WriteU16LeSafe_NullBuffer)
{
    EXPECT_FALSE(WriteU16LeSafe(nullptr, 8, 0, 0x1234));
}

TEST(SignalCodecWriteSafe, WriteU32LeSafe_ValidOffset)
{
    std::array<std::uint8_t, 8> buffer = {};
    EXPECT_TRUE(WriteU32LeSafe(buffer.data(), buffer.size(), 4, 0xDEADBEEF));
    EXPECT_EQ(buffer[4], 0xEF);
    EXPECT_EQ(buffer[5], 0xBE);
    EXPECT_EQ(buffer[6], 0xAD);
    EXPECT_EQ(buffer[7], 0xDE);
}

TEST(SignalCodecWriteSafe, WriteU32LeSafe_OverflowRejected)
{
    std::array<std::uint8_t, 4> buffer = {};
    EXPECT_FALSE(WriteU32LeSafe(buffer.data(), buffer.size(), 1, 0x12345678))
        << "offset+4 > buffer_size must return false";
}

TEST(SignalCodecWriteSafe, WriteU32LeSafe_NullBuffer)
{
    EXPECT_FALSE(WriteU32LeSafe(nullptr, 8, 0, 0x12345678));
}

TEST(SignalCodecWriteSafe, WriteU32LeSafe_ExactFit)
{
    std::array<std::uint8_t, 4> buffer = {};
    EXPECT_TRUE(WriteU32LeSafe(buffer.data(), buffer.size(), 0, 0xCAFEBABE));
    EXPECT_EQ(buffer[0], 0xBE);
    EXPECT_EQ(buffer[1], 0xBA);
    EXPECT_EQ(buffer[2], 0xFE);
    EXPECT_EQ(buffer[3], 0xCA);
}

// ==========================================================================
// UT-SIG-010: Boundary values
// ==========================================================================

TEST(SignalCodecBoundary, AllZeros)
{
    EXPECT_EQ(JoinU16Le(0x00, 0x00), static_cast<std::uint16_t>(0));
    EXPECT_EQ(JoinU32Le(0x00, 0x00, 0x00, 0x00), static_cast<std::uint32_t>(0));
}

TEST(SignalCodecBoundary, AllOnes)
{
    EXPECT_EQ(JoinU16Le(0xFF, 0xFF), static_cast<std::uint16_t>(0xFFFF));
    EXPECT_EQ(JoinU32Le(0xFF, 0xFF, 0xFF, 0xFF), static_cast<std::uint32_t>(0xFFFFFFFF));
}

TEST(SignalCodecBoundary, I32MinMax)
{
    // INT32_MIN = 0x80000000
    EXPECT_EQ(JoinI32Le(0x00, 0x00, 0x00, 0x80), std::numeric_limits<std::int32_t>::min());
    // INT32_MAX = 0x7FFFFFFF
    EXPECT_EQ(JoinI32Le(0xFF, 0xFF, 0xFF, 0x7F), std::numeric_limits<std::int32_t>::max());
}
