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

#include <iostream>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <functional>
#include "ara/core/vector.h"
#include <cstring>

#include "uds_interface.h"
#include "flash_data.h"
#include "ecu_reprogramming_sequence.h"
#include "ecu_reprogramming_sequence_error_domain.h"

EcuReprogrammingSequence::EcuReprogrammingSequence(std::shared_ptr<PduApiHandle>& pduApiHandle,
    std::unique_ptr<ProgConf> progConf,
    ara::core::String path)
    : progConf_{std::move(progConf)}
    , pathToFiles_(path)
    , udsInterface_{pduApiHandle, progConf_->getCanRequestId(), progConf_->getCanResponseId()}
{
    logger_.LogInfo() << "EcuReprogrammingSequence start";
}

uint8_t EcuReprogrammingSequence::GetProgress()
{
    return progress_;
}

ara::core::Future<void> EcuReprogrammingSequence::Init()
{
    ara::core::Promise<void> promise;

    if (udsInterface_.Init() != UdsInterfaceReturnType::kPositive) {
        logger_.LogError() << "UDS interface could not be initialized!";
        promise.SetError(RepSequenceErrorDomainErrc::kInitializationFailed);
        return promise.get_future();
    }
    promise.set_value();
    return promise.get_future();
}

ara::core::Future<void> EcuReprogrammingSequence::downloadFirmware(FlashData& flashData)
{
    ara::core::Promise<void> promise;
    UdsCommand request;
    UdsInterface::ResponseBuffer response;
    const ara::core::Vector<uint8_t> segmentBaseAddress = flashData.getSegmentBaseAddressAsVector();
    const ara::core::Vector<uint8_t> payloadLengthVec = flashData.getAbsoluteDataSizeAsVector();

    if (!progConf_->getUdsCommandByName("requestDownload", request)) {
        logger_.LogError() << "Get command requestDownload failed";
        promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
        return promise.get_future();
    }
    request.command.insert(request.command.end(), segmentBaseAddress.begin() + 1, segmentBaseAddress.end());
    request.command.insert(request.command.end(), payloadLengthVec.begin() + 1, payloadLengthVec.end());
    if (udsInterface_.requestAndValidate(request.command, request.positiveResponse, response)
        != UdsInterfaceReturnType::kValid) {
        logger_.LogError() << "requestDownload failed";
        promise.SetError(RepSequenceErrorDomainErrc::kRequestDownloadFailed);
        return promise.get_future();
    }
    logger_.LogInfo() << "Request Download was successful";

    // we should use the block count field of UDS response but we send the blocks as read from hex file

    if (!progConf_->getUdsCommandByName("transferData", request)) {
        logger_.LogError() << "Get command transferData failed";
        promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
        return promise.get_future();
    }
    uint32_t blockCnt = 0;
    for (const auto& dataRecord : flashData.getDataRecords()) {
        ara::core::Vector<uint8_t> transferDataRequest;
        transferDataRequest.push_back(request.command[0]);
        transferDataRequest.push_back(blockCnt);
        transferDataRequest.insert(transferDataRequest.end(), dataRecord.data.begin(), dataRecord.data.end());
        if (udsInterface_.requestAndValidate(transferDataRequest, request.positiveResponse, response)
            != UdsInterfaceReturnType::kValid) {
            logger_.LogError() << "transferData failed for block num: " << blockCnt;
            promise.SetError(RepSequenceErrorDomainErrc::kTransferDataFailed);
            return promise.get_future();
        }
        logger_.LogInfo() << "Transfer Data of block:" << blockCnt << "was successful";
        blockCnt++;
    }
    logger_.LogInfo() << "Transfer Data was successful";

    if (!progConf_->getUdsCommandByName("requestTransferExit", request)) {
        logger_.LogError() << "Get command requestTransferExit failed";
        promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
        return promise.get_future();
    }
    if (udsInterface_.requestAndValidate(request.command, request.positiveResponse, response)
        != UdsInterfaceReturnType::kValid) {
        logger_.LogError() << "requestTransferExit command failed";
        promise.SetError(RepSequenceErrorDomainErrc::kRequestTransferExitFailed);
        return promise.get_future();
    }
    logger_.LogInfo() << "Request Transfer Exit was successful";
    promise.set_value();
    return promise.get_future();
}

ara::core::Future<void> EcuReprogrammingSequence::eraseRoutine(FlashData& flashData)
{
    ara::core::Promise<void> promise;
    UdsCommand request;
    UdsInterface::ResponseBuffer response;
    if (!progConf_->getUdsCommandByName("eraseRoutine", request)) {
        logger_.LogError() << "Get command eraseRoutine failed";
        promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
        return promise.get_future();
    }

    const ara::core::Vector<uint8_t> segmentBaseAddress = flashData.getSegmentBaseAddressAsVector();
    const ara::core::Vector<uint8_t> payloadLengthVec = flashData.getAbsoluteDataSizeAsVector();

    request.command.insert(request.command.end(), segmentBaseAddress.begin() + 1, segmentBaseAddress.end());
    request.command.insert(request.command.end(), payloadLengthVec.begin() + 1, payloadLengthVec.end());

    if (udsInterface_.requestAndValidate(request.command, request.positiveResponse, response)
        != UdsInterfaceReturnType::kValid) {
        logger_.LogError() << "eraseRoutine command failed";
        promise.SetError(RepSequenceErrorDomainErrc::kEraseRoutineFailed);
        return promise.get_future();
    }
    logger_.LogInfo() << "Erase Routine was successful";
    promise.set_value();
    return promise.get_future();
}

bool EcuReprogrammingSequence::getFlashFileData(ara::core::String filePath, FlashData& flashData)
{
    std::ifstream intelHexInput;
    logger_.LogInfo() << "getFlashFileData" << filePath;
    intelHexInput.open(filePath.c_str(), std::ifstream::in);
    if (!intelHexInput.good()) {
        logger_.LogError() << "Couldn't open" << filePath;
        return false;
    }

    intelHexInput >> flashData;
    if (flashData.hasError()) {
        logger_.LogError() << "Flash file could not be parsed successfully!";
        return false;
    }

    return true;
}

ara::core::Future<void> EcuReprogrammingSequence::enterProgrammingSession()
{
    ara::core::Promise<void> promise;
    UdsCommand request;
    UdsInterface::ResponseBuffer response;

    if (!progConf_->getUdsCommandByName("reprogrammingSession", request)) {
        logger_.LogError() << "Get command reprogrammingSession failed";
        promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
        return promise.get_future();
    }
    if (udsInterface_.requestAndValidate(request.command, request.positiveResponse) != UdsInterfaceReturnType::kValid) {
        logger_.LogError() << "reprogrammingSession command failed";
        promise.SetError(RepSequenceErrorDomainErrc::kReprogrammingSessionSwitchFailed);
        return promise.get_future();
    }
    logger_.LogInfo() << "Extended Session was successful";
    logger_.LogInfo() << "Enter Programming Session was successful";
    promise.set_value();
    return promise.get_future();
}

ara::core::Future<void> EcuReprogrammingSequence::doEcuReset()
{
    ara::core::Promise<void> promise;
    UdsCommand request;

    if (!progConf_->getUdsCommandByName("ecuReset", request)) {
        logger_.LogError() << "Get command ecuReset failed";
        promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
        return promise.get_future();
    }
    if (udsInterface_.requestAndValidate(request.command, request.positiveResponse) != UdsInterfaceReturnType::kValid) {
        logger_.LogError() << "ecuReset command failed";
        promise.SetError(RepSequenceErrorDomainErrc::kEcuResetFailed);
        return promise.get_future();
    }
    logger_.LogInfo() << "Ecu Reset was successful";
    promise.set_value();
    return promise.get_future();
}

ara::core::Future<void> EcuReprogrammingSequence::doProgramming(const ProgConf::FileList& filePathList)
{
    ara::core::Promise<void> promise;
    UdsCommand request;

    auto future = enterProgrammingSession();
    if (!future.GetResult()) {
        return future;
    }
    progress_ = 20u;

    uint32_t fileCnt = 1u;
    for (const auto& file : filePathList) {
        FlashData flashData;

        if (!getFlashFileData(pathToFiles_ + "/" + file, flashData)) {
            logger_.LogError() << "File open and parse failed";
            promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
            return promise.get_future();
        }
        auto result = eraseRoutine(flashData).GetResult();
        if (result) {
            logger_.LogInfo() << "eraseRoutine completed for file" << file;
        } else {
            logger_.LogError() << "eraseRoutine failed for file" << file << "with error:" << result.Error();
            return promise.get_future();
        }

        result = downloadFirmware(flashData).GetResult();
        ;
        if (result) {
            logger_.LogInfo() << "downloadFirmware completed for file num" << file;
        } else {
            logger_.LogError() << "downloadFirmware failed for file num" << file << "with error:" << result.Error();
            return promise.get_future();
        }
        progress_ += (60u / fileCnt);
        fileCnt++;
    }

    promise.set_value();
    return promise.get_future();
}
ara::core::Future<void> EcuReprogrammingSequence::doDependencyCheck()
{
    ara::core::Promise<void> promise;
    UdsCommand request;
    if (!progConf_->getUdsCommandByName("dependencyCheck", request)) {
        logger_.LogError() << "Get command dependencyCheck failed";
        promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
        return promise.get_future();
    }
    if (udsInterface_.requestAndValidate(request.command, request.positiveResponse) != UdsInterfaceReturnType::kValid) {
        logger_.LogError() << "dependencyCheck command failed";
        promise.SetError(RepSequenceErrorDomainErrc::kDependencyCheckFailed);
        return promise.get_future();
    }
    logger_.LogInfo() << "Dependency Check finished successfully";
    promise.set_value();
    return promise.get_future();
}

ara::core::Future<void> EcuReprogrammingSequence::doReadVersion(ara::core::String udsCommandName)
{
    ara::core::Promise<void> promise;
    UdsCommand request;
    UdsInterface::ResponseBuffer response;
    if (!progConf_->getUdsCommandByName(udsCommandName, request)) {
        logger_.LogError() << "Get command" << udsCommandName << "failed";
        promise.SetError(RepSequenceErrorDomainErrc::kConfigurationInvalid);
        return promise.get_future();
    }
    UdsInterfaceReturnType retVal
        = udsInterface_.requestAndValidate(request.command, request.positiveResponse, response);

    if (retVal == UdsInterfaceReturnType::kInvalid) {
        logger_.LogError() << "Received version differs from expected version";
        promise.SetError(RepSequenceErrorDomainErrc::kVersionIncorrect);
        return promise.get_future();
    } else if (retVal != UdsInterfaceReturnType::kValid) {
        logger_.LogError() << "versionCheck command failed";
        promise.SetError(RepSequenceErrorDomainErrc::kReadVersionFailed);
        return promise.get_future();
    }

    logger_.LogInfo() << "Read Version finished successfully";
    logger_.LogInfo() << "New version:" << GetEcuSwVersion();
    promise.set_value();
    return promise.get_future();
}

ara::core::String EcuReprogrammingSequence::GetEcuSwName()
{
    return progConf_->GetEcuSwName();
}

ara::core::String EcuReprogrammingSequence::GetEcuSwVersion()
{
    return progConf_->GetEcuSwVersion();
}
