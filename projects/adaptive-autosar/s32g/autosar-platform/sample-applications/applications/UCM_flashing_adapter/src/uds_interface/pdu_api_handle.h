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

#ifndef UDS_INTERFACE_PDU_API_HANDLE_
#define UDS_INTERFACE_PDU_API_HANDLE_

#include "ara/log/logger.h"

#include "ucm/dpduapi/interface.h"

/// @brief COntainer class for D-PDU APi Module init and deinit.
///        It enables shared using of D-PDU Api Module
class PduApiHandle
{
public:
    PduApiHandle()
    {
        logger_.LogInfo() << "PduApiHandle created ";
    };
    ~PduApiHandle();

    /// @brief Initializates D-PDU Api
    /// @return true if D-PDU Api was successful, otherwise false
    bool init();

    /// @brief Returns module handle of D-PDU Api
    /// @return true if D-PDU Api is initializated, otherwise false
    UNUM32 getModuleHandle() const;

    /// @brief Returns internal init. state
    /// @return true if D-PDU Api is initializated, otherwise false
    bool isInit() const;

private:
    /// @brief Flag for init. state of D-PDU Api
    bool isInit_ = false;

    /// @brief Handle for used module of D-PDU Api
    UNUM32 hMod_ = 255u;

    ara::log::Logger& logger_{ara::log::CreateLogger("PDHA", "Pdu-Api Handle", ara::log::LogLevel::kVerbose)};
};

#endif  // UDS_INTERFACE_PDU_API_HANDLE_
