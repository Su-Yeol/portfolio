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

///
///  @brief demostration of TLV function.
///

#ifndef TLV_DEMO_SENDER_H_
#define TLV_DEMO_SENDER_H_

#include "ara/log/logger.h"
#include "ara/com/sample/tlv_skeleton.h"

namespace TlvDemo
{
class tlvImpl : public ara::com::sample::skeleton::tlvSkeleton
{
    using Skeleton = ara::com::sample::skeleton::tlvSkeleton;

public:
    tlvImpl(ara::core::InstanceSpecifier instanceSpec, ara::com::MethodCallProcessingMode mode)
        : Skeleton(std::move(instanceSpec), mode)
        , m_tlvCount_{0}
        , m_worker_(&tlvImpl::ProcessRequests, this)
    { }

    virtual ~tlvImpl()
    {
        m_worker_.join();
    }

    ///
    ///  @brief function used to demonstrate TLV feature.
    ///
    virtual auto State() -> decltype(Skeleton::State()) override;

private:
    void ProcessRequests();

    /// @brief counter used to count the called times of State() function
    int m_tlvCount_;

    /// @brief thread used for ProcessRquest()
    std::thread m_worker_;

    /// @brief logger used to log some useful information
    ara::log::Logger& m_logger_ctx1_{ara::log::CreateLogger("TLVI", "TLV implementation", ara::log::LogLevel::kInfo)};
};

class TlvDemoSender
{
public:
    TlvDemoSender();
    ~TlvDemoSender();

    ///
    ///  @brief Initializes TLV sender.
    ///
    void Init();

private:
    /// @brief TLV skeleton
    ara::com::sample::skeleton::tlvSkeleton* m_skeleton_;

    /// @brief logger to log some useful information
    ara::log::Logger& m_logger_ctx2_{ara::log::CreateLogger("TLVS", "TLV demo sender", ara::log::LogLevel::kInfo)};
};
}  // namespace TlvDemo
#endif
