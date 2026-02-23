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

#include "osiMappers/CameraFeatureDataMapper.h"
#include "osiMappers/GeneralHeaderMapper.h"

// TODO: CameraFeatureData mapping could be discarded as FeatureData contains
// data of all the sensors(See
// FeatureDataMapper.h)

osi3::FeatureData CameraFeatureDataMapper::mapCameraFeatureData(
    const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface) const
{

    osi3::FeatureData osiFeatureData{};
    // InterfaceVersion Mapping
    osi3::InterfaceVersion* osiInterfaceVersion = osiFeatureData.mutable_version();

    auto versionID{cameraDetectionsInterface.CameraDetectionInterfaceHeader.InformationInterface.VersionID};

    osiInterfaceVersion->set_version_major(static_cast<uint32_t>(versionID.InterfaceVersionIDMajor));
    osiInterfaceVersion->set_version_minor(static_cast<uint32_t>(versionID.InterfaceVersionIDMinor));
    osiInterfaceVersion->set_version_patch(static_cast<uint32_t>(versionID.InterfaceVersionIDPatch));

    // CameraDetectionData Mapping
    osi3::CameraDetectionData* osiCameraDetectionData = osiFeatureData.add_camera_sensor();

    *osiCameraDetectionData = mapCameraDetectionData(cameraDetectionsInterface);

    return osiFeatureData;
}

osi3::CameraDetectionData CameraFeatureDataMapper::mapCameraDetectionData(
    const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface) const
{
    osi3::CameraDetectionData osiCameraDetectionData{};
    osi3::SensorDetectionHeader* osiSensorDetectionHeader = osiCameraDetectionData.mutable_header();

    // Header Mapping
    *osiSensorDetectionHeader = mapSensorDetectionHeader(cameraDetectionsInterface);

    // CameraDetectionSpecificHeader Mapping: NA
    // osi3::CameraDetectionSpecificHeader specific_header{};
    // specific_header.set_number_of_valid_points()
    // osiCameraDetectionData.set_allocated_specific_header()

    // detection and point Mapping
    size_t numCameraDetections = cameraDetectionsInterface.ValidCameraDetectionList.size();

    // Warning for numCameraDetections already done in SensorDetectionHeader
    if (numCameraDetections == 0) {
        m_logger.LogError() << "numCameraDetections empty, cannot map";
        return {};
    }

    for (size_t index = 0; index < numCameraDetections; index++) {
        ara::adi::sensoritf::CameraShapesShapePoints camerShapePoints{};
        size_t numCameraShapePoints{};

        osi3::CameraDetection* osiCameraDetection = osiCameraDetectionData.add_detection();
        *osiCameraDetection = mapCameraDetection(cameraDetectionsInterface, index);

        // point Mapping
        camerShapePoints = cameraDetectionsInterface.ValidCameraDetectionList.at(index).CameraShapePoints;
        numCameraShapePoints = cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                   .CameraShapePoints.ValidShapePointsDetectionLevelList.size();

        if (numCameraShapePoints
            != static_cast<size_t>(cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                       .CameraShapePoints.NumberOfValidShapePointsDetectionLevel)) {

            m_logger.LogWarn() << "Warning: Mismatch between"
                                  "NumberOfValidShapePointsDetectionLevel: "
                               << static_cast<size_t>(cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                                          .CameraShapePoints.NumberOfValidShapePointsDetectionLevel)
                               << " and numCameraShapePoints: " << numCameraShapePoints;
        }

        if (numCameraShapePoints == 0) {
            m_logger.LogError() << "numCameraShapePoints empty, cannot map";
            return {};
        }

        for (size_t cameraPointIndex = 0; cameraPointIndex < numCameraShapePoints; cameraPointIndex++) {

            osi3::CameraPoint* osiCameraPoint = osiCameraDetectionData.add_point();
            *osiCameraPoint = mapCameraPoint(camerShapePoints, cameraPointIndex);
        }
    }

    return osiCameraDetectionData;
}

osi3::SensorDetectionHeader CameraFeatureDataMapper::mapSensorDetectionHeader(
    const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface) const
{
    osi3::SensorDetectionHeader osiSensorDetectionHeader{};
    // Measurement_time Mapping

    // datatype mismatch adi uint64 --> osi int64
    // osi3::Timestamp* osiTimestamp = osiSensorDetectionHeader.
    // mutable_measurement_time();
    // uint64_t CFITimestamp = cameraDetectionsInterface.
    // CameraDetectionInterfaceHeader.InformationInterface.TimeStamp;
    // osiTimestamp->set_seconds();
    // osiTimestamp->set_nanos();
    // osiSensorDetectionHeader.set_allocated_measurement_time();

    // mounting_position Mapping
    osi3::MountingPosition* osiMountingPosition = osiSensorDetectionHeader.mutable_mounting_position();

    osi3::Orientation3d* osiOrientation = osiMountingPosition->mutable_orientation();
    auto sensorOrientation{cameraDetectionsInterface.CameraDetectionInterfaceHeader.SensorPose.SensorOrientation};
    *osiOrientation = mapOrientation3D2Osi(sensorOrientation);

    osi3::Vector3d* osiPosition = osiMountingPosition->mutable_position();
    auto sensorOriginPoint{cameraDetectionsInterface.CameraDetectionInterfaceHeader.SensorPose.SensorOriginPoint};
    *osiPosition = mapPoint3D2Osi(sensorOriginPoint);

    // mounting_position_rmse Mapping
    osi3::MountingPosition* osiMountingPositionError = osiSensorDetectionHeader.mutable_mounting_position_rmse();

    osi3::Vector3d* osiPositionError = osiMountingPositionError->mutable_position();
    osi3::Orientation3d* osiOrientationError = osiMountingPositionError->mutable_orientation();

    auto sensorOriginPointError{
        cameraDetectionsInterface.CameraDetectionInterfaceHeader.SensorPose.SensorOriginPointError};
    *osiPositionError = mapPoint3DError2Osi(sensorOriginPointError);

    auto sensorOrientationError{
        cameraDetectionsInterface.CameraDetectionInterfaceHeader.SensorPose.SensorOrientationError};
    *osiOrientationError = mapToOrientation3dError(sensorOrientationError);

    // sensor_id Mapping
    osi3::Identifier* osiSensorID = osiSensorDetectionHeader.mutable_sensor_id();
    size_t index = 0;
    ara::adi::sensoritf::SensorCluster sensorCluster{
        cameraDetectionsInterface.CameraDetectionInterfaceHeader.SensorClusterInformation};

    if (sensorCluster.SensorIDReferenceList.size() == 0) {
        m_logger.LogError() << "sensorCluster.SensorIDReferenceList empty, cannot map";
        return {};
    }

    osiSensorID->set_value(static_cast<uint64_t>(sensorCluster.SensorIDReferenceList.at(index)));

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
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader for "
                              "data_qualifier:kReduceInViewAndPerformance not available."
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
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader"
                              "for DataQualifier:kReduceInViewAndPerformance "
                              "not available. Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInPerformance: {

        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader"
                              "for DataQualifier:kReduceInViewAndPerformance "
                              "not available. Mapped to osi equivalent "
                              "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER";
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kTestMode: {

        osiSDDataQualifier
            = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        m_logger.LogWarn() << "Exact mapping in mapSensorDetectionHeader"
                              "for DataQualifier:kReduceInViewAndPerformance "
                              "not available. Mapped to osi equivalent "
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
        m_logger.LogError() << "Error: mapSensorDetectionHeader: DataQualifier"
                               "--> Unknown enum entry: "
                               "SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN";
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

osi3::CameraDetection CameraFeatureDataMapper::mapCameraDetection(
    const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface,
    const size_t& index) const
{
    osi3::CameraDetection osiCameraDetection{};

    // existence_probability Mapping
    osiCameraDetection.set_existence_probability(
        static_cast<double>(cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                .CameraShapesStatus.ExistenceProbabilityDetectionLevel));

    // object_id Mapping
    osi3::Identifier* osiIdentifier = osiCameraDetection.mutable_object_id();
    osiIdentifier->set_value(static_cast<uint64_t>(
        cameraDetectionsInterface.ValidCameraDetectionList.at(index).CameraShapesStatus.ObjectID));

    // time_difference Mapping: NA
    // osiCameraDetection.set_allocated_time_difference();

    // image_shape_type Mapping
    // TODO: OTHER case NA in adi
    osi3::CameraDetection_ImageShapeType osiCameraShapeType;
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
                               "Unknown enum entry: "
                            << static_cast<std::uint8_t>(osiCameraShapeType);
        break;
    }
    }
    osiCameraDetection.set_image_shape_type(osiCameraShapeType);

    // image_shape classification
    ara::adi::sensoritf::ValidDetectionShapeClassificationVector validShapesList
        = cameraDetectionsInterface.ValidCameraDetectionList.at(index)
              .CameraShapeInformation.ValidShapeClassificationsList;

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

    if (validShapesList.size() == 0) {
        m_logger.LogError() << "validShapesList empty, cannot map";
        return {};
    }

    for (size_t i = 0; i < validShapesList.size(); i++) {

        switch (validShapesList.at(i).ShapeClassificationTypeDetectionLevel) {

        case ara::adi::sensoritf::ShapeClassificationType::kBackground: {

            osiCameraDetection.set_shape_classification_background(true);
            break;
        }
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //     osiCameraDetection.set_shape_classification_animal()
        //     break;
        // }
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
        //     osiCameraDetection.set_shape_classification_landmark();
        //     break;
        // }
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //     osiCameraDetection.set_shape_classification_moving_object();
        //     break;
        // }
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
        //     osiCameraDetection.set_shape_classification_road_marking();
        //     break;
        // }
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //     osiCameraDetection.set_shape_classification_sky();
        //     break;
        // }
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //     osiCameraDetection.set_shape_classification_stationary_object();
        //     break;
        // }
        // case ara::adi::sensoritf::ShapeClassificationType::
        //{
        //     osiCameraDetection.set_shape_classification_traffic_light();
        //     break;
        // }
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
                                   "ShapeClassificationType --> Unknown enum entry: ";
            //<< static_cast<std::uint8_t>(validShapesList.at(i).
            // ShapeClassificationType);
            break;
        }
        }
        // Set classification probability
        osiCameraDetection.set_shape_classification_probability(
            static_cast<double>(validShapesList.at(i).ShapeClassificationTypeConfidenceDetectionLevel));
    }

    // Color description Mapping
    ara::adi::sensoritf::ColourTone cameraShapeColourTone{
        cameraDetectionsInterface.ValidCameraDetectionList.at(index).CameraShapesShapeColourTone};
    ara::adi::sensoritf::ColourModelType cameraShapeColourModelType{
        cameraDetectionsInterface.CameraDetectionInterfaceHeader.InterfaceExtension.ColourModelType};

    osi3::ColorDescription* osiColorDescription = osiCameraDetection.mutable_color_description();
    *osiColorDescription = mapToColorDesription(cameraShapeColourModelType, cameraShapeColourTone);

    // color_probability Mapping
    auto colourToneConfidenceObjectLevel{cameraDetectionsInterface.ValidCameraDetectionList.at(index)
                                             .CameraShapesShapeColourTone.ColourToneConfidenceObjectLevel};

    osiCameraDetection.set_color_probability(colourToneConfidenceObjectLevel);

    // ambiguity_id Mapping
    auto shapeAmbiguityID{
        cameraDetectionsInterface.ValidCameraDetectionList.at(index).CameraShapeInformation.ShapeAmbiguityID};

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

osi3::CameraPoint CameraFeatureDataMapper::mapCameraPoint(
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
