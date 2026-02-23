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

/*!
 *  \file
 *  \brief Specification of communication management validator publisher.
 *
 *  \details Contains specification of communication management validator publisher.
 *  Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef CM_PROVIDERSUBSCRIBERSCENARIO_CM_VALIDATOR_PUBLISHER_IMPL_H_
#define CM_PROVIDERSUBSCRIBERSCENARIO_CM_VALIDATOR_PUBLISHER_IMPL_H_

#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

// includes for used services
#include "ara/com/sample/cmvalidatorpublisher_skeleton.h"

#include <string>
#include <random>

namespace cmvalidatorpublisher
{

/*!
 *  \brief Class implementing skeleton methods.
 *
 *  CMV Publisher's implementation.
 *
 *  \uptrace{SWS_CM_00191}
 */
class CMValidatorPublisherImp : public ara::com::sample::skeleton::CMValidatorPublisherSkeleton
{
    using Skeleton = ara::com::sample::skeleton::CMValidatorPublisherSkeleton;

public:
    CMValidatorPublisherImp(ara::core::InstanceSpecifier instanceSpec, ara::com::MethodCallProcessingMode mode)
        : Skeleton(std::move(instanceSpec), mode)
        , worker_(&CMValidatorPublisherImp::ProcessRequests, this)
        , methodsCapabilityCount_(0)
        , randEng_(rd_())
        , ud_0_100_(0, 100)
    { }

    virtual ~CMValidatorPublisherImp()
    {
        finished_ = true;
        worker_.join();
    }

    // /// @uptrace{SWS_CM_90435}
    virtual void Echo(const String& text) override;

    virtual auto MethodsCapability() -> decltype(Skeleton::MethodsCapability()) override;

private:
    /*!
     * \brief Defines how the incoming service method invocations are processed.
     *
     * \uptrace{SWS_CM_00198}
     * \uptrace{SWS_CM_00199}
     */
    void ProcessRequests();

    /*!
     * \brief Parameters for checking method, fields and events capabilities
     */
    std::atomic<bool> finished_{false};
    std::thread worker_;
    std::uint32_t methodsCapabilityCount_;
    std::random_device rd_;
    std::default_random_engine randEng_;
    std::uniform_int_distribution<std::uint16_t> ud_0_100_;

    ara::log::Logger& loggerCtxImp_{
        ara::log::CreateLogger("CMVPI", "context for implementation", ara::log::LogLevel::kInfo)};

    ara::log::Logger& loggerCtxMc_{
        ara::log::CreateLogger("CTXMC", "context for methods capability", ara::log::LogLevel::kInfo)};

    ara::log::Logger& loggerCtxFfmc_{
        ara::log::CreateLogger("CTXU", "context for fire and forget methods capability", ara::log::LogLevel::kInfo)};
};
}  // namespace cmvalidatorpublisher

#endif  // CM_PROVIDERSUBSCRIBERSCENARIO_CM_VALIDATOR_PUBLISHER_IMPL_H_
