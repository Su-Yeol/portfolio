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
 *  \brief Specification of perception activity.
 *
 *  \details Contains specification of perception activity.
 *  Stub for an activity, can be completely generated from the model. It includes the generic API of an activity and the
 * activity specific data access points (event sender/receiver, ...).
 */

#ifndef __VEHICLE_LOCATION_PROVIDER_H__
#define __VEHICLE_LOCATION_PROVIDER_H__

#include <string>
#include <random>

#include "ara/core/future.h"
#include "ara/core/promise.h"

// includes for used services
#include "katech/vehicle_location_skeleton.h"

#include "ara/core/future.h"
#include "ara/core/promise.h"

#include "etc/udpSender.h"

namespace katech
{

/*!
 *  \brief Class implementing skeleton methods.
 *
 *  vehicle_location's implementation.
 *
 *  \uptrace{SWS_CM_00191}
 */
class VehicleLocationImp : public katech::skeleton::vehicle_locationSkeleton
{
    using Skeleton = katech::skeleton::vehicle_locationSkeleton;

public:
    VehicleLocationImp(ara::core::InstanceSpecifier instanceSpec, ara::com::MethodCallProcessingMode mode)
        : Skeleton(std::move(instanceSpec), mode)
        , m_worker(&VehicleLocationImp::ProcessRequests, this)
    { }

    virtual ~VehicleLocationImp()
    {
        m_finished = true;
        m_worker.join();
    }

private:
    void ProcessRequests();

    std::atomic<bool> m_finished{false};
    std::thread m_worker;
};

class VehicleLocation_Provider
{
public:
    VehicleLocation_Provider();
    ~VehicleLocation_Provider();

    void init(std::string instance);
    void send(vehicle_location_Objects& data);

protected:
    katech::skeleton::vehicle_locationSkeleton* m_skeleton;
    std::uint32_t m_update_rate;
    bool mEnableUDP = false;
    std::vector<std::shared_ptr<katech::etc::udpSender>> mList_udpSender;
    int mUdp_Location_port = 10001;
    std::string mIP_CPU_A = "192.168.1.201";
    std::string mIP_CPU_B = "192.168.1.211";
    std::string mIP_S32G = "192.168.1.99";

};

}  // namespace katech
#endif  // __VEHICLE_LOCATION_PROVIDER_H__
