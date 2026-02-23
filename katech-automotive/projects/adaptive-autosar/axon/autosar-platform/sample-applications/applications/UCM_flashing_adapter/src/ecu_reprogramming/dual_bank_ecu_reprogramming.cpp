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

#include "dual_bank_ecu_reprogramming.h"
#include "ecu_reprogramming_sequence_error_domain.h"

uint8_t DualBankEcuReprogramming::GetProgress()
{
    return progress_;
}

ara::core::Future<void> DualBankEcuReprogramming::Install()
{
    const ProgConf::FileList& filePathList = progConf_->getFlashFiles();
    return doProgramming(filePathList);
}

ara::core::Future<void> DualBankEcuReprogramming::Activate()
{
    ara::core::Promise<void> promise;
    UdsCommand request;

    auto future = doDependencyCheck();
    if (!future.GetResult()) {
        logger_.LogError() << "Dependeny Check failed";
        return future;
    }

    if (!progConf_->getUdsCommandByName("activation", request)) {
        logger_.LogError() << "Get command activation failed";
        promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
        return promise.get_future();
    }
    if (udsInterface_.requestAndValidate(request.command, request.positiveResponse) != UdsInterfaceReturnType::kValid) {
        logger_.LogError() << "activation command failed";
        promise.SetError(RepSequenceErrorDomainErrc::kActivationFailed);
        return promise.get_future();
    }

    future = doEcuReset();
    if (!future.GetResult()) {
        return future;
    }

    promise.set_value();

    return promise.get_future();
    ;
}
ara::core::Future<void> DualBankEcuReprogramming::Verify()
{
    progress_ = 100u;
    return doReadVersion("readNewVersion");
}

ara::core::Future<void> DualBankEcuReprogramming::Rollback()
{
    ara::core::Promise<void> promise;
    UdsCommand request;

    auto future = enterProgrammingSession();
    if (!future.GetResult()) {
        return future;
    }

    if (!progConf_->getUdsCommandByName("rollback", request)) {
        logger_.LogError() << "Get command rollback failed";
        promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
        return promise.get_future();
    }
    if (udsInterface_.requestAndValidate(request.command, request.positiveResponse) != UdsInterfaceReturnType::kValid) {
        logger_.LogError() << "rollback command failed";
        promise.SetError(RepSequenceErrorDomainErrc::kRollbackFailed);
        return promise.get_future();
    }

    future = doEcuReset();
    if (!future.GetResult()) {
        return future;
    }

    future = doReadVersion("readPrevVersion");
    if (!future.GetResult()) {
        return future;
    }
    logger_.LogInfo() << "Rollback step finished successfully";
    promise.set_value();
    return promise.get_future();
}
