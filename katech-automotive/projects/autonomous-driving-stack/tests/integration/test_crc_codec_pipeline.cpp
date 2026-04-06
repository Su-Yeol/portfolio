/**
 * @file test_crc_codec_pipeline.cpp
 * @brief Integration test: CRC + SignalCodec + CanSpec E2E pipeline (ASIL D)
 * @ref SW-AEB-IT-001 TC-IT-001~002, SW-CMN-IT-001 IT-LIB-004~005
 *
 * ISO 26262-6 Clause 10: Integration Testing
 * Verifies the complete data integrity chain:
 *   CanSpec -> CRC seed/DataID -> SignalCodec encode -> CRC compute -> verify
 */

#include <gtest/gtest.h>
#include "CanSpecProvider.h"
#include "CrcProvider.h"
#include "SignalCodec.h"

#include <array>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <vector>

namespace
{

const std::string kTestIniPath = "integration/testdata/integration_can_spec.ini";

void CreateIntegrationCanSpec()
{
    std::ofstream out(kTestIniPath);
    out << "# Integration test CAN spec\n"
        << "gateway.crc_seed = 0xFFFF\n"
        << "gateway.msg_165_data_id_l = 0x07\n"
        << "gateway.msg_165_data_id_h = 0x65\n"
        << "aeb.cmd_can_id = 0x160\n"
        << "aeb.cmd_length = 8\n";
    out.close();
}

class CrcCodecPipelineEnvironment : public ::testing::Environment
{
public:
    void SetUp() override
    {
        // Create testdata directory
        system("mkdir -p integration/testdata");
        CreateIntegrationCanSpec();
        setenv("AUTODRIVE_CAN_SPEC", kTestIniPath.c_str(), 1);
    }
    void TearDown() override
    {
        unsetenv("AUTODRIVE_CAN_SPEC");
    }
};

::testing::Environment *const pipeline_env =
    ::testing::AddGlobalTestEnvironment(new CrcCodecPipelineEnvironment());

} // namespace

using namespace signal_codec;

// IT-LIB-004: Full E2E pipeline - encode, CRC, verify roundtrip
TEST(CrcCodecPipeline, EncodeComputeVerifyRoundtrip)
{
    const auto &spec = GetCanSpecProvider();
    const auto &crc = GetCrcProvider();

    // Step 1: Load CRC parameters from CAN spec
    auto seed = static_cast<std::uint16_t>(spec.GetU32("gateway.crc_seed", 0xFFFF));
    auto data_id_l = spec.GetU8("gateway.msg_165_data_id_l", 0);
    auto data_id_h = spec.GetU8("gateway.msg_165_data_id_h", 0);

    EXPECT_EQ(seed, 0xFFFF);
    EXPECT_EQ(data_id_l, 0x07);
    EXPECT_EQ(data_id_h, 0x65);

    // Step 2: Build a CAN frame payload using SignalCodec
    std::array<std::uint8_t, 8> frame = {};
    WriteU16Le(frame.data(), 0, 0x0123);  // Bytes 0-1: control command
    WriteU16Le(frame.data(), 2, 0x4567);  // Bytes 2-3: speed value
    frame[4] = Pack2(0x01, 0x0F, 0, 0x02, 0xF0, 4);  // Byte 4: status flags
    frame[5] = 0x00;  // Byte 5: reserved

    // Step 3: Compute CRC over payload bytes [0..5]
    auto computed_crc = crc.ComputeCrc16CcittWithDataId(
        frame.data(), 6, seed, data_id_l, data_id_h);

    // Step 4: Write CRC into frame bytes [6..7]
    EXPECT_TRUE(WriteU16LeSafe(frame.data(), frame.size(), 6, computed_crc));

    // Step 5: Receiver side - recompute CRC and verify
    auto received_crc = crc.ComputeCrc16CcittWithDataId(
        frame.data(), 6, seed, data_id_l, data_id_h);

    auto stored_crc = JoinU16Le(frame[6], frame[7]);
    EXPECT_EQ(received_crc, stored_crc)
        << "E2E CRC verification must pass for unmodified frame";
}

// IT-LIB-005: Data ID from CanSpec correctly affects CRC
TEST(CrcCodecPipeline, DataIdFromSpecAffectsCrc)
{
    const auto &spec = GetCanSpecProvider();
    const auto &crc = GetCrcProvider();

    auto seed = static_cast<std::uint16_t>(spec.GetU32("gateway.crc_seed", 0xFFFF));
    auto data_id_l = spec.GetU8("gateway.msg_165_data_id_l", 0);
    auto data_id_h = spec.GetU8("gateway.msg_165_data_id_h", 0);

    std::array<std::uint8_t, 4> payload = {0xAA, 0xBB, 0xCC, 0xDD};

    auto crc_with_id = crc.ComputeCrc16CcittWithDataId(
        payload.data(), payload.size(), seed, data_id_l, data_id_h);
    auto crc_without_id = crc.ComputeCrc16CcittWithDataId(
        payload.data(), payload.size(), seed, 0x00, 0x00);

    EXPECT_NE(crc_with_id, crc_without_id)
        << "Data ID from CAN spec must influence CRC result";
}

// IT-CAN-001: Frame construction matches CAN spec ID and length
TEST(CrcCodecPipeline, FrameMatchesCanSpecDefinition)
{
    const auto &spec = GetCanSpecProvider();

    auto can_id = spec.GetU32("aeb.cmd_can_id", 0);
    auto can_len = spec.GetU32("aeb.cmd_length", 0);

    EXPECT_EQ(can_id, static_cast<std::uint32_t>(0x160));
    EXPECT_EQ(can_len, static_cast<std::uint32_t>(8));

    // Verify frame size matches spec
    std::array<std::uint8_t, 8> frame = {};
    EXPECT_EQ(frame.size(), can_len);
}

// IT-E2E-001: Multiple frames maintain independent CRC integrity
TEST(CrcCodecPipeline, MultipleFramesIndependentCrc)
{
    const auto &crc = GetCrcProvider();
    constexpr std::uint16_t seed = 0xFFFF;

    std::array<std::uint8_t, 8> frame1 = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00, 0x00};
    std::array<std::uint8_t, 8> frame2 = {0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0x00, 0x00};

    auto crc1 = crc.ComputeCrc16CcittWithDataId(frame1.data(), 6, seed, 0, 0);
    auto crc2 = crc.ComputeCrc16CcittWithDataId(frame2.data(), 6, seed, 0, 0);

    WriteU16Le(frame1.data(), 6, crc1);
    WriteU16Le(frame2.data(), 6, crc2);

    // Each frame must verify independently
    auto verify1 = crc.ComputeCrc16CcittWithDataId(frame1.data(), 6, seed, 0, 0);
    auto verify2 = crc.ComputeCrc16CcittWithDataId(frame2.data(), 6, seed, 0, 0);

    EXPECT_EQ(verify1, JoinU16Le(frame1[6], frame1[7]));
    EXPECT_EQ(verify2, JoinU16Le(frame2[6], frame2[7]));
    EXPECT_NE(crc1, crc2) << "Different payloads must produce different CRCs";
}
