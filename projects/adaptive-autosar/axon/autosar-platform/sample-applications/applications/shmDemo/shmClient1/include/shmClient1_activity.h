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
 *  \file
 *  \brief Specification of SHM CLient1 activity.
 *
 *  \details Contains specification of SHM Client1 activity.
 *  Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef CM_PROVIDERSUBSCRIBERSCENARIO_RADAR_INC_RADAR_ACTIVITY_HPP_
#define CM_PROVIDERSUBSCRIBERSCENARIO_RADAR_INC_RADAR_ACTIVITY_HPP_

#include <string>
#include <random>

#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

// includes for used services
#include "ara/com/sample/healthinfo_skeleton.h"

#include "ara/core/future.h"
#include "ara/core/promise.h"

#define DEACTIVATED 0
#define OK 1
#define FAILED 2
#define EXPIRED 3
#define STOPPED 4

#define REL_FAILED 0
#define REL_RECOVERING 1
#define REL_GOOD 2

namespace shmClient1
{
// Header of Methods Output Messages.
constexpr auto METHODS_HEADER = "METHODS: ";
constexpr auto FIELDS_HEADER = "FIELDS: ";
constexpr auto ERROR_HEADER = "APPLICATION_ERROR: ";

/*!
 *  \brief Class implementing skeleton methods.
 *
 *  SHM Client1's implementation.
 *
 *
 */
class shmClient1Imp : public ara::com::sample::skeleton::HealthInfoSkeleton
{
    using Skeleton = ara::com::sample::skeleton::HealthInfoSkeleton;

public:
    shmClient1Imp(ara::core::InstanceSpecifier instanceSpec, ara::com::MethodCallProcessingMode mode)
        : Skeleton(std::move(instanceSpec), mode)
        , m_worker(&shmClient1Imp::ProcessRequests, this)
    { }

    virtual ~shmClient1Imp()
    {
        m_finished = true;
        m_worker.join();
    }

private:
    /*!
     * \brief Defines how the incoming service method invocations are processed.
     */
    void ProcessRequests();

    std::atomic<bool> m_finished{false};
    std::thread m_worker;

    ara::log::Logger& m_logger_ctx1{
        ara::log::CreateLogger("CTX1", "context for adjustment", ara::log::LogLevel::kVerbose)};
};

/*!
 *  \brief Class implementing SHM Client1 activity.
 *
 *  SHM Client1 activity implementing function of SHM Client1.
 */
class shmClient1Activity
{
public:
    shmClient1Activity();
    ~shmClient1Activity();

    int GSS_Send_Counter = 0;

    uint8_t GlobalSupervisionStatusValue; /* NOTE: Global Supervision Status to be received from PHM */

    /* 	Create a list of 25 elements to store previous values of GSS
     * 	and initialize with 50 (or any value other than 0 to 4 for easier identification of valid GSS on console.)
     */
    std::list<int> GSS_Provider_List
        = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50};

    /*!
     *  \brief Initializes SHM Client1 activity.
     *
     *  Initializes SHM Client1 activity. This is called during initialization of the runtime.
     */
    void init();

    /*!
     *  \brief Runs SHM Client1 activity.
     *
     *  Executable unit triggered to perform SHM Client1 activity.
     */
    void act();

    /*!
     *  \brief Determines SHM Client 1 Platform HeathIndicator.
     *
     *  Executable unit triggered to determine SHM Client1 platform HealthIndicator.
     */
    int16_t platform_HI_Determination(uint8_t globalSupervisionStatus);

protected:
    /*!
     * \brief A pointer to the skeleton object.
     */
    ara::com::sample::skeleton::HealthInfoSkeleton* m_skeleton;  // ASWS_HM_00514, ASWS_HM_00513

    ara::log::Logger& m_logger_ctx3{
        ara::log::CreateLogger("CTX3", "context for Global Supervision Status", ara::log::LogLevel::kVerbose)};
};

}  // namespace shmClient1

#endif  // CM_PROVIDERSUBSCRIBERSCENARIO_RADAR_INC_RADAR_ACTIVITY_HPP_
