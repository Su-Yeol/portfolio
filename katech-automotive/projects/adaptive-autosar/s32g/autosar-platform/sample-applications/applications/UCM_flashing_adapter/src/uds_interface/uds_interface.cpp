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

#include <functional>
#include <future>
#include "ara/core/vector.h"
#include <cstring>
#include "ara/core/string.h"
#include <cstdio>
#include <iostream>
#include <algorithm>

#include "uds_interface.h"

namespace
{
void DPUApiEventCallback(T_PDU_EVT_DATA /*eventType*/,
    UNUM32 /*hMod_*/,
    UNUM32 /*hCll_*/,
    void* pCllTag,
    void* /*pAPITag*/)
{
    reinterpret_cast<UdsInterface*>(pCllTag)->eventsAvailable();
}
}  // namespace

UdsInterface::~UdsInterface()
{
    UNUM32 retVal;
    retVal = PDUDisconnect(hMod_, hCll_);
    logger_.LogInfo() << "PDUDisconnect() API called and returned" << ara::log::HexFormat(retVal);
    // Destroy CLL
    retVal = PDUDestroyComLogicalLink(hMod_, hCll_);
    logger_.LogInfo() << "PDUDestroyComLogicalLink() API called and returned" << ara::log::HexFormat(retVal);

    shutdown_ = true;
    cond_.notify_one();
    if (task_.joinable()) {
        logger_.LogInfo() << "Waiting for thread to shutdown";
        task_.join();
    }
}

void UdsInterface::eventsAvailable()
{
    logger_.LogInfo() << "eventsAvailable";
    eventsAvailable_ = true;
    cond_.notify_one();
}

UdsInterfaceReturnType UdsInterface::Init()
{
    // Check if D-PDU Api handle was already initializated
    if (!pduApiHandle_->isInit()) {
        if (!pduApiHandle_->init()) {
            logger_.LogError() << "DPDUAPI master init failed";
            return UdsInterfaceReturnType::kPduApiError;
        }
    }

    hMod_ = pduApiHandle_->getModuleHandle();

    constexpr uint16_t ressourceId = 50;
    // Create CLL, as cllTag the reference of this object is used be able to use method as callback
    if (PDUCreateComLogicalLink(hMod_, nullptr, ressourceId, reinterpret_cast<void*>(this), &hCll_, nullptr)) {
        logger_.LogError() << "PDUCreateComLogicalLink() failed";
        return UdsInterfaceReturnType::kPduApiError;
    }

    // Register callback and start event handler thread
    if (PDURegisterEventCallback(hMod_, hCll_, DPUApiEventCallback)) {
        logger_.LogError() << "PDURegisterEventCallback() failed";
        return UdsInterfaceReturnType::kPduApiError;
    }
    task_ = std::thread(&UdsInterface::eventReaderTask, this);

    PDU_UNIQUE_RESP_ID_TABLE_ITEM* pURIDItem = nullptr;
    if (PDUGetUniqueRespIdTable(hMod_, hCll_, &pURIDItem)) {
        logger_.LogError() << "PDUGetUniqueRespIdTable() failed";
        return UdsInterfaceReturnType::kPduApiError;
    }

    if (pURIDItem != nullptr) {
        // Setting the the CAN request id
        constexpr uint16_t cpCanReqId = 0;  // The CAN Request ID in the UIRD table
        *reinterpret_cast<UNUM32*>(pURIDItem->pUniqueData[0].pParams[cpCanReqId].pComParamData) = canReqId_;
        logger_.LogInfo() << "Using CAN request id" << ara::log::HexFormat(canReqId_);
        // Setting the CAN response id
        constexpr uint16_t cpCanRespId = 1;  // The CAN Response ID in the UIRD table
        *reinterpret_cast<UNUM32*>(pURIDItem->pUniqueData[0].pParams[cpCanRespId].pComParamData) = canRespId_;
        logger_.LogInfo() << "Using CAN response id" << ara::log::HexFormat(canRespId_);

        if (PDUSetUniqueRespIdTable(hMod_, hCll_, pURIDItem)) {
            logger_.LogError() << "PDUSetUniqueRespIdTable() failed";
            return UdsInterfaceReturnType::kPduApiError;
        }
        // Destruct the URID item
        if (PDUDestroyItem(reinterpret_cast<PDU_ITEM*>(pURIDItem))) {
            logger_.LogError() << "PDUDestroyItem() failed";
            return UdsInterfaceReturnType::kPduApiError;
        }
    }

    // connect PDU
    if (PDUConnect(hMod_, hCll_)) {
        logger_.LogError() << "PDUConnect() failed";
        return UdsInterfaceReturnType::kPduApiError;
    }
    logger_.LogInfo() << "Init finished successfully";

    return UdsInterfaceReturnType::kPositive;
}

UdsInterfaceReturnType UdsInterface::sendRequest(ara::core::Vector<uint8_t>& requestData)
{
    logger_.LogInfo() << "sendRequest";
    T_PDU_ERROR retVal;
    UNUM32 hCoP = 0;
    std::promise<void> promise;
    statusPromise_ = std::move(promise);
    responseContainer_.responseBuffer.clear();
    responseContainer_.errorStatus = PDU_ERR_EVT_NOERROR;
    retVal = PDUStartComPrimitive(
        hMod_, hCll_, PDU_COPT_SENDRECV, requestData.size(), requestData.data(), &copCtrlData_, nullptr, &hCoP);
    if (retVal != PDU_STATUS_NOERROR) {
        return UdsInterfaceReturnType::kPduApiError;
    }

    return UdsInterfaceReturnType::kPositive;
}

void UdsInterface::eventReaderTask()
{
    logger_.LogInfo() << "eventReaderTask started";
    T_PDU_ERROR eventStatus = PDU_STATUS_NOERROR;
    PDU_EVENT_ITEM* pEventItem = nullptr;
    while (true) {
        std::unique_lock<std::mutex> mlock(mutex_);
        cond_.wait(mlock, [this] { return eventsAvailable_ || shutdown_; });
        if (!shutdown_) {
            eventsAvailable_ = false;
            do {
                eventStatus = PDUGetEventItem(hMod_, hCll_, &pEventItem);
                if ((nullptr != pEventItem) && (PDU_ERR_EVENT_QUEUE_EMPTY != eventStatus)) {
                    logger_.LogInfo() << "COP handle: " << pEventItem->hCoP;
                    switch (pEventItem->ItemType) {
                    case PDU_IT_RESULT: {
                        // Process the results data:
                        UNUM8* dataBytes = reinterpret_cast<PDU_RESULT_DATA*>(pEventItem->pData)->pDataBytes;
                        UNUM32 dataBytesNum = reinterpret_cast<PDU_RESULT_DATA*>(pEventItem->pData)->NumDataBytes;
                        responseContainer_.responseBuffer.resize(dataBytesNum);
                        std::memcpy(responseContainer_.responseBuffer.data(), dataBytes, dataBytesNum);
                        logger_.LogInfo() << "Received" << dataBytesNum << "bytes";
                    } break;
                    case PDU_IT_STATUS: {
                        T_PDU_STATUS* copStatus = reinterpret_cast<T_PDU_STATUS*>(pEventItem->pData);
                        logger_.LogInfo() << "Status PDU_IT_STATUS processed"
                                          << ara::log::HexFormat(*reinterpret_cast<UNUM32*>(copStatus));
                        if (*copStatus == PDU_COPST_FINISHED) {
                            statusPromise_.set_value();
                        }
                    } break;
                    case PDU_IT_ERROR: {
                        responseContainer_.errorStatus
                            = reinterpret_cast<PDU_ERROR_DATA*>(pEventItem->pData)->ErrorCodeId;
                        logger_.LogInfo() << "Status PDU_IT_ERROR processed, got error:"
                                          << ara::log::HexFormat(static_cast<UNUM32>(responseContainer_.errorStatus));
                    } break;
                    case PDU_IT_INFO: {
                        logger_.LogInfo() << "Status PDU_IT_INFO processed";
                    } break;
                    default:;
                    }
                    PDUDestroyItem(reinterpret_cast<PDU_ITEM*>(pEventItem));
                }
            } while (PDU_ERR_EVENT_QUEUE_EMPTY != eventStatus);
        } else {
            break;
        }
    }
}

bool UdsInterface::validate(ResponseBuffer& response, ResponseBuffer& toValidate)
{
    if ((response.size() == 0) || (toValidate.size() == 0) || (toValidate.size() > response.size())) {
        return false;
    }

    for (uint32_t i = 0; i < toValidate.size(); i++) {
        if (toValidate[i] != response[i]) {
            return false;
        }
    }
    return true;
}

UdsInterfaceReturnType UdsInterface::requestAndValidate(RequestBuffer& request, ResponseBuffer& toValidate)
{
    ResponseBuffer response;
    return requestAndValidate(request, toValidate, response);
}

UdsInterfaceReturnType UdsInterface::requestAndValidate(RequestBuffer& request,
    ResponseBuffer& toValidate,
    ResponseBuffer& response)
{
    logger_.LogInfo() << "requestAndValidate";
    UdsInterfaceReturnType retVal = sendRequest(request);
    if (retVal != UdsInterfaceReturnType::kPositive) {
        return retVal;
    }
    std::future<void> future = statusPromise_.get_future();

    if (future.wait_for(std::chrono::seconds(10)) == std::future_status::timeout) {
        logger_.LogInfo() << "requestAndValidate timeout waiting for callback to finish!";
        return UdsInterfaceReturnType::kInternalError;
    } else {

        if (responseContainer_.errorStatus == PDU_ERR_EVT_RX_TIMEOUT) {
            return UdsInterfaceReturnType::kTimeout;
        } else if (responseContainer_.errorStatus != PDU_ERR_EVT_NOERROR) {
            return UdsInterfaceReturnType::kGeneralError;
        }
        response = responseContainer_.responseBuffer;
        if (validate(response, toValidate)) {
            logger_.LogInfo() << "response valid";
            return UdsInterfaceReturnType::kValid;
        } else {
            logger_.LogInfo() << "response invalid";
            return UdsInterfaceReturnType::kInvalid;
        }
    }
}
