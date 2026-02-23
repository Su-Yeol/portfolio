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

#include "GenericAdasApplication.h"
#include <stdint.h>
#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>
#include <stdexcept>

GenericAdasApplication::GenericAdasApplication()
    : roadObjects()
    , staticObjects()
    , potentiallyMovingObjects()
    , cameraDetections()
    , ultrasonicDetections()
    , cameraFeatures()
    , ultrasonicFeatures()
    , lidarDetections()
    , radarDetections()
{
    m_logger.LogDebug() << "GenericAdasApplication start";
}

void GenericAdasApplication::Act()
{
    cameraDetections.Act();
    roadObjects.Act();
    cameraFeatures.Act();
    potentiallyMovingObjects.Act();
    radarDetections.Act();
    staticObjects.Act();
    ultrasonicDetections.Act();
    ultrasonicFeatures.Act();
    lidarDetections.Act();

    printRoadObjectsInterfaceValue();
    printStaticObjectsInterfaceValue();
    printPMObjectsInterfaceValue();
    printCameraDetectionInterfaceValue();
    printCameraFeatureInterfaceValue();
    printUltrasonicDetectionInterfaceValue();
    printLidarDetectionInterfaceValue();
    printRadarDetectionInterfaceValue();
    printUltrasonicFeatureInterfaceValue();
}

void GenericAdasApplication::printRoadObjectsInterfaceValue()
{

    auto adiroadObjectsInterfaceValue = (this->roadObjects).roadObjectsInterfaceValue;

    for (std::size_t roadBoundaryIndex = 0;
         roadBoundaryIndex < adiroadObjectsInterfaceValue.RoadBoundariesObjectList.RoadBoundaryList.size();
         roadBoundaryIndex++) {
        m_logger.LogInfo() << "RoadObjects Boundary Prob"
                           << adiroadObjectsInterfaceValue.RoadBoundariesObjectList.RoadBoundaryList
                                  .at(roadBoundaryIndex)
                                  .RoadBoundariesStatus.ExistenceProbabilityObjectLevel;
    }
}

void GenericAdasApplication::printStaticObjectsInterfaceValue()
{

    auto adiStaticObjectsInterfaceValue = (this->staticObjects).staticObjectInterfaceValue;
    for (std::size_t trafficLightIndex = 0;
         trafficLightIndex < adiStaticObjectsInterfaceValue.StaticObjectTrafficLights.TrafficLightList.size();
         trafficLightIndex++) {
        m_logger.LogInfo() << "TrafficLight Spot Pose: "
                           << adiStaticObjectsInterfaceValue.StaticObjectTrafficLights.TrafficLightList
                                  .at(trafficLightIndex)
                                  .TrafficLightsPosition.PositionObjectLevel.x;
    }
}

void GenericAdasApplication::printPMObjectsInterfaceValue()
{

    auto adiPMObjectsInterfaceValue = (this->potentiallyMovingObjects).potentiallyMovingObjectInterfaceValue;
    for (std::size_t PMOIndex = 0;
         PMOIndex < adiPMObjectsInterfaceValue.PotentiallyMovingObjectList.ValidPotentiallyMovingObjects.size();
         PMOIndex++) {
        m_logger.LogInfo() << "PMO Acc x: "
                           << adiPMObjectsInterfaceValue.PotentiallyMovingObjectList.ValidPotentiallyMovingObjects
                                  .at(PMOIndex)
                                  .PotentiallyMovingObjectsDynamics.Acceleration.x;
    }
}

void GenericAdasApplication::printCameraDetectionInterfaceValue()
{
    auto adiCameraDetectionsInterfaceValue = (this->cameraDetections).cameraDetectionsInterfaceValue;
    auto adiCamInterfaceHeader = adiCameraDetectionsInterfaceValue.CameraDetectionInterfaceHeader;
    auto adiCamSensorIdList = adiCamInterfaceHeader.SensorClusterInformation.SensorIDReferenceList;

    for (std::size_t camSensorIndex = 0; camSensorIndex < adiCamSensorIdList.size(); camSensorIndex++) {
        auto adiCamSensorID = adiCamInterfaceHeader.SensorClusterInformation.SensorIDReferenceList.at(camSensorIndex);
        m_logger.LogInfo() << "Camera Detection Sensor ID: " << adiCamSensorID;
    }
}

void GenericAdasApplication::printCameraFeatureInterfaceValue()
{
    auto adiCameraFeatureInterfaceHeader
        = (this->cameraFeatures).cameraFeatureInterfaceValue.CameraFeatureInterfaceHeader;
    auto adiCamFeatureInterfaceVer
        = adiCameraFeatureInterfaceHeader.InformationInterface.VersionID.InterfaceVersionIDMajor;
    m_logger.LogInfo() << "Camera Feature Major Version: " << adiCamFeatureInterfaceVer;
}

void GenericAdasApplication::printUltrasonicDetectionInterfaceValue()
{
    auto adiUltrasonicDetectionInterfaceValue = (this->ultrasonicDetections).ultrasonicDetectionsInterfaceValue;
    auto adiValidUltrasonicDetectionList = adiUltrasonicDetectionInterfaceValue.ValidUltrasonicDetectionList;
    for (std::size_t index = 0; index < adiValidUltrasonicDetectionList.size(); index++) {
        auto adiValidUltrasonicDetectionDistance
            = adiValidUltrasonicDetectionList.at(index).UltrasonicDetectionsPosition.Distance;
        m_logger.LogInfo() << "Ultrasonic Detection distance: " << adiValidUltrasonicDetectionDistance;
    }
}

void GenericAdasApplication::printLidarDetectionInterfaceValue()
{
    auto adiLidarInterfaceHeader = (this->lidarDetections).lidarDetectionsInterfaceValue.LidarDetectionInterfaceHeader;
    for (std::size_t index = 0; index < adiLidarInterfaceHeader.SensorClusterInformation.SensorIDReferenceList.size();
         index++) {
        auto adiLidSensorID = adiLidarInterfaceHeader.SensorClusterInformation.SensorIDReferenceList.at(index);
        m_logger.LogInfo() << "Lidar Detection sensor ID: " << adiLidSensorID;
    }
}

void GenericAdasApplication::printRadarDetectionInterfaceValue()
{
    auto adiRadarDetectionsInterface = (this->radarDetections).radarDetectionsInterfaceValue;
    for (std::size_t index = 0; index < adiRadarDetectionsInterface.ValidRadarDetectionsList.size(); index++) {

        auto adiRadarDetObjectID
            = adiRadarDetectionsInterface.ValidRadarDetectionsList.at(index).RadarDetectionsStatus.ObjectID;
        m_logger.LogInfo() << "Radar Detection object ID: " << adiRadarDetObjectID;
    }
}

void GenericAdasApplication::printUltrasonicFeatureInterfaceValue()
{
    auto adiValidUltrasonicFeatureVersionID = (this->ultrasonicFeatures)
                                                  .ultrasonicFeatureInterfaceValue.UltrasonicFeatureInterfaceHeader
                                                  .InformationInterface.VersionID.InterfaceVersionIDMajor;
    m_logger.LogInfo() << "Ultrasonic Feature Version ID: " << adiValidUltrasonicFeatureVersionID;
}
