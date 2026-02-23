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

#include "ara/core/instance_specifier.h"
#include "ara/core/promise.h"
#include "TlvDemoSender.h"

#include <thread>

namespace TlvDemo
{
auto tlvImpl::State() -> decltype(Skeleton::State())
{
    using ara::com::sample::TlvStateDefs;
    tlvImpl::StateOutput output;

    m_tlvCount_++;

    output.state.temperature = 25.0;
    output.state.voltage = 12.0;

    // demonstrate TLV feature, send current always
    output.state.current = 0.35;

    // send state in 1/3 times
    output.state.state = TlvStateDefs::RS_AVAILABLE;
    if ((m_tlvCount_ % 3) != 0) {
        output.state.state.reset();
    }

    decltype(Skeleton::State())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

void tlvImpl::ProcessRequests()
{
    while (true) {
        std::chrono::time_point<std::chrono::system_clock> deadline
            = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
        auto request_finished = ProcessNextMethodCall();
        if (request_finished.wait_until(deadline) != ara::core::future_status::ready) {
            m_logger_ctx1_.LogFatal() << "Request took too long";
        } else {
            std::this_thread::sleep_until(deadline);
        }
    }
}

TlvDemoSender::TlvDemoSender()
{
    ara::core::InstanceSpecifier instanceSpec{"TlvDemoSender/TlvDemoSender/tlv_PPort"};
    m_logger_ctx2_.LogInfo() << "Port In Executable Ref:" << instanceSpec;

    m_skeleton_ = new tlvImpl(instanceSpec, ara::com::MethodCallProcessingMode::kPoll);

    // The instance id resolution is not mandatory for service creation (but could be an option)
    // here it's intended to list ids for the offered service instances
    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(instanceSpec);
    for (auto const& instanceId : instanceIDs) {
        m_logger_ctx2_.LogInfo() << "Service Instance offered:" << instanceId.ToString();
    }

    m_logger_ctx2_.LogDebug() << "object address" << this;
}

TlvDemoSender::~TlvDemoSender()
{
    delete m_skeleton_;
}

void TlvDemoSender::Init()
{
    m_logger_ctx2_.LogDebug() << "TlvDemoSender init()";
    m_skeleton_->OfferService();
}
}  // namespace TlvDemo
