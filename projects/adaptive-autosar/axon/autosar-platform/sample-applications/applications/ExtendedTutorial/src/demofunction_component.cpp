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

#include "demofunction_component.h"

using OptionalHandle = ara::core::Optional<apd::proxy::TutorialInterfaceProxy::HandleType>;

DemoFunctionComponent::DemoFunctionComponent()
    : db_(nullptr)
    , proxy1_(nullptr)
    , proxy2_(nullptr)
{ }

void DemoFunctionComponent::init()
{
    logger_.LogInfo() << "Initializing DemoFunctionComponent...";

    logger_.LogInfo() << "Opening key-value-storage";
    ara::core::InstanceSpecifier kvsSpecifier{"ExtendedTutorial/DemoFunction/PRKvs"};
    auto result = ara::per::OpenKeyValueStorage(kvsSpecifier);

    if (!result) {
        logger_.LogError() << "Failed to open key-value-storage of InstanceSpecifier" << kvsSpecifier
                           << "due to:" << result.Error();
    } else {
        db_ = std::move(result).Value();
        logger_.LogInfo() << "Key-value-storage successfully opened";
    }

    ara::core::InstanceSpecifier portSpecifier{"ExtendedTutorial/DemoFunction/TutIf"};
    logger_.LogInfo() << "Port In Executable Ref:" << portSpecifier.ToString();

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        throw std::runtime_error{"No InstanceIdentifiers resolved from provided InstanceSpecifier"};
    }

    while (nullptr == proxy1_ || nullptr == proxy2_) {
        logger_.LogInfo() << "TutorialInterface FindService instance " << instanceIDs[0].ToString();
        auto handlesResult = apd::proxy::TutorialInterfaceProxy::FindService(portSpecifier);
        if (handlesResult.HasValue()) {
            logger_.LogInfo() << "TutorialInterface found number of instances: " << handlesResult.Value().size();
        } else {
            logger_.LogError() << "FindService returned error for TutorialInterface";
        }

        if (handlesResult.HasValue() && handlesResult.Value().size() > 0) {
            for (uint8_t foundInstanceIdx = 0; foundInstanceIdx < handlesResult.Value().size(); foundInstanceIdx++) {
                auto foundInstance = handlesResult.Value()[foundInstanceIdx].GetInstanceId().ToString();
                if (nullptr == proxy1_ && foundInstance == "SOME/IP:1") {
                    proxy1_
                        = std::make_shared<apd::proxy::TutorialInterfaceProxy>(handlesResult.Value()[foundInstanceIdx]);
                    logger_.LogInfo() << "Created proxy from handle with instance: " << foundInstance;
                    proxy1_->Data.Subscribe(1);  // subscribe to event with queue length 1
                    logger_.LogInfo() << "Subscription to event Data complete";
                }
                if (nullptr == proxy2_ && foundInstance == "SOME/IP:2") {
                    proxy2_
                        = std::make_shared<apd::proxy::TutorialInterfaceProxy>(handlesResult.Value()[foundInstanceIdx]);
                    logger_.LogInfo() << "Created proxy from handle with instance: " << foundInstance;
                    proxy2_->Data.Subscribe(1);  // subscribe to event with queue length 1
                    logger_.LogInfo() << "Subscription to event Data complete";
                }
                if (foundInstance != "SOME/IP:1" && foundInstance != "SOME/IP:2") {
                    logger_.LogError() << "Unexpected instanceID found: " << foundInstance;
                }
            }
        }
        if (nullptr == proxy1_ || nullptr == proxy2_) {
            std::this_thread::sleep_for(
                std::chrono::milliseconds(500));  // wait for service provider (SWC DemonstratorTutorial) to come up
        }
    }
}

void DemoFunctionComponent::act()
{
    logger_.LogInfo() << "DemoFunctionComponent is running (" << counterValue_++ << ")";

    if (db_) {
        if (!db_->KeyExists("sum") || !db_->GetValue<std::uint32_t>("sum").HasValue()) {
            db_->SetValue("sum", counterValue_);
            logger_.LogWarn() << "Initialized DB sum =" << counterValue_;
        } else {
            std::uint32_t sum = db_->GetValue<std::uint32_t>("sum").Value();
            sum += counterValue_;
            db_->SetValue("sum", sum);
            logger_.LogInfo() << "Updated DB sum =" << sum;
        }
    }

    proxy1_->Data.GetNewSamples([&](ara::com::SamplePtr<const uint32_t> pData) {
        logger_.LogInfo() << "Received From Instance 1: " << *pData;
    });
    proxy2_->Data.GetNewSamples([&](ara::com::SamplePtr<const uint32_t> pData) {
        logger_.LogInfo() << "Received From Instance 2: " << *pData;
    });
}

void DemoFunctionComponent::shutdown()
{
    logger_.LogInfo() << "Shutting down DemoFunctionComponent...";
    if (db_) {
        db_->SyncToStorage();
    }
}
