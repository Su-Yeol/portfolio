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

#include "ara/log/logger.h"

#include "osiMappers/FeatureDataMapper.h"
#include "osiMappers/GeneralHeaderMapper.h"

osi3::FeatureData FeatureDataMapper::mapFeatureData(
    const std::shared_ptr<ara::adi::sensoritf::CameraDetectionsInterface> cameraDetectionsInterface,
    const std::shared_ptr<ara::adi::sensoritf::UltrasonicDetectionsInterface> ultrasonicDetectionsInterface,
    const std::shared_ptr<ara::adi::sensoritf::LidarDetectionsInterface> lidarDetectionsInterface,
    const std::shared_ptr<ara::adi::sensoritf::RadarDetectionsInterface> radarDetectionsInterface) const
{
    osi3::FeatureData osiFeatureData{};

    // TODO : InterfaceVersion Mapping
    // One common feature data header for all the sensors, hence could map the
    // osi::InterfaceVersion to any one of the available sensor headers

    // osi3::InterfaceVersion* osiInterfaceVersion{osiFeatureData.mutable_version()};
    // osiInterfaceVersion->set_version_major(static_cast<uint32_t>(
    //    cameraDetectionsInterface.CameraDetectionInterfaceHeader.
    //    InformationInterface.VersionID.InterfaceVersionIDMajor));
    // osiInterfaceVersion->set_version_minor(static_cast<uint32_t>(
    //    cameraDetectionsInterface.CameraDetectionInterfaceHeader.
    //    InformationInterface.VersionID.InterfaceVersionIDMinor));
    // osiInterfaceVersion->set_version_patch(static_cast<uint32_t>(
    //    cameraDetectionsInterface.CameraDetectionInterfaceHeader.
    //    InformationInterface.VersionID.InterfaceVersionIDPatch));

    // CameraDetectionData Mapping
    if (cameraDetectionsInterface != nullptr) {
        osi3::CameraDetectionData* osiCameraDetectionData = osiFeatureData.add_camera_sensor();
        *osiCameraDetectionData = mapCameraDetectionData(*cameraDetectionsInterface);
    }

    // UltrasonicDetectionData Mapping
    if (ultrasonicDetectionsInterface != nullptr) {
        osi3::UltrasonicDetectionData* osiUltrasonicDetectionData = osiFeatureData.add_ultrasonic_sensor();
        *osiUltrasonicDetectionData = mapUltrasonicDetectionData(*ultrasonicDetectionsInterface);
    }

    // LidarDetectionData Maping
    if (lidarDetectionsInterface != nullptr) {
        osi3::LidarDetectionData* osiLidarDetectionData = osiFeatureData.add_lidar_sensor();
        *osiLidarDetectionData = mapLidarDetectionData(*lidarDetectionsInterface);
    }

    // RadarDetectionData Maping
    if (radarDetectionsInterface != nullptr) {
        osi3::RadarDetectionData* osiRadarDetectionData = osiFeatureData.add_radar_sensor();
        *osiRadarDetectionData = mapRadarDetectionData(*radarDetectionsInterface);
    }

    return osiFeatureData;
}

osi3::CameraDetectionData FeatureDataMapper::mapCameraDetectionData(
    const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface) const
{
    osi3::CameraDetectionData osiCameraDetectionData{};

    // Header Mapping
    osi3::SensorDetectionHeader* osiSensorDetectionHeader{osiCameraDetectionData.mutable_header()};
    *osiSensorDetectionHeader = mapSensorDetectionHeader(cameraDetectionsInterface);

    // CameraDetectionSpecificHeader Mapping: NA

    // osi3::CameraDetectionSpecificHeader specific_header{};
    // specific_header.set_number_of_valid_points()
    // osiCameraDetectionData.set_allocated_specific_header()

    // detection and point Mapping
    size_t numCameraDetections = cameraDetectionsInterface.ValidCameraDetectionList.size();

    if (numCameraDetections == 0) {
        return {};
    }
    // Warning for numCameraDetections already done in SensorDetectionHeader
    for (size_t index = 0; index < numCameraDetections; index++) {
        osi3::CameraDetection* osiCameraDetection{};
        ara::adi::sensoritf::CameraShapesShapePoints camerShapePoints{};
        size_t numCameraShapePoints{};

        osiCameraDetection = osiCameraDetectionData.add_detection();
        *osiCameraDetection = mapCameraDetection(cameraDetectionsInterface, index);

        // point Mapping
        camerShapePoints = cameraDetectionsInterface.ValidCameraDetectionList.at(index).CameraShapePoints;
        numCameraShapePoints = camerShapePoints.ValidShapePointsDetectionLevelList.size();

        if (numCameraShapePoints == 0) {
            return {};
        }

        if (numCameraShapePoints
            != static_cast<size_t>(cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                       .CameraShapePoints.NumberOfValidShapePointsDetectionLevel)) {
            m_logger.LogWarn() << "Warning: Mismatch between "
                                  "NumberOfValidShapePointsDetectionLevel: "
                               << static_cast<size_t>(cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                                          .CameraShapePoints.NumberOfValidShapePointsDetectionLevel)
                               << " and numCameraShapePoints: " << numCameraShapePoints;
        }

        for (size_t cameraPointIndex = 0; cameraPointIndex < numCameraShapePoints; cameraPointIndex++) {
            osi3::CameraPoint* osiCameraPoint = osiCameraDetectionData.add_point();
            *osiCameraPoint = mapCameraPoint(camerShapePoints, cameraPointIndex);
        }
    }

    return osiCameraDetectionData;
}

osi3::SensorDetectionHeader FeatureDataMapper::mapSensorDetectionHeader(
    const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface) const
{
    osi3::SensorDetectionHeader osiSensorDetectionHeader{};

    // Measurement_time Mapping

    // datatype mismatch adi uint64 --> osi int64
    // osi3::Timestamp osiTimestamp{};
    // uint64_t CFITimestamp = CameraDetectionsInterface.CameraFeatureInterfaceHeader.InformationInterface.TimeStamp;
    // osiTimestamp.set_seconds();
    // osiTimestamp.set_nanos();
    // osiSensorDetectionHeader.set_allocated_measurement_time();

    // mounting_position Mapping
    osi3::MountingPosition* osiMountingPosition{osiSensorDetectionHeader.mutable_mounting_position()};
    osi3::Orientation3d* osiOrientation{osiMountingPosition->mutable_orientation()};
    *osiOrientation
        = mapOrientation3D2Osi(cameraDetectionsInterface.CameraDetectionInterfaceHeader.SensorPose.SensorOrientation);

    osi3::Vector3d* osiPosition{osiMountingPosition->mutable_position()};
    *osiPosition
        = mapPoint3D2Osi(cameraDetectionsInterface.CameraDetectionInterfaceHeader.SensorPose.SensorOriginPoint);

    // mounting_position_rmse Mapping
    osi3::MountingPosition* osiMountingPositionError{osiSensorDetectionHeader.mutable_mounting_position_rmse()};
    osi3::Vector3d* osiPositionError{osiMountingPositionError->mutable_position()};
    *osiPositionError = mapPoint3DError2Osi(
        cameraDetectionsInterface.CameraDetectionInterfaceHeader.SensorPose.SensorOriginPointError);
    osi3::Orientation3d* osiOrientationError{osiMountingPositionError->mutable_orientation()};
    *osiOrientationError = mapToOrientation3dError(
        cameraDetectionsInterface.CameraDetectionInterfaceHeader.SensorPose.SensorOrientationError);

    // sensor_id Mapping
    auto sensorIDList{
        cameraDetectionsInterface.CameraDetectionInterfaceHeader.SensorClusterInformation.SensorIDReferenceList};

    if (sensorIDList.size() != 0) {
        size_t index = 0;
        osi3::Identifier* osiSensorID{osiSensorDetectionHeader.mutable_sensor_id()};
        osiSensorID->set_value(static_cast<uint64_t>(sensorIDList.at(index)));
    }

    // cycle_counter Mapping
    osiSensorDetectionHeader.set_cycle_counter(static_cast<uint64_t>(
        cameraDetectionsInterface.CameraDetectionInterfaceHeader.InformationInterface.CycleCounter));

    // data_qualifier Mapping
    osi3::SensorDetectionHeader_DataQualifier osiSDDataQualifier{};

    switch (cameraDetectionsInterface.CameraDetectionInterfaceHeader.InformationInterface.DataQualifier) {
    case ara::adi::sensoritf::DataQualifier::kNormal: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
        break;
    }
    // No exact mapping available
    case ara::adi::sensoritf::DataQualifier::kReduceInViewAndPerformance: {
        osiSDDataQualifier = osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for data_qualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent SensorDetectionHeader_DataQualifier_"
                              "DATA_QUALIFIER_AVAILABLE_REDUCED";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kInvalid: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_INVALID;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kNotAvailable: {
        osiSDDataQualifier = osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInCoverage: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent SensorDetectionHeader_DataQualifier_"
                              "DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInPerformance: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent SensorDetectionHeader_DataQualifier_"
                              "DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kTestMode: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent SensorDetectionHeader_DataQualifier_"
                              "DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kUnknown: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN;
        break;
    }
    default: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN;
        m_logger.LogError() << "Error: mapSensorDetectionHeader: "
                               "DataQualifier --> Unknown enum entry";
        break;
    }
    }
    osiSensorDetectionHeader.set_data_qualifier(osiSDDataQualifier);

    // extended_qualifier Mapping: Not avaialble
    // osiSensorDetectionHeader.set_extended_qualifier();

    // number_of_valid_detections Mapping
    uint32_t numCameraDetections = cameraDetectionsInterface.ValidCameraDetectionList.size();

    if (numCameraDetections != static_cast<uint32_t>(cameraDetectionsInterface.NumberOfValidShapes)) {

        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidShapes: "
                           << static_cast<size_t>(cameraDetectionsInterface.NumberOfValidShapes)
                           << " and numCameraDetections: " << numCameraDetections;
    }

    osiSensorDetectionHeader.set_number_of_valid_detections(numCameraDetections);

    return osiSensorDetectionHeader;
}

osi3::CameraDetection FeatureDataMapper::mapCameraDetection(
    const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface,
    const size_t& index) const
{
    osi3::CameraDetection osiCameraDetection{};

    // existence_probability Mapping
    osiCameraDetection.set_existence_probability(
        static_cast<double>(cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                .CameraShapesStatus.ExistenceProbabilityDetectionLevel));

    // object_id Mapping
    auto objectID{cameraDetectionsInterface.ValidCameraDetectionList.at(index).CameraShapesStatus.ObjectID};
    osi3::Identifier* osiIdentifier{osiCameraDetection.mutable_object_id()};
    osiIdentifier->set_value(static_cast<uint64_t>(objectID));

    // time_difference Mapping: NA
    // osiCameraDetection.set_allocated_time_difference();

    // image_shape_type Mapping
    // TODO: OTHER case NA in adi
    osi3::CameraDetection_ImageShapeType osiCameraShapeType{};

    switch (cameraDetectionsInterface.ValidCameraDetectionList.at(index).CameraShapePoints.ShapeTypeDetectionLevel) {
    case ara::adi::sensoritf::ShapeType::kBox: {
        osiCameraShapeType = osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_BOX;
        break;
    }
    case ara::adi::sensoritf::ShapeType::kEllipse: {
        osiCameraShapeType
            = osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_ELLIPSE;
        break;
    }
    case ara::adi::sensoritf::ShapeType::kPoint: {
        osiCameraShapeType
            = osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_POINT;
        break;
    }
    case ara::adi::sensoritf::ShapeType::kPointCloud: {
        osiCameraShapeType
            = osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_POINT_CLOUD;
        break;
    }
    case ara::adi::sensoritf::ShapeType::kPolygon: {
        osiCameraShapeType
            = osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_POLYGON;
        break;
    }
    case ara::adi::sensoritf::ShapeType::kPolyline: {
        osiCameraShapeType
            = osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_POLYLINE;
        break;
    }
    case ara::adi::sensoritf::ShapeType::kUnknown: {
        osiCameraShapeType
            = osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_UNKNOWN;
        break;
    }
    default: {
        osiCameraShapeType
            = osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_UNKNOWN;
        m_logger.LogError() << "Error: mapCameraDetection: ShapeType --> "
                               "Unknown enum entry";
        break;
    }
    }
    osiCameraDetection.set_image_shape_type(osiCameraShapeType);

    // image_shape classification
    ara::adi::sensoritf::ValidDetectionShapeClassificationVector validShapesList
        = cameraDetectionsInterface.ValidCameraDetectionList.at(index)
              .CameraShapeInformation.ValidShapeClassificationsList;

    auto numCameraShapePoints = validShapesList.size();

    if (numCameraShapePoints == 0) {
        return {};
    }

    if (validShapesList.size()
        != static_cast<size_t>(cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                   .CameraShapeInformation.NumberOfValidShapeClassificationsDetectionLevel)) {
        m_logger.LogWarn() << "Warning: Mismatch between "
                              "NumberOfValidShapeClassificationsDetectionLevel: "
                           << static_cast<size_t>(
                                  cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                      .CameraShapeInformation.NumberOfValidShapeClassificationsDetectionLevel)
                           << " and validShapesList.size(): " << validShapesList.size();
    }

    for (size_t i = 0; i < numCameraShapePoints; i++) {
        switch (validShapesList.at(i).ShapeClassificationTypeDetectionLevel) {
        case ara::adi::sensoritf::ShapeClassificationType::kBackground: {
            osiCameraDetection.set_shape_classification_background(true);
            break;
        }
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //    osiCameraDetection.set_shape_classification_animal()
        //    break;
        //}
        case ara::adi::sensoritf::ShapeClassificationType::kFlat: {
            osiCameraDetection.set_shape_classification_flat(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kForeground: {
            osiCameraDetection.set_shape_classification_foreground(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kGround: {
            osiCameraDetection.set_shape_classification_ground(true);
            break;
        }
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //    osiCameraDetection.set_shape_classification_landmark();
        //    break;
        //}
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //    osiCameraDetection.set_shape_classification_moving_object();
        //    break;
        //}
        case ara::adi::sensoritf::ShapeClassificationType::kSidewalk: {
            osiCameraDetection.set_shape_classification_non_driving_lane(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kNonRoad: {
            osiCameraDetection.set_shape_classification_non_road(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kPedestrian: {
            osiCameraDetection.set_shape_classification_pedestrian(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kPedestrianFront: {
            osiCameraDetection.set_shape_classification_pedestrian_front(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kPedestrianRear: {
            osiCameraDetection.set_shape_classification_pedestrian_rear(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kPedestrianSide: {
            osiCameraDetection.set_shape_classification_pedestrian_side(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kRoad: {
            osiCameraDetection.set_shape_classification_road(true);
            break;
        }
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //    osiCameraDetection.set_shape_classification_road_marking();
        //    break;
        //}
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //    osiCameraDetection.set_shape_classification_sky();
        //    break;
        //}
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //    osiCameraDetection.set_shape_classification_stationary_object();
        //    break;
        //}
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //    osiCameraDetection.set_shape_classification_traffic_light();
        //    break;
        //}
        case ara::adi::sensoritf::ShapeClassificationType::kTrafficSign: {
            osiCameraDetection.set_shape_classification_traffic_sign(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kUpright: {
            osiCameraDetection.set_shape_classification_upright(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kVegetation: {
            osiCameraDetection.set_shape_classification_vegetation(true);
            break;
        }
        case ara::adi::sensoritf::ShapeClassificationType::kVehicle: {
            osiCameraDetection.set_shape_classification_vehicle(true);
            break;
        }
        default: {
            m_logger.LogError() << "Error: mapCameraDetection: "
                                   "ShapeClassificationType --> Unknown enum entry";
            break;
        }
        }

        // Set classification probability
        osiCameraDetection.set_shape_classification_probability(
            static_cast<double>(validShapesList.at(i).ShapeClassificationTypeConfidenceDetectionLevel));
    }

    // color_probability Mapping
    auto colourToneConfidenceObjectLevel{cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                             .CameraShapesShapeColourTone.ColourToneConfidenceObjectLevel};

    osiCameraDetection.set_color_probability(colourToneConfidenceObjectLevel);

    // ambiguity_id Mapping
    auto shapeAmbiguity{
        cameraDetectionsInterface.ValidCameraDetectionList.at(index).CameraShapeInformation.ShapeAmbiguityID};
    auto shapeAmbiguityID{shapeAmbiguity};

    osi3::Identifier* osiAmbiguityID = osiCameraDetection.mutable_ambiguity_id();
    osiAmbiguityID->set_value(static_cast<uint64_t>(shapeAmbiguityID));

    // first_point_index Mapping: NA
    // osiCameraDetection.set_first_point_index();

    // number_of_points Mapping
    uint32_t numValidShapePointsDetectionLevelList = cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                                         .CameraShapePoints.ValidShapePointsDetectionLevelList.size();

    if (numValidShapePointsDetectionLevelList
        != static_cast<uint32_t>(cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                     .CameraShapePoints.NumberOfValidShapePointsDetectionLevel)) {

        m_logger.LogWarn() << "Warning: Mismatch between"
                              "NumberOfValidShapePointsDetectionLevel: "
                           << static_cast<uint32_t>(cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                                        .CameraShapePoints.NumberOfValidShapePointsDetectionLevel)
                           << " and numValidShapePointsDetectionLevelList" << numValidShapePointsDetectionLevelList;
    }

    osiCameraDetection.set_number_of_points(numValidShapePointsDetectionLevelList);

    return osiCameraDetection;
}

osi3::CameraPoint FeatureDataMapper::mapCameraPoint(
    const ara::adi::sensoritf::CameraShapesShapePoints& cameraShapePoints,
    const size_t& index) const
{
    osi3::CameraPoint osiCameraPoint{};

    // Point Mapping
    auto azimuth{cameraShapePoints.ValidShapePointsDetectionLevelList.at(index).Position.azimuth};
    auto distance{cameraShapePoints.ValidShapePointsDetectionLevelList.at(index).Position.distance};
    auto elevation{cameraShapePoints.ValidShapePointsDetectionLevelList.at(index).Position.elevation};

    osi3::Spherical3d* osiSpherical = osiCameraPoint.mutable_point();

    osiSpherical->set_azimuth(static_cast<double>(azimuth));
    osiSpherical->set_distance(static_cast<double>(distance));
    osiSpherical->set_elevation(static_cast<double>(elevation));

    // osiCameraPoint.set_allocated_point(&osiSpherical);

    // Point Rmse Mapping: NA in spherical coordinates

    // osiCameraPoint.set_allocated_point_rmse();

    // existence_probability Mapping
    auto pointExistenceProbabilityDetectionLevel{
        cameraShapePoints.ValidShapePointsDetectionLevelList.at(index).PointExistenceProbabilityDetectionLevel};

    osiCameraPoint.set_existence_probability(static_cast<double>(pointExistenceProbabilityDetectionLevel));

    return osiCameraPoint;
}

osi3::UltrasonicDetectionData FeatureDataMapper::mapUltrasonicDetectionData(
    const ara::adi::sensoritf::UltrasonicDetectionsInterface& ultrasonicDetectionsInterface) const
{
    osi3::UltrasonicDetectionData osiUltrasonicDetectionData{};

    // Header Mapping
    osi3::SensorDetectionHeader* osiSensorDetectionHeader{osiUltrasonicDetectionData.mutable_header()};
    *osiSensorDetectionHeader = mapUltrasonicSensorDetectionHeader(ultrasonicDetectionsInterface);

    // UltrasonicDetectionSpecificHeader Mapping: NA

    // osi3::UltrasonicDetectionSpecificHeader specific_header{};
    // specific_header.set_max_range();
    // specific_header.set_number_of_valid_indirect_detections();
    // osiUltrasonicDetectionData.set_allocated_specific_header();

    // detection Mapping
    // osi3::UltrasonicDetection* osiUltrasonicDetection{};
    size_t numUltrasonicDetections = ultrasonicDetectionsInterface.ValidUltrasonicDetectionList.size();

    if (numUltrasonicDetections == 0) {
        return {};
    }

    if (numUltrasonicDetections != ultrasonicDetectionsInterface.NoValidDetections) {
        m_logger.LogWarn() << "Warning: Mismatch between"
                              "NoValidDetections: "
                           << static_cast<uint32_t>(ultrasonicDetectionsInterface.NoValidDetections)
                           << " and numUltrasonicDetections" << numUltrasonicDetections;
    }

    // Warning for numUltrasonicDetections already done in SensorDetectionHeader
    for (size_t index = 0; index < numUltrasonicDetections; index++) {
        osi3::UltrasonicDetection* osiUltrasonicDetection = osiUltrasonicDetectionData.add_detection();
        *osiUltrasonicDetection = mapUltrasonicDetection(ultrasonicDetectionsInterface, index);
    }

    // Indirect Detection Mapping NA
    // osiUltrasonicDetectionData.add_indirect_detection();

    return osiUltrasonicDetectionData;
}

osi3::SensorDetectionHeader FeatureDataMapper::mapUltrasonicSensorDetectionHeader(
    const ara::adi::sensoritf::UltrasonicDetectionsInterface& ultrasonicDetectionsInterface) const
{
    osi3::SensorDetectionHeader osiSensorDetectionHeader{};

    // Measurement_time Mapping

    // datatype mismatch adi uint64 --> osi int64
    // osi3::Timestamp osiTimestamp{};
    // uint64_t CFITimestamp =
    // ultrasonicDetectionsInterface.CameraFeatureInterfaceHeader.InformationInterface.TimeStamp;
    // osiTimestamp.set_seconds();
    // osiTimestamp.set_nanos();
    // osiSensorDetectionHeader.set_allocated_measurement_time();

    osi3::MountingPosition* osiMountingPosition{osiSensorDetectionHeader.mutable_mounting_position()};
    osi3::Orientation3d* osiOrientation{osiMountingPosition->mutable_orientation()};
    *osiOrientation = mapOrientation3D2Osi(
        ultrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader.SensorPose.SensorOrientation);

    osi3::Vector3d* osiPosition{osiMountingPosition->mutable_position()};
    *osiPosition = mapPoint3D2Osi(
        ultrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader.SensorPose.SensorOriginPoint);

    // mounting_position_rmse Mapping
    osi3::MountingPosition* osiMountingPositionError{osiSensorDetectionHeader.mutable_mounting_position_rmse()};
    osi3::Vector3d* osiPositionError{osiMountingPositionError->mutable_position()};
    *osiPositionError = mapPoint3DError2Osi(
        ultrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader.SensorPose.SensorOriginPointError);
    osi3::Orientation3d* osiOrientationError{osiMountingPositionError->mutable_orientation()};
    *osiOrientationError = mapToOrientation3dError(
        ultrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader.SensorPose.SensorOrientationError);

    // sensor_id Mapping

    auto sensorIDList{ultrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader.SensorClusterInformation
                          .SensorIDReferenceList};

    if (sensorIDList.size() != 0) {
        osi3::Identifier* osiSensorID{osiSensorDetectionHeader.mutable_sensor_id()};
        size_t index = 0;
        osiSensorID->set_value(static_cast<uint64_t>(sensorIDList.at(index)));
    }

    // cycle_counter Mapping
    osiSensorDetectionHeader.set_cycle_counter(static_cast<uint64_t>(
        ultrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader.InformationInterface.CycleCounter));

    // data_qualifier Mapping
    osi3::SensorDetectionHeader_DataQualifier osiSDDataQualifier{};

    switch (ultrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader.InformationInterface.DataQualifier) {
    case ara::adi::sensoritf::DataQualifier::kNormal: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
        break;
    }
    // No exact mapping available
    case ara::adi::sensoritf::DataQualifier::kReduceInViewAndPerformance: {
        osiSDDataQualifier = osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for data_qualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kInvalid: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_INVALID;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kNotAvailable: {
        osiSDDataQualifier = osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInCoverage: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInPerformance: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kTestMode: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kUnknown: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN;
        break;
    }
    default: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN;
        m_logger.LogError() << "Error: mapSensorDetectionHeader: "
                               "DataQualifier --> Unknown enum entry";
        break;
    }
    }
    osiSensorDetectionHeader.set_data_qualifier(osiSDDataQualifier);

    // extended_qualifier Mapping: Not avaialble
    // osiSensorDetectionHeader.set_extended_qualifier();

    // number_of_valid_detections Mapping
    uint32_t numUltrasonicDetections = ultrasonicDetectionsInterface.ValidUltrasonicDetectionList.size();

    if (numUltrasonicDetections != static_cast<uint32_t>(ultrasonicDetectionsInterface.NoValidDetections)) {
        m_logger.LogWarn() << "Warning: Mismatch between NoValidDetections: "
                           << static_cast<size_t>(ultrasonicDetectionsInterface.NoValidDetections)
                           << " and numUltrasonicDetections: " << numUltrasonicDetections;
    }
    osiSensorDetectionHeader.set_number_of_valid_detections(numUltrasonicDetections);

    return osiSensorDetectionHeader;
}

osi3::UltrasonicDetection FeatureDataMapper::mapUltrasonicDetection(
    const ara::adi::sensoritf::UltrasonicDetectionsInterface& ultrasonicDetectionsInterface,
    const size_t& index) const
{
    osi3::UltrasonicDetection osiUltrasonicDetection{};

    auto objectIDAdi{
        ultrasonicDetectionsInterface.ValidUltrasonicDetectionList.at(index).UltrasonicDetectionStatus.ObjectID};
    osi3::Identifier* osiIdentifier{osiUltrasonicDetection.mutable_object_id()};
    osiIdentifier->set_value(static_cast<uint64_t>(objectIDAdi));

    // NA
    // osiUltrasonicDetection.set_distance();
    osiUltrasonicDetection.set_existence_probability(
        static_cast<double>(ultrasonicDetectionsInterface.ValidUltrasonicDetectionList.at(index)
                                .UltrasonicDetectionStatus.ExistenceProbabilityDetectionLevel));

    return osiUltrasonicDetection;
}

osi3::LidarDetectionData FeatureDataMapper::mapLidarDetectionData(
    const ara::adi::sensoritf::LidarDetectionsInterface& lidarDetectionsInterface) const
{
    osi3::LidarDetectionData osiLidarDetectionData{};

    // Header Mapping
    osi3::SensorDetectionHeader* osiSensorDetectionHeader{osiLidarDetectionData.mutable_header()};
    *osiSensorDetectionHeader = mapLidarSensorDetectionHeader(lidarDetectionsInterface);

    // NA
    // osiLidarDetectionData.add_detection();

    return osiLidarDetectionData;
}

// NA
// osi3::LidarDetection FeatureDataMapper::mapLidarDetection(
//    const ara::adi::sensoritf::LidarDetectionsInterface & lidarDetectionsInterface) const
//{
//
//}

osi3::SensorDetectionHeader FeatureDataMapper::mapLidarSensorDetectionHeader(
    const ara::adi::sensoritf::LidarDetectionsInterface& lidarDetectionsInterface) const
{
    m_logger.LogInfo() << "mapLidarSensorDetectionHeader";
    osi3::SensorDetectionHeader osiSensorDetectionHeader{};

    // Measurement_time Mapping

    // datatype mismatch adi uint64 --> osi int64
    // osi3::Timestamp osiTimestamp{};
    // uint64_t CFITimestamp =
    // ultrasonicDetectionsInterface.CameraFeatureInterfaceHeader.InformationInterface.TimeStamp;
    // osiTimestamp.set_seconds();
    // osiTimestamp.set_nanos();
    // osiSensorDetectionHeader.set_allocated_measurement_time();

    // mounting_position Mapping
    osi3::MountingPosition* osiMountingPosition{osiSensorDetectionHeader.mutable_mounting_position()};
    osi3::Orientation3d* osiOrientation{osiMountingPosition->mutable_orientation()};
    *osiOrientation
        = mapOrientation3D2Osi(lidarDetectionsInterface.LidarDetectionInterfaceHeader.SensorPose.SensorOrientation);

    osi3::Vector3d* osiPosition{osiMountingPosition->mutable_position()};
    *osiPosition = mapPoint3D2Osi(lidarDetectionsInterface.LidarDetectionInterfaceHeader.SensorPose.SensorOriginPoint);

    // mounting_position_rmse Mapping
    osi3::MountingPosition* osiMountingPositionError{osiSensorDetectionHeader.mutable_mounting_position_rmse()};
    osi3::Vector3d* osiPositionError{osiMountingPositionError->mutable_position()};
    *osiPositionError
        = mapPoint3DError2Osi(lidarDetectionsInterface.LidarDetectionInterfaceHeader.SensorPose.SensorOriginPointError);
    osi3::Orientation3d* osiOrientationError{osiMountingPositionError->mutable_orientation()};
    *osiOrientationError = mapToOrientation3dError(
        lidarDetectionsInterface.LidarDetectionInterfaceHeader.SensorPose.SensorOrientationError);

    // sensor_id Mapping

    auto sensorIDList{
        lidarDetectionsInterface.LidarDetectionInterfaceHeader.SensorClusterInformation.SensorIDReferenceList};

    if (sensorIDList.size() != 0) {
        osi3::Identifier* osiSensorID{osiSensorDetectionHeader.mutable_sensor_id()};
        size_t index = 0;
        osiSensorID->set_value(static_cast<uint64_t>(sensorIDList.at(index)));
    }

    // cycle_counter Mapping
    osiSensorDetectionHeader.set_cycle_counter(static_cast<uint64_t>(
        lidarDetectionsInterface.LidarDetectionInterfaceHeader.InformationInterface.CycleCounter));

    // data_qualifier Mapping
    osi3::SensorDetectionHeader_DataQualifier osiSDDataQualifier{};
    switch (lidarDetectionsInterface.LidarDetectionInterfaceHeader.InformationInterface.DataQualifier) {
    case ara::adi::sensoritf::DataQualifier::kNormal: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
        break;
    }
    // No exact mapping available
    case ara::adi::sensoritf::DataQualifier::kReduceInViewAndPerformance: {
        osiSDDataQualifier = osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for data_qualifier:kReduceInViewAndPerformance not available."
                              " Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kInvalid: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_INVALID;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kNotAvailable: {
        osiSDDataQualifier = osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInCoverage: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInPerformance: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader"
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kTestMode: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kUnknown: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN;
        break;
    }
    default: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN;
        m_logger.LogError() << "Error: mapSensorDetectionHeader: "
                               "DataQualifier --> Unknown enum entry";
        break;
    }
    }
    osiSensorDetectionHeader.set_data_qualifier(osiSDDataQualifier);

    // extended_qualifier Mapping: Not avaialble
    // osiSensorDetectionHeader.set_extended_qualifier();

    // number_of_valid_detections Mapping: NA
    // uint32_t numUltrasonicDetections = lidarDetectionsInterface. .size();
    // if (numUltrasonicDetections != static_cast<uint32_t>(lidarDetectionsInterface.NoValidDetections))
    //{
    //    m_logger.LogWarn() << "Warning: Mismatch between NoValidDetections: "
    //      << static_cast<size_t>(lidarDetectionsInterface.NoValidDetections)
    //      << " and numUltrasonicDetections: "
    //      << numUltrasonicDetections;
    //}

    // Cannot map the actual size of vector
    osiSensorDetectionHeader.set_number_of_valid_detections(lidarDetectionsInterface.NumberOfValidLidarDetections);

    return osiSensorDetectionHeader;
}

osi3::RadarDetectionData FeatureDataMapper::mapRadarDetectionData(
    const ara::adi::sensoritf::RadarDetectionsInterface& radarDetectionsInterface) const
{
    osi3::RadarDetectionData osiRadarDetectionData{};

    // Header Mapping
    osi3::SensorDetectionHeader* osiSensorDetectionHeader{osiRadarDetectionData.mutable_header()};
    *osiSensorDetectionHeader = mapRadarSensorDetectionHeader(radarDetectionsInterface);

    // detection Mapping
    // osi3::RadarDetection* osiRadarDetection;
    size_t numRadarDetections = radarDetectionsInterface.ValidRadarDetectionsList.size();

    if (numRadarDetections == 0) {
        return {};
    }

    if (numRadarDetections != static_cast<uint32_t>(radarDetectionsInterface.NumberOfValidDetections)) {
        m_logger.LogWarn() << "Warning: Mismatch between NoValidDetections: "
                           << static_cast<size_t>(radarDetectionsInterface.NumberOfValidDetections)
                           << " and numRadarDetections: " << numRadarDetections;
    }

    // Warning for numRadarDetections already done in SensorDetectionHeader
    for (size_t index = 0; index < numRadarDetections; index++) {
        osi3::RadarDetection* osiRadarDetection = osiRadarDetectionData.add_detection();
        *osiRadarDetection = mapRadarDetection(radarDetectionsInterface, index);
    }

    return osiRadarDetectionData;
}

osi3::RadarDetection FeatureDataMapper::mapRadarDetection(
    const ara::adi::sensoritf::RadarDetectionsInterface& radarDetectionsInterface,
    const size_t& index) const
{
    osi3::RadarDetection osiRadarDetection{};

    // existence_probability Mapping
    osiRadarDetection.set_existence_probability(
        static_cast<double>(radarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                .RadarDetectionsStatus.ExistenceProbabilityDetectionLevel));

    // ambiguity_id Mapping
    auto ambiguityAdi{
        radarDetectionsInterface.ValidRadarDetectionsList.at(index).RadarDetectionsInformation.AmbiguityID};
    osi3::Identifier* osiAmbiguityID{osiRadarDetection.mutable_ambiguity_id()};
    osiAmbiguityID->set_value(static_cast<uint64_t>(ambiguityAdi));

    // object_id Mapping
    auto objectIDAdi{radarDetectionsInterface.ValidRadarDetectionsList.at(index).RadarDetectionsStatus.ObjectID};
    osi3::Identifier* osiIdentifier{osiRadarDetection.mutable_object_id()};
    osiIdentifier->set_value(static_cast<uint64_t>(objectIDAdi));

    // position Mapping
    osi3::Spherical3d* osiSphericalPos{osiRadarDetection.mutable_position()};
    osiSphericalPos->set_azimuth(static_cast<double>(
        radarDetectionsInterface.ValidRadarDetectionsList.at(index).RadarDetectionsPosition.DetectionPosition.azimuth));
    osiSphericalPos->set_distance(static_cast<double>(radarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                                          .RadarDetectionsPosition.DetectionPosition.distance));
    osiSphericalPos->set_elevation(static_cast<double>(radarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                                           .RadarDetectionsPosition.DetectionPosition.elevation));

    // position rmse Mapping
    osi3::Spherical3d* osiSphericalPosRmse{osiRadarDetection.mutable_position_rmse()};
    osiSphericalPosRmse->set_azimuth(static_cast<double>(radarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                                             .RadarDetectionsPosition.DetectionPositionError.azimuth));
    osiSphericalPosRmse->set_distance(
        static_cast<double>(radarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                .RadarDetectionsPosition.DetectionPositionError.distance));
    osiSphericalPosRmse->set_elevation(
        static_cast<double>(radarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                .RadarDetectionsPosition.DetectionPositionError.elevation));

    // Classification Mapping
    // TODO: Classification should be of repeated type
    osi3::DetectionClassification osiDetectionClass{};

    ara::adi::sensoritf::ValidDetectionClassificationVector validDetectionClassList
        = radarDetectionsInterface.ValidRadarDetectionsList.at(index)
              .RadarDetectionsInformation.ValidDetectionClassificationList;

    if (validDetectionClassList.size() == 0) {
        return {};
    }

    if (validDetectionClassList.size()
        != static_cast<size_t>(radarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                   .RadarDetectionsInformation.NumberOfValidDetectionClassifications)) {
        m_logger.LogWarn() << "Warning: Mismatch between "
                              "NumberOfValidDetectionClassifications: "
                           << static_cast<size_t>(radarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                                      .RadarDetectionsInformation.NumberOfValidDetectionClassifications)
                           << " and validDetectionClassList.size(): " << validDetectionClassList.size();
    }
    for (size_t i = 0; i < validDetectionClassList.size(); i++) {
        switch (validDetectionClassList.at(i).DetectionClassificationType) {
        case ara::adi::sensoritf::DetectionClassificationType::kNoClassification: {
            m_logger.LogWarn() << "No accurate mapping for "
                                  "DetectionClassificationType::kNoClassification"
                               << "mapped to osi3::DetectionClassification::"
                                  "DETECTION_CLASSIFICATION_INVALID";
            osiDetectionClass = osi3::DetectionClassification::DETECTION_CLASSIFICATION_INVALID;
            break;
        }
        case ara::adi::sensoritf::DetectionClassificationType::kNoise: {
            osiDetectionClass = osi3::DetectionClassification::DETECTION_CLASSIFICATION_CLUTTER;
            break;
        }
        case ara::adi::sensoritf::DetectionClassificationType::kOverdrivable: {
            osiDetectionClass = osi3::DetectionClassification::DETECTION_CLASSIFICATION_OVERDRIVABLE;
            break;
        }
        case ara::adi::sensoritf::DetectionClassificationType::kUnderdriveable: {
            osiDetectionClass = osi3::DetectionClassification::DETECTION_CLASSIFICATION_UNDERDRIVABLE;
            break;
        }
        case ara::adi::sensoritf::DetectionClassificationType::kUnknown: {
            osiDetectionClass = osi3::DetectionClassification::DETECTION_CLASSIFICATION_UNKNOWN;
            break;
        }
        default: {
            m_logger.LogWarn() << "Warn: mapRadarDetection: "
                                  "DetectionClassificationType --> Other enum entry";
            break;
        }
        }

        osiRadarDetection.set_classification(osiDetectionClass);
    }

    // Point target probability Mapping TODO: Verify correctness
    osiRadarDetection.set_point_target_probability(static_cast<double>(
        radarDetectionsInterface.ValidRadarDetectionsList.at(index).RadarDetectionsInformation.MultiTargetProbability));

    // NA
    // osiRadarDetection.set_radial_velocity();

    // NA
    // osiRadarDetection.set_radial_velocity_rmse();

    // RCS Mapping
    osiRadarDetection.set_rcs(static_cast<double>(
        radarDetectionsInterface.ValidRadarDetectionsList.at(index).RadarDetectionsInformation.RadarCrossSection));

    // SNR Mapping
    osiRadarDetection.set_snr(static_cast<double>(radarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                                      .RadarDetectionsInformation.SignalToNoiseRatioDetectionLevel));

    return osiRadarDetection;
}

osi3::SensorDetectionHeader FeatureDataMapper::mapRadarSensorDetectionHeader(
    const ara::adi::sensoritf::RadarDetectionsInterface& radarDetectionsInterface) const
{
    osi3::SensorDetectionHeader osiSensorDetectionHeader{};

    // Measurement_time Mapping

    // datatype mismatch adi uint64 --> osi int64
    // osi3::Timestamp osiTimestamp{};
    // uint64_t CFITimestamp =
    // ultrasonicDetectionsInterface.CameraFeatureInterfaceHeader.InformationInterface.TimeStamp;
    // osiTimestamp.set_seconds();
    // osiTimestamp.set_nanos();
    // osiSensorDetectionHeader.set_allocated_measurement_time();

    // mounting_position Mapping

    osi3::MountingPosition* osiMountingPosition{osiSensorDetectionHeader.mutable_mounting_position()};
    osi3::Orientation3d* osiOrientation{osiMountingPosition->mutable_orientation()};
    *osiOrientation
        = mapOrientation3D2Osi(radarDetectionsInterface.RadarDetectionInterfaceHeader.SensorPose.SensorOrientation);

    osi3::Vector3d* osiPosition{osiMountingPosition->mutable_position()};
    *osiPosition = mapPoint3D2Osi(radarDetectionsInterface.RadarDetectionInterfaceHeader.SensorPose.SensorOriginPoint);

    // mounting_position_rmse Mapping
    osi3::MountingPosition* osiMountingPositionError{osiSensorDetectionHeader.mutable_mounting_position_rmse()};
    osi3::Vector3d* osiPositionError{osiMountingPositionError->mutable_position()};
    *osiPositionError
        = mapPoint3DError2Osi(radarDetectionsInterface.RadarDetectionInterfaceHeader.SensorPose.SensorOriginPointError);
    osi3::Orientation3d* osiOrientationError{osiMountingPositionError->mutable_orientation()};
    *osiOrientationError = mapToOrientation3dError(
        radarDetectionsInterface.RadarDetectionInterfaceHeader.SensorPose.SensorOrientationError);

    // sensor_id Mapping
    auto sensorIDList{
        radarDetectionsInterface.RadarDetectionInterfaceHeader.SensorClusterInformation.SensorIDReferenceList};

    if (sensorIDList.size() != 0) {
        osi3::Identifier* osiSensorID{osiSensorDetectionHeader.mutable_sensor_id()};
        size_t index = 0;
        osiSensorID->set_value(static_cast<uint64_t>(sensorIDList.at(index)));
    }
    // cycle_counter Mapping
    osiSensorDetectionHeader.set_cycle_counter(static_cast<uint64_t>(
        radarDetectionsInterface.RadarDetectionInterfaceHeader.InformationInterface.CycleCounter));

    // data_qualifier Mapping
    osi3::SensorDetectionHeader_DataQualifier osiSDDataQualifier{};
    switch (radarDetectionsInterface.RadarDetectionInterfaceHeader.InformationInterface.DataQualifier) {
    case ara::adi::sensoritf::DataQualifier::kNormal: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
        break;
    }
    // No exact mapping available
    case ara::adi::sensoritf::DataQualifier::kReduceInViewAndPerformance: {
        osiSDDataQualifier = osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for data_qualifier:kReduceInViewAndPerformance not available."
                              "Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kInvalid: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_INVALID;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kNotAvailable: {
        osiSDDataQualifier = osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInCoverage: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInPerformance: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kTestMode: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader "
                              "for DataQualifier:kReduceInViewAndPerformance not available. "
                              "Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kUnknown: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN;
        break;
    }
    default: {
        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN;
        m_logger.LogError() << "Error: mapSensorDetectionHeader: "
                               "DataQualifier --> Unknown enum entry";
        break;
    }
    }
    osiSensorDetectionHeader.set_data_qualifier(osiSDDataQualifier);

    // extended_qualifier Mapping: Not avaialble
    // osiSensorDetectionHeader.set_extended_qualifier();

    // number_of_valid_detections Mapping
    uint32_t numRadarDetections = radarDetectionsInterface.ValidRadarDetectionsList.size();
    if (numRadarDetections != static_cast<uint32_t>(radarDetectionsInterface.NumberOfValidDetections)) {
        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidDetections: "
                           << static_cast<size_t>(radarDetectionsInterface.NumberOfValidDetections)
                           << " and numRadarDetections: " << numRadarDetections;
    }

    osiSensorDetectionHeader.set_number_of_valid_detections(numRadarDetections);

    return osiSensorDetectionHeader;
}
