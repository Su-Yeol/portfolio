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

#include "ara/adi/sensoritf/impl_type_potentiallymovingobjects.h"

#include "osi3/osi_object.pb.h"

#include "osiMappers/MovingObjectMapper.h"

std::map<adiMOType, osiMOType> MovingObjectMapper::ClassificationTypeMap{
    {adiMOType::kAnimal, osiMOType::MovingObject_Type_TYPE_ANIMAL},
    {adiMOType::kPedestrian, osiMOType::MovingObject_Type_TYPE_PEDESTRIAN},
    {adiMOType::kSemitrailer, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kTrailer, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kWheelchair, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kBicycle, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kBus, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kCar, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kHeavyTruck, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kMotorbike, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kOtherVehicle, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kTrain, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kTram, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kTricycle, osiMOType::MovingObject_Type_TYPE_OTHER},
    {adiMOType::kVan, osiMOType::MovingObject_Type_TYPE_VEHICLE},
    {adiMOType::kUnknown, osiMOType::MovingObject_Type_TYPE_UNKNOWN}};

std::map<adiMOType, osiMOVehicleType> MovingObjectMapper::VehicleClassificationMap{
    {adiMOType::kSemitrailer, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_SEMITRAILER},
    {adiMOType::kTrailer, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_TRAILER},
    {adiMOType::kWheelchair, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_WHEELCHAIR},
    {adiMOType::kBicycle, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_BICYCLE},
    {adiMOType::kBus, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_BUS},
    {adiMOType::kCar, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_COMPACT_CAR},
    {adiMOType::kHeavyTruck, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_HEAVY_TRUCK},
    {adiMOType::kMotorbike, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_MOTORBIKE},
    {adiMOType::kOtherVehicle, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_OTHER},
    {adiMOType::kTrain, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_TRAIN},
    {adiMOType::kTram, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_TRAM},
    {adiMOType::kVan, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_DELIVERY_VAN},
    {adiMOType::kUnknown, osiMOVehicleType::MovingObject_VehicleClassification_Type_TYPE_UNKNOWN}};

osi3::MovingObject MovingObjectMapper::mapMovingObject(
    const ::ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const
{
    // Mapping only first index of moving object for mow
    size_t index = 0;

    osi3::MovingObject osiMovingObject{};
    // TODO: Identifier Mapping

    // BaseMoving mapping
    osi3::BaseMoving* osiBaseMoving{osiMovingObject.mutable_base()};
    *osiBaseMoving = mapBaseMoving(potentiallyMovingObject);

    // Type Mapping
    size_t classIndex = 0;
    auto PMOClassifications{
        potentiallyMovingObject.PotentiallyMovingObjectsInformation.PotentiallyMovingObjectClassifications};
    auto PMOClass{PMOClassifications.at(classIndex)};

    osi3::MovingObject_Type osiMovingObjectType = mapType(PMOClass);
    osiMovingObject.set_type(osiMovingObjectType);

    // TODO(check mapping possibility): assigned_lane_id Mapping
    // TODO(check mapping possibility): VehicleAttributes

    // VehicleClassification
    osi3::MovingObject_VehicleClassification* osiMOVehicleClassification{
        osiMovingObject.mutable_vehicle_classification()};
    *osiMOVehicleClassification = mapVehicleClassification(potentiallyMovingObject, index);

    // TODO(check mapping possibility): model_reference
    // TODO(check mapping possibility): StatePoint

    // TODO:MovingObjectClassification
    // adi does not contain lane ids. Hence other lane related info cannot be mapped

    // osi3::MovingObject_MovingObjectClassification osiMOClassification = mapClassification(
    //    potentiallyMovingObjects, index);
    // osiMovingObject.set_allocated_moving_object_classification();

    // TODO(check mapping possibility):ExternalReference
    // TODO(check mapping possibility):ColorDescription
    return osiMovingObject;
}

osi3::BaseMoving MovingObjectMapper::mapBaseMoving(
    const ::ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const
{
    m_logger.LogInfo() << "mapBaseMoving";
    osi3::BaseMoving osiBaseMoving{};

    osi3::Dimension3d* dimension{osiBaseMoving.mutable_dimension()};
    *dimension = mapDimension3D2Osi(potentiallyMovingObject.PotentiallyMovingObjectsBoundingBox.BoundingBoxExtent);

    osi3::Vector3d* position{osiBaseMoving.mutable_position()};
    *position = mapPoint3D2Osi(potentiallyMovingObject.PotentiallyMovingObjectsPosition.PositionObjectLevel);

    osi3::Orientation3d* orientation{osiBaseMoving.mutable_orientation()};
    *orientation = mapOrientation3D2Osi(potentiallyMovingObject.PotentiallyMovingObjectsPosition.Orientation);

    return osiBaseMoving;
}

osi3::MovingObject_Type MovingObjectMapper::mapType(
    const ::ara::adi::sensoritf::ValidPotentiallyMovingObjectClassification& PMOClass) const
{
    osi3::MovingObject_Type osiMovingObjType{osi3::MovingObject_Type::MovingObject_Type_TYPE_UNKNOWN};

    ara::adi::sensoritf::PotentiallyMovingObjectClassificationType PMOClassificationType{
        PMOClass.PotentiallyMovingObjectClassificationType};

    // TODO: For now, mapped different types of adi::vehicles to osi:: TYPE_VEHICLE

    if (ClassificationTypeMap.find(PMOClassificationType) != ClassificationTypeMap.end()) {

        osiMovingObjType = ClassificationTypeMap[PMOClassificationType];
    } else {
        m_logger.LogError() << "Error: mapType: Unknown enum-entry: "
                            << static_cast<std::uint8_t>(PMOClassificationType);
    }

    return osiMovingObjType;
}

// Unavailable data
// osi3::MovingObject_MovingObjectClassification MovingObjectMapper::
//    mapClassification(
//    const ::ara::adi::sensoritf::PotentiallyMovingObjects&
//    potentiallyMovingObjects, const size_t& index) const
//{
//    // adi does not contain lane ids. Hence other lane related info cannot be mapped
//
//    osi3::MovingObject_MovingObjectClassification osiMOClassification{};
//    // osiMOClassification.add_assigned_lane_id();
//    //osiMOClassification.add_assigned_lane_percentage();
//    // osiMOClassification.add_logical_lane_assignment();
//    // osiMOClassification.set_assigned_lane_percentage();
//
//    return osiMOClassification;
//}

osi3::MovingObject_VehicleClassification MovingObjectMapper::mapVehicleClassification(
    const ::ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject,
    const size_t& index) const
{
    m_logger.LogInfo() << "mapVehicleClassification";
    osi3::MovingObject_VehicleClassification osiMOVehicleClassification{};

    auto numPMOValidClasses{
        potentiallyMovingObject.PotentiallyMovingObjectsInformation.PotentiallyMovingObjectClassifications.size()};

    if (numPMOValidClasses == 0) {
        return {};
    }

    ara::adi::sensoritf::ValidPotentiallyMovingObjectClassification PMOValidClassification{
        potentiallyMovingObject.PotentiallyMovingObjectsInformation.PotentiallyMovingObjectClassifications.at(index)};

    // Vehicle Classification Type Mapping
    osi3::MovingObject_VehicleClassification_Type osiMOVehicleClassificationType
        = mapVehicleClassificationType(PMOValidClassification);
    osiMOVehicleClassification.set_type(osiMOVehicleClassificationType);

    // LightState
    osi3::MovingObject_VehicleClassification_LightState* osiMOVehicleClassLightState{
        osiMOVehicleClassification.mutable_light_state()};
    auto numPMOLights{potentiallyMovingObject.PotentiallyMovingObjectsLights.PotentiallyMovingObjectsLightList.size()};

    if (numPMOLights == 0) {
        return {};
    }

    if (numPMOLights != numPMOValidClasses) {
        m_logger.LogError() << "Mismatch between no. of Moving Object lights"
                               "and no. of Valid Classifications, both expected to be same";
        return {};
    }

    auto PMOLight{potentiallyMovingObject.PotentiallyMovingObjectsLights.PotentiallyMovingObjectsLightList.at(index)};

    *osiMOVehicleClassLightState = mapVehicleClassificationLightState(PMOLight);

    // Trailer: Not sure how to map trailer vehicle

    // trailer_id

    // Role

    return osiMOVehicleClassification;
}

osi3::MovingObject_VehicleClassification_Type MovingObjectMapper::mapVehicleClassificationType(
    const ara::adi::sensoritf::ValidPotentiallyMovingObjectClassification& validPotentiallyMovingObjectsClass) const
{
    m_logger.LogInfo() << "mapVehicleClassificationType";
    osi3::MovingObject_VehicleClassification_Type osiMOVehicleClassificationType{
        osi3::MovingObject_VehicleClassification_Type::MovingObject_VehicleClassification_Type_TYPE_UNKNOWN};
    ara::adi::sensoritf::PotentiallyMovingObjectClassificationType PMOClassType{
        validPotentiallyMovingObjectsClass.PotentiallyMovingObjectClassificationType};

    if (VehicleClassificationMap.find(PMOClassType) != VehicleClassificationMap.end()) {

        osiMOVehicleClassificationType = VehicleClassificationMap[PMOClassType];
    } else {
        m_logger.LogError() << "Error: mapType: Unknown enum-entry: " << static_cast<std::uint8_t>(PMOClassType);
    }

    return osiMOVehicleClassificationType;
}

osi3::MovingObject_VehicleClassification_LightState MovingObjectMapper::mapVehicleClassificationLightState(
    const ara::adi::sensoritf::PotentiallyMovingObjectsLight& PMOLight) const
{
    m_logger.LogInfo() << "mapVehicleClassificationLightState";
    osi3::MovingObject_VehicleClassification_LightState osiMOVehicleClassificationLightState{};
    osi3::MovingObject_VehicleClassification_LightState_IndicatorState osiMOVehicleClassificationIndLightState{};
    // osi3::MovingObject_VehicleClassification_LightState_GenericLightState
    //    osiMOVehicleClassificationGenLightState{};
    osi3::MovingObject_VehicleClassification_LightState_BrakeLightState osiMOVehicleBrakeLightState{};

    ara::adi::sensoritf::LightType lightType{};
    ara::adi::sensoritf::LightStatus lightStatus{};

    lightType = PMOLight.PMOLightType;
    lightStatus = PMOLight.PMOLightStatus;
    switch (lightType) {
    case (ara::adi::sensoritf::LightType::kCentreBrakeLight):
    case (ara::adi::sensoritf::LightType::kLeftBrakeLight):
    case (ara::adi::sensoritf::LightType::kRightBrakeLight): {
        osiMOVehicleBrakeLightState = mapBrakeLightState(lightStatus);
        break;
    }
    case (ara::adi::sensoritf::LightType::kLeftFlashLight): {
        osiMOVehicleClassificationIndLightState
            = mapIndicatorLightState(lightStatus, ara::adi::sensoritf::LightType::kLeftFlashLight);
        break;
    }
    case (ara::adi::sensoritf::LightType::kRightFlashLight): {
        osiMOVehicleClassificationIndLightState
            = mapIndicatorLightState(lightStatus, ara::adi::sensoritf::LightType::kRightFlashLight);
        break;
    }
    case (ara::adi::sensoritf::LightType::kHazardFlashLight): {
        osiMOVehicleClassificationIndLightState
            = mapIndicatorLightState(lightStatus, ara::adi::sensoritf::LightType::kHazardFlashLight);
        break;
    }

    // Generic Light mapping unavailable

    // case (ara::adi::sensoritf::LightType::kCentreOtherLight):
    // case (ara::adi::sensoritf::LightType::kLeftOtherLight):
    // case (ara::adi::sensoritf::LightType::kRightOtherLight):
    //{
    //    osiMOVehicleClassificationGenLightState = mapGenericLightState(lightStatus);
    //    break;
    //}
    default: {
        m_logger.LogError() << "Error: mapBrakeLightType: Unknown enum-entry: " << static_cast<std::uint8_t>(lightType);
        break;
    }
    }

    // Assumed that there are unique light types in
    // ara::adi::sensoritf::PotentiallyMovingObjectsLightVector

    osiMOVehicleClassificationLightState.set_brake_light_state(osiMOVehicleBrakeLightState);

    // osiMOVehicleClassificationLightState.set_emergency_vehicle_illumination();

    // osiMOVehicleClassificationLightState.set_front_fog_light();

    // osiMOVehicleClassificationLightState.set_head_light();

    // osiMOVehicleClassificationLightState.set_high_beam();

    osiMOVehicleClassificationLightState.set_indicator_state(osiMOVehicleClassificationIndLightState);

    // osiMOVehicleClassificationLightState.set_license_plate_illumination_rear();

    // osiMOVehicleClassificationLightState.set_rear_fog_light();

    // osiMOVehicleClassificationLightState.set_reversing_light();

    // osiMOVehicleClassificationLightState.set_service_vehicle_illumination();

    return osiMOVehicleClassificationLightState;
}

osi3::MovingObject_VehicleClassification_LightState_BrakeLightState MovingObjectMapper::mapBrakeLightState(
    const ara::adi::sensoritf::LightStatus& lightState) const
{
    // TODO: Exact mapping unavailable. Mapped roughly
    osi3::MovingObject_VehicleClassification_LightState_BrakeLightState osiMOVehicleClassificationLightState{
        osi3::MovingObject_VehicleClassification_LightState_BrakeLightState::
            MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_UNKNOWN};

    switch (lightState) {
    case (ara::adi::sensoritf::LightStatus::kBrake): {
        osiMOVehicleClassificationLightState = osi3::MovingObject_VehicleClassification_LightState_BrakeLightState::
            MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_STRONG;
        break;
    }
    case (ara::adi::sensoritf::LightStatus::kOff): {
        osiMOVehicleClassificationLightState = osi3::MovingObject_VehicleClassification_LightState_BrakeLightState::
            MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_OFF;
        break;
    }
    case (ara::adi::sensoritf::LightStatus::kOn): {
        osiMOVehicleClassificationLightState = osi3::MovingObject_VehicleClassification_LightState_BrakeLightState::
            MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_NORMAL;
        break;
    }
    case (ara::adi::sensoritf::LightStatus::kOther): {
        osiMOVehicleClassificationLightState = osi3::MovingObject_VehicleClassification_LightState_BrakeLightState::
            MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_OTHER;
        break;
    }
    default: {
        osiMOVehicleClassificationLightState = osi3::MovingObject_VehicleClassification_LightState_BrakeLightState::
            MovingObject_VehicleClassification_LightState_BrakeLightState_BRAKE_LIGHT_STATE_UNKNOWN;
        m_logger.LogError() << "Error: mapBrakeLightState: Unknown enum-entry: "
                            << static_cast<std::uint8_t>(lightState);
        break;
    }
    }

    return osiMOVehicleClassificationLightState;
}

osi3::MovingObject_VehicleClassification_LightState_IndicatorState MovingObjectMapper::mapIndicatorLightState(
    const ara::adi::sensoritf::LightStatus& lightState,
    const ara::adi::sensoritf::LightType& lightType) const
{
    // TODO: Exact mapping unavailable. Mapped roughly
    osi3::MovingObject_VehicleClassification_LightState_IndicatorState osiMOVehicleClassificationIndLightState{
        osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_UNKNOWN};

    switch (lightState) {
    case (ara::adi::sensoritf::LightStatus::kFlash): {
        if (lightType == ara::adi::sensoritf::LightType::kLeftFlashLight) {
            osiMOVehicleClassificationIndLightState
                = osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
                    MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_LEFT;
            break;
        } else if (lightType == ara::adi::sensoritf::LightType::kRightFlashLight) {
            osiMOVehicleClassificationIndLightState
                = osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
                    MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_RIGHT;
            break;
        } else {
            m_logger.LogError() << "Error: mapIndicatorLightState: Unknown enum-entry: "
                                << static_cast<std::uint8_t>(lightState);
            break;
        }
    }
    case (ara::adi::sensoritf::LightStatus::kOff): {
        osiMOVehicleClassificationIndLightState = osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_OFF;
        break;
    }
    case (ara::adi::sensoritf::LightStatus::kWarning): {
        osiMOVehicleClassificationIndLightState = osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_WARNING;
        break;
    }
    case (ara::adi::sensoritf::LightStatus::kOther): {
        osiMOVehicleClassificationIndLightState = osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_OTHER;
        break;
    }
    default: {
        osiMOVehicleClassificationIndLightState = osi3::MovingObject_VehicleClassification_LightState_IndicatorState::
            MovingObject_VehicleClassification_LightState_IndicatorState_INDICATOR_STATE_UNKNOWN;
        m_logger.LogError() << "Error: mapIndicatorLightState: Unknown enum-entry: "
                            << static_cast<std::uint8_t>(lightState);
        break;
    }
    }

    return osiMOVehicleClassificationIndLightState;
}

osi3::MovingObject_VehicleClassification_LightState_GenericLightState MovingObjectMapper::mapGenericLightState(
    const ara::adi::sensoritf::LightStatus& lightState) const
{
    // TODO: Exact mapping unavailable. Mapped roughly
    osi3::MovingObject_VehicleClassification_LightState_GenericLightState osiMOVehicleClassificationGenLightState{
        osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
            MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_UNKNOWN};

    switch (lightState) {
    case (ara::adi::sensoritf::LightStatus::kOn): {
        osiMOVehicleClassificationGenLightState
            = osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
                MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_ON;
        break;
    }
    case (ara::adi::sensoritf::LightStatus::kOff): {
        osiMOVehicleClassificationGenLightState
            = osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
                MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OFF;
        break;
    }
    case (ara::adi::sensoritf::LightStatus::kOther): {
        osiMOVehicleClassificationGenLightState
            = osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
                MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_OTHER;
        break;
    }
    default: {
        osiMOVehicleClassificationGenLightState
            = osi3::MovingObject_VehicleClassification_LightState_GenericLightState::
                MovingObject_VehicleClassification_LightState_GenericLightState_GENERIC_LIGHT_STATE_UNKNOWN;
        m_logger.LogError() << "Error: mapGenericLightState: Unknown enum-entry: "
                            << static_cast<std::uint8_t>(lightState);
        break;
    }
    }

    return osiMOVehicleClassificationGenLightState;
}
