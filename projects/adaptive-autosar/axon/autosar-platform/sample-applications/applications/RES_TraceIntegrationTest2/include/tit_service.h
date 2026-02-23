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

#ifndef TitService_H_
#define TitService_H_

#include "apd/wgres/tit/titservice_skeleton.h"
#include "ara/log/logger.h"  // Logger CreateLogger LogLevel

namespace apd
{
namespace wgres
{
namespace tit
{

class TitServiceImp : public apd::wgres::tit::skeleton::TitServiceSkeleton
{
    using Skeleton = apd::wgres::tit::skeleton::TitServiceSkeleton;

public:
    explicit TitServiceImp(ara::com::InstanceIdentifier instance_id);
    auto Action(const std::uint32_t& intensity) -> decltype(Skeleton::Action(intensity)) override;

    void StartSendingData();
    void StopSendingData();
    void SendPeriodic();

private:
    int periodMs_;
    bool terminate_;
    std::unique_ptr<std::thread> sendThread_;

private:
    /// @brief Logger for logging errors
    ara::log::Logger& logger_{ara::log::CreateLogger("TSAS", "Tit Service", ara::log::LogLevel::kVerbose)};
};
} /* namespace tit */
} /* namespace wgres */
} /* namespace apd */

#endif /* TitService_H_ */
