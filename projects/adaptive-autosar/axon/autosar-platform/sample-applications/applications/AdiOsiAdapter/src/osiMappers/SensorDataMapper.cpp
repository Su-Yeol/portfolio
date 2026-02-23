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

#include "osiMappers/SensorDataMapper.h"

SensorDataMapper::SensorDataMapper()
    : ptrRoadObjectInterface_(nullptr)
    , ptrStaticObjectInterface_(nullptr)
    , ptrPMOInterface_(nullptr)
    , ptrCameraDetectionsInterface_(nullptr)
    , ptrCameraFeatureInterface_(nullptr)
    , ptrLidarDetectionInterface_(nullptr)
    , ptrRadarDetectionInterface_(nullptr)
    , ptrUltrasonicDetectionInterface_(nullptr)
    , ptrUltrasonicFeatureInterface_(nullptr)
{ }

void SensorDataMapper::setCameraDetectionInterface(
    const ara::adi::sensoritf::CameraDetectionsInterface cameraDetectionsInterface)
{
    using CameraDetectionInterface = ara::adi::sensoritf::CameraDetectionsInterface;
    ptrCameraDetectionsInterface_ = std::make_shared<CameraDetectionInterface>(cameraDetectionsInterface);
}

void SensorDataMapper::setCameraFeatureInterface(
    const ara::adi::sensoritf::CameraFeatureInterface cameraFeatureInterface)
{
    using CameraFeatureInterface = ara::adi::sensoritf::CameraFeatureInterface;
    ptrCameraFeatureInterface_ = std::make_shared<CameraFeatureInterface>(cameraFeatureInterface);
}

void SensorDataMapper::setPMOInterface(
    const ara::adi::sensoritf::PotentiallyMovingObjectInterface potentiallyMovingObjectInterface)
{
    using PotentiallyMovingObjectInterface = ara::adi::sensoritf::PotentiallyMovingObjectInterface;
    ptrPMOInterface_ = std::make_shared<PotentiallyMovingObjectInterface>(potentiallyMovingObjectInterface);
}

void SensorDataMapper::setRoadObjectInterface(const ara::adi::sensoritf::RoadObjectInterface roadObjectInterface)
{
    m_logger.LogInfo() << "setRoadObjectInterface called";

    ptrRoadObjectInterface_ = std::make_shared<ara::adi::sensoritf::RoadObjectInterface>(roadObjectInterface);
}

void SensorDataMapper::setStaticObjectInterface(const ara::adi::sensoritf::StaticObjectInterface staticObjectInterface)
{
    m_logger.LogInfo() << "setStaticObjectInterface called";

    ptrStaticObjectInterface_ = std::make_shared<ara::adi::sensoritf::StaticObjectInterface>(staticObjectInterface);
}

void SensorDataMapper::setLidarDetectionInterface(
    const ara::adi::sensoritf::LidarDetectionsInterface lidarDetectionInterface)
{
    using LidarDetectionsInterface = ara::adi::sensoritf::LidarDetectionsInterface;
    ptrLidarDetectionInterface_ = std::make_shared<LidarDetectionsInterface>(lidarDetectionInterface);
}

void SensorDataMapper::setRadarDetectionInterface(
    const ara::adi::sensoritf::RadarDetectionsInterface radarDetectionInterface)
{
    using RadarDetectionsInterface = ara::adi::sensoritf::RadarDetectionsInterface;
    ptrRadarDetectionInterface_ = std::make_shared<RadarDetectionsInterface>(radarDetectionInterface);
}

void SensorDataMapper::setUltrasonicDetectionInterface(
    const ara::adi::sensoritf::UltrasonicDetectionsInterface ultrasonicDetectionInterface)
{
    using UltrasonicDetectionInterface = ara::adi::sensoritf::UltrasonicDetectionsInterface;
    ptrUltrasonicDetectionInterface_ = std::make_shared<UltrasonicDetectionInterface>(ultrasonicDetectionInterface);
}

void SensorDataMapper::setUltrasonicFeatureInterface(
    const ara::adi::sensoritf::UltrasonicFeatureInterface ultrasonicFeatureInterface)
{
    using UltrasonicFeatureInterface = ara::adi::sensoritf::UltrasonicFeatureInterface;
    ptrUltrasonicFeatureInterface_ = std::make_shared<UltrasonicFeatureInterface>(ultrasonicFeatureInterface);
}

osi3::SensorData SensorDataMapper::mapSensorData() const
{
    osi3::SensorData sensorData{};

    // TODO: Confirm if single sensor-related information is common among all the
    // service interfaces. Mapped sensor information from one of the service interface

    // if (ptrRoadObjectInterface_ == nullptr) {
    //    return {};
    //}
    //
    //// Map InterfaceVersion
    // osi3::InterfaceVersion* version = sensorData.mutable_version();
    //*version =
    // mapInterfaceVersion((*ptrRoadObjectInterface_).RoadObjectInterfaceHeader.InformationInterface.VersionID);
    //
    //// Map Timestamp
    // osi3::Timestamp* time = sensorData.mutable_timestamp();
    //*time = mapToTimestamp((*ptrRoadObjectInterface_).RoadObjectInterfaceHeader.InformationInterface.TimeStamp);
    //
    //// Map BaseMoving
    //// No Vehicle Postion is available
    //// sensorData.set_allocated_host_vehicle_location();
    //// sensorData.set_allocated_host_vehicle_location_rmse();
    //
    //// Map sensor_id
    // m_logger.LogInfo() << "Map sensor_id";
    // auto sensors = (*ptrRoadObjectInterface_).RoadObjectInterfaceHeader.InformationInterface.ValidServingSensors;
    //// TODO unclear why there are more than one sensor.
    //// There is also just one sonsor pose. So how could exist seleral sensors with the same pose?
    // if (sensors.size() > 0) {
    //    std::uint64_t id = static_cast<std::uint64_t>(sensors.at(0));
    //    osi3::Identifier* sensorID = sensorData.mutable_sensor_id();
    //    sensorID->set_value(id);
    //}
    //
    //// Map mounting_position
    // osi3::MountingPosition* mountingPosition = sensorData.mutable_mounting_position();
    //*mountingPosition = mapSensorPose2Osi((*ptrRoadObjectInterface_).RoadObjectInterfaceHeader.SensorPose);

    // Map mounting_position_rmse
    // osi3::MountingPosition* mountingPositionRsme = sensorData.mutable_mounting_position_rmse();
    //*mountingPositionRsme =
    // mapToMountingPositionError((*ptrRoadObjectInterface_).RoadObjectInterfaceHeader.SensorPose);

    // Map sensor_view
    // BD: Sensor view contains ground truth typically from simulators. Might not receive this info from adi.
    // TODO Implement as soon as the detected interfaces are implemented
    // sensorData.add_sensor_view

    // Map last_measurement_time
    // TODO Is there a ADI equivalent?
    // osi3::Timestamp measurementTime;
    // measurementTime.set_seconds(0);
    // sensorData.set_allocated_last_measurement_time(&measurementTime);

    // Common Entity Header for all Static Objects Interface Elements
    detectedStationaryObjects(sensorData);

    // Common Entity Header for all Potentially Moving Objects Interface Elements
    detectedPotentiallyMovingObj(sensorData);

    // Map detected traffic signs
    detectedTrafficSigns(sensorData);

    // Map detected traffic lights
    detectedTrafficLights(sensorData);

    // Common Entity Header for all Road Object Interface Elements
    // Map road_marking
    detectedRoadMarking(sensorData);

    // Map lane boundary
    detectedLaneBoundary(sensorData);

    // Map occupant_header: NA

    // Map DetectedOccupant: NA

    // Map FeatureData (Camera Feature Data seperated from all Feature data)
    // TODO: Verify correctness for fusion data
    featureData(sensorData);

    // Map LogicalDetectionData
    // (Can Map either Camera Logical data or Ultrasonic Logical data)
    // TODO: Verify correctness for data when both the logical data available
    logicalDetectionData(sensorData);

    return sensorData;
}

osi3::MountingPosition SensorDataMapper::mapSensorPose2Osi(
    const ara::adi::sensoritf::InformationSensorPose& sensorPose) const
{
    m_logger.LogInfo() << "mapSensorPose2Osi";
    osi3::MountingPosition mountingPose;

    ara::adi::sensoritf::Point3D point = sensorPose.SensorOriginPoint;
    osi3::Vector3d* position = mountingPose.mutable_position();
    *position = mapPoint3D2Osi(point);

    ara::adi::sensoritf::Orientation3D orientation = sensorPose.SensorOrientation;
    osi3::Orientation3d* osiOrientation = mountingPose.mutable_orientation();
    *osiOrientation = mapOrientation3D2Osi(orientation);

    return mountingPose;
}

osi3::MountingPosition SensorDataMapper::mapToMountingPositionError(
    const ara::adi::sensoritf::InformationSensorPose& sensorPose) const
{
    m_logger.LogInfo() << "mapToMountingPositionError";
    osi3::MountingPosition mountingPose;

    ara::adi::sensoritf::Point3DError pointError = sensorPose.SensorOriginPointError;
    osi3::Vector3d* position = mountingPose.mutable_position();
    *position = mapPoint3DError2Osi(pointError);

    ara::adi::sensoritf::Orientation3DError orientationError = sensorPose.SensorOrientationError;
    osi3::Orientation3d* osiOrientation = mountingPose.mutable_orientation();
    *osiOrientation = mapToOrientation3dError(orientationError);

    return mountingPose;
}

void SensorDataMapper::detectedRoadMarking(osi3::SensorData& sensorData) const
{
    m_logger.LogInfo() << "detectedRoadMarking";
    if (ptrRoadObjectInterface_ == nullptr) {
        return;
    }

    // header
    ara::adi::sensoritf::InformationInterface intformationInterface
        = (*ptrRoadObjectInterface_).RoadObjectInterfaceHeader.InformationInterface;
    osi3::DetectedEntityHeader* pRoadMarkingHeader{sensorData.mutable_road_marking_header()};
    osi3::DetectedEntityHeader roadMarkingHeader{
        detectedEntityHeaderMapper.mapDetectedEntityHeader(intformationInterface)};
    *pRoadMarkingHeader = roadMarkingHeader;

    // Map DetectedRoadMarking

    auto roadMarkings = (*ptrRoadObjectInterface_).RoadMarkingObjectList.ValidRoadMarkings;

    std::uint16_t numroadMarkings = roadMarkings.size();

    if (numroadMarkings == 0) {
        return;
    }

    if (roadMarkings.size() != (*ptrRoadObjectInterface_).RoadMarkingObjectList.NumberOfValidRoadMarkings) {
        m_logger.LogWarn() << "Warning: Missmatch between NumberOfValidRoadMarkings: "
                           << (*ptrRoadObjectInterface_).RoadMarkingObjectList.NumberOfValidRoadMarkings
                           << " and ValidRoadMarkings.size(): " << roadMarkings.size();
        m_logger.LogWarn() << "Warning: Missmatch between NumberOfValidRoadMarkings: "
                           << (*ptrRoadObjectInterface_).RoadMarkingObjectList.NumberOfValidRoadMarkings
                           << " and roadMarkings.size(): " << roadMarkings.size();
    }

    for (auto it = roadMarkings.begin(); it != roadMarkings.end(); ++it) {
        osi3::DetectedRoadMarking* detectedRoadMarking{sensorData.add_road_marking()};
        auto colourModel = (*ptrRoadObjectInterface_).RoadObjectInterfaceHeader.InterfaceExtension.ColourModelType;
        *detectedRoadMarking = detectedRoadMarkingMapper.mapDetectedRoadMaking(*it.base(), colourModel);
    }
}

void SensorDataMapper::detectedLaneBoundary(osi3::SensorData& sensorData) const
{
    m_logger.LogInfo() << "detectedLaneBoundary";
    if (ptrRoadObjectInterface_ == nullptr) {
        return;
    }

    // Map lane_boundary_header
    ara::adi::sensoritf::InformationInterface intformationInterface
        = (*ptrRoadObjectInterface_).RoadObjectInterfaceHeader.InformationInterface;
    osi3::DetectedEntityHeader* pLaneBoundaryHeader{sensorData.mutable_lane_boundary_header()};
    osi3::DetectedEntityHeader laneBoundaryHeader{
        detectedEntityHeaderMapper.mapDetectedEntityHeader(intformationInterface)};
    *pLaneBoundaryHeader = laneBoundaryHeader;

    // Map DetectedLaneBoundary
    auto laneBoundary = (*ptrRoadObjectInterface_).RoadBoundariesObjectList.RoadBoundaryList;

    std::uint16_t numlaneBoundary = laneBoundary.size();

    if (numlaneBoundary == 0) {
        return;
    }

    if (laneBoundary.size() != (*ptrRoadObjectInterface_).RoadBoundariesObjectList.NumberOfValidRoadBoundaries) {

        m_logger.LogWarn() << "Warning: Missmatch between NumberOfValidRoadBoundaries: "
                           << (*ptrRoadObjectInterface_).RoadBoundariesObjectList.NumberOfValidRoadBoundaries
                           << " and laneBoundary.size(): " << laneBoundary.size();
    }
    for (auto it = laneBoundary.begin(); it != laneBoundary.end(); ++it) {
        osi3::DetectedLaneBoundary* ptrOsiDetectedLaneBoundary = sensorData.add_lane_boundary();

        // TODO Check the need of color model
        // auto colourModel =
        //    (*ptrRoadObjectInterface_).RoadObjectInterfaceHeader.
        //    InterfaceExtension.ColourModelType;
        //*ptrOsiDetectedLaneBoundary = detectedLaneBoundaryMapper.
        //    mapDetectedLaneBoundary(*it.base(), colourModel);

        *ptrOsiDetectedLaneBoundary = detectedLaneBoundaryMapper.mapDetectedLaneBoundary(*it.base());
    }

    // Map lane_header
    osi3::DetectedEntityHeader* laneHeader = sensorData.mutable_lane_header();
    *laneHeader = laneBoundaryHeader;

    // Map DetectedLane
    // TODO: Corrections needed in adi RoadSurface structure
    // sensorData.add_lane();
    //
}

void SensorDataMapper::detectedPotentiallyMovingObj(osi3::SensorData& sensorData) const
{
    m_logger.LogInfo() << "detectedPotentiallyMovingObj";

    if (ptrPMOInterface_ == nullptr) {
        return;
    }

    // Map moving_object_header
    ara::adi::sensoritf::InformationInterface potentiallyMovObjInformationInterface{
        (*ptrPMOInterface_).PotentiallyMovingObjectInterfaceHeader.InformationInterface};
    osi3::DetectedEntityHeader* osiMovingObjHeader{sensorData.mutable_moving_object_header()};

    *osiMovingObjHeader = detectedEntityHeaderMapper.mapDetectedEntityHeader(potentiallyMovObjInformationInterface);

    // Map DetectedMovingObject
    std::uint16_t numDetectedMovingObj
        = (*ptrPMOInterface_).PotentiallyMovingObjectList.ValidPotentiallyMovingObjects.size();

    if (numDetectedMovingObj == 0) {
        return;
    }

    if (numDetectedMovingObj != (*ptrPMOInterface_).PotentiallyMovingObjectList.NumberOfValidPotentiallyMovingObjects) {

        m_logger.LogWarn() << "Warning: Mismatch between "
                              "NumberOfValidPotentiallyMovingObjects: "
                           << (*ptrPMOInterface_).PotentiallyMovingObjectList.NumberOfValidPotentiallyMovingObjects
                           << " and numDetectedMovingObj " << numDetectedMovingObj;
    }

    // osi3::DetectedMovingObject* ptrDetectedMovingObject{};

    auto validPotentiallyMovingObjs{(*ptrPMOInterface_).PotentiallyMovingObjectList.ValidPotentiallyMovingObjects};
    for (auto it = validPotentiallyMovingObjs.begin(); it != validPotentiallyMovingObjs.end(); ++it) {

        osi3::DetectedMovingObject* ptrDetectedMovingObject = sensorData.add_moving_object();
        *ptrDetectedMovingObject = detectedMovingObjectMapper.mapDetectedMovingObject(*it.base());
    }
}

void SensorDataMapper::detectedStationaryObjects(osi3::SensorData& sensorData) const
{
    m_logger.LogInfo() << "detectedStationaryObjects";

    if (ptrStaticObjectInterface_ == nullptr) {
        return;
    }

    ara::adi::sensoritf::InformationInterface staticObjInformationInterface
        = (*ptrStaticObjectInterface_).StaticObjectInterfaceHeader.InformationInterface;

    // Map stationary_object_header
    osi3::DetectedEntityHeader* osiStationaryObjHeader{sensorData.mutable_stationary_object_header()};
    *osiStationaryObjHeader = detectedEntityHeaderMapper.mapDetectedEntityHeader(staticObjInformationInterface);

    // Map DetectedStationaryObject
    std::uint16_t numDetectedStationaryObj
        = (*ptrStaticObjectInterface_).StaticObjectGeneralLandmarks.GeneralLandmarksList.size();

    if (numDetectedStationaryObj == 0) {
        return;
    }

    if (numDetectedStationaryObj
        != (*ptrStaticObjectInterface_).StaticObjectGeneralLandmarks.NumberOfValidGeneralLandmarks) {
        m_logger.LogWarn() << "Warning: Mismatch between "
                              "NumberOfValidGeneralLandmarks: "
                           << (*ptrStaticObjectInterface_).StaticObjectGeneralLandmarks.NumberOfValidGeneralLandmarks
                           << " and numDetectedStationaryObj " << numDetectedStationaryObj;
    }

    // osi3::DetectedStationaryObject* ptrDetectedStationaryObject;

    auto detectedStationaryObjList{(*ptrStaticObjectInterface_).StaticObjectGeneralLandmarks.GeneralLandmarksList};

    for (auto it = detectedStationaryObjList.begin(); it != detectedStationaryObjList.end(); ++it) {

        osi3::DetectedStationaryObject* ptrDetectedStationaryObject = sensorData.add_stationary_object();
        *ptrDetectedStationaryObject = detectedStationaryObjectMapper.mapDetectedStationaryObject(*it.base());
    }
}

void SensorDataMapper::detectedTrafficLights(osi3::SensorData& sensorData) const
{
    m_logger.LogInfo() << "detectedTrafficLights";

    if (ptrStaticObjectInterface_ == nullptr) {
        return;
    }
    // Map traffic_light_header
    ara::adi::sensoritf::InformationInterface staticObjInformationInterface
        = (*ptrStaticObjectInterface_).StaticObjectInterfaceHeader.InformationInterface;

    osi3::DetectedEntityHeader* osiStationaryObjHeader{sensorData.mutable_traffic_light_header()};
    *osiStationaryObjHeader = detectedEntityHeaderMapper.mapDetectedEntityHeader(staticObjInformationInterface);

    // Map DetectedTrafficLight
    std::uint16_t numberTrafficLights = (*ptrStaticObjectInterface_).StaticObjectTrafficLights.TrafficLightList.size();

    if (numberTrafficLights == 0) {
        return;
    }

    if (numberTrafficLights != (*ptrStaticObjectInterface_).StaticObjectTrafficLights.NumberOfValidTrafficLights) {
        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidTrafficLights: "
                           << (*ptrStaticObjectInterface_).StaticObjectTrafficLights.NumberOfValidTrafficLights
                           << " and TrafficLightList.size(): " << numberTrafficLights;
    }

    auto trafficLightsList{(*ptrStaticObjectInterface_).StaticObjectTrafficLights.TrafficLightList};

    for (std::uint16_t i = 0; i < numberTrafficLights; ++i) {
        std::uint16_t numberTrafficLight = (*ptrStaticObjectInterface_)
                                               .StaticObjectTrafficLights.TrafficLightList.at(i)
                                               .TrafficLightsSpots.ValidTrafficLightSpotList.size();

        if (numberTrafficLight
            != (*ptrStaticObjectInterface_)
                   .StaticObjectTrafficLights.TrafficLightList.at(i)
                   .TrafficLightsSpots.NumberOfValidTrafficLightSpots) {

            m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidTrafficLightSpots: "
                               << (*ptrStaticObjectInterface_)
                                      .StaticObjectTrafficLights.TrafficLightList.at(i)
                                      .TrafficLightsSpots.NumberOfValidTrafficLightSpots
                               << " and ValidTrafficLightSpotList.size(): " << numberTrafficLight;
        }

        // osi3::DetectedTrafficLight* ptrDetectedTrafficLight;

        for (std::uint16_t j = 0; j < numberTrafficLight; ++j) {
            osi3::DetectedTrafficLight* ptrDetectedTrafficLight = sensorData.add_traffic_light();
            *ptrDetectedTrafficLight = detectedTrafficLightMapper.mapDetectedTrafficLight(
                (*ptrStaticObjectInterface_).StaticObjectTrafficLights.TrafficLightList.at(i), j);
        }
    }
}

void SensorDataMapper::detectedTrafficSigns(osi3::SensorData& sensorData) const
{
    m_logger.LogInfo() << "detectedTrafficSigns";

    if (ptrStaticObjectInterface_ == nullptr) {
        return;
    }

    // Map traffic_sign_header

    ara::adi::sensoritf::InformationInterface staticObjInformationInterface
        = (*ptrStaticObjectInterface_).StaticObjectInterfaceHeader.InformationInterface;

    osi3::DetectedEntityHeader* osiStationaryObjHeader{sensorData.mutable_traffic_sign_header()};
    *osiStationaryObjHeader = detectedEntityHeaderMapper.mapDetectedEntityHeader(staticObjInformationInterface);

    // Map DetectedTrafficSign

    std::uint16_t numberTrafficSigns = (*ptrStaticObjectInterface_).StaticObjectTrafficSigns.TrafficSignsList.size();
    auto trafficSignsList{(*ptrStaticObjectInterface_).StaticObjectTrafficSigns.TrafficSignsList};

    if (numberTrafficSigns == 0) {
        return;
    }

    if (numberTrafficSigns != (*ptrStaticObjectInterface_).StaticObjectTrafficSigns.NumberOfValidTrafficSigns) {
        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidTrafficSigns: "
                           << (*ptrStaticObjectInterface_).StaticObjectTrafficSigns.NumberOfValidTrafficSigns
                           << " and TrafficSignsList.size(): " << numberTrafficSigns;
    }

    // osi3::DetectedTrafficSign* ptrDetectedTrafficSign;

    for (auto it = trafficSignsList.begin(); it != trafficSignsList.end(); ++it) {

        osi3::DetectedTrafficSign* ptrDetectedTrafficSign = sensorData.add_traffic_sign();
        *ptrDetectedTrafficSign = detectedTrafficSignMapper.mapDetectedTrafficSign(*it.base());
    }
}

void SensorDataMapper::featureData(osi3::SensorData& sensorData) const
{
    // Test only Camera Featre data
    // if (ptrCameraDetectionsInterface_ == nullptr) {
    //    return;
    //}
    //
    // osi3::FeatureData* osiFeatureData = sensorData.mutable_feature_data();
    //
    //*osiFeatureData = cameraFeatureDataMapper.
    //    mapCameraFeatureData(*ptrCameraDetectionsInterface_);

    m_logger.LogInfo() << "featureData";

    // Test all feature data
    if (ptrCameraDetectionsInterface_ == nullptr || ptrUltrasonicDetectionInterface_ == nullptr
        || ptrLidarDetectionInterface_ == nullptr || ptrRadarDetectionInterface_ == nullptr) {

        return;
    }

    osi3::FeatureData* osiFeatureData = sensorData.mutable_feature_data();

    *osiFeatureData = featureDataMapper.mapFeatureData(ptrCameraDetectionsInterface_,
        ptrUltrasonicDetectionInterface_,
        ptrLidarDetectionInterface_,
        ptrRadarDetectionInterface_);
}

void SensorDataMapper::logicalDetectionData(osi3::SensorData& sensorData) const
{
    m_logger.LogInfo() << "logicalDetectionData";
    osi3::LogicalDetectionData* osiLogicalDetectionData{sensorData.mutable_logical_detection_data()};

    // Test only Camera Feature Interface

    // if (ptrCameraFeatureInterface_ == nullptr) {
    //    m_logger.LogInfo() << "ptrCameraFeatureInterface_ null";
    //    return;
    //}
    //*osiLogicalDetectionData = logicalDetectionDataMapper.
    //    mapLogicalDetectionData(*ptrCameraFeatureInterface_);

    // Test only Ultrasonic Feature Interface

    if (ptrUltrasonicFeatureInterface_ == nullptr) {
        m_logger.LogInfo() << "ptrUltrasonicFeatureInterface_ null";
        return;
    }
    *osiLogicalDetectionData
        = ultrasonicLogicalDetectionDataMapper.mapLogicalDetectionData(*ptrUltrasonicFeatureInterface_);
}
