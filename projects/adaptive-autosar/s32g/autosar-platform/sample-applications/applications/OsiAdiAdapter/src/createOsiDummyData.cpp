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

#include "createOsiDummyData.h"
#include <ara/log/logger.h>

osi3::Vector3d setVector3D(const osiVector3DValue& osiVector3DValue)
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "setVector3D";

    osi3::Vector3d osiVector3d;
    osiVector3d.set_x(osiVector3DValue.x);
    osiVector3d.set_y(osiVector3DValue.y);
    osiVector3d.set_z(osiVector3DValue.z);

    return osiVector3d;
}

osi3::Orientation3d setOrientation3D(const osiOrientation3DValue& osiOrientation3DValue)
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "setOrientation3D";

    osi3::Orientation3d osiOrientation3d;
    osiOrientation3d.set_pitch(osiOrientation3DValue.p);
    osiOrientation3d.set_roll(osiOrientation3DValue.r);
    osiOrientation3d.set_yaw(osiOrientation3DValue.y);

    return osiOrientation3d;
}

osi3::Dimension3d setDimension3D(const osiDimension3DValue& osiDimension3DValue)
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "setDimension3D";

    osi3::Dimension3d osiDimension3d;
    osiDimension3d.set_height(osiDimension3DValue.h);
    osiDimension3d.set_length(osiDimension3DValue.l);
    osiDimension3d.set_width(osiDimension3DValue.w);

    return osiDimension3d;
}

osi3::Spherical3d setSpherical3D(const osiSpherical3DValue& osiSpherical3DValue)
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "setSpherical3D";

    osi3::Spherical3d osiSpherical3d;
    osiSpherical3d.set_azimuth(osiSpherical3DValue.a);
    osiSpherical3d.set_distance(osiSpherical3DValue.d);
    osiSpherical3d.set_elevation(osiSpherical3DValue.e);

    return osiSpherical3d;
}

osi3::DetectedLane createDetectedLaneData()
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createDetectedLaneData";

    osi3::DetectedLane osiDetectedLane{};
    osi3::DetectedLane_CandidateLane* osiDLCandidateLane = osiDetectedLane.add_candidate();

    osi3::Lane_Classification* classification = osiDLCandidateLane->mutable_classification();
    ;
    // classification->mutable_road_condition();
    // classification->set_centerline_is_driving_direction();
    // classification->set_is_host_vehicle_lane();
    classification->set_subtype(osi3::Lane_Classification_Subtype::Lane_Classification_Subtype_SUBTYPE_OTHER);
    classification->set_type(osi3::Lane_Classification_Type::Lane_Classification_Type_TYPE_DRIVING);

    osiDLCandidateLane->set_probability(0.800);

    return osiDetectedLane;
}

osi3::DetectedItemHeader setDetectedItemHeader(const osiDetectedItemHeaderValue& osiDetectedItemHeaderValue)
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "setDetectedItemHeader";

    osi3::DetectedItemHeader osiDetectedItemHeader{};

    osi3::Identifier* osiGTIdentifier = osiDetectedItemHeader.add_ground_truth_id();
    osiGTIdentifier->set_value(osiDetectedItemHeaderValue.osiGroundtruthID);

    osi3::Identifier* osiSIDIdentifier = osiDetectedItemHeader.add_sensor_id();
    osiSIDIdentifier->set_value(osiDetectedItemHeaderValue.osiSensorID);

    osi3::Identifier* osiTIDIdentifier = osiDetectedItemHeader.mutable_tracking_id();
    osiTIDIdentifier->set_value(osiDetectedItemHeaderValue.osiTrackingID);

    osiDetectedItemHeader.set_existence_probability(osiDetectedItemHeaderValue.osiExistProb);
    osiDetectedItemHeader.set_measurement_state(osiDetectedItemHeaderValue.osiDIHMeasurement);

    return osiDetectedItemHeader;
}

osi3::BaseMoving setBaseMoving(const osiBaseMovingValue& osiBaseMovingValue)
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "setBaseMoving";

    osi3::BaseMoving osiBaseMoving{};

    osi3::Vector3d* osiBMAcc = osiBaseMoving.mutable_acceleration();
    *osiBMAcc = setVector3D(osiBaseMovingValue.osiAccValue);

    osi3::Dimension3d* osiBMDim = osiBaseMoving.mutable_dimension();
    *osiBMDim = setDimension3D(osiBaseMovingValue.osiDimValue);

    osi3::Orientation3d* osiBMOrient = osiBaseMoving.mutable_orientation();
    *osiBMOrient = setOrientation3D(osiBaseMovingValue.osiOrientValue);

    // osiBaseMoving.mutable_orientation_acceleration();
    // osiBaseMoving.mutable_orientation_rate();
    osi3::Vector3d* osiBMPosition = osiBaseMoving.mutable_position();
    *osiBMPosition = setVector3D(osiBaseMovingValue.osiPosValue);

    osi3::Vector3d* osiBMVel = osiBaseMoving.mutable_velocity();
    *osiBMVel = setVector3D(osiBaseMovingValue.osiVelValue);
    // osiBaseMoving.add_base_polygon();

    return osiBaseMoving;
}

osi3::BaseStationary setBaseStationary(const osiBaseStationaryValue& osiBaseStationaryValue)
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "setBaseStationary";

    osi3::BaseStationary osiBaseStationary{};

    osi3::Dimension3d* osiDRMDim = osiBaseStationary.mutable_dimension();
    *osiDRMDim = setDimension3D(osiBaseStationaryValue.osiDimValue);

    osi3::Orientation3d* osiDRMOrient = osiBaseStationary.mutable_orientation();
    *osiDRMOrient = setOrientation3D(osiBaseStationaryValue.osiOrientValue);

    osi3::Vector3d* osiDRMPos = osiBaseStationary.mutable_position();
    *osiDRMPos = setVector3D(osiBaseStationaryValue.osiPosValue);

    return osiBaseStationary;
}

osi3::DetectedLaneBoundary createDetectedLaneBoundary()
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createDetectedLaneBoundary";

    osi3::DetectedLaneBoundary osiDetectedLaneBoundary{};

    // osiDetectedLaneBoundary->mutable_color_description();

    osi3::DetectedItemHeader* osiDetectedItemHeader = osiDetectedLaneBoundary.mutable_header();
    osiDetectedItemHeaderValue osiDetectedItemHeaderValues;
    osiDetectedItemHeaderValues.osiDIHMeasurement
        = osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED;
    osiDetectedItemHeaderValues.osiExistProb = 0.84;
    osiDetectedItemHeaderValues.osiGroundtruthID = 4;
    osiDetectedItemHeaderValues.osiSensorID = 2;
    osiDetectedItemHeaderValues.osiTrackingID = 3;
    *osiDetectedItemHeader = setDetectedItemHeader(osiDetectedItemHeaderValues);

    osi3::LaneBoundary_BoundaryPoint* osiLBBoundaryPoint = osiDetectedLaneBoundary.add_boundary_line();
    osiVector3DValue value{2, 2, 2};
    osi3::Vector3d* osiLBPoint = osiLBBoundaryPoint->mutable_position();
    *osiLBPoint = setVector3D(value);

    osiLBBoundaryPoint->set_height(0.05);
    osiLBBoundaryPoint->set_width(2.5);

    osiDetectedLaneBoundary.add_boundary_line_confidences(0.78);

    osi3::LaneBoundary_BoundaryPoint* osiLBBoundaryPointRmse = osiDetectedLaneBoundary.add_boundary_line_rmse();
    value = {0, 0, 0};
    osi3::Vector3d* osiLBPointRmse = osiLBBoundaryPointRmse->mutable_position();
    *osiLBPointRmse = setVector3D(value);

    osi3::DetectedLaneBoundary_CandidateLaneBoundary* osiDLBCandidate = osiDetectedLaneBoundary.add_candidate();
    osi3::LaneBoundary_Classification* osiLBClass = osiDLBCandidate->mutable_classification();
    osiLBClass->add_limiting_structure_id();
    osiLBClass->set_color(osi3::LaneBoundary_Classification_Color::LaneBoundary_Classification_Color_COLOR_BLUE);
    osiLBClass->set_type(osi3::LaneBoundary_Classification_Type::LaneBoundary_Classification_Type_TYPE_CURB);
    osiDLBCandidate->set_probability(0.94);

    return osiDetectedLaneBoundary;
}

osi3::DetectedMovingObject createMovingObject()
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createMovingObject";

    osi3::DetectedMovingObject osiMovingObject{};

    osi3::DetectedMovingObject_CandidateMovingObject* osiDMOCandidate = osiMovingObject.add_candidate();
    osiOrientation3DValue osiOrientationValue{0.03, 0.03, 0.03};
    osi3::Orientation3d* osiDMOHeadPose = osiDMOCandidate->mutable_head_pose();
    *osiDMOHeadPose = setOrientation3D(osiOrientationValue);
    osi3::MovingObject_MovingObjectClassification* osiMovingObjClass
        = osiDMOCandidate->mutable_moving_object_classification();
    // osi3::Identifier *osiLaneId = osiMovingObjClass->add_assigned_lane_id();
    osiMovingObjClass->add_assigned_lane_percentage(70);
    // osiMovingObjClass->set_assigned_lane_percentage(0, 70);

    osi3::Orientation3d* osiUppderBodyOrient = osiDMOCandidate->mutable_upper_body_pose();

    osiOrientation3DValue osiOrientationUpperBodyValue{0.5, 0.5, 0.5};
    *osiUppderBodyOrient = setOrientation3D(osiOrientationUpperBodyValue);

    osi3::MovingObject_VehicleClassification* osiMovingObjVehicleClass
        = osiDMOCandidate->mutable_vehicle_classification();

    osi3::MovingObject_VehicleClassification_LightState* osiMOVehicleClassLight
        = osiMovingObjVehicleClass->mutable_light_state();
    osiMOVehicleClassLight->set_brake_light_state(osi3::MovingObject_VehicleClassification_LightState_BrakeLightState::
            MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_NORMAL);
    osiMOVehicleClassLight->set_emergency_vehicle_illumination(
        osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
            MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_FLASHING_AMBER);
    osiMOVehicleClassLight->set_front_fog_light(osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
            MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_FLASHING_BLUE);
    osiMOVehicleClassLight->set_head_light(osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
            MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_FLASHING_BLUE);
    osiMOVehicleClassLight->set_high_beam(osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
            MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_FLASHING_AMBER);
    osiMOVehicleClassLight->set_indicator_state(osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_LEFT);
    osiMOVehicleClassLight->set_license_plate_illumination_rear(
        osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
            MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_FLASHING_AMBER);

    osiMOVehicleClassLight->set_rear_fog_light(osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
            MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OTHER);
    osiMOVehicleClassLight->set_reversing_light(osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
            MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_ON);
    osiMOVehicleClassLight->set_service_vehicle_illumination(
        osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
            MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF);

    osi3::Identifier* osiTrailerId = osiMovingObjVehicleClass->mutable_trailer_id();
    osiTrailerId->set_value(1);

    osiMovingObjVehicleClass->set_has_trailer(true);
    osiMovingObjVehicleClass->set_type(
        osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_COMPACT_CAR);

    osiOrientationValue = {0.4, 0.4, 0.4};
    osi3::Orientation3d* osiHeadPoseOrient = osiDMOCandidate->mutable_head_pose();
    *osiHeadPoseOrient = setOrientation3D(osiOrientationValue);

    osiDMOCandidate->set_probability(0.95);
    osiDMOCandidate->set_type(osi3::MovingObject_Type::MovingObject_Type_TYPE_VEHICLE);

    osiBaseMovingValue osiBMValue;
    osiBMValue.osiAccValue = {3.4, 3.9, 4.4};
    osiBMValue.osiDimValue = {4.0, 4.0, 4.0};
    osiBMValue.osiOrientValue = {0.4, 0.4, 0.4};
    osiBMValue.osiPosValue = {3, 4, 5};
    osiBMValue.osiVelValue = {3, 3, 0};
    osi3::BaseMoving* osiBaseMoving = osiMovingObject.mutable_base();
    *osiBaseMoving = setBaseMoving(osiBMValue);

    osiBaseMovingValue osiBMValueRmse;
    osiBMValueRmse.osiAccValue = {0.1, 0.2, 0.1};
    osiBMValueRmse.osiDimValue = {0.005, 0.002, 0.003};
    osiBMValueRmse.osiOrientValue = {0.4, 0.4, 0.4};
    osiBMValueRmse.osiPosValue = {0.0, 0.0, 0.0};
    osiBMValueRmse.osiVelValue = {0.2, 0.1, 0.05};
    osi3::BaseMoving* osiBaseMovingRmse = osiMovingObject.mutable_base_rmse();
    *osiBaseMovingRmse = setBaseMoving(osiBMValueRmse);

    // osiBaseMovingRmse->add_base_polygon();

    osi3::DetectedItemHeader* osiMODetectedItemHeader = osiMovingObject.mutable_header();
    osiDetectedItemHeaderValue osiDetectedItemHeaderValues;
    osiDetectedItemHeaderValues.osiDIHMeasurement
        = osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED;
    osiDetectedItemHeaderValues.osiExistProb = 0.94;
    osiDetectedItemHeaderValues.osiGroundtruthID = 2;
    osiDetectedItemHeaderValues.osiSensorID = 2;
    osiDetectedItemHeaderValues.osiTrackingID = 4;
    *osiMODetectedItemHeader = setDetectedItemHeader(osiDetectedItemHeaderValues);

    // osiMovingObject->mutable_camera_specifics();
    // osiMovingObject->mutable_color_description();

    // osiMovingObject->mutable_lidar_specifics();
    // osiMovingObject->mutable_radar_specifics();
    // osiMovingObject->mutable_ultrasonic_specifics();
    osiMovingObject.set_movement_state(
        osi3::DetectedMovingObject_MovementState::DetectedMovingObject_MovementState_MOVEMENT_STATE_STATIONARY);
    osiMovingObject.set_percentage_side_lane_left(90);
    osiMovingObject.set_percentage_side_lane_right(10);
    osiMovingObject.set_reference_point(
        osi3::DetectedMovingObject_ReferencePoint::DetectedMovingObject_ReferencePoint_REFERENCE_POINT_CENTER);

    // osi3::DetectedOccupant *osiDetectedOccupant = osiSensorData.add_occupant();
    // osiDetectedOccupant->add_candidate();
    // osiDetectedOccupant->mutable_header();
    return osiMovingObject;
}

osi3::DetectedRoadMarking createRoadMarking()
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createRoadMarking";

    osi3::DetectedRoadMarking osiDetectedRoadMarking{};
    osi3::BaseStationary* osiDRMBase = osiDetectedRoadMarking.mutable_base();

    osiBaseStationaryValue osiBaseStatValue;
    osiBaseStatValue.osiDimValue = {0.02, 2.5, 0.5};
    osiBaseStatValue.osiOrientValue = {0.0, 0.0, 0.0};
    osiBaseStatValue.osiPosValue = {4.5, 3.5, 0};
    *osiDRMBase = setBaseStationary(osiBaseStatValue);

    osi3::BaseStationary* osiDRMBaseRmse = osiDetectedRoadMarking.mutable_base_rmse();
    osiBaseStationaryValue osiBaseStatValueRmse;
    osiBaseStatValueRmse.osiDimValue = {0.02, 0.32, 0.005};
    osiBaseStatValueRmse.osiOrientValue = {0.0, 0.0, 0.0};
    osiBaseStatValueRmse.osiPosValue = {0, 0.4, 0.32};
    *osiDRMBaseRmse = setBaseStationary(osiBaseStatValueRmse);

    // osi3::ColorDescription *osiColorDescription;
    // osi3::ColorRGB *osiRGB;
    // osiRGB->set_blue(0.01);
    // osiRGB->set_green(0.01);
    // osiRGB->set_red(0.01);
    // osiColorDescription->mutable_rgb(osiRGB);

    osi3::DetectedItemHeader* osiDRMDetectedItemHeader = osiDetectedRoadMarking.mutable_header();
    osiDetectedItemHeaderValue osiDetectedItemHeaderValues;
    osiDetectedItemHeaderValues.osiDIHMeasurement
        = osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED;
    osiDetectedItemHeaderValues.osiExistProb = 0.89;
    osiDetectedItemHeaderValues.osiGroundtruthID = 2;
    osiDetectedItemHeaderValues.osiSensorID = 2;
    osiDetectedItemHeaderValues.osiTrackingID = 4;
    *osiDRMDetectedItemHeader = setDetectedItemHeader(osiDetectedItemHeaderValues);

    osi3::DetectedRoadMarking_CandidateRoadMarking* osiDRMCandidate = osiDetectedRoadMarking.add_candidate();

    // osiDetectedRoadMarking->mutable_color_description(osiColorDescription);

    osi3::RoadMarking_Classification* osiRMClass = osiDRMCandidate->mutable_classification();
    // TODO: Other values to be set for class
    osiRMClass->set_type(osi3::RoadMarking_Classification_Type::RoadMarking_Classification_Type_TYPE_GENERIC_LINE);

    osiDRMCandidate->set_probability(0.92);
    return osiDetectedRoadMarking;
}

osi3::DetectedStationaryObject createStationaryObject()
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createStationaryObject";

    osi3::DetectedStationaryObject osiDetectedStationaryObject{};
    osi3::BaseStationary* osiDSOBase = osiDetectedStationaryObject.mutable_base();

    osiBaseStationaryValue osiBaseStatValue;
    osiBaseStatValue.osiDimValue = {0.02, 2.5, 0.5};
    osiBaseStatValue.osiOrientValue = {0.0, 0.0, 0.0};
    osiBaseStatValue.osiPosValue = {4.5, 3.5, 0};
    *osiDSOBase = setBaseStationary(osiBaseStatValue);

    osi3::BaseStationary* osiDSOBaseRmse = osiDetectedStationaryObject.mutable_base_rmse();
    osiBaseStationaryValue osiBaseStatValueRmse;
    osiBaseStatValueRmse.osiDimValue = {0.02, 0.32, 0.005};
    osiBaseStatValueRmse.osiOrientValue = {0.0, 0.0, 0.0};
    osiBaseStatValueRmse.osiPosValue = {0, 0.4, 0.32};
    *osiDSOBaseRmse = setBaseStationary(osiBaseStatValue);

    osi3::DetectedItemHeader* osiDSODetectedItemHeader = osiDetectedStationaryObject.mutable_header();
    osiDetectedItemHeaderValue osiDetectedItemHeaderValues;
    osiDetectedItemHeaderValues.osiDIHMeasurement
        = osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED;
    osiDetectedItemHeaderValues.osiExistProb = 0.89;
    osiDetectedItemHeaderValues.osiGroundtruthID = 2;
    osiDetectedItemHeaderValues.osiSensorID = 2;
    osiDetectedItemHeaderValues.osiTrackingID = 4;
    *osiDSODetectedItemHeader = setDetectedItemHeader(osiDetectedItemHeaderValues);

    osi3::DetectedStationaryObject_CandidateStationaryObject* osiDSOCandidate
        = osiDetectedStationaryObject.add_candidate();

    osi3::StationaryObject_Classification* osiSOClass = osiDSOCandidate->mutable_classification();
    // osiSOClass->add_assigned_lane_id();
    // osiSOClass->add_assigned_lane_percentage();
    // osiSOClass->mutable_emitting_structure_attribute();
    osiSOClass->add_assigned_lane_percentage(91);
    osiSOClass->set_color(osi3::StationaryObject_Classification_Color::StationaryObject_Classification_Color_COLOR_RED);
    osiSOClass->set_density(
        osi3::StationaryObject_Classification_Density::StationaryObject_Classification_Density_DENSITY_LARGE_MESH);
    osiSOClass->set_material(
        osi3::StationaryObject_Classification_Material::StationaryObject_Classification_Material_MATERIAL_METAL);
    osiSOClass->set_type(osi3::StationaryObject_Classification_Type::StationaryObject_Classification_Type_TYPE_BARRIER);

    osiDSOCandidate->set_probability(0.85);

    return osiDetectedStationaryObject;
}

osi3::DetectedTrafficLight createTrafficLight()
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createTrafficLight";

    osi3::DetectedTrafficLight osiDetectedTrafficLight{};

    osi3::BaseStationary* osiDTLBase = osiDetectedTrafficLight.mutable_base();
    osiBaseStationaryValue osiBaseStatValue;
    osiBaseStatValue.osiDimValue = {3.0, 4.0, 5.0};
    osiBaseStatValue.osiOrientValue = {0.4, 0.4, 0.4};
    osiBaseStatValue.osiPosValue = {3, 4, 5};
    *osiDTLBase = setBaseStationary(osiBaseStatValue);

    osi3::BaseStationary* osiDLTBaseRmse = osiDetectedTrafficLight.mutable_base_rmse();

    osiBaseStationaryValue osiBaseStatValueRmse;
    osiBaseStatValueRmse.osiDimValue = {0.005, 0.001, 0.003};
    osiBaseStatValueRmse.osiOrientValue = {0.0, 0.2, 0.1};
    osiBaseStatValueRmse.osiPosValue = {0.1, 0.2, 0.3};
    *osiDLTBaseRmse = setBaseStationary(osiBaseStatValueRmse);

    osi3::DetectedItemHeader* osiDTLDetectedItemHeader = osiDetectedTrafficLight.mutable_header();
    osiDetectedItemHeaderValue osiDetectedItemHeaderValues;
    osiDetectedItemHeaderValues.osiDIHMeasurement
        = osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED;
    osiDetectedItemHeaderValues.osiExistProb = 0.9;
    osiDetectedItemHeaderValues.osiGroundtruthID = 5;
    osiDetectedItemHeaderValues.osiSensorID = 2;
    osiDetectedItemHeaderValues.osiTrackingID = 4;
    *osiDTLDetectedItemHeader = setDetectedItemHeader(osiDetectedItemHeaderValues);

    osi3::DetectedTrafficLight_CandidateTrafficLight* osiDTLCandidate = osiDetectedTrafficLight.add_candidate();

    // osiDetectedTrafficLight->mutable_color_description(osi3::ColorDescription *color_desc);

    osi3::TrafficLight_Classification* osiTLClass = osiDTLCandidate->mutable_classification();
    // osiTLClass->add_assigned_lane_id();

    osiTLClass->set_color(osi3::TrafficLight_Classification_Color::TrafficLight_Classification_Color_COLOR_GREEN);
    osiTLClass->set_counter(20);
    osiTLClass->set_icon(osi3::TrafficLight_Classification_Icon::TrafficLight_Classification_Icon_ICON_ARROW_LEFT);
    osiTLClass->set_is_out_of_service(false);
    osiTLClass->set_mode(osi3::TrafficLight_Classification_Mode::TrafficLight_Classification_Mode_MODE_COUNTING);

    osiDTLCandidate->set_probability(0.88);
    return osiDetectedTrafficLight;
}

osi3::DetectedTrafficSign createTrafficSign()
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createTrafficSign";

    osi3::DetectedTrafficSign osiDetectedTrafficSign{};

    osi3::DetectedItemHeader* osiTSDetectedItemHeader = osiDetectedTrafficSign.mutable_header();
    osiDetectedItemHeaderValue osiDetectedItemHeaderValues;
    osiDetectedItemHeaderValues.osiDIHMeasurement
        = osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED;
    osiDetectedItemHeaderValues.osiExistProb = 0.84;
    osiDetectedItemHeaderValues.osiGroundtruthID = 5;
    osiDetectedItemHeaderValues.osiSensorID = 2;
    osiDetectedItemHeaderValues.osiTrackingID = 3;
    *osiTSDetectedItemHeader = setDetectedItemHeader(osiDetectedItemHeaderValues);

    osi3::DetectedTrafficSign_DetectedMainSign* osiDTMainSign = osiDetectedTrafficSign.mutable_main_sign();

    osi3::BaseStationary* osiBaseStationary = osiDTMainSign->mutable_base();
    // osiBaseStationary->add_base_polygon();
    osiBaseStationaryValue osiBaseStatValue;
    osiBaseStatValue.osiDimValue = {4.0, 4.0, 4.0};
    osiBaseStatValue.osiOrientValue = {0.4, 0.4, 0.4};
    osiBaseStatValue.osiPosValue = {3, 4, 5};
    *osiBaseStationary = setBaseStationary(osiBaseStatValue);

    // osiBaseStationary->add_base_polygon();

    osi3::BaseStationary* osiBaseStationaryRmse = osiDTMainSign->mutable_base_rmse();

    osiBaseStationaryValue osiBaseStatValueRmse;
    osiBaseStatValueRmse.osiDimValue = {0.02, 0.35, 0.09};
    osiBaseStatValueRmse.osiOrientValue = {0.00, 0.00, 0.01};
    osiBaseStatValueRmse.osiPosValue = {0.01, 0.02, 0.00};
    *osiBaseStationaryRmse = setBaseStationary(osiBaseStatValueRmse);

    osi3::DetectedTrafficSign_DetectedMainSign_CandidateMainSign* osiDTSMainSignCandidate
        = osiDTMainSign->add_candidate();

    osiDTMainSign->set_geometry(osi3::DetectedTrafficSign_DetectedMainSign_Geometry::
            DetectedTrafficSign_DetectedMainSign_Geometry_GEOMETRY_ARROW_LEFT);

    osi3::TrafficSign_MainSign_Classification* osiTSMainSignClass = osiDTSMainSignCandidate->mutable_classification();
    // TODO: Other values to be set for class
    osiTSMainSignClass->set_type(
        osi3::TrafficSign_MainSign_Classification_Type::TrafficSign_MainSign_Classification_Type_TYPE_STOP);

    osi3::TrafficSignValue* osiTSMainSignValue = osiTSMainSignClass->mutable_value();
    osiTSMainSignValue->set_value(30);
    osiTSMainSignValue->set_value_unit(osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_KILOMETER_PER_HOUR);

    osiDTSMainSignCandidate->set_probability(0.54);

    osi3::DetectedTrafficSign_DetectedSupplementarySign* osiDTSSupplSign
        = osiDetectedTrafficSign.add_supplementary_sign();

    osi3::DetectedTrafficSign_DetectedSupplementarySign_CandidateSupplementarySign* osiSupplSignCandidate
        = osiDTSSupplSign->add_candidate();
    osi3::TrafficSign_SupplementarySign_Classification* osiSupplSignClass
        = osiSupplSignCandidate->mutable_classification();
    osi3::TrafficSignValue* osiTSValue = osiSupplSignClass->add_value();
    osiSupplSignClass->set_type(osi3::TrafficSign_SupplementarySign_Classification_Type::
            TrafficSign_SupplementarySign_Classification_Type_TYPE_TEXT);
    osiSupplSignClass->set_variability(osi3::TrafficSign_Variability::TrafficSign_Variability_VARIABILITY_FIXED);
    osiTSValue->set_value(35);
    osiTSValue->set_value_unit(osi3::TrafficSignValue_Unit::TrafficSignValue_Unit_UNIT_METER);
    osiSupplSignCandidate->set_probability(0.78);

    osi3::BaseStationary* osiBaseStationaryDTS = osiDTSSupplSign->mutable_base();
    osiBaseStationaryValue osiBaseStatDTSValue;
    osiBaseStatDTSValue.osiDimValue = {2.0, 2.0, 2.0};
    osiBaseStatDTSValue.osiOrientValue = {0.4, 0.4, 0.4};
    osiBaseStatDTSValue.osiPosValue = {3, 4, 5};
    *osiBaseStationaryDTS = setBaseStationary(osiBaseStatDTSValue);

    osi3::BaseStationary* osiBaseStationaryDTSRmse = osiDTSSupplSign->mutable_base_rmse();

    osiBaseStationaryValue osiBaseStatDTSValueRmse;
    osiBaseStatDTSValueRmse.osiDimValue = {0.02, 0.35, 0.09};
    osiBaseStatDTSValueRmse.osiOrientValue = {0.00, 0.00, 0.01};
    osiBaseStatDTSValueRmse.osiPosValue = {0.01, 0.02, 0.00};
    *osiBaseStationaryDTSRmse = setBaseStationary(osiBaseStatDTSValueRmse);

    return osiDetectedTrafficSign;
}

osi3::MountingPosition createMountingPosition(const osiMountingPoseValue& osiMountPoseValue)
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createMountingPosition";

    osi3::MountingPosition osiMountingPos;

    osi3::Orientation3d* osiCamMountPosOrient = osiMountingPos.mutable_orientation();
    *osiCamMountPosOrient = setOrientation3D(osiMountPoseValue.osiMountingOrientation);

    osi3::Vector3d* osiCamMountPosPos = osiMountingPos.mutable_position();
    *osiCamMountPosPos = setVector3D(osiMountPoseValue.osiMountingPosition);

    return osiMountingPos;
}
osi3::CameraDetection createCameraDetection()
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createCameraDetection";
    osi3::CameraDetection osiCameraDetection{};

    osi3::Identifier* osiCDAmbiguityID = osiCameraDetection.mutable_ambiguity_id();
    osiCDAmbiguityID->set_value(1);

    osi3::ColorDescription* osiCDColorDescription = osiCameraDetection.mutable_color_description();
    osi3::ColorRGB* osiCameraDetectionRGB = osiCDColorDescription->mutable_rgb();
    osiCameraDetectionRGB->set_blue(2);
    osiCameraDetectionRGB->set_green(2);
    osiCameraDetectionRGB->set_red(2);

    osi3::Identifier* osiCameraDetectionId = osiCameraDetection.mutable_object_id();
    osiCameraDetectionId->set_value(1);

    // osiCameraDetection->mutable_time_difference();
    osiCameraDetection.set_color(osi3::CameraDetection_Color::CameraDetection_Color_COLOR_BLUE);
    osiCameraDetection.set_color_probability(0.82);
    osiCameraDetection.set_existence_probability(0.94);
    osiCameraDetection.set_first_point_index(0);
    osiCameraDetection.set_image_shape_type(
        osi3::CameraDetection_ImageShapeType::CameraDetection_ImageShapeType_IMAGE_SHAPE_TYPE_BOX);
    osiCameraDetection.set_number_of_points(500);
    osiCameraDetection.set_shape_classification_animal(false);
    osiCameraDetection.set_shape_classification_background(false);
    osiCameraDetection.set_shape_classification_flat(false);
    osiCameraDetection.set_shape_classification_foreground(false);
    osiCameraDetection.set_shape_classification_ground(false);
    osiCameraDetection.set_shape_classification_landmark(false);
    osiCameraDetection.set_shape_classification_moving_object(true);
    osiCameraDetection.set_shape_classification_non_driving_lane(false);
    osiCameraDetection.set_shape_classification_non_road(false);
    osiCameraDetection.set_shape_classification_pedestrian(false);
    osiCameraDetection.set_shape_classification_pedestrian_front(false);
    osiCameraDetection.set_shape_classification_pedestrian_rear(false);
    osiCameraDetection.set_shape_classification_pedestrian_side(false);
    osiCameraDetection.set_shape_classification_probability(0.90);
    osiCameraDetection.set_shape_classification_road(false);
    osiCameraDetection.set_shape_classification_road_marking(false);
    osiCameraDetection.set_shape_classification_sky(false);
    osiCameraDetection.set_shape_classification_stationary_object(false);
    osiCameraDetection.set_shape_classification_traffic_light(false);
    osiCameraDetection.set_shape_classification_traffic_sign(false);
    osiCameraDetection.set_shape_classification_upright(false);
    osiCameraDetection.set_shape_classification_vegetation(false);
    osiCameraDetection.set_shape_classification_vehicle(true);

    // osi3::Timestamp *osiTimestamp;
    // osiTimestamp->set_nanos();
    // osiTimestamp->set_seconds();

    return osiCameraDetection;
}

osi3::SensorDetectionHeader createSensorHeader(const osiSensorHeaderValues& sensorHeaderValues)
{
    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createSensorHeader";

    osi3::SensorDetectionHeader osiSensorDetectionHeader;

    osiMountingPoseValue osiMountPoseValue, osiMountPoseValueRmse;
    osiMountPoseValue.osiMountingOrientation = sensorHeaderValues.osiOrientation;
    osiMountPoseValue.osiMountingPosition = sensorHeaderValues.osiPosition;

    osi3::MountingPosition* osiCameraSensorMountingPos = osiSensorDetectionHeader.mutable_mounting_position();
    *osiCameraSensorMountingPos = createMountingPosition(osiMountPoseValue);

    osiMountPoseValueRmse.osiMountingOrientation = sensorHeaderValues.osiOrientationRmse;
    osiMountPoseValueRmse.osiMountingPosition = sensorHeaderValues.osiPositionRmse;

    osi3::MountingPosition* osiCameraSensorMountingPosRmse = osiSensorDetectionHeader.mutable_mounting_position_rmse();
    *osiCameraSensorMountingPosRmse = createMountingPosition(osiMountPoseValueRmse);

    osi3::Identifier* osiCameraSensorID = osiSensorDetectionHeader.mutable_sensor_id();
    osiCameraSensorID->set_value(sensorHeaderValues.osiSensorId);

    // osiSensorDetectionHeader.mutable_measurement_time();

    osiSensorDetectionHeader.set_cycle_counter(sensorHeaderValues.osiCycleCounter);
    osiSensorDetectionHeader.set_data_qualifier(sensorHeaderValues.osiSDataQualifier);
    // osiSensorDetectionHeader.set_extended_qualifier();
    osiSensorDetectionHeader.set_number_of_valid_detections(sensorHeaderValues.numDetections);
    return osiSensorDetectionHeader;
}

osi3::DetectedEntityHeader createDetectedEntityHeader(const osiDetectedEntityHeaderValue& osiDetectedEntityHeaderValue)
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createDetectedEntityHeader";

    osi3::DetectedEntityHeader osiDetectedEntityHeader;
    // osiDetectedEntityHeader.mutable_measurement_time();
    osiDetectedEntityHeader.set_cycle_counter(osiDetectedEntityHeaderValue.osiCycleCounter);
    osiDetectedEntityHeader.set_data_qualifier(osiDetectedEntityHeaderValue.osiDataQualifier);

    return osiDetectedEntityHeader;
}

osi3::InterfaceVersion createInterfaceVersion(const osiVersionValue& osiInterfaceVer)
{
    auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    m_logger_dummy_data.LogInfo() << "createInterfaceVersion";
    osi3::InterfaceVersion osiInterfaceVersion;
    osiInterfaceVersion.set_version_major(osiInterfaceVer.major);
    osiInterfaceVersion.set_version_minor(osiInterfaceVer.minor);
    osiInterfaceVersion.set_version_patch(osiInterfaceVer.patch);

    return osiInterfaceVersion;
}

osi3::LogicalDetection createLogicalDetection()
{

    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createLogicalDetection";

    osi3::LogicalDetection osiLogicalDetection{};

    osi3::Identifier* osiLDIdentifier = osiLogicalDetection.add_sensor_id();
    osiLDIdentifier->set_value(3);

    osi3::Identifier* osiLDObjectID = osiLogicalDetection.mutable_object_id();
    osiLDObjectID->set_value(0);

    osiLogicalDetection.set_existence_probability(94.23);

    osi3::Vector3d* osiLDPos = osiLogicalDetection.mutable_position();
    osiVector3DValue osiLDPosValue{25.95, 89.34, 0};
    *osiLDPos = setVector3D(osiLDPosValue);

    osi3::Vector3d* osiLDPosRmse = osiLogicalDetection.mutable_position_rmse();
    osiVector3DValue osiLDPosValueRmse{0.9, 0.8, 0.7};
    *osiLDPosRmse = setVector3D(osiLDPosValueRmse);

    // osiLogicalDetection.mutable_unknown_fields();
    osi3::Vector3d* osiLDVel = osiLogicalDetection.mutable_velocity();
    osiVector3DValue osiLDVelValue{19, 2, 0};
    *osiLDVel = setVector3D(osiLDVelValue);

    osi3::Vector3d* osiLDVelRmse = osiLogicalDetection.mutable_velocity_rmse();
    osiVector3DValue osiLDVelValueRmse{0, 0, 0};
    *osiLDVelRmse = setVector3D(osiLDVelValueRmse);

    return osiLogicalDetection;
}

osi3::SensorData createOsiDummyData()
{
    // auto& m_logger_dummy_data = ara::log::CreateLogger("DFLT", "Default logger", ara::log::LogLevel::kVerbose);
    // m_logger_dummy_data.LogInfo() << "createOsiDummyData";

    osi3::SensorData osiSensorData{};

    osi3::DetectedLane* osiDetectedLane = osiSensorData.add_lane();
    *osiDetectedLane = createDetectedLaneData();

    osi3::DetectedLaneBoundary* osiDetectedLaneBoundary = osiSensorData.add_lane_boundary();
    *osiDetectedLaneBoundary = createDetectedLaneBoundary();

    osi3::DetectedMovingObject* osiMovingObject = osiSensorData.add_moving_object();
    *osiMovingObject = createMovingObject();

    osi3::DetectedRoadMarking* osiDetectedRoadMarking = osiSensorData.add_road_marking();
    *osiDetectedRoadMarking = createRoadMarking();

    osi3::DetectedStationaryObject* osiDetectedStationaryObject = osiSensorData.add_stationary_object();
    *osiDetectedStationaryObject = createStationaryObject();

    osi3::DetectedTrafficLight* osiDetectedTrafficLight = osiSensorData.add_traffic_light();
    *osiDetectedTrafficLight = createTrafficLight();

    osi3::DetectedTrafficSign* osiDetectedTrafficSign = osiSensorData.add_traffic_sign();
    *osiDetectedTrafficSign = createTrafficSign();

    osi3::FeatureData* osiFeatureData = osiSensorData.mutable_feature_data();
    osi3::CameraDetectionData* osiCameraDetectionData = osiFeatureData->add_camera_sensor();
    osi3::LidarDetectionData* osiLidarDetectionData = osiFeatureData->add_lidar_sensor();
    osi3::RadarDetectionData* osiRadarDetectionData = osiFeatureData->add_radar_sensor();
    osi3::UltrasonicDetectionData* osiUltrasonicDetectionData = osiFeatureData->add_ultrasonic_sensor();
    osi3::CameraDetection* osiCameraDetection = osiCameraDetectionData->add_detection();
    *osiCameraDetection = createCameraDetection();

    osi3::InterfaceVersion* osiCameraInterfaceVer = osiFeatureData->mutable_version();
    osiVersionValue osiCamInterfaceVersionValue{1, 0, 0};
    *osiCameraInterfaceVer = createInterfaceVersion(osiCamInterfaceVersionValue);

    osi3::SensorDetectionHeader* osiCameraSensorDetectionHeader = osiCameraDetectionData->mutable_header();
    osiSensorHeaderValues camSensorHeaderValues;
    camSensorHeaderValues.osiOrientation = {0.9, 3.66, 2.5};
    camSensorHeaderValues.osiOrientationRmse = {0.009, 0.001, 0.001};
    camSensorHeaderValues.osiPosition = {5.5, 4.4, 3.5};
    camSensorHeaderValues.osiPositionRmse = {0.001, 0.001, 0.002};
    camSensorHeaderValues.osiSensorId = 2;
    camSensorHeaderValues.osiCycleCounter = 25;
    camSensorHeaderValues.osiSDataQualifier
        = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
    camSensorHeaderValues.numDetections = 3;
    *osiCameraSensorDetectionHeader = createSensorHeader(camSensorHeaderValues);

    osi3::CameraPoint* osiCameraPoint = osiCameraDetectionData->add_point();

    // osiCameraDetectionData->mutable_specific_header();

    osiSpherical3DValue osiSpherical3d = {9.4, 12.84, 3.2};
    osiSpherical3DValue osiSpherical3dRmse = {0.005, 0.001, 0.003};

    osi3::Spherical3d* osiCamPoint = osiCameraPoint->mutable_point();
    *osiCamPoint = setSpherical3D(osiSpherical3d);

    osi3::Spherical3d* osiCamPointRmse = osiCameraPoint->mutable_point_rmse();
    *osiCamPointRmse = setSpherical3D(osiSpherical3dRmse);

    // m_logger_dummy_data.LogInfo() << "setSpherical3D done";
    osiCameraPoint->set_existence_probability(0.86);

    osi3::SensorDetectionHeader* osiLidarSensorDetectionHeader = osiLidarDetectionData->mutable_header();
    osiSensorHeaderValues lidarSensorHeaderValues;
    lidarSensorHeaderValues.osiOrientation = {0.9, 3.59, 2.5};
    lidarSensorHeaderValues.osiOrientationRmse = {0.009, 0.002, 0.011};
    lidarSensorHeaderValues.osiPosition = {5.5, 4.4, 3.5};
    lidarSensorHeaderValues.osiPositionRmse = {0.003, 0.003, 0.006};
    lidarSensorHeaderValues.osiSensorId = 4;
    lidarSensorHeaderValues.osiCycleCounter = 40;
    lidarSensorHeaderValues.osiSDataQualifier
        = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
    lidarSensorHeaderValues.numDetections = 5;
    *osiLidarSensorDetectionHeader = createSensorHeader(lidarSensorHeaderValues);
    // m_logger_dummy_data.LogInfo() << "lidarSensorHeaderValues done";

    osi3::LidarDetection* osiLidarDetection = osiLidarDetectionData->add_detection();

    osi3::Identifier* osiLidarObjectId = osiLidarDetection->mutable_object_id();
    osiLidarObjectId->set_value(1);

    osiSpherical3d = {3.5, 10.35, 0.5};
    osiSpherical3dRmse = {0.0045, 0.029, 0.0065};

    osi3::Spherical3d* osiLidarDetectionPos = osiLidarDetection->mutable_position();
    *osiLidarDetectionPos = setSpherical3D(osiSpherical3d);

    osi3::Spherical3d* osiLidarDetectionPosRmse = osiLidarDetection->mutable_position_rmse();
    *osiLidarDetectionPosRmse = setSpherical3D(osiSpherical3dRmse);

    osiLidarDetection->set_classification(osi3::DetectionClassification::DETECTION_CLASSIFICATION_OVERDRIVABLE);
    osiLidarDetection->set_echo_pulse_width(1.53);
    osiLidarDetection->set_existence_probability(0.912);
    osiLidarDetection->set_free_space_probability(0.54);
    osiLidarDetection->set_height(3.25);
    osiLidarDetection->set_height_rmse(0.023);
    osiLidarDetection->set_intensity(4.523);
    osiLidarDetection->set_reflectivity(1.45);

    // m_logger_dummy_data.LogInfo() << "osiLidarDetection done";

    osi3::SensorDetectionHeader* osiRadarSensorDetectionHeader = osiRadarDetectionData->mutable_header();
    osiSensorHeaderValues radarSensorHeaderValues;
    radarSensorHeaderValues.osiOrientation = {0.9, 3.566, 2.5};
    radarSensorHeaderValues.osiOrientationRmse = {0.9, 0.001, 0.001};
    radarSensorHeaderValues.osiPosition = {5.5, 4.4, 3.5};
    radarSensorHeaderValues.osiPositionRmse = {0.001, 0.001, 0.002};
    radarSensorHeaderValues.osiSensorId = 3;
    radarSensorHeaderValues.osiCycleCounter = 30;
    radarSensorHeaderValues.osiSDataQualifier
        = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
    radarSensorHeaderValues.numDetections = 5;
    *osiRadarSensorDetectionHeader = createSensorHeader(radarSensorHeaderValues);

    // m_logger_dummy_data.LogInfo() << "radarSensorHeaderValues done";

    osi3::RadarDetection* osiRadarDetection = osiRadarDetectionData->add_detection();

    osi3::Identifier* osiRDAmbiguityID = osiRadarDetection->mutable_ambiguity_id();
    osiRDAmbiguityID->set_value(3);

    // osi3::ColorDescription *osiRDColorDescription;
    // osi3::ColorRGB *osiRadarDetectionRGB = osiRDColorDescription->mutable_rgb();
    // osiRadarDetectionRGB->set_blue(43);
    // osiRadarDetectionRGB->set_green(44);
    // osiRadarDetectionRGB->set_red(45);

    osi3::Identifier* osiRadarDetectionId = osiRadarDetection->mutable_object_id();
    osiRadarDetectionId->set_value(3);

    osiSpherical3d = {4.56, 10.45, 0.5};

    osiSpherical3dRmse = {0.064, 0.24, 0.09};

    osi3::Spherical3d* osiRadarDetectionPos = osiRadarDetection->mutable_position();
    *osiRadarDetectionPos = setSpherical3D(osiSpherical3d);

    osi3::Spherical3d* osiRadarDetectionPosRmse = osiRadarDetection->mutable_position_rmse();
    *osiRadarDetectionPosRmse = setSpherical3D(osiSpherical3dRmse);

    osiRadarDetection->set_classification(osi3::DetectionClassification::DETECTION_CLASSIFICATION_UNKNOWN);
    osiRadarDetection->set_existence_probability(0.95);
    // osiRadarDetection->set_point_target_probability();
    osiRadarDetection->set_radial_velocity(10);
    osiRadarDetection->set_radial_velocity_rmse(0.001);
    osiRadarDetection->set_rcs(7);
    osiRadarDetection->set_snr(1);

    // m_logger_dummy_data.LogInfo() << "osiRadarDetection done";

    osi3::SensorDetectionHeader* osiUltrasonicSensorDetectionHeader = osiUltrasonicDetectionData->mutable_header();
    osiSensorHeaderValues ultrasonicSensorHeaderValues;
    ultrasonicSensorHeaderValues.osiOrientation = {0.9, 3.566, 2.5};
    ultrasonicSensorHeaderValues.osiOrientationRmse = {0.001, 0.00, 0.001};
    ultrasonicSensorHeaderValues.osiPosition = {5.5, 4.4, 3.5};
    ultrasonicSensorHeaderValues.osiPositionRmse = {0.001, 0.001, 0.002};
    ultrasonicSensorHeaderValues.osiSensorId = 4;
    ultrasonicSensorHeaderValues.osiCycleCounter = 27;
    ultrasonicSensorHeaderValues.osiSDataQualifier
        = osi3::SensorDetectionHeader_DataQualifier::SensorDetectionHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
    ultrasonicSensorHeaderValues.numDetections = 2;
    *osiUltrasonicSensorDetectionHeader = createSensorHeader(ultrasonicSensorHeaderValues);

    // m_logger_dummy_data.LogInfo() << "ultrasonicSensorHeaderValues done";

    osi3::UltrasonicDetection* osiUltrasonicDetection = osiUltrasonicDetectionData->add_detection();
    // osiUltrasonicDetectionData->add_indirect_detection();

    // osiUltrasonicDetectionData->mutable_specific_header();

    osi3::Identifier* osiUltrasonicId = osiUltrasonicDetection->mutable_object_id();
    osiUltrasonicId->set_value(5);

    osiUltrasonicDetection->set_distance(35.4);
    osiUltrasonicDetection->set_existence_probability(0.923);

    osi3::BaseMoving* osiHostVehicleLocation = osiSensorData.mutable_host_vehicle_location();
    osiBaseMovingValue osiBMValue;
    osiBMValue.osiAccValue = {10, 0, 0};
    osiBMValue.osiDimValue = {2, 4, 3};
    osiBMValue.osiOrientValue = {0.0, 0.0, 3.45};
    osiBMValue.osiPosValue = {67.89, 5.35, 0};
    osiBMValue.osiVelValue = {67.89, 5.35, 0};
    *osiHostVehicleLocation = setBaseMoving(osiBMValue);

    // m_logger_dummy_data.LogInfo() << "osiBMValue done";

    osi3::BaseMoving* osiHostVehicleLocationRmse = osiSensorData.mutable_host_vehicle_location_rmse();
    osiBaseMovingValue osiBMValueRmse;
    osiBMValueRmse.osiAccValue = {10, 0, 0};
    osiBMValueRmse.osiDimValue = {2, 4, 3};
    osiBMValueRmse.osiOrientValue = {0.0, 0.0, 3.45};
    osiBMValueRmse.osiPosValue = {67.89, 5.35, 0};
    osiBMValueRmse.osiVelValue = {67.89, 5.35, 0};
    *osiHostVehicleLocationRmse = setBaseMoving(osiBMValueRmse);

    // m_logger_dummy_data.LogInfo() << "ultrasonicSensorHeaderValues done";

    osi3::DetectedEntityHeader* osiLaneBoundaryHeader = osiSensorData.mutable_lane_boundary_header();
    osi3::DetectedEntityHeader* osiLaneHeader = osiSensorData.mutable_lane_header();
    // osiSensorData.mutable_last_measurement_time();
    osi3::LogicalDetectionData* osiLogicalDetectionData = osiSensorData.mutable_logical_detection_data();

    osi3::MountingPosition* osiSensorMountingPos = osiSensorData.mutable_mounting_position();

    osi3::MountingPosition* osiSensorMountingPosRmse = osiSensorData.mutable_mounting_position_rmse();
    osi3::DetectedEntityHeader* osiMObjectHeader = osiSensorData.mutable_moving_object_header();
    osi3::DetectedEntityHeader* osiOccupantHeader = osiSensorData.mutable_occupant_header();
    osi3::DetectedEntityHeader* osiRoadMarkingHeader = osiSensorData.mutable_road_marking_header();
    osi3::Identifier* osiSensorID = osiSensorData.mutable_sensor_id();
    osi3::DetectedEntityHeader* osiStatObjHeader = osiSensorData.mutable_stationary_object_header();
    // osiSensorData.mutable_timestamp();
    osi3::DetectedEntityHeader* osiTrafficLightHeader = osiSensorData.mutable_traffic_light_header();
    osi3::DetectedEntityHeader* osiTrafficSignHeader = osiSensorData.mutable_traffic_sign_header();
    osi3::InterfaceVersion* osiSensorVer = osiSensorData.mutable_version();

    // m_logger_dummy_data.LogInfo() << "ultrasonicSensorHeaderValues done";

    osiDetectedEntityHeaderValue osiLaneBoundaryValue;
    osiLaneBoundaryValue.osiCycleCounter = 23;
    osiLaneBoundaryValue.osiDataQualifier
        = osi3::DetectedEntityHeader_DataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
    *osiLaneBoundaryHeader = createDetectedEntityHeader(osiLaneBoundaryValue);

    osiDetectedEntityHeaderValue osiLaneHeaderValue;
    osiLaneHeaderValue.osiCycleCounter = 24;
    osiLaneHeaderValue.osiDataQualifier
        = osi3::DetectedEntityHeader_DataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED;
    *osiLaneHeader = createDetectedEntityHeader(osiLaneHeaderValue);

    osi3::LogicalDetection* osiLogicalDetection = osiLogicalDetectionData->add_logical_detection();
    *osiLogicalDetection = createLogicalDetection();

    osi3::LogicalDetectionDataHeader* osiLDHeader = osiLogicalDetectionData->mutable_header();
    osi3::Identifier* osiLDSensorID = osiLDHeader->add_sensor_id();
    osiLDSensorID->set_value(3);

    // osiLDHeader->mutable_logical_detection_time();
    osiLDHeader->set_data_qualifier(osi3::LogicalDetectionDataHeader_DataQualifier::
            LogicalDetectionDataHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE);
    osiLDHeader->set_number_of_valid_logical_detections(4);

    // osiLogicalDetectionData->mutable_unknown_fields();
    osi3::InterfaceVersion* osiLogicalDetectionVer = osiLogicalDetectionData->mutable_version();
    osiVersionValue osiLDVer{1, 1, 0};
    *osiLogicalDetectionVer = createInterfaceVersion(osiLDVer);

    // m_logger_dummy_data.LogInfo() << "osiLDHeader done";

    osiMountingPoseValue osiSensorMPValue;
    osiSensorMPValue.osiMountingOrientation = {2.0, 3.0, 0.0};
    osiSensorMPValue.osiMountingPosition = {2.0, 2.0, 0.0};
    *osiSensorMountingPos = createMountingPosition(osiSensorMPValue);

    osiMountingPoseValue osiSensorMPValueRmse;
    osiSensorMPValueRmse.osiMountingOrientation = {0.02, 0.01, 0.0};
    osiSensorMPValueRmse.osiMountingPosition = {0.02, 0.01, 0.0};
    *osiSensorMountingPosRmse = createMountingPosition(osiSensorMPValueRmse);

    // m_logger_dummy_data.LogInfo() << "osiSensorMPValue done";

    osiDetectedEntityHeaderValue osiMObjectHeaderValue;
    osiMObjectHeaderValue.osiCycleCounter = 25;
    osiMObjectHeaderValue.osiDataQualifier
        = osi3::DetectedEntityHeader_DataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED;
    *osiMObjectHeader = createDetectedEntityHeader(osiMObjectHeaderValue);

    osiDetectedEntityHeaderValue osiOccupantHeaderValue;
    osiOccupantHeaderValue.osiCycleCounter = 26;
    osiOccupantHeaderValue.osiDataQualifier
        = osi3::DetectedEntityHeader_DataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE;
    *osiOccupantHeader = createDetectedEntityHeader(osiOccupantHeaderValue);

    osiDetectedEntityHeaderValue osiRoadObjHeaderValue;
    osiRoadObjHeaderValue.osiCycleCounter = 30;
    osiRoadObjHeaderValue.osiDataQualifier = osi3::DetectedEntityHeader_DataQualifier::
        DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_TEMPORARY_AVAILABLE;
    *osiRoadMarkingHeader = createDetectedEntityHeader(osiRoadObjHeaderValue);

    osiSensorID->set_value(1);

    // m_logger_dummy_data.LogInfo() << "osiRoadObjHeaderValue done";

    osiDetectedEntityHeaderValue osiStatObjHeaderValue;
    osiStatObjHeaderValue.osiCycleCounter = 27;
    osiStatObjHeaderValue.osiDataQualifier = osi3::DetectedEntityHeader_DataQualifier::
        DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_TEMPORARY_AVAILABLE;
    *osiStatObjHeader = createDetectedEntityHeader(osiStatObjHeaderValue);

    osiDetectedEntityHeaderValue osiTrafficLightHeaderValue;
    osiTrafficLightHeaderValue.osiCycleCounter = 28;
    osiTrafficLightHeaderValue.osiDataQualifier
        = osi3::DetectedEntityHeader_DataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
    *osiTrafficLightHeader = createDetectedEntityHeader(osiTrafficLightHeaderValue);

    osiDetectedEntityHeaderValue osiTrafficSignHeaderValue;
    osiTrafficSignHeaderValue.osiCycleCounter = 29;
    osiTrafficSignHeaderValue.osiDataQualifier
        = osi3::DetectedEntityHeader_DataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
    *osiTrafficSignHeader = createDetectedEntityHeader(osiTrafficSignHeaderValue);

    osiVersionValue osiInterfaceVersionValue{1, 1, 0};
    *osiSensorVer = createInterfaceVersion(osiInterfaceVersionValue);

    // m_logger_dummy_data.LogInfo() << "osiSensorData done";

    return osiSensorData;
}
