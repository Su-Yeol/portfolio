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

#ifndef OSI_ADI_ADAPTER_LOGICALDETECTION_CAMERAFEATURES_MAPPER_HPP_
#define OSI_ADI_ADAPTER_LOGICALDETECTION_CAMERAFEATURES_MAPPER_HPP_

#include "ara/log/logger.h"

#include "ara/adi/sensoritf/camerafeaturesservice_skeleton.h"

#include "osi3/osi_sensordata.pb.h"

#include "adiMappers/GeneralElementsMapper.h"
#include "adiMappers/ObjectStatusMapper.h"

#include "ara/adi/sensoritf/impl_type_informationinterface.h"
#include "ara/adi/sensoritf/impl_type_dataqualifier.h"

using namespace ara::adi::sensoritf;

class CameraFeaturesMapper : public GeneralElementsMapper
{

public:
    CameraFeaturesMapper() = default;
    CameraFeatureInterface mapCameraFeatureInterface(const osi3::SensorData& osiSensorData);

private:
    InformationInterface mapInformationInterface(const osi3::LogicalDetectionData& osiLogicalDetectionData);
    DataQualifier mapDataQualifier(const osi3::LogicalDetectionData& osiLogicalDetectionData);
    FeatureStatus mapFeatureStatus(const osi3::LogicalDetectionData& osiLogicalDetectionData, const int& index);
    ShapePoint mapShapePoint(const osi3::LogicalDetectionData& osiLogicalDetectionData, const int& index);

private:
    ara::log::Logger& m_cf_logger{ara::log::CreateLogger("CFM", "...", ara::log::LogLevel::kVerbose)};
    ObjectStatusMapper objectStatusMapper;
};

#endif  // OSI_ADI_ADAPTER_LOGICALDETECTION_CAMERAFEATURES_MAPPER_HPP_
