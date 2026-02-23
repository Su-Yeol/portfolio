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

#include "adiMappers/RadarDetectionsMapper.h"
#include "osi3/osi_sensordata.pb.h"

RadarDetectionsInterface RadarDetectionsMapper::mapRadarDetectionsInterface(osi3::SensorData osiSensorData)
{

    // Expecting osi sensor data is from one sensor only
    RadarDetectionsInterface adiRadarDetectionsInterface{};
    InterfaceHeader adiRDInterfaceHeader{};
    InformationInterface adiRDInformationInterface{};
    ValidRadarDetectionVector adiValidRadarDetList{};
    ara::adi::sensoritf::RadarDetection adiRadarDetection{};

    if (osiSensorData.has_feature_data()) {
        auto osiFeatureData = osiSensorData.feature_data();
        // Expecting there should be only one Radar sensor
        for (auto index = 0; index < osiFeatureData.radar_sensor_size(); index++) {

            osi3::RadarDetectionData osiRadarSensor{osiFeatureData.radar_sensor(index)};

            if (osiRadarSensor.has_header()) {

                adiRDInterfaceHeader = mapRDInterfaceHeader(osiFeatureData, osiRadarSensor);
                adiRadarDetectionsInterface.RadarDetectionInterfaceHeader = adiRDInterfaceHeader;
            }

            auto numOsiDetection = osiRadarSensor.detection_size();
            adiRadarDetectionsInterface.NumberOfValidDetections = numOsiDetection;
            for (auto osiDetectionIndex = 0; osiDetectionIndex < numOsiDetection; osiDetectionIndex++) {

                adiRadarDetection = mapRadarDetection(osiRadarSensor, osiDetectionIndex);

                // ShapeClassificationType adiShapeClassType{};
                adiValidRadarDetList.push_back(adiRadarDetection);
            }

            adiRadarDetectionsInterface.ValidRadarDetectionsList = adiValidRadarDetList;
        }
    }

    return adiRadarDetectionsInterface;
}

ara::adi::sensoritf::RadarDetection RadarDetectionsMapper::mapRadarDetection(osi3::RadarDetectionData osiRadarSensor,
    int osiDetectionIndex)
{
    ara::adi::sensoritf::RadarDetection adiRadarDetection{};
    DetectionStatus adiDetectionStatus{};

    osi3::RadarDetection osiRadarDetection = osiRadarSensor.detection(osiDetectionIndex);

    adiRadarDetection.RadarDetectionsInformation = mapRadarDetectionInfo(osiRadarDetection);

    adiRadarDetection.RadarDetectionsPosition = mapRadarDetectionPosition(osiRadarSensor, osiDetectionIndex);
    adiRadarDetection.RadarDetectionsStatus = mapRadarDetectionStatus(osiRadarSensor, osiDetectionIndex);

    // Absolute velocity available in OSI
    // adiRadarDetection.RadarDetectionsDynamics.RelativeVelocityRadialDistance;

    return adiRadarDetection;
}

DetectionStatus RadarDetectionsMapper::mapRadarDetectionStatus(osi3::RadarDetectionData osiRadarSensor,
    int osiDetectionIndex)
{
    auto osiRadarSensorDetection{osiRadarSensor.detection(osiDetectionIndex)};
    DetectionStatus adiDetectionStatus{};

    if (osiRadarSensorDetection.has_existence_probability()) {
        adiDetectionStatus.ExistenceProbabilityDetectionLevel = osiRadarSensorDetection.existence_probability();
    }

    if (osiRadarSensorDetection.has_object_id()) {
        if (osiRadarSensorDetection.object_id().has_value()) {
            adiDetectionStatus.ObjectID = osiRadarSensorDetection.object_id().value();
        }
    }

    return adiDetectionStatus;
}

RadarDetectionsInformation RadarDetectionsMapper::mapRadarDetectionInfo(osi3::RadarDetection osiRadarSensorDetection)
{
    RadarDetectionsInformation adiRadarDetectionInfo{};
    ValidDetectionClassificationVector adiValidDetectionClassificationVector{};
    DetectionClassification adiDetectionClassification{};

    // NA
    // adiRadarDetectionInfo.DetectionAmbiguityProbability;
    // adiRadarDetectionInfo.FreeSpaceProbability;

    // Point target probability available OSI
    // adiRadarDetectionInfo.MultiTargetProbability;

    // adiRadarDetectionInfo.RadarCrossSectionError;

    // adiRadarDetectionInfo.SignalToNoiseRatioDetectionLevelError;

    if (osiRadarSensorDetection.has_ambiguity_id()) {
        adiRadarDetectionInfo.AmbiguityID = static_cast<float>(osiRadarSensorDetection.ambiguity_id().value());
    }

    if (osiRadarSensorDetection.has_rcs()) {
        adiRadarDetectionInfo.RadarCrossSection = static_cast<float>(osiRadarSensorDetection.rcs());
    }

    if (osiRadarSensorDetection.has_snr()) {
        adiRadarDetectionInfo.SignalToNoiseRatioDetectionLevel = static_cast<float>(osiRadarSensorDetection.snr());
    }

    if (osiRadarSensorDetection.has_classification()) {
        adiDetectionClassification.DetectionClassificationType
            = mapRadarClassification(osiRadarSensorDetection.classification());
        adiValidDetectionClassificationVector.push_back(adiDetectionClassification);
    }

    return adiRadarDetectionInfo;
}

DetectionClassificationType RadarDetectionsMapper::mapRadarClassification(
    osi3::DetectionClassification osiRadarCLassification)
{
    DetectionClassificationType adiDetectionClassificationType{};

    switch (osiRadarCLassification) {
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

DetectionsPosition RadarDetectionsMapper::mapRadarDetectionPosition(osi3::RadarDetectionData osiRadarSensor,
    int osiDetectionIndex)
{
    DetectionsPosition adiRadarDetectionPosition{};

    osi3::RadarDetection osiRadarSensorDetection = osiRadarSensor.detection(osiDetectionIndex);

    if (osiRadarSensorDetection.has_position()) {
        adiRadarDetectionPosition.DetectionPosition = mapPositiontoADI(osiRadarSensorDetection.position());
    }

    if (osiRadarSensorDetection.has_position_rmse()) {
        adiRadarDetectionPosition.DetectionPositionError = mapPositionErrtoADI(osiRadarSensorDetection.position_rmse());
    }

    return adiRadarDetectionPosition;
}

InterfaceHeader RadarDetectionsMapper::mapRDInterfaceHeader(osi3::FeatureData osiFeatureData,
    osi3::RadarDetectionData osiRadarSensor)
{

    InterfaceHeader adiInterfaceHeader{};
    SensorCluster adiSensorCluster{};

    auto osiCamSensorDetHeader{osiRadarSensor.header()};

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
            adiSensorPoseInfo.SensorOrientation
                = mapOrientationtoADI(osiCamSensorDetHeader.mounting_position_rmse().orientation());
        }
    }

    adiInterfaceHeader.InformationInterface = mapSensorDetectionHeader(osiFeatureData, osiRadarSensor);
    adiInterfaceHeader.SensorPose = adiSensorPoseInfo;

    return adiInterfaceHeader;
}

InformationInterface RadarDetectionsMapper::mapSensorDetectionHeader(osi3::FeatureData osiFeatureData,
    osi3::RadarDetectionData osiRadarSensor)
{

    InformationInterface adiInformationInterface{};
    auto osiCamSensorDetHeader{osiRadarSensor.header()};

    if (osiCamSensorDetHeader.has_cycle_counter()) {
        adiInformationInterface.CycleCounter = static_cast<uint8_t>(osiCamSensorDetHeader.cycle_counter());
    }

    if (osiCamSensorDetHeader.has_data_qualifier()) {
        osi3::SensorDetectionHeader_DataQualifier osiDataQualifier = osiCamSensorDetHeader.data_qualifier();
        DataQualifier adiDataQualifier{};

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
