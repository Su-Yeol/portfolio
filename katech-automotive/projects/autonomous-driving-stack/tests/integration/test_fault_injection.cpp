/**
 * @file test_fault_injection.cpp
 * @brief Fault injection tests for safety mechanism validation (ASIL D)
 * @ref SW-AEB-UT-001 TC-FI-001~005, SW-CMN-IT-001 IT-FI-001~003
 *
 * ISO 26262-6 Clause 9-10: Fault Injection Testing
 * Validates that safety mechanisms correctly detect and handle faults.
 */

#include <gtest/gtest.h>
#include "CanSpecProvider.h"
#include "CrcProvider.h"
#include "SignalCodec.h"
#include "modules/shared/config_parser.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <fstream>

using namespace signal_codec;

// ==========================================================================
// TC-FI-001: CRC bit-flip detection
// Validates: SWREQ-AEB-004 (CAN communication error handling)
// ==========================================================================

TEST(FaultInjection, CrcDetectsSingleBitFlip)
{
    const auto &crc = GetCrcProvider();
    constexpr std::uint16_t seed = 0xFFFF;

    // Build a valid frame with CRC
    std::array<std::uint8_t, 8> frame = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x00, 0x00};
    auto valid_crc = crc.ComputeCrc16CcittWithDataId(frame.data(), 6, seed, 0x07, 0x65);
    WriteU16Le(frame.data(), 6, valid_crc);

    // Inject fault: flip 1 bit in payload byte 0
    frame[0] ^= 0x01;

    // Receiver recomputes CRC
    auto recomputed = crc.ComputeCrc16CcittWithDataId(frame.data(), 6, seed, 0x07, 0x65);
    auto stored = JoinU16Le(frame[6], frame[7]);

    EXPECT_NE(recomputed, stored)
        << "CRC must detect single-bit corruption in payload";
}

TEST(FaultInjection, CrcDetectsMultiBitFlip)
{
    const auto &crc = GetCrcProvider();
    constexpr std::uint16_t seed = 0xFFFF;

    std::array<std::uint8_t, 8> frame = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x00};
    auto valid_crc = crc.ComputeCrc16CcittWithDataId(frame.data(), 6, seed, 0, 0);
    WriteU16Le(frame.data(), 6, valid_crc);

    // Inject fault: flip multiple bits across bytes
    frame[1] ^= 0xFF;  // All bits flipped
    frame[4] ^= 0x80;  // MSB flipped

    auto recomputed = crc.ComputeCrc16CcittWithDataId(frame.data(), 6, seed, 0, 0);
    auto stored = JoinU16Le(frame[6], frame[7]);

    EXPECT_NE(recomputed, stored)
        << "CRC must detect multi-bit corruption";
}

// ==========================================================================
// TC-FI-002: CRC field corruption detection
// Validates: E2E protection integrity
// ==========================================================================

TEST(FaultInjection, CrcFieldCorruptionDetected)
{
    const auto &crc = GetCrcProvider();
    constexpr std::uint16_t seed = 0xFFFF;

    std::array<std::uint8_t, 8> frame = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00};
    auto valid_crc = crc.ComputeCrc16CcittWithDataId(frame.data(), 6, seed, 0, 0);
    WriteU16Le(frame.data(), 6, valid_crc);

    // Inject fault: corrupt the CRC field itself
    frame[6] ^= 0x01;

    auto recomputed = crc.ComputeCrc16CcittWithDataId(frame.data(), 6, seed, 0, 0);
    auto stored = JoinU16Le(frame[6], frame[7]);

    EXPECT_NE(recomputed, stored)
        << "Corrupted CRC field must be detected";
}

// ==========================================================================
// TC-FI-003: Wrong Data ID detection
// Validates: E2E message authentication (wrong sender/receiver ID)
// ==========================================================================

TEST(FaultInjection, WrongDataIdDetected)
{
    const auto &crc = GetCrcProvider();
    constexpr std::uint16_t seed = 0xFFFF;

    std::array<std::uint8_t, 8> frame = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x00, 0x00};

    // Sender uses correct Data ID
    auto sender_crc = crc.ComputeCrc16CcittWithDataId(
        frame.data(), 6, seed, 0x07, 0x65);
    WriteU16Le(frame.data(), 6, sender_crc);

    // Receiver uses WRONG Data ID (message routing error)
    auto receiver_crc = crc.ComputeCrc16CcittWithDataId(
        frame.data(), 6, seed, 0x08, 0x65);  // data_id_l mismatch

    auto stored = JoinU16Le(frame[6], frame[7]);

    EXPECT_NE(receiver_crc, stored)
        << "Wrong Data ID must cause CRC verification failure";
}

// ==========================================================================
// TC-FI-004: Config file corruption (SWREQ-AEB-005)
// Validates: Graceful handling of corrupted config
// ==========================================================================

TEST(FaultInjection, CorruptedConfigFileGracefulHandling)
{
    // Create a corrupted INI file
    system("mkdir -p integration/testdata");
    {
        std::ofstream out("integration/testdata/corrupted.ini");
        out << "valid_key = 42\n"
            << "=no_key_line\n"        // malformed: empty key
            << "broken line without equals\n"  // no delimiter
            << "another_valid = 100\n"
            << "\n"                    // empty line
            << "# comment line\n";     // CConfigParser doesn't skip comments
    }

    CConfigParser parser("integration/testdata/corrupted.ini");
    // Parser should still succeed (it loaded at least some entries)
    EXPECT_TRUE(parser.IsSuccess());

    // Valid entries should be accessible
    EXPECT_EQ(parser.GetInt("valid_key"), 42);
    EXPECT_EQ(parser.GetInt("another_valid"), 100);
}

// ==========================================================================
// TC-FI-005: SignalCodec bounds violation (SWREQ-CMN-SIG-004)
// Validates: Safe write functions reject out-of-bounds access
// ==========================================================================

TEST(FaultInjection, SignalCodecBoundsViolationRejected)
{
    std::array<std::uint8_t, 4> small_buffer = {0xAA, 0xBB, 0xCC, 0xDD};

    // Attempt to write beyond buffer
    EXPECT_FALSE(WriteU16LeSafe(small_buffer.data(), small_buffer.size(), 3, 0x1234))
        << "WriteU16LeSafe must reject offset that would overflow";
    EXPECT_FALSE(WriteU32LeSafe(small_buffer.data(), small_buffer.size(), 1, 0x12345678))
        << "WriteU32LeSafe must reject offset that would overflow";

    // Verify buffer contents are unchanged
    EXPECT_EQ(small_buffer[0], 0xAA);
    EXPECT_EQ(small_buffer[1], 0xBB);
    EXPECT_EQ(small_buffer[2], 0xCC);
    EXPECT_EQ(small_buffer[3], 0xDD);
}

// ==========================================================================
// TC-FI-006: Null pointer fault injection
// Validates: Defensive programming against null inputs
// ==========================================================================

TEST(FaultInjection, NullPointerHandling)
{
    const auto &crc = GetCrcProvider();

    // CRC with null payload and non-zero length
    auto result = crc.ComputeCrc16CcittWithDataId(nullptr, 100, 0xFFFF, 0, 0);
    EXPECT_EQ(result, static_cast<std::uint16_t>(0xFFFF))
        << "Null payload with non-zero length must return seed (safe fallback)";

    // Safe write with null buffer
    EXPECT_FALSE(WriteU16LeSafe(nullptr, 8, 0, 0x1234));
    EXPECT_FALSE(WriteU32LeSafe(nullptr, 8, 0, 0x12345678));
}

// ==========================================================================
// TC-FI-007: Seed mismatch (wrong initialization)
// ==========================================================================

TEST(FaultInjection, SeedMismatchDetected)
{
    const auto &crc = GetCrcProvider();

    std::array<std::uint8_t, 6> payload = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

    // Sender uses seed 0xFFFF
    auto sender_crc = crc.ComputeCrc16CcittWithDataId(
        payload.data(), payload.size(), 0xFFFF, 0, 0);

    // Receiver uses different seed (configuration error)
    auto receiver_crc = crc.ComputeCrc16CcittWithDataId(
        payload.data(), payload.size(), 0x0000, 0, 0);

    EXPECT_NE(sender_crc, receiver_crc)
        << "Seed mismatch must produce different CRC (detected by receiver)";
}
