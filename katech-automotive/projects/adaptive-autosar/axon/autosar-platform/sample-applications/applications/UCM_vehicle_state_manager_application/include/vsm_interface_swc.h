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

#ifndef ARA_UCM_VSM_SAMPLE_APP_INTERFACE_SWC_H_
#define ARA_UCM_VSM_SAMPLE_APP_INTERFACE_SWC_H_

#include <memory>
#include <mutex>

#include "ara/log/logger.h"
#include "apd/rest/server.h"
#include "apd/rest/routing.h"

#include "ara/vucm/vehiclestatemanagerinterface_proxy.h"
#include "ara/vucm/impl_type_campaignstatetype.h"

#ifdef PICAR
#    include "parking_brake.h"
#endif

class VsmInterfaceSwc
{
public:
#ifdef PICAR
    std::thread myVSMILoopThread;
#endif
    VsmInterfaceSwc(std::shared_ptr<ara::vucm::proxy::VehicleStateManagerInterfaceProxy>& proxy,
        ara::log::Logger& logger);
    VsmInterfaceSwc(const VsmInterfaceSwc&) = delete;
    VsmInterfaceSwc& operator=(const VsmInterfaceSwc&) = delete;
    VsmInterfaceSwc(VsmInterfaceSwc&&) = delete;
    VsmInterfaceSwc& operator=(VsmInterfaceSwc&&) = delete;
    ~VsmInterfaceSwc();
    void RespondApprovalRequest();
    void RespondVehicleCheck();
    ara::vucm::CampaignStateType GetCampaignState();

#ifdef PICAR
    bool StartMyVSMILoopThread();
    [[noreturn]] void MyVSMILoopThreadFunction();
#endif

    std::atomic_bool areSafetyConditionsAvailable_{false};
    // std::atomic_bool isVehCheckResNeeded_{false};
    std::uint32_t rolloutStepNumber_{0};

private:
    apd::rest::Router CreateRouter();

    /// @uptrace{SWS_UCM_CONSTR_00015, 49cca825197a36cb6ae1df60657dc7e56924d25d}
    apd::rest::Route::Upshot VehicleCheckHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);

    /// @uptrace{SWS_UCM_CONSTR_00007, 7b528e1162cbad09834b2d134b0c27578aab3167}
    /// @uptrace{SWS_UCM_01278, 1e06fb555a4ff2174d72e0901bccae1a52590450}
    apd::rest::Route::Upshot SafetyStatesHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);

    /// @uptrace{SWS_UCM_CONSTR_00005, 45da5d8ac5a2befab3e93930aec1590c3de146a5}
    /// @uptrace{SWS_UCM_CONSTR_00009, 6f820ae7c0a4f0aa334fbce8586133e9f88ebd88}
    void EstablishSubscriptions();

    std::unique_ptr<apd::rest::Server> restServer_;
    std::shared_ptr<ara::vucm::proxy::VehicleStateManagerInterfaceProxy> proxy_;
    ara::vucm::VehicleConditionCollectionType safetyConditions_;
    ara::vucm::CampaignStateType campaignState_;
    std::mutex mutexSafetyStates_;
    std::mutex mutexVehicleCheck_;
    std::mutex mutexCampaignState_;
    std::mutex mutexSafetyConditions_;
    ara::log::Logger& logger_;
};

#endif  // ARA_UCM_VSM_SAMPLE_APP_INTERFACE_SWC_H_
