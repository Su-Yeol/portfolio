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

/// @file
/// @brief Read data identifier implementation of DIAGApp01
/// @note Project: Adaptive Platform Test Applications
/// @ingroup DIAGApp01

#ifndef DIAGAPP01_DATA_IDENTIFIER_H_
#define DIAGAPP01_DATA_IDENTIFIER_H_

#include <memory>
#include <ara/com/types.h>
#include "ara/diag/dataidentifier_skeleton.h"
#include "ara/log/logger.h"

class DIAGApp01DataIdentifier : public ara::diag::skeleton::DataIdentifierSkeleton
{
    // Give DataIdentifierSkeleton an alias Skeleton
    using Skeleton = ara::diag::skeleton::DataIdentifierSkeleton;

private:
    // Private object data
    // value is written by Write()
    uint8_t numDIAGApp01DataIdentifier;

public:
    uint8_t readData{0x05};

    static constexpr uint8_t readAdd{0x03};

    ara::log::Logger& m_logger_dst3{
        ara::log::CreateLogger("DST3", "context for main module", ara::log::LogLevel::kVerbose)};

    /// @brief Constructor
    explicit DIAGApp01DataIdentifier(ara::com::InstanceIdentifier);

    /// @brief Destructor
    ~DIAGApp01DataIdentifier();

    /// @brief Write data field
    /// @param[in]       dataRecord Currently not used. Post-RID message payload.
    /// @return          Future that can be used to throw an exception.
    auto Write(const ByteArray& dataRecord) -> decltype(Skeleton::Write(dataRecord)) override;

    /// @brief Read data field
    /// @return          Future that can be used to access the data or throw an exception.
    auto Read() -> decltype(Skeleton::Read()) override;
};

#endif  // DIAGAPP01_DATA_IDENTIFIER_H_
