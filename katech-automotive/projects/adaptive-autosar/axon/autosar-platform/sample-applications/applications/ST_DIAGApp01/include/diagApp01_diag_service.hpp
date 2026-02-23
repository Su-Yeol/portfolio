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
/// @brief Management of the skeleton which provides the diagnostics services offered by DIAGApp01
/// @note Project: Adaptive Platform Test Applications
/// @ingroup DIAGApp01

#ifndef DIAGAPP01_DIAG_SERVICE_H_
#define DIAGAPP01_DIAG_SERVICE_H_
#include <memory>
#include "diagApp01_data_identifier.hpp"

class DIAGApp01DiagService
{
private:
    // skeleton offering E2E DataIdentifier service
    DIAGApp01DataIdentifier* skeletonDIAGApp01DataIdentifier{nullptr};

public:
    /// @brief Constructor
    DIAGApp01DiagService();

    /// @brief Destructor
    ~DIAGApp01DiagService();

    /// @brief Initialization
    /// @return          boolean
    /// @retval          TRUE:         successful
    /// @retval          FALSE:        error occurred
    bool Init();

    /// @brief Deinitialization
    /// @return          boolean
    /// @retval          TRUE:         successful
    /// @retval          FALSE:        error occurred
    bool DeInit();

    ara::log::Logger& m_logger_dst2{
        ara::log::CreateLogger("dst2", "context for main module", ara::log::LogLevel::kVerbose)};
};

#endif  // DIAGAPP01_DIAG_SERVICE_H_
