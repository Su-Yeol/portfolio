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

#include "AdiOsiAdapter.h"
#include <stdint.h>
#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>
#include <stdexcept>

AdiOsiAdapter::AdiOsiAdapter()
    : roadObjects(&sensorDataMapper)
    , staticObjects(&sensorDataMapper)
    , potentiallyMovingObjects(&sensorDataMapper)
    , cameraDetections(&sensorDataMapper)
    , ultrasonicDetections(&sensorDataMapper)
    , cameraFeatures(&sensorDataMapper)
    , ultrasonicFeatures(&sensorDataMapper)
    , lidarDetections(&sensorDataMapper)
    , radarDetections(&sensorDataMapper)

{
    m_logger.LogDebug() << "AdiOsiAdapter start";
}

void AdiOsiAdapter::Act()
{
    roadObjects.Act();
    staticObjects.Act();
    potentiallyMovingObjects.Act();
    ultrasonicDetections.Act();
    cameraFeatures.Act();
    ultrasonicFeatures.Act();
    lidarDetections.Act();
    radarDetections.Act();
    cameraDetections.Act();

    osiMappedSensorData_ = sensorDataMapper.mapSensorData();

    PrintMappedFeatureData();
    PrintMappedRoadMarkings();
    PrintMappedLaneBoundary();
    PrintMappedDetectedMovingObjects();
    PrintMappedStationaryObjects();
    PrintMappedTrafficSigns();
    PrintMappedTrafficLights();
    PrintMappedLogicalDetectionData();

    // TODO: Print the remianing Mapped data
}

void AdiOsiAdapter::PrintMappedFeatureData() const
{
    m_logger.LogInfo() << "PrintMappedFeatureData";

    // auto osiVersionMajor{osiMappedSensorData_.feature_data().version().version_major()};
    // auto osiVersionMinor{osiMappedSensorData_.feature_data().version().version_minor()};
    // auto osiVersionPatch{osiMappedSensorData_.feature_data().version().version_patch()};

    // Print log output for each variable
    // Uncomment to verify mapped values
    // m_logger.LogInfo() << "osiVersionMajor: " << osiVersionMajor;
    // m_logger.LogInfo() << "osiVersionMinor: " << osiVersionMinor;
    // m_logger.LogInfo() << "osiVersionPatch: " << osiVersionPatch;

    auto osiCameraSensor{osiMappedSensorData_.feature_data().camera_sensor()};

    if (osiCameraSensor.size() == 0)
        return;

    for (auto i = 0; i < osiCameraSensor.size(); i++) {
        auto osiCamPosX{osiCameraSensor.Get(i).header().mounting_position().position().x()};
        auto osiCamPosY{osiCameraSensor.Get(i).header().mounting_position().position().y()};
        auto osiCamPosZ{osiCameraSensor.Get(i).header().mounting_position().position().z()};

        auto osiCamOrientP{osiCameraSensor.Get(i).header().mounting_position().orientation().pitch()};
        auto osiCamOrientR{osiCameraSensor.Get(i).header().mounting_position().orientation().roll()};
        auto osiCamOrientY{osiCameraSensor.Get(i).header().mounting_position().orientation().yaw()};

        m_logger.LogInfo() << "osiCamPosX: " << osiCamPosX;
        m_logger.LogInfo() << "osiCamPosY: " << osiCamPosY;

        // Uncomment to verify mapped values
        // m_logger.LogInfo() << "osiCamPosZ: " << osiCamPosZ;

        // m_logger.LogInfo() << "osiCamOrientP: " << osiCamOrientP;
        // m_logger.LogInfo() << "osiCamOrientR: " << osiCamOrientR;
        // m_logger.LogInfo() << "osiCamOrientY: " << osiCamOrientY;
    }

    // test Lidar Mapping
    auto osiLidarSensor{osiMappedSensorData_.feature_data().lidar_sensor()};
    if (osiLidarSensor.size() == 0)
        return;

    for (auto i = 0; i < osiLidarSensor.size(); i++) {
        auto osiLidarPosX{osiLidarSensor.Get(i).header().mounting_position().position().x()};
        auto osiLidarPosY{osiLidarSensor.Get(i).header().mounting_position().position().y()};
        auto osiLidarPosZ{osiLidarSensor.Get(i).header().mounting_position().position().z()};

        m_logger.LogInfo() << "osiLidarPosX: " << osiLidarPosX;
        m_logger.LogInfo() << "osiLidarPosY: " << osiLidarPosY;
        // m_logger.LogInfo() << "osiLidarPosZ: " << osiLidarPosZ;

        auto osiLidarOrientP{osiLidarSensor.Get(i).header().mounting_position().orientation().pitch()};
        auto osiLidarOrientR{osiLidarSensor.Get(i).header().mounting_position().orientation().roll()};
        auto osiLidarOrientY{osiLidarSensor.Get(i).header().mounting_position().orientation().yaw()};

        // Uncomment to verify mapped values
        // m_logger.LogInfo() << "osiLidarOrientP: " << osiLidarOrientP;
        // m_logger.LogInfo() << "osiLidarOrientR: " << osiLidarOrientR;
        // m_logger.LogInfo() << "osiLidarOrientY: " << osiLidarOrientY;
    }

    // test Radar Mapping
    auto osiRadarSensor{osiMappedSensorData_.feature_data().radar_sensor()};
    if (osiRadarSensor.size() == 0)
        return;

    for (auto i = 0; i < osiRadarSensor.size(); i++) {
        auto osiRadarPosX{osiRadarSensor.Get(i).header().mounting_position().position().x()};
        auto osiRadarPosY{osiRadarSensor.Get(i).header().mounting_position().position().y()};
        auto osiRadarPosZ{osiRadarSensor.Get(i).header().mounting_position().position().z()};

        m_logger.LogInfo() << "osiRadarPosX: " << osiRadarPosX;
        m_logger.LogInfo() << "osiRadarPosY: " << osiRadarPosY;
        // m_logger.LogInfo() << "osiRadarPosZ: " << osiRadarPosZ;

        auto osiRadarOrientP{osiRadarSensor.Get(i).header().mounting_position().orientation().pitch()};
        auto osiRadarOrientR{osiRadarSensor.Get(i).header().mounting_position().orientation().roll()};
        auto osiRadarOrientY{osiRadarSensor.Get(i).header().mounting_position().orientation().yaw()};

        // Uncomment to verify mapped values
        // m_logger.LogInfo() << "osiRadarOrientP: " << osiRadarOrientP;
        // m_logger.LogInfo() << "osiRadarOrientR: " << osiRadarOrientR;
        // m_logger.LogInfo() << "osiRadarOrientY: " << osiRadarOrientY;
    }

    // test Ultrasonic Mapping
    auto osiUltrasonicSensor{osiMappedSensorData_.feature_data().radar_sensor()};
    if (osiUltrasonicSensor.size() == 0)
        return;

    for (auto i = 0; i < osiUltrasonicSensor.size(); i++) {
        auto osiUltrasonicPosX{osiUltrasonicSensor.Get(i).header().mounting_position().position().x()};
        auto osiUltrasonicPosY{osiUltrasonicSensor.Get(i).header().mounting_position().position().y()};
        auto osiUltrasonicPosZ{osiUltrasonicSensor.Get(i).header().mounting_position().position().z()};

        m_logger.LogInfo() << "osiUltrasonicPosX: " << osiUltrasonicPosX;
        m_logger.LogInfo() << "osiUltrasonicPosY: " << osiUltrasonicPosY;
        // m_logger.LogInfo() << "osiUltrasonicPosZ: " << osiUltrasonicPosZ;

        auto osiUltrasonicOrientP{osiUltrasonicSensor.Get(i).header().mounting_position().orientation().pitch()};
        auto osiUltrasonicOrientR{osiUltrasonicSensor.Get(i).header().mounting_position().orientation().roll()};
        auto osiUltrasonicOrientY{osiUltrasonicSensor.Get(i).header().mounting_position().orientation().yaw()};

        // Uncomment to verify mapped values
        // m_logger.LogInfo() << "osiUltrasonicOrientP: " << osiUltrasonicOrientP;
        // m_logger.LogInfo() << "osiUltrasonicOrientR: " << osiUltrasonicOrientR;
        // m_logger.LogInfo() << "osiUltrasonicOrientY: " << osiUltrasonicOrientY;
    }
}

void AdiOsiAdapter::PrintMappedRoadMarkings() const
{
    m_logger.LogInfo() << "Print RoadMarking data!";

    if (osiMappedSensorData_.road_marking().size() == 0) {
        return;
    }

    for (int osiRMindex = 0; osiRMindex < osiMappedSensorData_.road_marking().size(); osiRMindex++) {

        auto osiRMHeader{osiMappedSensorData_.road_marking().Get(osiRMindex).header()};

        auto osiRMHeaderAge{osiRMHeader.age()};
        auto osiRMHeaderExistProb{osiRMHeader.existence_probability()};
        auto osiRMHeaderTrackIDValue{osiRMHeader.tracking_id().value()};
        auto osiRMHeaderMeasureState{osiRMHeader.measurement_state()};

        auto osiRMBase{osiMappedSensorData_.road_marking().Get(osiRMindex).base()};
        auto osiRMBaseXPOs{osiRMBase.position().x()};
        auto osiRMBaseYPOs{osiRMBase.position().y()};

        m_logger.LogInfo() << "osiRMindex: " << osiRMindex;
        m_logger.LogInfo() << "osiRMHeaderAge: " << osiRMHeaderAge;

        // Uncomment to verify mapped values
        // m_logger.LogInfo() << "osiRMHeaderExistProb: " << osiRMHeaderExistProb;
        // m_logger.LogInfo() << "osiRMHeaderTrackIDValue: " << osiRMHeaderTrackIDValue;
        // m_logger.LogInfo() << "osiRMHeaderMeasureState: " << static_cast<int>(osiRMHeaderMeasureState);
        // m_logger.LogInfo() << "osiRMBaseXPOs: " << osiRMBaseXPOs;
        // m_logger.LogInfo() << "osiRMBaseYPOs: " << osiRMBaseYPOs;
    }
}

void AdiOsiAdapter::PrintMappedLaneBoundary() const
{
    m_logger.LogInfo() << "Print LaneBoundary data!";

    if (osiMappedSensorData_.lane_boundary().size() == 0) {
        return;
    }

    for (int osiLBindex = 0; osiLBindex < osiMappedSensorData_.lane_boundary().size(); osiLBindex++) {

        auto osiLBHeader{osiMappedSensorData_.lane_boundary().Get(osiLBindex).header()};

        auto osiLBHeaderAge{osiLBHeader.age()};
        m_logger.LogInfo() << "osiLBHeaderAge: " << osiLBHeaderAge;

        auto osiLBLines{osiMappedSensorData_.lane_boundary().Get(0).boundary_line()};

        if (osiLBLines.size() == 0) {
            return;
        }

        for (int osiLBLineIndex = 0; osiLBLineIndex < osiLBLines.size(); osiLBLineIndex++) {

            auto osiLBLine{osiLBLines.Get(osiLBLineIndex)};
            auto osiLBLineXPOs{osiLBLine.position().x()};
            auto osiLBLineYPOs{osiLBLine.position().y()};
            auto osiLBLineZPOs{osiLBLine.position().z()};
            auto osiLBLineHeight{osiLBLine.height()};
            auto osiLBLineWidth{osiLBLine.width()};

            m_logger.LogInfo() << "osiLBLineIndex: " << osiLBLineIndex;

            // Uncomment to verify mapped values
            // m_logger.LogInfo() << "osiLBLineXPOs: " << osiLBLineXPOs;
            // m_logger.LogInfo() << "osiLBLineYPOs: " << osiLBLineYPOs;
            // m_logger.LogInfo() << "osiLBLineZPOs: " << osiLBLineZPOs;
            // m_logger.LogInfo() << "osiLBLineHeight: " << osiLBLineHeight;
            // m_logger.LogInfo() << "osiLBLineWidth: " << osiLBLineWidth;
        }
    }
}

void AdiOsiAdapter::PrintMappedDetectedMovingObjects() const
{
    m_logger.LogInfo() << "Print DetectedMovingObjects data!";

    auto osiDMObjects{osiMappedSensorData_.moving_object()};

    if (osiDMObjects.size() == 0)
        return;

    for (int osiDMOIndex = 0; osiDMOIndex < osiDMObjects.size(); osiDMOIndex++) {

        auto osiDMOHeader{osiDMObjects.Get(osiDMOIndex).header()};

        auto osiDMOHeaderAge{osiDMOHeader.age()};
        auto osiDMOHeaderExistProb{osiDMOHeader.existence_probability()};
        auto osiDMOHeaderTrackIDValue{osiDMOHeader.tracking_id().value()};

        auto osiDMOBaseDMOse{osiDMObjects.Get(osiDMOIndex).base_rmse()};
        auto osiDMOBaseXPOsErr{osiDMOBaseDMOse.position().x()};
        auto osiDMOBaseYPOsErr{osiDMOBaseDMOse.position().y()};
        auto osiDMOBaseZPOsErr{osiDMOBaseDMOse.position().z()};

        auto osiDMORefPoint{osiDMObjects.Get(osiDMOIndex).reference_point()};

        m_logger.LogInfo() << "osiDMOHeaderAge: " << osiDMOHeaderAge;
        m_logger.LogInfo() << "osiDMOHeaderExistProb: " << osiDMOHeaderExistProb;

        // Uncomment to verify mapped values
        // m_logger.LogInfo() << "osiDMOHeaderTrackIDValue: " << osiDMOHeaderTrackIDValue;
        // m_logger.LogInfo() << "osiDMOBaseXPOsErr: " << osiDMOBaseXPOsErr;
        // m_logger.LogInfo() << "osiDMOBaseYPOsErr: " << osiDMOBaseYPOsErr;
        // m_logger.LogInfo() << "osiDMOBaseZPOsErr: " << osiDMOBaseZPOsErr;
        // m_logger.LogInfo() << "osiDMORefPt: " << static_cast<int>(osiDMORefPoint);
    }
}

void AdiOsiAdapter::PrintMappedStationaryObjects() const
{
    m_logger.LogInfo() << "Print MappedStationaryObjects data!";
    if (osiMappedSensorData_.stationary_object().size() == 0) {
        return;
    }

    for (int osiSOindex = 0; osiSOindex < osiMappedSensorData_.stationary_object().size(); osiSOindex++) {

        m_logger.LogInfo() << "osiSOindex: " << osiSOindex;

        auto osiSOHeader{osiMappedSensorData_.stationary_object().Get(osiSOindex).header()};

        auto osiSOHeaderAge{osiSOHeader.age()};
        auto osiSOHeaderExistProb{osiSOHeader.existence_probability()};
        auto osiSOHeaderTrackIDValue{osiSOHeader.tracking_id().value()};
        auto osiSOHeaderMeasureState{osiSOHeader.measurement_state()};

        m_logger.LogInfo() << "osiSOHeaderAge: " << osiSOHeaderAge;
        m_logger.LogInfo() << "osiSOHeaderExistProb: " << osiSOHeaderExistProb;
        // m_logger.LogInfo() << "osiSOHeaderTrackIDValue: " << osiSOHeaderTrackIDValue;
        // m_logger.LogInfo() << "osiSOHeaderMeasureState: " << static_cast<int>(osiSOHeaderMeasureState);

        auto osiSOBase{osiMappedSensorData_.stationary_object().Get(osiSOindex).base()};
        auto osiSOBaseDimH{osiSOBase.dimension().height()};
        auto osiSOBaseDimL{osiSOBase.dimension().length()};
        auto osiSOBaseDimW{osiSOBase.dimension().width()};

        // Uncomment to verify mapped values
        // m_logger.LogInfo() << "osiSOBaseDimH: " << osiSOBaseDimH;
        // m_logger.LogInfo() << "osiSOBaseDimL: " << osiSOBaseDimL;
        // m_logger.LogInfo() << "osiSOBaseDimW: " << osiSOBaseDimW;
    }
}

void AdiOsiAdapter::PrintMappedTrafficLights() const
{
    m_logger.LogInfo() << "Print MappedTrafficLights data!";
    for (int osiTLindex = 0; osiTLindex < osiMappedSensorData_.traffic_light().size(); osiTLindex++) {

        m_logger.LogInfo() << "osiTLindex: " << osiTLindex;

        auto osiTLBase{osiMappedSensorData_.traffic_light().Get(osiTLindex).base()};
        auto osiTLBaseXPOs{osiTLBase.position().x()};
        auto osiTLBaseYPOs{osiTLBase.position().y()};
        auto osiTLBaseZPOs{osiTLBase.position().z()};

        m_logger.LogInfo() << "osiTLBaseXPOs: " << osiTLBaseXPOs;

        // Uncomment to verify mapped values
        // m_logger.LogInfo() << "osiTLBaseYPOs: " << osiTLBaseYPOs;
        // m_logger.LogInfo() << "osiTLBaseZPOs: " << osiTLBaseZPOs;

        auto osiTLCandidates{osiMappedSensorData_.traffic_light().Get(osiTLindex).candidate()};

        if (osiTLCandidates.size() == 0) {
            return;
        }

        for (int osiTLCandIndex = 0; osiTLCandIndex < osiTLCandidates.size(); osiTLCandIndex++) {

            m_logger.LogInfo() << "osiTLCandIndex: " << osiTLCandIndex;

            auto osiTLCandidate{osiTLCandidates.Get(osiTLCandIndex)};

            auto osiTLCandidateProb{osiTLCandidate.probability()};
            auto osiTLCandidateClass{osiTLCandidate.classification()};
            auto osiTLCandidateClassCounter{osiTLCandidateClass.counter()};
            auto osiTLCandidateClassLaneID{osiTLCandidateClass.assigned_lane_id().Get(0).value()};
            auto osiTLCandidateClassIsOutOfServ{osiTLCandidateClass.is_out_of_service()};

            // Uncomment to verify mapped values
            // m_logger.LogInfo() << "osiTLCandidateProb: " << osiTLCandidateProb;
            // m_logger.LogInfo() << "osiTLCandidateClassCounter: " << osiTLCandidateClassCounter;
            // m_logger.LogInfo() << "osiTLCandidateClassLaneID: " << osiTLCandidateClassLaneID;
            // m_logger.LogInfo() << "osiTLCandidateClassIsOutOfServ: " << osiTLCandidateClassIsOutOfServ;
        }
    }
}

void AdiOsiAdapter::PrintMappedTrafficSigns() const
{
    m_logger.LogInfo() << "Print MappedTrafficSigns data!";
    if (osiMappedSensorData_.traffic_sign().size() == 0) {
        return;
    }

    for (int osiTSindex = 0; osiTSindex < osiMappedSensorData_.traffic_sign().size(); osiTSindex++) {

        auto osiTSHeader{osiMappedSensorData_.traffic_sign().Get(osiTSindex).header()};
        m_logger.LogInfo() << "osiTSindex: " << osiTSindex;

        auto osiTSHeaderAge{osiTSHeader.age()};
        auto osiTSHeaderExistProb{osiTSHeader.existence_probability()};
        auto osiTSHeaderTrackIDValue{osiTSHeader.tracking_id().value()};
        auto osiTSHeaderMeasureState{osiTSHeader.measurement_state()};

        m_logger.LogInfo() << "osiTSHeaderAge: " << osiTSHeaderAge;
        m_logger.LogInfo() << "osiTSHeaderExistProb: " << osiTSHeaderExistProb;

        // Uncomment to verify mapped values
        // m_logger.LogInfo() << "osiTSHeaderTrackIDValue: " << osiTSHeaderTrackIDValue;
        // m_logger.LogInfo() << "osiTSHeaderMeasureState: " << static_cast<int>(osiTSHeaderMeasureState);

        auto osiTSMainSign{osiMappedSensorData_.traffic_sign().Get(osiTSindex).main_sign()};
        auto osiTSMainSignXPOs{osiTSMainSign.base().position().x()};
        auto osiTSMainSignYPOs{osiTSMainSign.base().position().y()};
        auto osiTSMainSignZPOs{osiTSMainSign.base().position().z()};

        // Uncomment to verify mapped values
        // m_logger.LogInfo() << "osiTSMainSignXPOs: " << osiTSMainSignXPOs;
        // m_logger.LogInfo() << "osiTSMainSignYPOs: " << osiTSMainSignYPOs;
        // m_logger.LogInfo() << "osiTSMainSignZPOs: " << osiTSMainSignZPOs;
    }
}

void AdiOsiAdapter::PrintMappedLogicalDetectionData() const
{
    m_logger.LogInfo() << "Print MappedLogicalDetectionData data!";
    // auto osiVersionMajor{osiMappedSensorData_.logical_detection_data().version().version_major()};
    // auto osiVersionMinor{osiMappedSensorData_.logical_detection_data().version().version_minor()};
    // auto osiVersionPatch{osiMappedSensorData_.logical_detection_data().version().version_patch()};

    // m_logger.LogInfo() << "osiVersionMajor: " << osiVersionMajor;

    // Uncomment to verify mapped values
    // m_logger.LogInfo() << "osiVersionMinor: " << osiVersionMinor;
    // m_logger.LogInfo() << "osiVersionPatch: " << osiVersionPatch;

    auto osiLogicalDetHeader{osiMappedSensorData_.logical_detection_data().header()};

    if (osiLogicalDetHeader.sensor_id().size() == 0)
        return;

    for (auto index = 0; index < osiLogicalDetHeader.sensor_id().size(); index++) {
        auto osiLogicalDetValue{osiLogicalDetHeader.sensor_id().Get(index).value()};
        m_logger.LogInfo() << "osiLogicalDetValue: " << osiLogicalDetValue;
    }

    auto osiLDDataQualifier{osiLogicalDetHeader.data_qualifier()};
    auto osiLDValidDetections{osiLogicalDetHeader.number_of_valid_logical_detections()};
    m_logger.LogInfo() << "osiLDDataQualifier: " << static_cast<int>(osiLDDataQualifier);
    // m_logger.LogInfo() << "osiLDValidDetections: " << osiLDValidDetections;
}
