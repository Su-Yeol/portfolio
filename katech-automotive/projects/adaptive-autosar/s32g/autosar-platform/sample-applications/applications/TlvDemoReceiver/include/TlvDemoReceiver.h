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
///  @brief Demonstrate TLV function.
///

#ifndef TLV_DEMO_RECEIVER_H_
#define TLV_DEMO_RECEIVER_H_

#include "ara/log/logger.h"
#include "ara/com/sample/tlv_proxy.h"

#include <mutex>

namespace TlvDemo
{
class TlvDemoReceiver
{
    using Proxy = ara::com::sample::proxy::tlvProxy;

public:
    TlvDemoReceiver();

    ///
    ///  @brief Initializes TLV receiver.
    ///
    void Init();

    ///
    ///  @brief main function of TLV sender.
    ///
    void Act();

    ///
    ///  @brief service availability callback function.
    ///
    void ServiceAvailabilityCallback(ara::com::ServiceHandleContainer<Proxy::HandleType> handles,
        ara::com::FindServiceHandle handler);

private:
    /// @brief shared ptr to TLV proxy
    std::shared_ptr<Proxy> m_proxy_;

    /// @brief mutex to protect the operation of proxy
    std::mutex m_proxyMutex_;

    /// @brief logger to log some useful information
    ara::log::Logger& m_logger_{
        ara::log::CreateLogger("TLVR", "TLV Demo Receiver Component", ara::log::LogLevel::kInfo)};
};
}  // namespace TlvDemo
#endif
