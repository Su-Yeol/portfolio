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

#ifndef ADI_OSI_ADAPTER_FEATUREDATA_CAMERADETECTION_MAPPING_HPP_
#define ADI_OSI_ADAPTER_FEATUREDATA_CAMERADETECTION_MAPPING_HPP_

#include "ara/log/logger.h"

#include "ara/adi/sensoritf/impl_type_cameradetectionsinterface.h"
#include "ara/adi/sensoritf/impl_type_ultrasonicdetectionsinterface.h"
#include "ara/adi/sensoritf/impl_type_lidardetectionsinterface.h"
#include "ara/adi/sensoritf/impl_type_radardetectionsinterface.h"

#include "GeneralHeaderMapper.h"

class FeatureDataMapper : public GeneralHeaderMapper
{
public:
    FeatureDataMapper() = default;

public:
    osi3::FeatureData mapFeatureData(
        const std::shared_ptr<ara::adi::sensoritf::CameraDetectionsInterface> cameraDetectionsInterface,
        const std::shared_ptr<ara::adi::sensoritf::UltrasonicDetectionsInterface> ultrasonicDetectionsInterface,
        const std::shared_ptr<ara::adi::sensoritf::LidarDetectionsInterface> lidarDetectionsInterface,
        const std::shared_ptr<ara::adi::sensoritf::RadarDetectionsInterface> radarDetectionsInterface) const;

private:
    osi3::CameraDetectionData mapCameraDetectionData(
        const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface) const;

    osi3::CameraDetection mapCameraDetection(
        const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface,
        const size_t& index) const;

    osi3::CameraPoint mapCameraPoint(const ara::adi::sensoritf::CameraShapesShapePoints& cameraShapePoints,
        const size_t& index) const;

    osi3::SensorDetectionHeader mapSensorDetectionHeader(
        const ara::adi::sensoritf::CameraDetectionsInterface& cameraDetectionsInterface) const;

    osi3::UltrasonicDetectionData mapUltrasonicDetectionData(
        const ara::adi::sensoritf::UltrasonicDetectionsInterface& ultrasonicDetectionsInterface) const;

    osi3::UltrasonicDetection mapUltrasonicDetection(
        const ara::adi::sensoritf::UltrasonicDetectionsInterface& ultrasonicDetectionsInterface,
        const size_t& index) const;

    osi3::SensorDetectionHeader mapUltrasonicSensorDetectionHeader(
        const ara::adi::sensoritf::UltrasonicDetectionsInterface& ultrasonicDetectionsInterface) const;

    osi3::LidarDetectionData mapLidarDetectionData(
        const ara::adi::sensoritf::LidarDetectionsInterface& lidarDetectionsInterface) const;

    osi3::LidarDetection mapLidarDetection(
        const ara::adi::sensoritf::LidarDetectionsInterface& lidarDetectionsInterface) const;

    osi3::SensorDetectionHeader mapLidarSensorDetectionHeader(
        const ara::adi::sensoritf::LidarDetectionsInterface& lidarDetectionsInterface) const;

    osi3::RadarDetectionData mapRadarDetectionData(
        const ara::adi::sensoritf::RadarDetectionsInterface& radarDetectionsInterface) const;

    osi3::RadarDetection mapRadarDetection(
        const ara::adi::sensoritf::RadarDetectionsInterface& radarDetectionsInterface,
        const size_t& index) const;

    osi3::SensorDetectionHeader mapRadarSensorDetectionHeader(
        const ara::adi::sensoritf::RadarDetectionsInterface& radarDetectionsInterface) const;

    ara::log::Logger& m_logger{ara::log::CreateLogger("FDM", "...", ara::log::LogLevel::kVerbose)};
};

#endif  // ADI_OSI_ADAPTER_FEATUREDATA_CAMERADETECTION_MAPPING_HPP_
