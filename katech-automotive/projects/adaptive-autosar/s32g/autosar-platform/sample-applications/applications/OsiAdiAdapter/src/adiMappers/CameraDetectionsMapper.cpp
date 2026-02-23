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

#include "adiMappers/CameraDetectionsMapper.h"
#include "osi3/osi_sensordata.pb.h"

CameraDetectionsInterface CameraDetectionsMapper::mapCameraDetectionsInterface(osi3::SensorData osiSensorData)
{
    // TODO: Assuming osi sensor data is from one sensor only
    CameraDetectionsInterface adiCameraDetectionsInterface{};
    InterfaceHeader adiCDInterfaceHeader{};
    InformationInterface adiCDInformationInterface{};
    ValidCameraShapeVector adiValidCamShapeList{};
    CameraShape adiCameraShape{};

    if (osiSensorData.has_feature_data()) {
        auto osiFeatureData = osiSensorData.feature_data();
        // TODO: Assuming there should be only one camera sensor
        for (auto index = 0; index < osiFeatureData.camera_sensor_size(); index++) {

            osi3::CameraDetectionData osiCameraSensor{osiFeatureData.camera_sensor(index)};

            if (osiCameraSensor.has_header()) {

                auto osiCamSensorDetHeader{osiCameraSensor.header()};

                // osiCamSensorDetHeader.has_number_of_valid_detections();

                adiCDInterfaceHeader = mapCDInterfaceHeader(osiFeatureData, osiCameraSensor);
                adiCameraDetectionsInterface.CameraDetectionInterfaceHeader = adiCDInterfaceHeader;
            }

            auto numOsiDetection = osiCameraSensor.detection_size();
            adiCameraDetectionsInterface.NumberOfValidShapes = numOsiDetection;
            for (auto osiDetectionIndex = 0; osiDetectionIndex < numOsiDetection; osiDetectionIndex++) {

                // NA
                // osiCamSensorDetection.has_color();
                // osiCamSensorDetection.has_color_description();
                // osiCamSensorDetection.has_color_probability();
                // osiCamSensorDetection.has_first_point_index();

                adiCameraShape = mapCameraShape(osiCameraSensor, osiDetectionIndex);

                // ShapeClassificationType adiShapeClassType{};
                adiValidCamShapeList.push_back(adiCameraShape);
            }

            adiCameraDetectionsInterface.ValidCameraDetectionList = adiValidCamShapeList;
        }
    }
    return adiCameraDetectionsInterface;
}

CameraShape CameraDetectionsMapper::mapCameraShape(osi3::CameraDetectionData osiCameraSensor, int osiDetectionIndex)
{
    CameraShape adiCameraShape{};
    // DetectionStatus adiDetectionStatus{};

    osi3::CameraDetection osiCameraDetection = osiCameraSensor.detection(osiDetectionIndex);

    adiCameraShape.CameraShapeInformation = mapCameraShapeInfo(osiCameraDetection);
    adiCameraShape.CameraShapePoints = mapCameraShapePoints(osiCameraSensor, osiDetectionIndex);
    adiCameraShape.CameraShapesStatus = mapCameraDetectionStatus(osiCameraSensor, osiDetectionIndex);

    return adiCameraShape;
}

DetectionStatus CameraDetectionsMapper::mapCameraDetectionStatus(osi3::CameraDetectionData osiCameraSensor,
    int osiDetectionIndex)
{
    auto osiCamSensorDetection{osiCameraSensor.detection(osiDetectionIndex)};
    DetectionStatus adiDetectionStatus{};

    if (osiCamSensorDetection.has_existence_probability()) {
        adiDetectionStatus.ExistenceProbabilityDetectionLevel = osiCamSensorDetection.existence_probability();
    }

    if (osiCamSensorDetection.has_object_id()) {
        if (osiCamSensorDetection.object_id().has_value()) {
            adiDetectionStatus.ObjectID = osiCamSensorDetection.object_id().has_value();
        }
    }

    return adiDetectionStatus;
}

CameraShapesShapeInformation CameraDetectionsMapper::mapCameraShapeInfo(osi3::CameraDetection osiCamSensorDetection)
{
    CameraShapesShapeInformation adiCameraShapeInfo{};

    if (osiCamSensorDetection.has_ambiguity_id()) {
        if (osiCamSensorDetection.ambiguity_id().has_value()) {
            adiCameraShapeInfo.ShapeAmbiguityID = osiCamSensorDetection.ambiguity_id().value();
        }
    }

    DetectionShapeClassification adiShapeClass{};

    if (osiCamSensorDetection.has_shape_classification_background()
        && osiCamSensorDetection.shape_classification_background()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kBackground;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_flat()
        && osiCamSensorDetection.shape_classification_flat()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kFlat;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_foreground()
        && osiCamSensorDetection.shape_classification_foreground()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kForeground;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_ground()
        && osiCamSensorDetection.shape_classification_ground()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kGround;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_non_road()
        && osiCamSensorDetection.shape_classification_non_road()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kNonRoad;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_pedestrian()
        && osiCamSensorDetection.shape_classification_pedestrian()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kPedestrian;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_pedestrian_front()
        && osiCamSensorDetection.shape_classification_pedestrian_front()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kPedestrianFront;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_pedestrian_rear()
        && osiCamSensorDetection.shape_classification_pedestrian_rear()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kPedestrianRear;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_pedestrian_side()
        && osiCamSensorDetection.shape_classification_pedestrian_side()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kPedestrianSide;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_road()
        && osiCamSensorDetection.shape_classification_road()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kRoad;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_non_driving_lane()
        && osiCamSensorDetection.shape_classification_non_driving_lane()) {

        // TODO: Warning
        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kSidewalk;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_traffic_sign()
        && osiCamSensorDetection.shape_classification_traffic_sign()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kTrafficSign;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_upright()
        && osiCamSensorDetection.shape_classification_upright()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kUpright;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_vegetation()
        && osiCamSensorDetection.shape_classification_vegetation()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kVegetation;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else if (osiCamSensorDetection.has_shape_classification_vehicle()
        && osiCamSensorDetection.shape_classification_vehicle()) {

        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kVehicle;

        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    else {
        adiShapeClass.ShapeClassificationTypeDetectionLevel = ShapeClassificationType::kUnknown;
        if (osiCamSensorDetection.has_shape_classification_probability()) {
            adiShapeClass.ShapeClassificationTypeConfidenceDetectionLevel
                = osiCamSensorDetection.shape_classification_probability();
        }

        adiCameraShapeInfo.ValidShapeClassificationsList.push_back(adiShapeClass);
    }

    return adiCameraShapeInfo;
}

CameraShapesShapePoints CameraDetectionsMapper::mapCameraShapePoints(osi3::CameraDetectionData osiCameraSensor,
    int osiDetectionIndex)
{
    CameraShapesShapePoints adiCameraShapePoints{};

    osi3::CameraDetection osiCamSensorDetection = osiCameraSensor.detection(osiDetectionIndex);

    if (osiCamSensorDetection.has_number_of_points()) {
        adiCameraShapePoints.NumberOfValidShapePointsDetectionLevel
            = static_cast<uint32_t>(osiCamSensorDetection.number_of_points());
    }

    if (osiCamSensorDetection.has_image_shape_type()) {
        switch (osiCamSensorDetection.image_shape_type()) {
        case osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_BOX: {
            adiCameraShapePoints.ShapeTypeDetectionLevel = ShapeType ::kBox;
            break;
        }
        case osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_ELLIPSE: {
            adiCameraShapePoints.ShapeTypeDetectionLevel = ShapeType ::kEllipse;
            break;
        }
        case osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_OTHER: {
            adiCameraShapePoints.ShapeTypeDetectionLevel = ShapeType ::kUnknown;
            break;
        }
        case osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_POINT: {
            adiCameraShapePoints.ShapeTypeDetectionLevel = ShapeType ::kPoint;
            break;
        }
        case osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_POINT_CLOUD: {
            adiCameraShapePoints.ShapeTypeDetectionLevel = ShapeType ::kPointCloud;
            break;
        }
        case osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_POLYGON: {
            adiCameraShapePoints.ShapeTypeDetectionLevel = ShapeType ::kPolygon;
            break;
        }
        case osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_UNKNOWN: {
            adiCameraShapePoints.ShapeTypeDetectionLevel = ShapeType ::kUnknown;
            break;
        }
        default: {
            adiCameraShapePoints.ShapeTypeDetectionLevel = ShapeType ::kUnknown;
            break;
        }
        }
    }

    adiCameraShapePoints.ValidShapePointsDetectionLevelList = mapCameraShapePointDetection(osiCameraSensor);

    return adiCameraShapePoints;
}

ValidShapePointDetectionLevelVector CameraDetectionsMapper::mapCameraShapePointDetection(
    osi3::CameraDetectionData osiCameraSensor)
{
    auto numPoints = osiCameraSensor.point_size();
    ValidShapePointDetectionLevelVector adiShapePointDetLevelList{};

    for (auto osiPoint = 0; osiPoint < numPoints; osiPoint++) {

        auto osiCamSensorPoint{osiCameraSensor.point(osiPoint)};
        ShapePointDetectionLevel adiShapePointDetectionLevel{};
        if (osiCamSensorPoint.has_existence_probability()) {
            adiShapePointDetectionLevel.PointExistenceProbabilityDetectionLevel
                = static_cast<float>(osiCamSensorPoint.existence_probability());
        }

        if (osiCamSensorPoint.has_point()) {

            adiShapePointDetectionLevel.Position = mapPositiontoADI(osiCamSensorPoint.point());
        }

        if (osiCamSensorPoint.has_point_rmse()) {

            adiShapePointDetectionLevel.PositionError = mapPositionErrtoADI(osiCamSensorPoint.point_rmse());
        }

        adiShapePointDetLevelList.push_back(adiShapePointDetectionLevel);
    }

    return adiShapePointDetLevelList;
}

InterfaceHeader CameraDetectionsMapper::mapCDInterfaceHeader(osi3::FeatureData osiFeatureData,
    osi3::CameraDetectionData osiCameraSensor)
{

    InterfaceHeader adiInterfaceHeader{};
    SensorCluster adiSensorCluster{};

    auto osiCamSensorDetHeader{osiCameraSensor.header()};

    adiSensorCluster.NumberOfValidSensors = static_cast<uint8_t>(osiFeatureData.camera_sensor_size());

    if (osiCamSensorDetHeader.has_sensor_id()) {
        if (osiCamSensorDetHeader.sensor_id().has_value())
            adiSensorCluster.SensorIDReferenceList.push_back(
                static_cast<uint8_t>(osiCamSensorDetHeader.sensor_id().value()));
    }

    adiInterfaceHeader.SensorClusterInformation = adiSensorCluster;

    InformationSensorPose adiSensorPoseInfo{};
    if (osiCamSensorDetHeader.has_mounting_position()) {
        if (osiCamSensorDetHeader.mounting_position().has_position()) {
            adiSensorPoseInfo.SensorOriginPoint
                = mapPositiontoADI(osiCamSensorDetHeader.mounting_position().position());
        }

        if (osiCamSensorDetHeader.mounting_position().has_orientation()) {
            adiSensorPoseInfo.SensorOrientation
                = mapOrientationtoADI(osiCamSensorDetHeader.mounting_position().orientation());
        }
    }

    if (osiCamSensorDetHeader.has_mounting_position_rmse()) {
        if (osiCamSensorDetHeader.mounting_position_rmse().has_position()) {
            adiSensorPoseInfo.SensorOriginPointError
                = mapPositionErrtoADI(osiCamSensorDetHeader.mounting_position_rmse().position());
        }

        if (osiCamSensorDetHeader.mounting_position_rmse().has_orientation()) {
            adiSensorPoseInfo.SensorOrientationError
                = mapOrientationErrortoADI(osiCamSensorDetHeader.mounting_position_rmse().orientation());
        }
    }

    adiInterfaceHeader.InformationInterface = mapSensorDetectionHeader(osiFeatureData, osiCameraSensor);
    adiInterfaceHeader.SensorPose = adiSensorPoseInfo;

    return adiInterfaceHeader;
}

InformationInterface CameraDetectionsMapper::mapSensorDetectionHeader(osi3::FeatureData osiFeatureData,
    osi3::CameraDetectionData osiCameraSensor)
{

    InformationInterface adiInformationInterface{};
    auto osiCamSensorDetHeader{osiCameraSensor.header()};

    if (osiCamSensorDetHeader.has_cycle_counter()) {
        adiInformationInterface.CycleCounter = static_cast<uint8_t>(osiCamSensorDetHeader.cycle_counter());
    }

    if (osiCamSensorDetHeader.has_data_qualifier()) {
        osi3::SensorDetectionHeader_DataQualifier osiDataQualifier = osiCamSensorDetHeader.data_qualifier();
        ara::adi::sensoritf::DataQualifier adiDataQualifier{};

        switch (osiDataQualifier) {
        case osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE: {
            adiDataQualifier = DataQualifier::kNormal;

            break;
        }
        case osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED: {
            adiDataQualifier = DataQualifier::kReduceInViewAndPerformance;
            break;
        }
        case osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_BLINDNESS: {
            adiDataQualifier = DataQualifier::kReduceInCoverage;

            break;
        }
        case osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_INVALID: {
            adiDataQualifier = DataQualifier::kInvalid;
            break;
        }
        case osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE: {
            adiDataQualifier = DataQualifier::kNotAvailable;

            break;
        }
        case osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_OTHER: {
            adiDataQualifier = DataQualifier::kUnknown;
            break;
        }
        case osi3::SensorDetectionHeader_DataQualifier::
            SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_TEMPORARY_AVAILABLE: {
            adiDataQualifier = DataQualifier::kReduceInViewAndPerformance;
            break;
        }
        case osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN: {
            adiDataQualifier = DataQualifier::kUnknown;
            break;
        }
        default: {
            adiDataQualifier = DataQualifier::kUnknown;
            break;
        }
        }

        adiInformationInterface.DataQualifier = adiDataQualifier;
    }

    if (osiFeatureData.has_version()) {
        adiInformationInterface.VersionID = mapInterfaceVersion(osiFeatureData.version());
    }

    return adiInformationInterface;
}
