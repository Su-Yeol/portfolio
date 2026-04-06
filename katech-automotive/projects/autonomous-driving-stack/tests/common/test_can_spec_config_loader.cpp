/**
 * @file test_can_spec_config_loader.cpp
 * @brief Unit tests for CAN specification config loader (ASIL D)
 * @ref SW-CMN-REQ-CFG-001 ~ CFG-004
 *
 * ISO 26262-6 Clause 9: Unit Testing
 * Note: Uses AUTODRIVE_CAN_SPEC env var to point to test INI file.
 *       The singleton nature of GetCanSpecProvider() means these tests
 *       share a single provider instance initialized at first call.
 */

#include <gtest/gtest.h>
#include "CanSpecProvider.h"

#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>

namespace
{

const std::string kTestIniPath = "common/testdata/test_can_spec.ini";

void CreateTestCanSpecIni()
{
    std::ofstream out(kTestIniPath);
    out << "# Test CAN spec for unit testing\n"
        << "aeb.cmd_can_id = 0x160\n"
        << "aeb.cmd_length = 8\n"
        << "gateway.crc_seed = 0xFFFF\n"
        << "gateway.msg_165_data_id_l = 0x07\n"
        << "gateway.msg_165_data_id_h = 0x65\n"
        << "calibration.gravity = 9.81\n"
        << "calibration.look_ahead_gain = 0.6\n"
        << "calibration.handle_saturation = 400\n"
        << "calibration.vehicle_wheelbase = 2.865\n"
        << "signed_value = -42\n"
        << "gateway.crc_test.payload = 0,0,211,123,117,137,226,0,0,0,0,0,0,0,0,0,0,28,1,0,0,0,0,0\n"
        << "gateway.crc_test.length = 22\n"
        << "list_values = 100,200,300,400,500\n"
        // Edge cases for branch coverage
        << "empty_value = \n"
        << "invalid_number = not_a_number\n"
        << "hex_upper = 0X1A\n"
        << "whitespace_value =   42  \n"
        << "# This is a comment line\n"
        << "\n"  // empty line
        << "empty_list = ,,,\n"
        << "invalid_float = abc\n"
        << "hex_signed = 0xFF\n";
    out.close();
}

// Environment setup: must happen before GetCanSpecProvider() is first called
class CanSpecTestEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        CreateTestCanSpecIni();
        setenv("AUTODRIVE_CAN_SPEC", kTestIniPath.c_str(), 1);
    }

    void TearDown() override
    {
        unsetenv("AUTODRIVE_CAN_SPEC");
    }
};

::testing::Environment *const can_spec_env =
    ::testing::AddGlobalTestEnvironment(new CanSpecTestEnvironment());

} // namespace

class CanSpecConfigLoaderTest : public ::testing::Test
{
protected:
    const ICanSpecProvider &spec_ = GetCanSpecProvider();
};

// UT-CFG-001: GetU32 with hex value
TEST_F(CanSpecConfigLoaderTest, GetU32HexValue)
{
    EXPECT_EQ(spec_.GetU32("aeb.cmd_can_id", 0), static_cast<std::uint32_t>(0x160));
}

// UT-CFG-002: GetU32 with decimal value
TEST_F(CanSpecConfigLoaderTest, GetU32DecimalValue)
{
    EXPECT_EQ(spec_.GetU32("aeb.cmd_length", 0), static_cast<std::uint32_t>(8));
}

// UT-CFG-003: GetU32 fallback for missing key
TEST_F(CanSpecConfigLoaderTest, GetU32FallbackOnMissingKey)
{
    EXPECT_EQ(spec_.GetU32("nonexistent.key", 999), static_cast<std::uint32_t>(999));
}

// UT-CFG-004: GetU8 returns lower 8 bits
TEST_F(CanSpecConfigLoaderTest, GetU8Value)
{
    EXPECT_EQ(spec_.GetU8("gateway.msg_165_data_id_l", 0), static_cast<std::uint8_t>(0x07));
    EXPECT_EQ(spec_.GetU8("gateway.msg_165_data_id_h", 0), static_cast<std::uint8_t>(0x65));
}

// UT-CFG-005: GetI32 with negative value
TEST_F(CanSpecConfigLoaderTest, GetI32NegativeValue)
{
    EXPECT_EQ(spec_.GetI32("signed_value", 0), -42);
}

// UT-CFG-006: GetF64 with floating point
TEST_F(CanSpecConfigLoaderTest, GetF64Value)
{
    EXPECT_DOUBLE_EQ(spec_.GetF64("calibration.gravity", 0.0), 9.81);
    EXPECT_DOUBLE_EQ(spec_.GetF64("calibration.look_ahead_gain", 0.0), 0.6);
    EXPECT_DOUBLE_EQ(spec_.GetF64("calibration.vehicle_wheelbase", 0.0), 2.865);
}

// UT-CFG-007: GetF64 fallback for missing key
TEST_F(CanSpecConfigLoaderTest, GetF64FallbackOnMissingKey)
{
    EXPECT_DOUBLE_EQ(spec_.GetF64("nonexistent.float", 3.14), 3.14);
}

// UT-CFG-008: GetU32List parses comma-separated values
TEST_F(CanSpecConfigLoaderTest, GetU32ListParsesCorrectly)
{
    std::vector<std::uint32_t> fallback = {};
    auto result = spec_.GetU32List("list_values", fallback);
    ASSERT_EQ(result.size(), 5u);
    EXPECT_EQ(result[0], 100u);
    EXPECT_EQ(result[1], 200u);
    EXPECT_EQ(result[2], 300u);
    EXPECT_EQ(result[3], 400u);
    EXPECT_EQ(result[4], 500u);
}

// UT-CFG-009: GetU32List fallback for missing key
TEST_F(CanSpecConfigLoaderTest, GetU32ListFallbackOnMissingKey)
{
    std::vector<std::uint32_t> fallback = {1, 2, 3};
    auto result = spec_.GetU32List("nonexistent.list", fallback);
    ASSERT_EQ(result.size(), 3u);
    EXPECT_EQ(result[0], 1u);
}

// UT-CFG-010: ActiveConfigPath returns the loaded path
TEST_F(CanSpecConfigLoaderTest, ActiveConfigPathMatchesEnvVar)
{
    EXPECT_EQ(spec_.ActiveConfigPath(), kTestIniPath);
}

// UT-CFG-011: CRC seed loaded correctly (used for E2E verification)
TEST_F(CanSpecConfigLoaderTest, CrcSeedLoadedCorrectly)
{
    EXPECT_EQ(spec_.GetU32("gateway.crc_seed", 0), static_cast<std::uint32_t>(0xFFFF));
}

// UT-CFG-012: CRC test payload loaded as list
TEST_F(CanSpecConfigLoaderTest, CrcTestPayloadLoaded)
{
    std::vector<std::uint32_t> fallback = {};
    auto payload = spec_.GetU32List("gateway.crc_test.payload", fallback);
    ASSERT_GE(payload.size(), 22u);
    EXPECT_EQ(payload[0], 0u);    // first byte
    EXPECT_EQ(payload[2], 211u);  // third byte
}

// UT-CFG-013: Singleton consistency
TEST_F(CanSpecConfigLoaderTest, SingletonReturnsConsistentInstance)
{
    const ICanSpecProvider &ref1 = GetCanSpecProvider();
    const ICanSpecProvider &ref2 = GetCanSpecProvider();
    EXPECT_EQ(&ref1, &ref2);
}

// --- Branch coverage enhancement tests ---

// UT-CFG-014: Empty value returns fallback
TEST_F(CanSpecConfigLoaderTest, EmptyValueReturnsFallback)
{
    EXPECT_EQ(spec_.GetU32("empty_value", 77), static_cast<std::uint32_t>(77));
    EXPECT_EQ(spec_.GetI32("empty_value", -99), -99);
    EXPECT_DOUBLE_EQ(spec_.GetF64("empty_value", 1.23), 1.23);
}

// UT-CFG-015: Invalid number returns fallback (catch branch)
TEST_F(CanSpecConfigLoaderTest, InvalidNumberReturnsFallback)
{
    EXPECT_EQ(spec_.GetU32("invalid_number", 55), static_cast<std::uint32_t>(55));
    EXPECT_EQ(spec_.GetI32("invalid_number", -55), -55);
    EXPECT_DOUBLE_EQ(spec_.GetF64("invalid_float", 9.99), 9.99);
}

// UT-CFG-016: Hex with uppercase 0X prefix
TEST_F(CanSpecConfigLoaderTest, HexUppercasePrefix)
{
    EXPECT_EQ(spec_.GetU32("hex_upper", 0), static_cast<std::uint32_t>(0x1A));
    EXPECT_EQ(spec_.GetI32("hex_signed", 0), static_cast<std::int32_t>(0xFF));
}

// UT-CFG-017: Whitespace trimming
TEST_F(CanSpecConfigLoaderTest, WhitespaceTrimmedCorrectly)
{
    EXPECT_EQ(spec_.GetU32("whitespace_value", 0), static_cast<std::uint32_t>(42));
}

// UT-CFG-018: Empty list entries filtered
TEST_F(CanSpecConfigLoaderTest, EmptyListEntriesFiltered)
{
    std::vector<std::uint32_t> fallback = {999};
    auto result = spec_.GetU32List("empty_list", fallback);
    // All entries are empty after split, so fallback is returned
    EXPECT_EQ(result, fallback);
}

// UT-CFG-019: GetI32 fallback for missing key
TEST_F(CanSpecConfigLoaderTest, GetI32FallbackOnMissingKey)
{
    EXPECT_EQ(spec_.GetI32("nonexistent.signed", -123), -123);
}

// UT-CFG-020: GetU8 fallback for missing key
TEST_F(CanSpecConfigLoaderTest, GetU8FallbackOnMissingKey)
{
    EXPECT_EQ(spec_.GetU8("nonexistent.byte", 0xAA), static_cast<std::uint8_t>(0xAA));
}
