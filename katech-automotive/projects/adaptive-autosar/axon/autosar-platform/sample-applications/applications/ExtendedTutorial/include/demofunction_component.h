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

#ifndef DEMO_FUNCTION_COMPONENT_HPP_
#define DEMO_FUNCTION_COMPONENT_HPP_

#include "ara/log/logger.h"
#include "ara/per/key_value_storage.h"
#include "ara/per/per_error_domain.h"
#include "apd/tutorialinterface_proxy.h"

#include <cstdint>

class DemoFunctionComponent
{
public:
    DemoFunctionComponent();
    void init();
    void act();
    void shutdown();

private:
    std::uint32_t counterValue_ = 0;

    ara::log::Logger& logger_{ara::log::CreateLogger("DeFu", "Demo Function Component", ara::log::LogLevel::kVerbose)};
    ara::per::SharedHandle<ara::per::KeyValueStorage> db_;
    std::shared_ptr<apd::proxy::TutorialInterfaceProxy> proxy1_;
    std::shared_ptr<apd::proxy::TutorialInterfaceProxy> proxy2_;
};

#endif  // DEMO_FUNCTION_COMPONENT_HPP_
