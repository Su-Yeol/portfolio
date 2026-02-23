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

#ifndef OSI_ADI_ADAPTER_FEATUREDATA_ULTRASONICDETECTION_MAPPER_HPP_
#define OSI_ADI_ADAPTER_FEATUREDATA_ULTRASONICDETECTION_MAPPER_HPP_

#include "ara/log/logger.h"

#include "ara/adi/sensoritf/ultrasonicdetectionsservice_skeleton.h"

#include "osi3/osi_sensordata.pb.h"

#include "adiMappers/GeneralElementsMapper.h"
#include "adiMappers/ObjectStatusMapper.h"

using namespace ara::adi::sensoritf;

class UltrasonicDetectionsMapper : public GeneralElementsMapper
{

public:
    UltrasonicDetectionsMapper() = default;
    UltrasonicDetectionsInterface mapUltrasonicDetectionsInterface(osi3::SensorData osiSensorData);

private:
    InformationInterface mapSensorDetectionHeader(osi3::FeatureData osiFeatureData,
        osi3::UltrasonicDetectionData osiUltrasonicSensor);
    InterfaceHeader mapUDInterfaceHeader(osi3::FeatureData osiFeatureData,
        osi3::UltrasonicDetectionData osiUltrasonicSensor);
    UltrasonicDetection mapUltrasonicDetection(osi3::UltrasonicDetectionData osiUltrasonicSensor,
        int osiDetectionIndex);
    // UltrasonicDetectionsInformation mapUltrasonicDetectionInfo(osi3::UltrasonicDetection osiCamSensorDetection);
    UltrasonicDetectionsPosition mapUltrasonicDetectionsPosition(osi3::UltrasonicDetectionData osiUltrasonicSensor,
        int osiDetectionIndex);
    DetectionStatus mapUltrasonicDetectionStatus(osi3::UltrasonicDetectionData osiUltrasonicSensor,
        int osiDetectionIndex);

private:
    ara::log::Logger& m_cf_logger{ara::log::CreateLogger("UDM", "...", ara::log::LogLevel::kVerbose)};
    ObjectStatusMapper objectStatusMapper;
};

#endif  // OSI_ADI_ADAPTER_FEATUREDATA_ULTRASONICDETECTION_MAPPER_HPP_
