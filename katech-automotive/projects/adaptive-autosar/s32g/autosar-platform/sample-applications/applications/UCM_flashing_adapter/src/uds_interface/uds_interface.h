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

#ifndef UDS_INTERFACE_UDS_INTERFACE_
#define UDS_INTERFACE_UDS_INTERFACE_

#include "ara/core/vector.h"
#include <atomic>
#include <future>
#include "ara/core/string.h"

#include "ara/log/logger.h"

#include "ucm/dpduapi/interface.h"
#include "pdu_api_handle.h"

enum class UdsInterfaceReturnType
{
    kPositive = 0,
    kNegative,
    kValid,
    kInvalid,
    kTimeout,
    kInternalError,
    kPduApiError,
    kGeneralError = 255
};

/// @brief Interface to communicate to ECU via CAN using UDS commands
class UdsInterface
{
public:
    typedef ara::core::Vector<UNUM8> RequestBuffer;
    typedef ara::core::Vector<UNUM8> ResponseBuffer;
    UdsInterface(std::shared_ptr<PduApiHandle>& pduApiHandle, uint32_t canReqId, uint32_t canRespId)
        : pduApiHandle_(pduApiHandle)
        , canReqId_(canReqId)
        , canRespId_(canRespId)
        , cond_()
        , eventsAvailable_(false)
        , shutdown_(false)
    {
        copCtrlData_.NumReceiveCycles = 1;
        copCtrlData_.NumSendCycles = 1;
    };
    ~UdsInterface();

    /// @brief Initialize UdsInterface and the underlying D-PDU API
    /// @return kPduApiError, if underlying D-PDU API returned an error
    UdsInterfaceReturnType Init();

    /// @brief Signaling internal eventReaderTask that new events have to read
    void eventsAvailable();

    /// @brief Send a given request to peer and checks the response against
    ///        given expected response
    /// @param request Request data to be send
    /// @param response received response
    /// @return kValid, if response was received and is valid
    ///         kInvalid, if response was received but was invalid
    ///         kTimeout, if no response was received in time
    ///         kInternalError, if an internal error occured
    ///         kPduApiError, if underlying D-PDU API returned an error
    UdsInterfaceReturnType requestAndValidate(RequestBuffer& request, ResponseBuffer& toValidate);

    /// @brief Send a given request to peer, checks the response against
    ///        given expected response and returns the received response
    /// @param request Request data to be send
    /// @param toValidate expected response data to check against
    /// @param response received response
    /// @return kValid, if response was received and is valid
    ///         kInvalid, if response was received but was invalid
    ///         kTimeout, if no response was received in time
    ///         kInternalError, if an internal error occured
    ///         kPduApiError, if underlying D-PDU API returned an error
    UdsInterfaceReturnType requestAndValidate(RequestBuffer& request,
        ResponseBuffer& toValidate,
        ResponseBuffer& response);

private:
    /// @brief Internal Datatype for response UDS handling
    struct ResponseContainer
    {
        T_PDU_ERR_EVT errorStatus;
        ResponseBuffer responseBuffer;
    };

    /// @brief Validate the response against a given expected response
    /// @param response response to check
    /// @param toValidate expected response to check against
    /// @return true if response is equal to expected response
    bool validate(ResponseBuffer& response, ResponseBuffer& toValidate);

    /// @brief Internal sending method to send UDS request over D-PDU API
    /// @param requestData data to be send via D-PDU API
    /// @return UdsInterfaceReturnType
    UdsInterfaceReturnType sendRequest(RequestBuffer& requestData);

    /// @brief Thread method used for reading event data
    void eventReaderTask();

    /// @brief Shared handle of D-PDU Api
    std::shared_ptr<PduApiHandle> pduApiHandle_;

    /// @brief Handle for used module
    UNUM32 hMod_ = 255u;

    /// @brief Handle for used com logical link
    UNUM32 hCll_ = 255u;

    /// @brief Control data used for D-PDU API com primitive
    PDU_COP_CTRL_DATA copCtrlData_;

    /// @brief CAN ID used for send CAN frames
    uint32_t canReqId_ = 0u;

    /// @brief CAN response ID (CAN ID of the ECU)
    uint32_t canRespId_ = 0u;

    /// @brief Internal promise used for syncing event thread and receive mehtod
    std::promise<void> statusPromise_;

    /// @brief Internal container holding the response of a UDS request
    ResponseContainer responseContainer_;

    /// @brief thread object of the task
    std::thread task_;

    /// @brief Mutex for condition variable
    mutable std::mutex mutex_;

    /// @brief Condition variable
    std::condition_variable cond_;

    /// @brief Flag for events availability
    std::atomic<bool> eventsAvailable_;

    /// @brief Flag for thread shutdown
    std::atomic<bool> shutdown_;

    ara::log::Logger& logger_{ara::log::CreateLogger("UDSI", "UDS Interface", ara::log::LogLevel::kVerbose)};
};

#endif  // UDS_INTERFACE_UDS_INTERFACE_
