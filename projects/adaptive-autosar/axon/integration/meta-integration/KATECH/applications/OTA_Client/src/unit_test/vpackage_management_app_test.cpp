// --------------------------------------------------------------------------
// |              _    _ _______     .----.      _____         _____        |
// |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
// |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
// |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
// |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
// |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
// |                              . _ _  .                                  |
// --------------------------------------------------------------------------
//
// All Rights Reserved.
// Any use of this source code is subject to a license agreement with the
// AUTOSAR development cooperation.
// More information is available at www.autosar.org.
//
// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "vpackage_management_app.h"

#include "boost/property_tree/json_parser.hpp"
#include "boost/property_tree/ptree.hpp"
#include <cstdio>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <vector>

namespace katech
{
namespace ucm
{
namespace ota
{

using ::testing::Eq;
using ::testing::StrEq;

class VpackageManagementAppTestFixture : public ::testing::Test
{

public:
    void SetUp()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    void TearDown()
    {
        std::cout << __FUNCTION__ << std::endl;
    }

    const ara::core::String kTestDataPath = "/usr/share/apdtest/ucm";
    const ara::core::String kTestInstallSwPackagePath = kTestDataPath + "/SWP_TEST1_INS.zip";
};

TEST_F(VpackageManagementAppTestFixture, LoadBinary)
{
    std::vector<uint8_t> file;
    ASSERT_TRUE(apd::ucm::ota::LoadBinaryFile(kTestInstallSwPackagePath, file));
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
