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
 *  \brief Adapter to map ASAM OSI sensor data to AUTOSAR ADI format
 *
 *  \details Contains an Adapter Application to map OSI data to ADI format
 *  It makes use of ADI Sensor interfaces to publish and send the mapped ADI data
 *  to subscribing applications
 */

#ifndef CM_PROVIDERSUBSCRIBERSCENARIO_OSIADIADAPTER_H_
#define CM_PROVIDERSUBSCRIBERSCENARIO_OSIADIADAPTER_H_

#include <string>
#include <random>

#include "ara/log/logger.h"
#include "ara/core/future.h"
#include "ara/core/promise.h"

#include "serviceInterfaces/cameraDetectionsServiceImp.h"
#include "serviceInterfaces/roadObjectsServiceImp.h"
#include "serviceInterfaces/cameraFeaturesServiceImp.h"
#include "serviceInterfaces/pmoServiceImp.h"
#include "serviceInterfaces/radarDetectionsServiceImp.h"
#include "serviceInterfaces/lidarDetectionsServiceImp.h"
#include "serviceInterfaces/staticObjectsServiceImp.h"
#include "serviceInterfaces/ultrasonicDetectionsServiceImp.h"
#include "serviceInterfaces/ultrasonicFeaturesServiceImp.h"

class OsiAdiAdapter
{

public:
    /// @brief Constructor to initialize service interfaces
    OsiAdiAdapter();

    /// @brief Send event for each service
    void Act();

private:
    /// @brief Initialize and create service interfaces based on the first instance id of the instance id container
    /// @return
    bool InitializeServiceInterfaces();

private:
    std::unique_ptr<CameraDetectionsServiceImp> ptrCameraDetectionsServiceImp_;
    std::unique_ptr<RoadObjectsServiceImp> ptrRoadObjectsServiceImp_;
    std::unique_ptr<CameraFeaturesServiceImp> ptrCameraFeaturesServiceImp_;
    std::unique_ptr<PMOServiceImp> ptrPMOServiceImp_;
    std::unique_ptr<RadarDetectionsServiceImp> ptrRadarDetectionsServiceImp_;
    std::unique_ptr<LidarDetectionsServiceImp> ptrLidarDetectionsServiceImp_;
    std::unique_ptr<StaticObjectsServiceImp> ptrStaticObjectsServiceImp_;
    std::unique_ptr<UltrasonicDetectionsServiceImp> ptrUltrasonicDetectionsServiceImp_;
    std::unique_ptr<UltrasonicFeaturesServiceImp> ptrUltrasonicFeaturesServiceImp_;

    ara::log::Logger& m_logger_{ara::log::CreateLogger("OAA", "context for update rate", ara::log::LogLevel::kVerbose)};
};

#endif  // CM_PROVIDERSUBSCRIBERSCENARIO_OSIADIADAPTER_H_
