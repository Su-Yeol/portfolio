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

#ifndef ADI_OSI_ADAPTER_SENSOR_DATA_MAPPING_HPP_
#define ADI_OSI_ADAPTER_SENSOR_DATA_MAPPING_HPP_

#include "ara/log/logger.h"

#include "osi3/osi_roadmarking.pb.h"
#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_datarecording.pb.h"

#include "ara/adi/sensoritf/impl_type_roadobjectinterface.h"
#include "ara/adi/sensoritf/impl_type_staticobjectinterface.h"
#include "ara/adi/sensoritf/impl_type_potentiallymovingobjectinterface.h"
#include "ara/adi/sensoritf/impl_type_cameradetectionsinterface.h"
#include "ara/adi/sensoritf/impl_type_lidardetectionsinterface.h"
#include "ara/adi/sensoritf/impl_type_radardetectionsinterface.h"
#include "ara/adi/sensoritf/impl_type_ultrasonicdetectionsinterface.h"
#include "ara/adi/sensoritf/impl_type_ultrasonicfeatureinterface.h"

#include "DetectedEntityHeaderMapper.h"
#include "DetectedRoadMarkingMapper.h"
#include "DetectedTrafficSignMapper.h"
#include "DetectedTrafficLightMapper.h"
#include "DetectedStationaryObjectMapper.h"
#include "DetectedMovingObjectMapper.h"
#include "DetectedLaneBoundaryMapper.h"
#include "DetectedLaneMapper.h"
#include "LogicalDetectionDataMapper.h"
#include "CameraFeatureDataMapper.h"
#include "UltrasonicLogicalDetectionDataMapper.h"
#include "FeatureDataMapper.h"

class SensorDataMapper : public GeneralHeaderMapper
{
public:
    SensorDataMapper();

public:
    osi3::SensorData mapSensorData() const;

    // Set Sensor service interface values received
    void setCameraDetectionInterface(const ara::adi::sensoritf::CameraDetectionsInterface cameraDetectionsInterface);
    void setCameraFeatureInterface(const ara::adi::sensoritf::CameraFeatureInterface cameraFeatureInterfacei);
    void setPMOInterface(const ara::adi::sensoritf::PotentiallyMovingObjectInterface potentiallyMovingObjectInterface);
    void setRoadObjectInterface(const ara::adi::sensoritf::RoadObjectInterface roadObjectInterface);
    void setStaticObjectInterface(const ara::adi::sensoritf::StaticObjectInterface staticObjectInterface);
    void setLidarDetectionInterface(const ara::adi::sensoritf::LidarDetectionsInterface lidarDetectionInterface);
    void setRadarDetectionInterface(const ara::adi::sensoritf::RadarDetectionsInterface radarDetectionInterface);
    void setUltrasonicDetectionInterface(
        const ara::adi::sensoritf::UltrasonicDetectionsInterface ultrasonicDetectionInterface);
    void setUltrasonicFeatureInterface(
        const ara::adi::sensoritf::UltrasonicFeatureInterface ultrasonicFeatureInterface);

private:
    osi3::MountingPosition mapSensorPose2Osi(const ara::adi::sensoritf::InformationSensorPose& sensorPose) const;
    osi3::MountingPosition mapToMountingPositionError(
        const ara::adi::sensoritf::InformationSensorPose& sensorPose) const;

    void detectedRoadMarking(osi3::SensorData& sensorData) const;
    void detectedLaneBoundary(osi3::SensorData& sensorData) const;
    void detectedPotentiallyMovingObj(osi3::SensorData& sensorData) const;
    void detectedStationaryObjects(osi3::SensorData& sensorData) const;
    void detectedTrafficLights(osi3::SensorData& sensorData) const;
    void detectedTrafficSigns(osi3::SensorData& sensorData) const;
    void featureData(osi3::SensorData& sensorData) const;
    void logicalDetectionData(osi3::SensorData& sensorData) const;

private:
    DetectedRoadMarkingMapper detectedRoadMarkingMapper;
    DetectedTrafficLightMapper detectedTrafficLightMapper;
    DetectedTrafficSignMapper detectedTrafficSignMapper;
    DetectedEntityHeaderMapper detectedEntityHeaderMapper;
    DetectedStationaryObjectMapper detectedStationaryObjectMapper;
    DetectedMovingObjectMapper detectedMovingObjectMapper;
    CameraFeatureDataMapper cameraFeatureDataMapper;
    DetectedLaneBoundaryMapper detectedLaneBoundaryMapper;
    DetectedLaneMapper detectedLaneMapper;
    LogicalDetectionDataMapper logicalDetectionDataMapper;
    UltrasonicLogicalDetectionDataMapper ultrasonicLogicalDetectionDataMapper;
    FeatureDataMapper featureDataMapper;

    std::shared_ptr<ara::adi::sensoritf::RoadObjectInterface> ptrRoadObjectInterface_;
    std::shared_ptr<ara::adi::sensoritf::StaticObjectInterface> ptrStaticObjectInterface_;
    std::shared_ptr<ara::adi::sensoritf::PotentiallyMovingObjectInterface> ptrPMOInterface_;
    std::shared_ptr<ara::adi::sensoritf::CameraDetectionsInterface> ptrCameraDetectionsInterface_;
    std::shared_ptr<ara::adi::sensoritf::CameraFeatureInterface> ptrCameraFeatureInterface_;
    std::shared_ptr<ara::adi::sensoritf::LidarDetectionsInterface> ptrLidarDetectionInterface_;
    std::shared_ptr<ara::adi::sensoritf::RadarDetectionsInterface> ptrRadarDetectionInterface_;
    std::shared_ptr<ara::adi::sensoritf::UltrasonicDetectionsInterface> ptrUltrasonicDetectionInterface_;
    std::shared_ptr<ara::adi::sensoritf::UltrasonicFeatureInterface> ptrUltrasonicFeatureInterface_;

private:
    ara::log::Logger& m_logger{ara::log::CreateLogger("SDM", "...", ara::log::LogLevel::kVerbose)};
};

#endif  // ADI_OSI_ADAPTER_SENSOR_DATA_MAPPING_HPP_
