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

#ifndef CM_PROVIDERSUBSCRIBERSCENARIO_SMART_CAMERA_SENSOR_H_
#define CM_PROVIDERSUBSCRIBERSCENARIO_SMART_CAMERA_SENSOR_H_

#include <string>
#include <random>

#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

#include "serviceInterfaces/roadObjectsService.h"
#include "serviceInterfaces/staticObjectsService.h"
#include "serviceInterfaces/potentiallyMovingObjectsService.h"
#include "serviceInterfaces/cameraDetectionsService.h"
#include "serviceInterfaces/ultrasonicDetectionsService.h"
#include "serviceInterfaces/cameraFeaturesService.h"
#include "serviceInterfaces/ultrasonicFeaturesService.h"
#include "serviceInterfaces/lidarDetectionsService.h"
#include "serviceInterfaces/radarDetectionsService.h"

class SmartCamera
{

public:
    /// @brief Constructor to initialize service interfaces
    SmartCamera();

    /// @brief Send event for each service
    void Act();

private:
    /// @brief Initialize and create service interfaces based on the first instance id of the instance id container
    /// @return
    bool InitializeServiceInterfaces();

private:
    std::unique_ptr<RoadObjectsService> ptrRoadObjectsService_;
    std::unique_ptr<StaticObjectsService> ptrStaticObjectsService_;
    std::unique_ptr<PotentiallyMovingObjectsService> ptrPMObjectsService_;
    std::unique_ptr<CameraDetectionsService> ptrCameraDetectionsService_;
    std::unique_ptr<UltrasonicDetectionsService> ptrUltrasonicDetectionsService_;
    std::unique_ptr<CameraFeaturesService> ptrCameraFeaturesService_;
    std::unique_ptr<UltrasonicFeaturesService> ptrUltrasonicFeaturesService_;
    std::unique_ptr<LidarDetectionsService> ptrLidarDetectionsService_;
    std::unique_ptr<RadarDetectionsService> ptrRadarDetectionsService_;

    ara::log::Logger& m_logger_{ara::log::CreateLogger("SC", "context for update rate", ara::log::LogLevel::kVerbose)};
};

#endif  // CM_PROVIDERSUBSCRIBERSCENARIO_SMART_CAMERA_SENSOR_H
