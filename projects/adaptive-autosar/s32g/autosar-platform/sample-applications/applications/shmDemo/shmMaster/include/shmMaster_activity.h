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

/*!
 * \file
 * \brief Specification of SHM Master activity.
 * \details Contains specification of SHM Master activity.
 * Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef CM_PROVIDERSUBSCRIBERSCENARIO_FUSION_INC_FUSION_ACTIVITY_HPP_
#define CM_PROVIDERSUBSCRIBERSCENARIO_FUSION_INC_FUSION_ACTIVITY_HPP_

#include <mutex>
#include <string>
#include <random>

#include <list>
#include <iostream>
#include <fstream>

// includes for used services
#include "ara/com/sample/healthinfo_proxy.h"
#include "ara/com/sample/healthindicator_skeleton.h"

#include "ara/log/logger.h"

#define DEACTIVATED 0
#define OK 1
#define FAILED 2
#define EXPIRED 3
#define STOPPED 4

#define REL_FAILED 0
#define REL_RECOVERING 1
#define REL_GOOD 2

#define PER_INIT 0
#define PER_VERY_LOW 1
#define PER_OK 2
#define PER_LOW 3
#define PER_EXCELLENT 4
#define PER_ERROR 10

enum Result_Enum
{
    E_OK,
    E_NOT_OK
};

typedef struct HealthIndicator
{
    std::int16_t Performance;
    std::int16_t Reliability;
    std::uint8_t Subsystem_State;
} HealthIndicator;

namespace shmMaster_healthindicator
{
/* ******************************************************* Class for HealthIndicator service skeleton
 * *******************************************************************  */
class healthindicatorImp : public ara::com::sample::skeleton::HealthIndicatorSkeleton
{
    using Skeleton = ara::com::sample::skeleton::HealthIndicatorSkeleton;

public:
    healthindicatorImp(ara::core::InstanceSpecifier instanceSpecHI, ara::com::MethodCallProcessingMode mode)
        : Skeleton(std::move(instanceSpecHI), mode)
        , m_worker(&healthindicatorImp::ProcessRequests, this)
    { }

    virtual ~healthindicatorImp()
    {
        m_finished = true;
        m_worker.join();
    }

private:
    /*!
     * \brief Defines how the incoming service method invocations are processed.
     *
     * \uptrace{SWS_CM_00198}
     * \uptrace{SWS_CM_00199}
     */
    void ProcessRequests();

    std::atomic<bool> m_finished{false};
    std::thread m_worker;

    ara::log::Logger& m_logger_ctx1{
        ara::log::CreateLogger("CTX1", "context for Health Indicator", ara::log::LogLevel::kVerbose)};
};
/* **************************************************************************************************************************************************************************************
 */

/* ************************************************************************* Class for Health Indicator service activity
 * *************************************************************** */
/*!
 *  \brief Class implementing healthindicator activity.
 *
 *  HealthIndicator activity implementing function of health indication.
 */
class healthindicatorActivity
{
public:
    healthindicatorActivity();
    ~healthindicatorActivity();

    /*!
     *  \brief Initializes healthindicator activity.
     *
     *  Initializes healthindicator activity. This is called during initialization of the runtime.
     */
    void init();

    /*!
     *  \brief Runs healthindicator activity.
     *
     *  Executable unit triggered to perform healthindicator activity.
     */
    void act();

protected:
    /*!
     * \brief A pointer to the skeleton object.
     */
    ara::com::sample::skeleton::HealthIndicatorSkeleton* m_skeleton_HI;  // ASWS_HM_00510, ASWS_HM_00512

    ara::log::Logger& m_logger_ctx3{
        ara::log::CreateLogger("CTX3", "context for Health Indication Activity", ara::log::LogLevel::kVerbose)};
};

/* **********************************************************************************************************************************************************************************************
 */
}  // namespace shmMaster_healthindicator

namespace shmMaster
{
// Header of Methods Output Messages.
constexpr auto METHODS_HEADER = "METHODS: ";
constexpr auto FIELDS_HEADER = "FIELDS: ";
constexpr auto ERROR_HEADER = "APPLICATION_ERROR: ";

/*!
 *  \brief Class implementing shmMaster activity.
 *
 *  shmMaster activity implementing function of data shmMaster.
 */
class shmMasterActivity
{
    using Proxy = ara::com::sample::proxy::HealthInfoProxy;

public:
    shmMasterActivity();

    /* 	Create a list of 25 elements to store previous values of determined HealthIndicator - Performance
     * 	and initialize with 50 (or any value other than 0 to 4 for easier identification of valid Performance values on
     * console.)
     */
    std::list<int> HI_Performance_List
        = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50};

    /* 	Create a list of 25 elements to store received Global Supervision Status values
     * 	and initialize with 50 (or any value other than 0 to 4 for easier identification of valid GSS on console.)
     */
    std::list<int> GSS_Received_List
        = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50};

    /*!
     *  \brief Initializes shmMaster activity.
     *
     *  Initializes shmMaster activity. This is called during initialization of the runtime.
     */
    void init();

    /*!
     *  \brief Runs shmMaster activity.
     *
     *  Executable unit triggered to perform shmMaster activity.
     */
    void act();

    /*!
     *  \brief Callback to change to healthInfo service offer changes.
     *
     *  Callback executed whenever a change for healthInfo service offers happen.
     *
     */

    void serviceAvailabilityCallback(ara::com::ServiceHandleContainer<Proxy::HandleType> handles,
        ara::com::FindServiceHandle handler)
    {
        for (auto it : handles) {
            m_logger.LogInfo() << "Instance " << it.GetInstanceId().ToString() << " is available";
        }
        if (handles.size() > 0) {
            std::lock_guard<std::mutex> lock(m_proxy_mutex);
            if (nullptr == m_proxy) {
                m_proxy = std::make_shared<Proxy>(handles[0]);
                m_logger.LogInfo() << "Created proxy from handle with instance: "
                                   << m_proxy->GetHandle().GetInstanceId().ToString();
                // Construct some handles (implementation-specific).
                auto first_handle = handles[0];
                auto aux_handle = first_handle;
                for (auto current_handle : handles) {
                    // Call equality operator.
                    m_logger.LogInfo() << "Check handle::operator==: "
                                       << static_cast<uint8_t>(aux_handle == current_handle);
                    // Call copy assignment operator.
                    aux_handle = current_handle;
                    // Call less-than operator.
                    m_logger.LogInfo() << "Check handle::operator<: "
                                       << static_cast<uint8_t>(aux_handle < first_handle);
                }
            }
        }
    }

    /*!
     *  \brief Determines HeathIndicator from SHM Master.
     *
     *  Executable unit triggered to determine SHM Master HealthIndicator.
     */
    Result_Enum HI_Determination(std::uint8_t globalSupervisionStatus,
        std::uint8_t HealthIndicatorID,
        std::uint8_t received_Reliability);

protected:
    std::shared_ptr<Proxy> m_proxy;
    std::mutex m_proxy_mutex;

    // this class own logger
    ara::log::Logger& m_logger{
        ara::log::CreateLogger("SHAT", "shmMasterActivity class own context", ara::log::LogLevel::kVerbose)};
};
}  // namespace shmMaster

#endif  // CM_PROVIDERSUBSCRIBERSCENARIO_FUSION_INC_FUSION_ACTIVITY_HPP_
