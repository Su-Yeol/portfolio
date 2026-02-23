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

#include "adiMappers/LidarDetectionsMapper.h"
#include "osi3/osi_sensordata.pb.h"

LidarDetectionsInterface LidarDetectionsMapper::mapLidarDetectionsInterface(osi3::SensorData osiSensorData)
{

    // Expecting osi sensor data is from one sensor only
    LidarDetectionsInterface adiLidarDetectionsInterface{};
    InterfaceHeader adiLDInterfaceHeader{};
    InformationInterface adiLDInformationInterface{};
    ValidLidarDetectionVector adiValidLidarDetList{};
    ara::adi::sensoritf::LidarDetection adiLidarDetection{};

    if (osiSensorData.has_feature_data()) {
        auto osiFeatureData = osiSensorData.feature_data();
        // Expecting there should be only one lidar sensor
        for (auto index = 0; index < osiFeatureData.lidar_sensor_size(); index++) {

            osi3::LidarDetectionData osiLidarSensor{osiFeatureData.lidar_sensor(index)};

            if (osiLidarSensor.has_header()) {

                adiLDInterfaceHeader = mapLDInterfaceHeader(osiFeatureData, osiLidarSensor);
                adiLidarDetectionsInterface.LidarDetectionInterfaceHeader = adiLDInterfaceHeader;
            }

            auto numOsiDetection = osiLidarSensor.detection_size();
            adiLidarDetectionsInterface.NumberOfValidLidarDetections = numOsiDetection;
            for (auto osiDetectionIndex = 0; osiDetectionIndex < numOsiDetection; osiDetectionIndex++) {

                adiLidarDetection = mapLidarDetection(osiLidarSensor, osiDetectionIndex);

                // ShapeClassificationType adiShapeClassType{};
                adiValidLidarDetList.push_back(adiLidarDetection);
            }

            adiLidarDetectionsInterface.ValidLidarDetectionsList = adiValidLidarDetList;
        }
    }

    return adiLidarDetectionsInterface;
}

ara::adi::sensoritf::LidarDetection LidarDetectionsMapper::mapLidarDetection(osi3::LidarDetectionData osiLidarSensor,
    int osiDetectionIndex)
{
    ara::adi::sensoritf::LidarDetection adiLidarDetection{};

    osi3::LidarDetection osiLidarDetection = osiLidarSensor.detection(osiDetectionIndex);

    adiLidarDetection.LidarDetectionsInformation = mapLidarDetectionInfo(osiLidarDetection);

    adiLidarDetection.LidarDetectionsPosition = mapLidarDetectionPosition(osiLidarSensor, osiDetectionIndex);
    adiLidarDetection.LidarDetectionStatus = mapLidarDetectionStatus(osiLidarSensor, osiDetectionIndex);

    // NA
    // adiLidarDetection.LidarDetectionsDynamics.RelativeVelocityRadialDistance;
    return adiLidarDetection;
}

DetectionStatus LidarDetectionsMapper::mapLidarDetectionStatus(osi3::LidarDetectionData osiLidarSensor,
    int osiDetectionIndex)
{
    auto osiLidarSensorDetection{osiLidarSensor.detection(osiDetectionIndex)};
    DetectionStatus adiDetectionStatus{};

    if (osiLidarSensorDetection.has_existence_probability()) {
        adiDetectionStatus.ExistenceProbabilityDetectionLevel = osiLidarSensorDetection.existence_probability();
    }

    if (osiLidarSensorDetection.has_object_id()) {
        if (osiLidarSensorDetection.object_id().has_value()) {
            adiDetectionStatus.ObjectID = osiLidarSensorDetection.object_id().has_value();
        }
    }

    return adiDetectionStatus;
}

LidarDetectionsInformation LidarDetectionsMapper::mapLidarDetectionInfo(osi3::LidarDetection osiLidarSensorDetection)
{
    LidarDetectionsInformation adiLidarDetectionInfo{};
    ValidDetectionClassificationVector adiValidDetectionClassificationVector{};
    DetectionClassification adiDetectionClassification{};

    if (osiLidarSensorDetection.has_free_space_probability()) {
        adiLidarDetectionInfo.FreeSpaceProbability
            = static_cast<float>(osiLidarSensorDetection.free_space_probability());
    }

    if (osiLidarSensorDetection.has_reflectivity()) {
        adiLidarDetectionInfo.Reflectivity = static_cast<float>(osiLidarSensorDetection.reflectivity());
    }

    if (osiLidarSensorDetection.has_classification()) {
        adiDetectionClassification.DetectionClassificationType
            = mapLidarClassification(osiLidarSensorDetection.classification());
        adiValidDetectionClassificationVector.push_back(adiDetectionClassification);
    }

    return adiLidarDetectionInfo;
}

DetectionClassificationType LidarDetectionsMapper::mapLidarClassification(
    osi3::DetectionClassification osiLidarCLassification)
{
    DetectionClassificationType adiDetectionClassificationType{};

    switch (osiLidarCLassification) {
    case osi3::DetectionClassification::DETECTION_CLASSIFICATION_CLUTTER: {
        adiDetectionClassificationType = DetectionClassificationType::kNoise;
        break;
    }
    case osi3::DetectionClassification::DETECTION_CLASSIFICATION_OVERDRIVABLE: {
        adiDetectionClassificationType = DetectionClassificationType::kOverdrivable;
        break;
    }
    case osi3::DetectionClassification::DETECTION_CLASSIFICATION_UNDERDRIVABLE: {
        adiDetectionClassificationType = DetectionClassificationType::kUnderdriveable;
        break;
    }
    default: {
        adiDetectionClassificationType = DetectionClassificationType::kUnknown;
        break;
    }
    }

    return adiDetectionClassificationType;
}

LidarDetectionsPosition LidarDetectionsMapper::mapLidarDetectionPosition(osi3::LidarDetectionData osiLidarSensor,
    int osiDetectionIndex)
{
    LidarDetectionsPosition adiLidarDetectionPosition{};

    osi3::LidarDetection osiLidarSensorDetection = osiLidarSensor.detection(osiDetectionIndex);

    if (osiLidarSensorDetection.has_height()) {
        adiLidarDetectionPosition.HeightLidar = static_cast<float>(osiLidarSensorDetection.height());
    }

    if (osiLidarSensorDetection.has_height_rmse()) {
        adiLidarDetectionPosition.HeightLidarError = static_cast<float>(osiLidarSensorDetection.height_rmse());
    }

    if (osiLidarSensorDetection.has_position()) {
        adiLidarDetectionPosition.DetectionPosition = mapPositiontoADI(osiLidarSensorDetection.position());
    }

    if (osiLidarSensorDetection.has_position_rmse()) {
        adiLidarDetectionPosition.DetectionPositionError = mapPositionErrtoADI(osiLidarSensorDetection.position_rmse());
    }

    return adiLidarDetectionPosition;
}

InterfaceHeader LidarDetectionsMapper::mapLDInterfaceHeader(osi3::FeatureData osiFeatureData,
    osi3::LidarDetectionData osiLidarSensor)
{
    InterfaceHeader adiInterfaceHeader{};
    SensorCluster adiSensorCluster{};

    auto osiCamSensorDetHeader{osiLidarSensor.header()};

    adiSensorCluster.NumberOfValidSensors = static_cast<uint8_t>(osiFeatureData.lidar_sensor_size());

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

    adiInterfaceHeader.InformationInterface = mapSensorDetectionHeader(osiFeatureData, osiLidarSensor);
    adiInterfaceHeader.SensorPose = adiSensorPoseInfo;

    return adiInterfaceHeader;
}

InformationInterface LidarDetectionsMapper::mapSensorDetectionHeader(osi3::FeatureData osiFeatureData,
    osi3::LidarDetectionData osiLidarSensor)
{
    InformationInterface adiInformationInterface{};
    auto osiCamSensorDetHeader{osiLidarSensor.header()};

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
