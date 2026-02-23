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
 *  \brief Class to receive OSI Sensor data in ADI format
 */

#ifndef CM_PROVIDERSUBSCRIBERSCENARIO_GENERICADASAPPLICATION_H_
#define CM_PROVIDERSUBSCRIBERSCENARIO_GENERICADASAPPLICATION_H_

#include <string>
#include <random>
#include <mutex>

#include "ara/log/logger.h"
#include "ara/com/e2e_helper.h"

#include <stdint.h>
#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>
#include <stdexcept>

#include "serviceInterfaces/roadObjects.h"
#include "serviceInterfaces/staticObjects.h"
#include "serviceInterfaces/potentiallyMovingObjects.h"
#include "serviceInterfaces/cameraDetections.h"
#include "serviceInterfaces/ultrasonicDetections.h"
#include "serviceInterfaces/cameraFeatures.h"
#include "serviceInterfaces/ultrasonicFeatures.h"
#include "serviceInterfaces/lidarDetections.h"
#include "serviceInterfaces/radarDetections.h"

class GenericAdasApplication
{

public:
    /// @brief Start the App
    GenericAdasApplication();

    void printRoadObjectsInterfaceValue();
    void printStaticObjectsInterfaceValue();
    void printPMObjectsInterfaceValue();
    void printCameraDetectionInterfaceValue();
    void printCameraFeatureInterfaceValue();
    void printUltrasonicDetectionInterfaceValue();
    void printLidarDetectionInterfaceValue();
    void printRadarDetectionInterfaceValue();
    void printUltrasonicFeatureInterfaceValue();

    void Act();

private:
    RoadObjects roadObjects;
    StaticObjects staticObjects;
    PotentiallyMovingObjects potentiallyMovingObjects;
    CameraDetections cameraDetections;
    UltrasonicDetections ultrasonicDetections;
    CameraFeatures cameraFeatures;
    UltrasonicFeatures ultrasonicFeatures;
    LidarDetections lidarDetections;
    RadarDetections radarDetections;

    ara::log::Logger& m_logger{ara::log::CreateLogger("GAA", "context for update rate", ara::log::LogLevel::kVerbose)};
};

#endif  // CM_PROVIDERSUBSCRIBERSCENARIO_GENERICADASAPPLICATION_H_
