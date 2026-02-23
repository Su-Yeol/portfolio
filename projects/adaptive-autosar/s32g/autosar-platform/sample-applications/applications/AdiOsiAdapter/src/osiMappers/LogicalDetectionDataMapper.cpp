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

#include "osiMappers/LogicalDetectionDataMapper.h"

#include "osi3/osi_sensordata.pb.h"

osi3::LogicalDetectionData LogicalDetectionDataMapper::mapLogicalDetectionData(
    const ara::adi::sensoritf::CameraFeatureInterface& cameraFeatureInterface) const
{
    osi3::LogicalDetectionData osiLogicalDetectionData{};

    // Interface Version Mapping
    osi3::InterfaceVersion* osiInterfaceversion{osiLogicalDetectionData.mutable_version()};
    *osiInterfaceversion
        = mapInterfaceVersion(cameraFeatureInterface.CameraFeatureInterfaceHeader.InformationInterface.VersionID);

    // LogicalDetectionDataHeader Mapping
    // InterfaceHeader as argument cannot be sent as other info also required
    osi3::LogicalDetectionDataHeader* osiLogicalDetectionDataHeader{osiLogicalDetectionData.mutable_header()};
    *osiLogicalDetectionDataHeader = mapLogicalDetectionDataHeader(cameraFeatureInterface);

    // LogicalDetection Mapping
    auto cameraFeatureList{cameraFeatureInterface.ValidCameraFeaturesList};

    if (cameraFeatureList.size() == 0) {
        return {};
    }

    if (cameraFeatureList.size() != cameraFeatureInterface.NumberOfValidFeatures) {
        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidFeatures: "
                           << cameraFeatureInterface.NumberOfValidFeatures
                           << " and cameraFeatureList.size(): " << cameraFeatureList.size();
    }

    for (size_t index = 0; index < cameraFeatureList.size(); index++) {

        osi3::LogicalDetection* osiLogicalDetection = osiLogicalDetectionData.add_logical_detection();
        *osiLogicalDetection = mapLogicalDetection(cameraFeatureInterface, index);
    }

    return osiLogicalDetectionData;
}

osi3::LogicalDetectionDataHeader LogicalDetectionDataMapper::mapLogicalDetectionDataHeader(
    const ara::adi::sensoritf::CameraFeatureInterface& cameraFeatureInterface) const
{
    osi3::LogicalDetectionDataHeader osiLogicalDetectionDataHeader{};
    size_t numValidSensors
        = cameraFeatureInterface.CameraFeatureInterfaceHeader.InformationInterface.ValidServingSensors.size();

    if (numValidSensors == 0) {
        return {};
    }

    if (numValidSensors
        != cameraFeatureInterface.CameraFeatureInterfaceHeader.InformationInterface.NumberOfValidServingSensors) {

        m_logger.LogWarn()
            << "Warning: Mismatch between NumberOfValidServingSensors: "
            << cameraFeatureInterface.CameraFeatureInterfaceHeader.InformationInterface.NumberOfValidServingSensors
            << " and numValidSensors: "
            << cameraFeatureInterface.CameraFeatureInterfaceHeader.InformationInterface.ValidServingSensors.size();
    }

    for (size_t sensorIndex = 0; sensorIndex < numValidSensors; sensorIndex++) {
        osi3::Identifier* ptrOsiSensorID = osiLogicalDetectionDataHeader.add_sensor_id();
        ptrOsiSensorID->set_value(static_cast<uint64_t>(
            cameraFeatureInterface.CameraFeatureInterfaceHeader.InformationInterface.ValidServingSensors.at(
                sensorIndex)));
    }
    // Semantics in osi and adi different

    // osi3::Timestamp osiLogicalDetectionTime{};
    // osiLogicalDetectionTime.set_seconds();
    // osiLogicalDetectionTime.set_nanos();
    // osiLogicalDetectionDataHeader.set_allocated_logical_detection_time();

    // Data Qualifier Mapping
    osi3::LogicalDetectionDataHeader_DataQualifier osiLDDataQualifier{};
    switch (cameraFeatureInterface.CameraFeatureInterfaceHeader.InformationInterface.DataQualifier) {
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
        m_logger.LogWarn() << "No perfect mapping was possible: DataQualifier::"
                              "kReduceInCoverage -> "
                              "osi3::LogicalDetectionDataHeader_DataQualifier::LogicalDetectionDataHeader_"
                              "DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED";

        osiLDDataQualifier = osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInPerformance: {
        m_logger.LogWarn() << "No perfect mapping was possible: DataQualifier::"
                              "kReduceInPerformance -> "
                              "osi3::LogicalDetectionDataHeader_DataQualifier::LogicalDetectionDataHeader_"
                              "DataQualifier_DATA_QUALIFIER_OTHER";

        osiLDDataQualifier = osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_OTHER;
        break;
    }
    case ara::adi::sensoritf::DataQualifier::kReduceInViewAndPerformance: {
        m_logger.LogWarn() << "No perfect mapping was possible: DataQualifier::"
                              "kReduceInViewAndPerformance -> "
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
        m_logger.LogError() << "Error: mapLogicalDetectionData DataQualifier: Unknown enum entry ";
        break;
    }
    }

    osiLogicalDetectionDataHeader.set_data_qualifier(osiLDDataQualifier);

    // Valid logical detections Mapping
    uint32_t numValidFeatures = cameraFeatureInterface.ValidCameraFeaturesList.size();

    if (numValidFeatures != cameraFeatureInterface.NumberOfValidFeatures) {
        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidFeatures: "
                           << cameraFeatureInterface.NumberOfValidFeatures
                           << " and numValidFeatures: " << cameraFeatureInterface.ValidCameraFeaturesList.size();
    }
    osiLogicalDetectionDataHeader.set_number_of_valid_logical_detections(numValidFeatures);

    return osiLogicalDetectionDataHeader;
}

osi3::LogicalDetection LogicalDetectionDataMapper::mapLogicalDetection(
    const ara::adi::sensoritf::CameraFeatureInterface& cameraFeatureInterface,
    const size_t& index) const
{
    // Sensor id Mapping
    osi3::LogicalDetection osiLogicalDetection{};

    auto numSensorID{
        cameraFeatureInterface.CameraFeatureInterfaceHeader.SensorClusterInformation.SensorIDReferenceList};

    // ToDo Check: SensorID first element chosen
    if (numSensorID.size() != 0) {

        for (size_t sensorIndex = 0; sensorIndex < numSensorID.size(); sensorIndex++) {

            auto ptrOsiSensorID = osiLogicalDetection.add_sensor_id();
            ptrOsiSensorID->set_value(static_cast<uint64_t>(
                cameraFeatureInterface.CameraFeatureInterfaceHeader.SensorClusterInformation.SensorIDReferenceList.at(
                    sensorIndex)));
        }
    }

    if (cameraFeatureInterface.ValidCameraFeaturesList.size() == 0) {
        return {};
    }

    // Object id Mapping
    // ToDo: Verify correctness of Object id Mapping

    auto featureIDAdi{cameraFeatureInterface.ValidCameraFeaturesList.at(index).CameraFeaturesStatus.FeatureID};
    osi3::Identifier* osiObjectID{osiLogicalDetection.mutable_object_id()};
    osiObjectID->set_value(featureIDAdi);

    // position Mapping: NA  // ToDo: Verify correctness of Mapping
    // For now, mapped the first vertice of the shape
    size_t detectionIndex = 0;
    auto numValidSegmentPointsList{
        cameraFeatureInterface.ValidCameraFeaturesList.at(index).CameraFeaturesShapePoints.ValidShapePointsList};

    if (numValidSegmentPointsList.size() == 0) {
        return {};
    }

    osi3::Vector3d* osiPosition{osiLogicalDetection.mutable_position()};
    *osiPosition = mapPoint3D2Osi(cameraFeatureInterface.ValidCameraFeaturesList.at(index)
                                      .CameraFeaturesShapePoints.ValidShapePointsList.at(detectionIndex)
                                      .Position);

    osi3::Vector3d* osiPositionError{osiLogicalDetection.mutable_position_rmse()};
    *osiPositionError = mapPoint3DError2Osi(cameraFeatureInterface.ValidCameraFeaturesList.at(index)
                                                .CameraFeaturesShapePoints.ValidShapePointsList.at(detectionIndex)
                                                .PositionError);

    // NA for Camera
    // osiLogicalDetection.set_allocated_velocity();

    // NA for Camera
    // osiLogicalDetection.set_allocated_velocity_rmse();

    // NA for Camera
    // osi3::LogicalDetectionClassification osiLDClassification{};
    // osiLogicalDetection.set_classification();

    // ToDo: Verify correctness of Mapping
    osiLogicalDetection.set_existence_probability(
        static_cast<double>(cameraFeatureInterface.ValidCameraFeaturesList.at(index)
                                .CameraFeaturesStatus.ExistenceProbabilityFeatureLevel));

    // NA for Camera
    // osiLogicalDetection.set_intensity();

    // NA for Camera
    // osiLogicalDetection.set_point_target_probability();

    // NA for Camera
    // osiLogicalDetection.set_snr();

    return osiLogicalDetection;
}
