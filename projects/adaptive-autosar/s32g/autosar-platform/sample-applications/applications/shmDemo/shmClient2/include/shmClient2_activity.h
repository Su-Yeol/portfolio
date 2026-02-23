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
 * \brief Specification of SHM CLient2 activity.
 * \details Contains specification of SHM CLient2 activity.
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
#include "ara/com/sample/healthindicator_proxy.h"

#include "ara/log/logger.h"

#define DEACTIVATED 0
#define OK 1
#define FAILED 2
#define EXPIRED 3
#define STOPPED 4

#define REL_INIT 0
#define REL_VERY_LOW 1
#define REL_LOW 2
#define REL_EXCELLENT 3
#define REL_ERROR 10

namespace shmClient2
{
// Header of Methods Output Messages.
constexpr auto METHODS_HEADER = "METHODS: ";
constexpr auto FIELDS_HEADER = "FIELDS: ";
constexpr auto ERROR_HEADER = "APPLICATION_ERROR: ";

enum Result_Enum
{
    E_OK,
    E_NOT_OK
};

typedef struct HealthIndicator
{
    std::int16_t Performance;
    std::uint8_t Reliability;
    std::uint8_t Subsystem_State;
} HealthIndicator;

/*!
 *  \brief Class implementing shmClient2 activity.
 *
 *  shmClient2 activity implementing function of SHM Client2.
 */
class shmClient2Activity
{
    using Proxy = ara::com::sample::proxy::HealthIndicatorProxy;

public:
    /* 	Create a list of 25 elements to store the received values of the determined HealthIndicator - Performance and
     * view on console. and initialize with 50 (or any value other than 0 to 4 for easier identification of valid
     * Performance values on console.)
     */
    std::list<int> HI_Performance_List
        = {50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50};

    shmClient2Activity();

    /*!
     *  \brief Initializes shmClient2 activity.
     *
     *  Initializes shmClient2 activity. This is called during initialization of the runtime.
     */
    void init();

    /*!
     *  \brief Runs shmClient2 activity.
     *
     *  Executable unit triggered to perform shmClient2 activity.
     */
    void act();

    /*!
     *  \brief Runs Health Indicator processing activity.
     *
     *  Executable unit triggered to process Health Indicators.
     */
    Result_Enum HI_Processing(std::int16_t Performance);

    /*!
     *  \brief Callback to change to healthindicator service offer changes.
     *
     *  Callback executed whenever a change for healthindicator service offers happen.
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

protected:
    std::shared_ptr<Proxy> m_proxy;
    std::mutex m_proxy_mutex;

    // this class own logger
    ara::log::Logger& m_logger{
        ara::log::CreateLogger("S2CT", "shmClient2 class own context", ara::log::LogLevel::kVerbose)};
};
}  // namespace shmClient2

#endif  // CM_PROVIDERSUBSCRIBERSCENARIO_FUSION_INC_FUSION_ACTIVITY_HPP_
