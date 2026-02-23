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

#ifndef APD_SM_TOOL_TRIGGER_IN_H_
#define APD_SM_TOOL_TRIGGER_IN_H_

#include "apd/sm/triggerin_proxy.h"
#include "ara/core/string.h"
#include <memory>

namespace apd
{
namespace smtool
{

using apd::sm::proxy::TriggerInProxy;

class TriggerIn final
{
public:
    TriggerIn();
    /// @brief Sends a setstate request to state manager via the trigger in service interface.
    ///
    /// @param stateRequest The request for state manager in the form of "functiongroup.functiongroupstate"".
    void SendSetStateRequest(const ara::core::String& stateRequest);

private:
    /// @brief Initializes the proxy and starts the search for the trigger in service.
    void Init();
    /// @brief Callback that is used when the trigger in service was found.
    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<TriggerInProxy::HandleType> handles);
    std::unique_ptr<TriggerInProxy> proxy_;
};

}  // namespace smtool
}  // namespace apd

#endif  // APD_SM_TOOL_TRIGGER_IN_H_
