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

#include "adiMappers/UltrasonicFeaturesMapper.h"
#include "osi3/osi_sensordata.pb.h"

UltrasonicFeatureInterface UltrasonicFeaturesMapper::mapUltrasonicFeatureInterface(
    const osi3::SensorData& osiSensorData)
{

    osi3::LogicalDetectionData osiLogicalDetectionData{};
    osi3::LogicalDetectionDataHeader osiLogicalDetectionDataHeader{};
    UltrasonicFeatureInterface adiUltrasonicFeatureInterface{};

    InterfaceHeader adiInterfaceHeader{};

    ValidUltrasonicFeatureVector adiValidUltrasonicFeaturesList{};

    // NA in ADI
    // osiSensorData.logical_detection_data().header().logical_detection_time();

    // osiSensorData.logical_detection_data().logical_detection(0).classification();
    // osiSensorData.logical_detection_data().logical_detection(0).point_target_probability();
    // osiSensorData.logical_detection_data().logical_detection(0).sensor_id();
    // osiSensorData.logical_detection_data().logical_detection(0).snr();

    if (osiSensorData.has_logical_detection_data()) {

        osiLogicalDetectionData = osiSensorData.logical_detection_data();
        adiInterfaceHeader.InformationInterface = mapInformationInterface(osiLogicalDetectionData);

        // adiUltrasonicFeatureInterface.NumberOfValidFeatures
        //    = osiLogicalDetectionData.header().number_of_valid_logical_detections();

        auto osiNumLD = osiLogicalDetectionData.logical_detection_size();
        adiUltrasonicFeatureInterface.NumberOfValidFeatures = osiNumLD;

        FeatureStatus adiFeatureStatus{};
        // UltrasonicSegmentInformation adiUltrasonicSegmentInformation{};
        UltrasonicSegmentPoints adiUltrasonicSegmentPoints{};
        // ShapePoints adiShapePoints{};
        // ValidShapePointVector adiValidShapePointVector{};
        UltrasonicFeature adiUltrasonicFeature{};

        for (auto index = 0; index < osiNumLD; index++) {

            // Classification NA
            // if (osiLogicalDetectionData.logical_detection(index).has_classification()) {
            //
            // }

            adiFeatureStatus = mapFeatureStatus(osiLogicalDetectionData, index);

            adiUltrasonicSegmentPoints = mapUltrasonicSegmentPoints(osiLogicalDetectionData, index);

            adiUltrasonicFeature.UltrasonicFeaturesStatus = adiFeatureStatus;
            adiUltrasonicFeature.UltrasonicFeaturesSegmentPoints = adiUltrasonicSegmentPoints;
            adiValidUltrasonicFeaturesList.push_back(adiUltrasonicFeature);
        }

        adiUltrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader = adiInterfaceHeader;
    }

    return adiUltrasonicFeatureInterface;
}

InformationInterface UltrasonicFeaturesMapper::mapInformationInterface(
    const osi3::LogicalDetectionData& osiLogicalDetectionData)
{

    InformationInterface adiInformationInterface{};

    if (osiLogicalDetectionData.has_header()) {
        if (osiLogicalDetectionData.header().has_data_qualifier()) {
            adiInformationInterface.DataQualifier = mapDataQualifier(osiLogicalDetectionData);
        }

        // if (osiLogicalDetectionData.header().has_logical_detection_time()) {
        //    adiInformationInterface.TimeStamp
        //    osi3::Timestamp osiTimeStamp = osiLogicalDetectionData.header().logical_detection_time();
        //}

        auto osiNumSensorIds = osiLogicalDetectionData.header().sensor_id_size();

        adiInformationInterface.NumberOfValidServingSensors = static_cast<uint8_t>(osiNumSensorIds);

        SensorID adiSensorID{};
        for (auto index = 0; index < osiNumSensorIds; index++) {
            adiSensorID = static_cast<uint8_t>(osiLogicalDetectionData.header().sensor_id(index).value());
            adiInformationInterface.ValidServingSensors.push_back(adiSensorID);
        }
    }

    if (osiLogicalDetectionData.has_version()) {
        adiInformationInterface.VersionID = mapInterfaceVersion(osiLogicalDetectionData.version());
    }

    return adiInformationInterface;
}

DataQualifier UltrasonicFeaturesMapper::mapDataQualifier(const osi3::LogicalDetectionData& osiLogicalDetectionData)
{
    DataQualifier adiDataQualifier{};
    osi3::LogicalDetectionDataHeader_DataQualifier osiLDDataQualifier
        = osiLogicalDetectionData.header().data_qualifier();

    switch (osiLDDataQualifier) {
    case osi3::LogicalDetectionDataHeader_DataQualifier::
        LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN: {
        adiDataQualifier = DataQualifier::kUnknown;

        break;
    }
    case osi3::LogicalDetectionDataHeader_DataQualifier::
        LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_BLINDNESS: {
        adiDataQualifier = DataQualifier::kReduceInCoverage;

        break;
    }
    case osi3::LogicalDetectionDataHeader_DataQualifier::
        LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE: {
        adiDataQualifier = DataQualifier::kNotAvailable;

        break;
    }
    case osi3::LogicalDetectionDataHeader_DataQualifier::
        LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE: {
        adiDataQualifier = DataQualifier::kNormal;

        break;
    }
    case osi3::LogicalDetectionDataHeader_DataQualifier::
        LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED: {
        adiDataQualifier = DataQualifier::kReduceInCoverage;

        break;
    }
    case osi3::LogicalDetectionDataHeader_DataQualifier::
        LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_OTHER: {
        adiDataQualifier = DataQualifier::kUnknown;

        break;
    }
    default: {
        m_uf_logger.LogError() << "Error: mapInterfaceHeader: DataQualifier"
                                  "--> Unknown enum entry: ";
        adiDataQualifier = DataQualifier::kUnknown;
    }
    }

    return adiDataQualifier;
}

FeatureStatus UltrasonicFeaturesMapper::mapFeatureStatus(const osi3::LogicalDetectionData& osiLogicalDetectionData,
    const int& index)
{
    FeatureStatus adiFeatureStatus{};

    if (osiLogicalDetectionData.logical_detection(index).has_existence_probability()) {
        adiFeatureStatus.ExistenceProbabilityFeatureLevel
            = static_cast<float>(osiLogicalDetectionData.logical_detection(index).existence_probability());
    }

    if (osiLogicalDetectionData.logical_detection(index).has_object_id()) {
        adiFeatureStatus.FeatureID
            = static_cast<uint16_t>(osiLogicalDetectionData.logical_detection(index).object_id().value());
    }

    return adiFeatureStatus;
}

UltrasonicSegmentPoints UltrasonicFeaturesMapper::mapUltrasonicSegmentPoints(
    const osi3::LogicalDetectionData& osiLogicalDetectionData,
    const int& index)
{
    UltrasonicSegmentPoints adiUltrasonicSegmentPoints{};
    ValidSegmentPointVector adiValidSegmentPointVector{};
    SegmentPoint adiSegmentPoint{};

    // Mapping to first ADI point
    if (osiLogicalDetectionData.logical_detection(index).has_position()) {
        adiSegmentPoint.Position = mapPositiontoADI(osiLogicalDetectionData.logical_detection(index).position());
    }

    if (osiLogicalDetectionData.logical_detection(index).has_position_rmse()) {
        adiSegmentPoint.PositionError
            = mapPositionErrtoADI(osiLogicalDetectionData.logical_detection(index).position_rmse());
    }

    if (osiLogicalDetectionData.logical_detection(index).has_velocity()) {
        adiSegmentPoint.VelocityUltrasonic
            = mapPosition2DtoADI(osiLogicalDetectionData.logical_detection(index).velocity());
    }

    if (osiLogicalDetectionData.logical_detection(index).has_velocity_rmse()) {
        adiSegmentPoint.VelocityUltrasonicError
            = mapPosition2DErrtoADI(osiLogicalDetectionData.logical_detection(index).velocity_rmse());
    }

    adiValidSegmentPointVector.push_back(adiSegmentPoint);
    adiUltrasonicSegmentPoints.ValidSegmentPointsList = adiValidSegmentPointVector;

    return adiUltrasonicSegmentPoints;
}
