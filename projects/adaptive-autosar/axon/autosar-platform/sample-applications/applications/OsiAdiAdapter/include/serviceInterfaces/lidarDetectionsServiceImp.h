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

#ifndef OSIADIADAPTER_LIDARDETECTIONSSERVICE_H_
#define OSIADIADAPTER_LIDARDETECTIONSSERVICE_H_

#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"
#include "ara/core/string.h"

#include "ara/adi/sensoritf/lidardetectionsservice_skeleton.h"
#include "adiMappers/LidarDetectionsMapper.h"
#include "createOsiDummyData.h"

class LidarDetectionsServiceImp : public ara::adi::sensoritf::skeleton::LidarDetectionsServiceSkeleton
{
    using LDSSkeleton = ara::adi::sensoritf::skeleton::LidarDetectionsServiceSkeleton;
    using ptrLidarDetectionInterface = ara::com::SampleAllocateePtr<ara::adi::sensoritf::LidarDetectionsInterface>;

public:
    /// @brief Initializes a new instance of this service with the given instance id
    ///
    /// @param instance_id Instance identifier required by ara com
    LidarDetectionsServiceImp(ara::com::InstanceIdentifier instance_id);

    /// @brief virtual Destructor for virtual function and join threads
    virtual ~LidarDetectionsServiceImp();

    /// @brief Initializes the service interface, registers set handler and initializes the field.
    void InitializeService();

    void SendEvent();

    /// @brief Initialize and Assign value to Capability Vector
    /// @return Future
    virtual auto LidarDetectionsCapability() -> decltype(LDSSkeleton::LidarDetectionsCapability()) override;

private:
    /// @brief Timer for processing service request
    void ProcessRequests();

    /// @brief Dummy data generation
    ptrLidarDetectionInterface createLidarDetection(ptrLidarDetectionInterface pLidarDetectionsService);

    LidarDetectionsMapper lidarDetectionsMapper;
    std::atomic<bool> m_finished_{false};
    std::thread m_worker_;

    /// @brief Logger for logging errors
    ara::log::Logger& m_logger_{
        ara::log::CreateLogger("LDS", "OAA_APPL Lidar Detection", ara::log::LogLevel::kVerbose)};
};

#endif  // OSIADIADAPTER_LIDARDETECTIONSSERVICE_H_
