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

#ifndef APD_UCM_VDA_SAMPLE_DRIVER_INTERFACE_SWC_H_
#define APD_UCM_VDA_SAMPLE_DRIVER_INTERFACE_SWC_H_

#include <memory>
#include <mutex>

#include "ara/log/logger.h"
#include "apd/rest/server.h"
#include "apd/rest/routing.h"

#include "ara/vucm/vehicledriverapplicationinterface_proxy.h"

#include "ara_software_update.h"

class DriverInterfaceSwc
{
public:
    std::thread requestLoopThread;

    explicit DriverInterfaceSwc(std::shared_ptr<ara::vucm::proxy::VehicleDriverApplicationInterfaceProxy>& proxy);

    DriverInterfaceSwc(const DriverInterfaceSwc&) = delete;
    DriverInterfaceSwc& operator=(const DriverInterfaceSwc&) = delete;
    DriverInterfaceSwc(DriverInterfaceSwc&&) = delete;
    DriverInterfaceSwc& operator=(DriverInterfaceSwc&&) = delete;
    ~DriverInterfaceSwc();

    bool StartMyrequestLoopThread();
    [[noreturn]] void RequestLoopThreadFunction();

    /// @brief Information about the software packages handles in the campaign.
    ///
    /// @return a vector of SwDescType.
    bool RequestSwPackageDescriptions();

    /// @brief Information about the Vehicle Package  .
    ///
    /// @return a vector of SwDescType.
    bool RequestVehiclePackageDescriptions();

    /// @brief Information about the software clusters installed in the .
    ///
    /// @return a vector of SwDescType.
    bool RequestSwClusterDescriptions();

    /// @brief Information about the progrees of processing of the campaign.
    ///
    /// @return true if the request succeeded.
    bool RequestProgressInformation();

    /// @brief Information about the progrees of processing of the campaign.
    ///
    /// @return true if the request succeeded.
    bool RequestTransferInformation();
    /// @brief Information about the progrees of processing of the campaign.
    ///
    /// @return true if the request succeeded.
    bool RequestCampignHistory();
    /// @brief Getter for the approvalRequired variable
    ///
    /// @return approvalRequired_
    bool GetApprovalRequired() const;

    void SetApprovalRequired(bool approvalRequired);

    /// @brief Sends response to server if driver approval is requested
    ///
    /// @param driverResponse resolution of the driver
    /// @return acnokledge of receiving from server
    bool RespondApprovalRequest(bool driverResponse);

    /// @brief Check the expected status.
    ///
    /// @param askedStatus a status it should be
    /// @return True if the current status is equal to askedStatus, else false.
    bool IsCorrectCampaignState(const ara::vucm::CampaignStateType askedStatus) const;

    /// @brief Getter for the proxy_ object
    ///
    /// @return proxy_ object
    ara::vucm::proxy::VehicleDriverApplicationInterfaceProxy& GetAppProxy();

    std::mutex ara_software_update_mutex_;
    std::condition_variable ara_software_update_cond_;

private:
    apd::rest::Router CreateRouter();
    apd::rest::Route::Upshot GetSoftwareUpdateHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);
    apd::rest::Route::Upshot PostApprovalStatusHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);

    apd::rest::Route::Upshot GetprogressInformationHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);

    apd::rest::Route::Upshot GetpackageInformationHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);

    apd::rest::Route::Upshot GetCampaignHistoryHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);

    void EstablishSubscriptions();

    /// @brief The callback to handle CampaignState field change notification.
    void OnCampaignStatusChanged();

    /// @brief The callback to handle ApprovalRequired field change notification.
    void OnApprovalRequiredChanged();

    std::unique_ptr<apd::rest::Server> restServer_;
    std::shared_ptr<ara::vucm::proxy::VehicleDriverApplicationInterfaceProxy> proxy_;

    ara::log::Logger& logger_{
        ara::log::CreateLogger("DSWC", "Driver Interface SWC context", ara::log::LogLevel::kVerbose)};
    ara::log::Logger& loggerDriverApproval_{ara::log::CreateLogger("APPR",
        "Driver Interface SWC - approval handler context",
        ara::log::LogLevel::kVerbose)};

    AraSoftwareUpdate updateInfo_;
    progressInformationStruct progressInfo_;
    packageInformationStruct packageInfo_;
    campaignHistoryStruct historyInfo_;
    bool campaignStateUpdated = true;  // for ecu startup make sure to read
};

#endif  // APD_UCM_VDA_SAMPLE_DRIVER_INTERFACE_SWC_H_
