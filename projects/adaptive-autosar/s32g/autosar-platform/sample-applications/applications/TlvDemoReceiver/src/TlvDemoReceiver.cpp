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

#include "ara/core/abort.h"
#include "ara/core/future.h"
#include "TlvDemoReceiver.h"

#include <thread>

namespace TlvDemo
{
TlvDemoReceiver::TlvDemoReceiver()
    : m_proxy_(nullptr)
{ }

void TlvDemoReceiver::ServiceAvailabilityCallback(ara::com::ServiceHandleContainer<Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    for (auto it : handles) {
        m_logger_.LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
    }
    if (handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxyMutex_);
        if (nullptr == m_proxy_) {
            m_proxy_ = std::make_shared<Proxy>(handles[0]);
            m_logger_.LogInfo() << "Created proxy from handle with instance: "
                                << m_proxy_->GetHandle().GetInstanceId().ToString();
            // Construct some handles (implementation-specific).
            auto first_handle = handles[0];
            auto aux_handle = first_handle;
            for (auto current_handle : handles) {
                // Call equality operator.
                m_logger_.LogInfo() << "Check handle::operator==: "
                                    << static_cast<std::uint8_t>(aux_handle == current_handle);
                // Call copy assignment operator.
                aux_handle = current_handle;
                // Call less-than operator.
                m_logger_.LogInfo() << "Check handle::operator<: "
                                    << static_cast<std::uint8_t>(aux_handle < first_handle);
            }
        }
    }
}

void TlvDemoReceiver::Init()
{
    m_logger_.LogInfo() << "TlvDemoReceiver::init() enter, resolve instance and find service";

    ara::core::InstanceSpecifier portSpecifier{"TlvDemoReceiver/TlvDemoReceiver/tlv_RPort"};
    m_logger_.LogInfo() << "Port In Executable Ref:" << portSpecifier;

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        ara::core::Abort("No InstanceIdentifiers resolved from provided InstanceSpecifier");
    }
    m_logger_.LogInfo() << "Searching for Service Instance:" << instanceIDs[0].ToString();

    auto services = Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<Proxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            TlvDemoReceiver::ServiceAvailabilityCallback(std::move(handles), handler);
        },
        portSpecifier);
    if (!services) {
        m_logger_.LogError() << "StartFindService failed with error: " << services.Error();
        ara::core::Abort("StartFindService failed");
    }

    m_logger_.LogInfo() << "TlvDemoReceiver::init() exit";
}

void TlvDemoReceiver::Act()
{
    if (m_proxy_ == nullptr) {
        m_logger_.LogInfo() << "Proxy is empty... waiting...";
        return;
    }

    auto state_future = m_proxy_->State();
    while (!state_future.is_ready()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        m_logger_.LogInfo() << "Waiting for tlv state info ...";
    }

    auto state_result = state_future.GetResult();
    auto state_output = state_result.Value();
    m_logger_.LogInfo() << "temperature: " << state_output.state.temperature;
    m_logger_.LogInfo() << "voltage: " << state_output.state.voltage;

    // check if optional variable has a value
    if (state_output.state.current) {
        m_logger_.LogInfo() << "current: " << *state_output.state.current;
    } else {
        m_logger_.LogWarn() << "current is not sent.";
    }

    if (state_output.state.state.has_value()) {
        using ara::com::sample::TlvStateDefs;
        if (*state_output.state.state == TlvStateDefs::RS_AVAILABLE) {
            m_logger_.LogInfo() << "state: Available ";
        }
        if (*state_output.state.state == TlvStateDefs::RS_OFF) {
            m_logger_.LogInfo() << "state: Off ";
        }
    } else {
        m_logger_.LogWarn() << "state is not sent.";
    }
}
}  // namespace TlvDemo
