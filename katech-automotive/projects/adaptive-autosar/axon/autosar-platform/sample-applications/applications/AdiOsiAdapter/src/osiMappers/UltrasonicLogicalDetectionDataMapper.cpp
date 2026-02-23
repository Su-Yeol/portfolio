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

/*!
 * \file
 * \brief Specification of ...
 * \details Contains specification of ...).
 */

#include "ara/adi/sensoritf/impl_type_ultrasonicfeatureinterface.h"

#include "osiMappers/UltrasonicLogicalDetectionDataMapper.h"

osi3::LogicalDetectionData UltrasonicLogicalDetectionDataMapper::mapLogicalDetectionData(
    const ara::adi::sensoritf::UltrasonicFeatureInterface& ultrasonicFeatureInterface) const
{
    osi3::LogicalDetectionData osiLogicalDetectionData{};

    // Interface Version Mapping
    osi3::InterfaceVersion* osiInterfaceversion{osiLogicalDetectionData.mutable_version()};
    *osiInterfaceversion = mapInterfaceVersion(
        ultrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface.VersionID);

    // LogicalDetectionDataHeader Mapping
    // InterfaceHeader as argument cannot be sent as other info also required
    osi3::LogicalDetectionDataHeader* osiLogicalDetectionDataHeader{osiLogicalDetectionData.mutable_header()};
    *osiLogicalDetectionDataHeader = mapLogicalDetectionDataHeader(ultrasonicFeatureInterface);

    // LogicalDetection Mapping
    auto numValidUltrasonicFeaturesList = ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.size();

    if (numValidUltrasonicFeaturesList == 0) {
        return {};
    }

    if (numValidUltrasonicFeaturesList != ultrasonicFeatureInterface.NumberOfValidFeatures) {
        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidFeatures: "
                           << static_cast<size_t>(ultrasonicFeatureInterface.NumberOfValidFeatures)
                           << " and numValidUltrasonicFeaturesList: " << numValidUltrasonicFeaturesList;
    }

    for (size_t index = 0; index < numValidUltrasonicFeaturesList; index++) {
        osi3::LogicalDetection* osiLogicalDetection = osiLogicalDetectionData.add_logical_detection();
        *osiLogicalDetection = mapLogicalDetection(ultrasonicFeatureInterface, index);
    }

    return osiLogicalDetectionData;
}

osi3::LogicalDetectionDataHeader UltrasonicLogicalDetectionDataMapper::mapLogicalDetectionDataHeader(
    const ara::adi::sensoritf::UltrasonicFeatureInterface& ultrasonicFeatureInterface) const
{
    osi3::LogicalDetectionDataHeader osiLogicalDetectionDataHeader{};
    size_t numValidSensors
        = ultrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface.ValidServingSensors.size();

    if (numValidSensors == 0) {
        return {};
    }

    if (numValidSensors
        != ultrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface
               .NumberOfValidServingSensors) {
        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidServingSensors: "
                           << ultrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface
                                  .NumberOfValidServingSensors
                           << " and numValidSensors: "
                           << ultrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface
                                  .ValidServingSensors.size();
    }

    for (size_t sensorIndex = 0; sensorIndex < numValidSensors; sensorIndex++) {
        osi3::Identifier* ptrOsiSensorID = osiLogicalDetectionDataHeader.add_sensor_id();
        ptrOsiSensorID->set_value(static_cast<uint64_t>(
            ultrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface.ValidServingSensors.at(
                sensorIndex)));
    }
    // Semantics in osi and adi different

    // osi3::Timestamp osiLogicalDetectionTime{};
    // osiLogicalDetectionTime.set_seconds();
    // osiLogicalDetectionTime.set_nanos();
    // osiLogicalDetectionDataHeader.set_allocated_logical_detection_time();

    // Data Qualifier Mapping
    osi3::LogicalDetectionDataHeader_DataQualifier osiLDDataQualifier{};
    switch (ultrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface.DataQualifier) {
    case ara::adi::sensoritf::DataQualifier::kInvalid: {
        osiLDDataQualifier = osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_INVALID;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kNormal: {
        osiLDDataQualifier = osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kNotAvailable: {
        osiLDDataQualifier = osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInCoverage: {
        m_logger.LogWarn() << "No perfect mapping was possible: DataQualifier::kReduceInCoverage "
                              "-> "
                              "osi3::LogicalDetectionDataHeader_DataQualifier::LogicalDetectionDataHeader_"
                              "DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED";
        osiLDDataQualifier = osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInPerformance: {
        m_logger.LogWarn() << "No perfect mapping was possible: DataQualifier::kReduceInPerformance "
                              "-> "
                              "osi3::LogicalDetectionDataHeader_DataQualifier::LogicalDetectionDataHeader_"
                              "DataQualifier_DATA_QUALIFIER_OTHER";
        osiLDDataQualifier = osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInViewAndPerformance: {
        m_logger.LogWarn() << "No perfect mapping was possible: DataQualifier::kReduceInViewAndPerformance "
                              "-> "
                              "osi3::LogicalDetectionDataHeader_DataQualifier::LogicalDetectionDataHeader_"
                              "DataQualifier_DATA_QUALIFIER_OTHER";
        osiLDDataQualifier = osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kTestMode: {
        osiLDDataQualifier = osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_TEMPORARY_AVAILABLE;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kUnknown: {
        osiLDDataQualifier = osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN;
        break;
    }
    default: {
        m_logger.LogError()
            << "Error: mapLogicalDetectionData DataQualifier: Unknown enum entry: "
            << static_cast<std::uint8_t>(
                   ultrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface.DataQualifier);
        break;
    }
    }
    osiLogicalDetectionDataHeader.set_data_qualifier(osiLDDataQualifier);

    // Valid logical detections Mapping
    uint32_t numValidFeatures = ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.size();

    if (numValidFeatures != ultrasonicFeatureInterface.NumberOfValidFeatures) {
        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidFeatures: "
                           << ultrasonicFeatureInterface.NumberOfValidFeatures
                           << " and numValidFeatures: " << numValidFeatures;
    }
    osiLogicalDetectionDataHeader.set_number_of_valid_logical_detections(numValidFeatures);

    return osiLogicalDetectionDataHeader;
}

osi3::LogicalDetection UltrasonicLogicalDetectionDataMapper::mapLogicalDetection(
    const ara::adi::sensoritf::UltrasonicFeatureInterface& ultrasonicFeatureInterface,
    const size_t& index) const
{
    // Sensor id Mapping
    osi3::LogicalDetection osiLogicalDetection{};

    auto numSensorID{
        ultrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.SensorClusterInformation.SensorIDReferenceList};

    // ToDo Check: SensorID first element chosen
    if (numSensorID.size() != 0) {

        for (size_t sensorIndex = 0; sensorIndex < numSensorID.size(); sensorIndex++) {

            auto ptrOsiSensorID = osiLogicalDetection.add_sensor_id();
            ptrOsiSensorID->set_value(
                static_cast<uint64_t>(ultrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader
                                          .SensorClusterInformation.SensorIDReferenceList.at(sensorIndex)));
        }
    }

    if (ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.size() == 0) {
        return {};
    }

    // Object id Mapping
    // ToDo: Verify correctness of Object id Mapping
    auto featureIDAdi{
        ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.at(index).UltrasonicFeaturesStatus.FeatureID};
    osi3::Identifier* osiObjectID{osiLogicalDetection.mutable_object_id()};
    osiObjectID->set_value(featureIDAdi);

    // ToDo: Verify correctness of Mapping
    osiLogicalDetection.set_existence_probability(
        static_cast<double>(ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.at(index)
                                .UltrasonicFeaturesStatus.ExistenceProbabilityFeatureLevel));

    // position Mapping: NA  // ToDo: Verify correctness of Mapping
    // For now, mapped the first vertice of the shape
    size_t detectionIndex = 0;
    auto numValidSegmentPointsList{ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.at(index)
                                       .UltrasonicFeaturesSegmentPoints.ValidSegmentPointsList};

    if (numValidSegmentPointsList.size() == 0) {
        return {};
    }

    osi3::Vector3d* osiPosition{osiLogicalDetection.mutable_position()};
    *osiPosition = mapPoint3D2Osi(ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.at(index)
                                      .UltrasonicFeaturesSegmentPoints.ValidSegmentPointsList.at(detectionIndex)
                                      .Position);

    osi3::Vector3d* osiPositionError{osiLogicalDetection.mutable_position_rmse()};
    *osiPositionError
        = mapPoint3DError2Osi(ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.at(index)
                                  .UltrasonicFeaturesSegmentPoints.ValidSegmentPointsList.at(detectionIndex)
                                  .PositionError);

    // For Ultrasonic 2d velocity available. Mapped accordingly
    osi3::Vector3d* osiVelocity{osiLogicalDetection.mutable_velocity()};
    *osiVelocity = map2DVelocity2Osi(ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.at(index)
                                         .UltrasonicFeaturesSegmentPoints.ValidSegmentPointsList.at(detectionIndex)
                                         .VelocityUltrasonic);

    // For Ultrasonic 2d velocity error available. Mapped accordingly
    osi3::Vector3d* osiVelocityError{osiLogicalDetection.mutable_velocity_rmse()};
    *osiVelocityError
        = map2DVelocityError2Osi(ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.at(index)
                                     .UltrasonicFeaturesSegmentPoints.ValidSegmentPointsList.at(detectionIndex)
                                     .VelocityUltrasonicError);

    // NA
    // osi3::LogicalDetectionClassification osiLDClassification{};
    // osiLogicalDetection.set_classification();

    // ToDo: Verify correctness of Mapping
    osiLogicalDetection.set_existence_probability(
        static_cast<double>(ultrasonicFeatureInterface.ValidUltrasonicFeaturesList.at(index)
                                .UltrasonicFeaturesStatus.ExistenceProbabilityFeatureLevel));

    // NA for Camera
    // osiLogicalDetection.set_intensity();

    // NA for Camera
    // osiLogicalDetection.set_point_target_probability();

    // NA for Camera
    // osiLogicalDetection.set_snr();

    return osiLogicalDetection;
}

osi3::Vector3d UltrasonicLogicalDetectionDataMapper::map2DVelocity2Osi(const ara::adi::sensoritf::Point2D& point) const
{
    osi3::Vector3d vector;

    vector.set_x(point.x);
    vector.set_y(point.y);

    // Setting 0 for now, as 3D velocity NA
    vector.set_z(0);

    return vector;
}

osi3::Vector3d UltrasonicLogicalDetectionDataMapper::map2DVelocityError2Osi(
    const ara::adi::sensoritf::Point2DError& pointError) const
{
    osi3::Vector3d vector;

    vector.set_x(pointError.xError);
    vector.set_y(pointError.yError);

    // Setting 0 for now, as 3D velocity NA
    vector.set_z(0);

    return vector;
}
