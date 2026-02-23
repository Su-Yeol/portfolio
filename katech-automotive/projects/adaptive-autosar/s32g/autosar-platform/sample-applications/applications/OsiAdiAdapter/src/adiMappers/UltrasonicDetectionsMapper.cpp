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

#include "adiMappers/UltrasonicDetectionsMapper.h"
#include "osi3/osi_sensordata.pb.h"

UltrasonicDetectionsInterface UltrasonicDetectionsMapper::mapUltrasonicDetectionsInterface(
    osi3::SensorData osiSensorData)
{

    // TODO: Assuming osi sensor data is from one sensor only
    UltrasonicDetectionsInterface adiUltrasonicDetectionsInterface{};
    InterfaceHeader adiUDInterfaceHeader{};
    InformationInterface adiCDInformationInterface{};
    ValidUltrasonicDetectionVector adiValidUltrasonicDetectionList{};

    UltrasonicDetection adiUltrasonicDetection{};

    // adiUltrasonicDetectionsInterface.NoValidDetections;
    // adiUltrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader;

    if (osiSensorData.has_feature_data()) {
        auto osiFeatureData = osiSensorData.feature_data();
        // TODO: Assuming there should be only one camera sensor
        for (auto index = 0; index < osiFeatureData.ultrasonic_sensor_size(); index++) {

            osi3::UltrasonicDetectionData osiUltrasonicSensor{osiFeatureData.ultrasonic_sensor(index)};

            if (osiUltrasonicSensor.has_header()) {

                auto osiUltrasonicSensorDetHeader{osiUltrasonicSensor.header()};

                // osiUltrasonicSensorDetHeader.has_number_of_valid_detections();

                adiUDInterfaceHeader = mapUDInterfaceHeader(osiFeatureData, osiUltrasonicSensor);
                adiUltrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader = adiUDInterfaceHeader;
            }

            auto numOsiDetection = osiUltrasonicSensor.detection_size();
            adiUltrasonicDetectionsInterface.NoValidDetections = numOsiDetection;
            for (auto osiDetectionIndex = 0; osiDetectionIndex < numOsiDetection; osiDetectionIndex++) {

                // NA
                // osiCamSensorDetection.has_color();
                // osiCamSensorDetection.has_color_description();
                // osiCamSensorDetection.has_color_probability();
                // osiCamSensorDetection.has_first_point_index();

                adiUltrasonicDetection = mapUltrasonicDetection(osiUltrasonicSensor, osiDetectionIndex);

                // ShapeClassificationType adiShapeClassType{};
                adiValidUltrasonicDetectionList.push_back(adiUltrasonicDetection);
            }

            adiUltrasonicDetectionsInterface.ValidUltrasonicDetectionList = adiValidUltrasonicDetectionList;
        }
    }

    return adiUltrasonicDetectionsInterface;
}

UltrasonicDetection UltrasonicDetectionsMapper::mapUltrasonicDetection(
    osi3::UltrasonicDetectionData osiUltrasonicSensor,
    int osiDetectionIndex)
{
    UltrasonicDetection adiUltrasonicDetection{};
    // DetectionStatus adiDetectionStatus{};

    // osi3::UltrasonicDetection osiUltrasonicDetection = osiUltrasonicSensor.detection(osiDetectionIndex);

    // auto adiUltrasonicDetectionsInfo{};

    // adiUltrasonicDetectionsInfo = mapUltrasonicDetectionInfo(osiUltrasonicDetection);
    // adiUltrasonicDetection.UltrasonicDetectionsInformation = adiUltrasonicDetectionsInfo;

    adiUltrasonicDetection.UltrasonicDetectionsPosition
        = mapUltrasonicDetectionsPosition(osiUltrasonicSensor, osiDetectionIndex);

    adiUltrasonicDetection.UltrasonicDetectionStatus
        = mapUltrasonicDetectionStatus(osiUltrasonicSensor, osiDetectionIndex);

    return adiUltrasonicDetection;
}

DetectionStatus UltrasonicDetectionsMapper::mapUltrasonicDetectionStatus(
    osi3::UltrasonicDetectionData osiUltrasonicSensor,
    int osiDetectionIndex)
{
    auto osiUltrasonicSensorDetection{osiUltrasonicSensor.detection(osiDetectionIndex)};
    DetectionStatus adiDetectionStatus{};

    if (osiUltrasonicSensorDetection.has_existence_probability()) {
        adiDetectionStatus.ExistenceProbabilityDetectionLevel = osiUltrasonicSensorDetection.existence_probability();
    }

    if (osiUltrasonicSensorDetection.has_object_id()) {
        if (osiUltrasonicSensorDetection.object_id().has_value()) {
            adiDetectionStatus.ObjectID = osiUltrasonicSensorDetection.object_id().value();
        }
    }

    return adiDetectionStatus;
}

// UltrasonicDetectionsInformation UltrasonicDetectionsMapper::mapUltrasonicDetectionInfo(
//     osi3::UltrasonicDetection osiCamSensorDetection)
// {
//     UltrasonicDetectionsInformation adiUltrasonicDetectionsInfo{};
//
//     // adiUltrasonicDetectionsInfo.Reflectivity;
//     // adiUltrasonicDetectionsInfo.SecondSensorIDReference;
//
//     return adiUltrasonicDetectionsInfo;
// }

UltrasonicDetectionsPosition UltrasonicDetectionsMapper::mapUltrasonicDetectionsPosition(
    osi3::UltrasonicDetectionData osiUltrasonicSensor,
    int osiDetectionIndex)
{
    UltrasonicDetectionsPosition adiUltrasonicDetectionsPosition{};

    // NA
    // adiUltrasonicDetectionsPosition.DistanceError;
    // adiUltrasonicDetectionsPosition.HeightUltrasonic;
    // adiUltrasonicDetectionsPosition.HeightUltrasonicError;

    osi3::UltrasonicDetection osiUltrasonicDetection = osiUltrasonicSensor.detection(osiDetectionIndex);

    if (osiUltrasonicDetection.has_distance()) {
        adiUltrasonicDetectionsPosition.Distance = static_cast<float>(osiUltrasonicDetection.distance());
    }

    return adiUltrasonicDetectionsPosition;
}

InterfaceHeader UltrasonicDetectionsMapper::mapUDInterfaceHeader(osi3::FeatureData osiFeatureData,
    osi3::UltrasonicDetectionData osiUltrasonicSensor)
{

    InterfaceHeader adiInterfaceHeader{};
    SensorCluster adiSensorCluster{};

    auto osiUltrasonicSensorDetHeader{osiUltrasonicSensor.header()};

    adiSensorCluster.NumberOfValidSensors = static_cast<uint8_t>(osiFeatureData.ultrasonic_sensor_size());

    if (osiUltrasonicSensorDetHeader.has_sensor_id()) {
        if (osiUltrasonicSensorDetHeader.sensor_id().has_value())
            adiSensorCluster.SensorIDReferenceList.push_back(
                static_cast<uint8_t>(osiUltrasonicSensorDetHeader.sensor_id().value()));
    }

    adiInterfaceHeader.SensorClusterInformation = adiSensorCluster;

    InformationSensorPose adiSensorPoseInfo{};
    if (osiUltrasonicSensorDetHeader.has_mounting_position()) {
        if (osiUltrasonicSensorDetHeader.mounting_position().has_position()) {
            adiSensorPoseInfo.SensorOriginPoint
                = mapPositiontoADI(osiUltrasonicSensorDetHeader.mounting_position().position());
        }

        if (osiUltrasonicSensorDetHeader.mounting_position().has_orientation()) {
            adiSensorPoseInfo.SensorOrientation
                = mapOrientationtoADI(osiUltrasonicSensorDetHeader.mounting_position().orientation());
        }
    }

    if (osiUltrasonicSensorDetHeader.has_mounting_position_rmse()) {
        if (osiUltrasonicSensorDetHeader.mounting_position_rmse().has_position()) {
            adiSensorPoseInfo.SensorOriginPointError
                = mapPositionErrtoADI(osiUltrasonicSensorDetHeader.mounting_position_rmse().position());
        }

        if (osiUltrasonicSensorDetHeader.mounting_position_rmse().has_orientation()) {
            adiSensorPoseInfo.SensorOrientation
                = mapOrientationtoADI(osiUltrasonicSensorDetHeader.mounting_position_rmse().orientation());
        }
    }

    adiInterfaceHeader.SensorPose = adiSensorPoseInfo;

    adiInterfaceHeader.InformationInterface = mapSensorDetectionHeader(osiFeatureData, osiUltrasonicSensor);

    return adiInterfaceHeader;
}

InformationInterface UltrasonicDetectionsMapper::mapSensorDetectionHeader(osi3::FeatureData osiFeatureData,
    osi3::UltrasonicDetectionData osiUltrasonicSensor)
{

    InformationInterface adiInformationInterface{};

    auto osiUltrasonicSensorDetHeader{osiUltrasonicSensor.header()};

    if (osiUltrasonicSensorDetHeader.has_cycle_counter()) {
        adiInformationInterface.CycleCounter = static_cast<uint8_t>(osiUltrasonicSensorDetHeader.cycle_counter());
    }

    if (osiUltrasonicSensorDetHeader.has_data_qualifier()) {
        osi3::SensorDetectionHeader_DataQualifier osiDataQualifier = osiUltrasonicSensorDetHeader.data_qualifier();
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
