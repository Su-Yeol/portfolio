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

#include "reportalive_component.h"

void ReportAliveComponent::init()
{
    logger_.LogInfo() << "Initializing ReportAliveComponent...";

    ara::core::InstanceSpecifier portSpecifier1{"DemonstratorTutorial/ReportAlive/TutIfInst1"};
    logger_.LogInfo() << "Instance 1 Port In Executable Ref:" << portSpecifier1.ToString();
    skeletonInstance1_ = std::make_shared<apd::skeleton::TutorialInterfaceSkeleton>(
        portSpecifier1, ara::com::MethodCallProcessingMode::kEvent);
    skeletonInstance1_->OfferService();
    logger_.LogInfo() << "TutorialInterface Instance 1 offered";

    ara::core::InstanceSpecifier portSpecifier2{"DemonstratorTutorial/ReportAlive/TutIfInst2"};
    logger_.LogInfo() << "Instance 2 Port In Executable Ref:" << portSpecifier2.ToString();
    skeletonInstance2_ = std::make_shared<apd::skeleton::TutorialInterfaceSkeleton>(
        portSpecifier2, ara::com::MethodCallProcessingMode::kEvent);
    skeletonInstance2_->OfferService();
    logger_.LogInfo() << "TutorialInterface Instance 2 offered";
}

void ReportAliveComponent::act()
{
    logger_.LogInfo() << "ReportAliveComponent is running (" << counterValue_++ << ")";
    logger_.LogInfo() << "TutorialInterface Instance 1 sends 1";
    skeletonInstance1_->Data.Send(1);
    logger_.LogInfo() << "TutorialInterface Instance 2 sends 2";
    skeletonInstance2_->Data.Send(2);
}
