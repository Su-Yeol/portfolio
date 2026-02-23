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

#ifndef ADI_OSI_ADAPTER_MOVINGOBJECT_POTENTIALLYMOVINGOBJECTS_MAPPING_HPP_
#define ADI_OSI_ADAPTER_MOVINGOBJECT_POTENTIALLYMOVINGOBJECTS_MAPPING_HPP_

#include "ara/log/logger.h"

#include "ara/adi/sensoritf/impl_type_potentiallymovingobjects.h"

#include "osi3/osi_object.pb.h"

#include "GeneralHeaderMapper.h"

using osiMOType = osi3::MovingObject_Type;
using osiMOVehicleType = osi3::MovingObject_VehicleClassification_Type;
using adiMOType = ara::adi::sensoritf::PotentiallyMovingObjectClassificationType;

class MovingObjectMapper : public GeneralHeaderMapper
{
public:
    MovingObjectMapper() = default;

public:
    // TODO: Map other elements of Moving Object

    osi3::MovingObject mapMovingObject(
        const ::ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const;

protected:
    // base
    osi3::BaseMoving mapBaseMoving(
        const ::ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const;

    osi3::MovingObject_MovingObjectClassification mapClassification(
        const ::ara::adi::sensoritf::PotentiallyMovingObjects& potentiallyMovingObjects,
        const size_t& index) const;

    osi3::MovingObject_Type mapType(
        const ::ara::adi::sensoritf::ValidPotentiallyMovingObjectClassification& PMOClass) const;

    osi3::MovingObject_VehicleClassification mapVehicleClassification(
        const ::ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject,
        const size_t& index) const;

private:
    ara::log::Logger& m_logger{ara::log::CreateLogger("StyOM", "...", ara::log::LogLevel::kVerbose)};
    static std::map<adiMOType, osiMOType> ClassificationTypeMap;
    static std::map<adiMOType, osiMOVehicleType> VehicleClassificationMap;

    osi3::MovingObject_VehicleClassification_Type mapVehicleClassificationType(
        const ara::adi::sensoritf::ValidPotentiallyMovingObjectClassification& validPotentiallyMovingObjectsClass)
        const;

    osi3::MovingObject_VehicleClassification_LightState mapVehicleClassificationLightState(
        const ara::adi::sensoritf::PotentiallyMovingObjectsLight& PMOLight) const;

    // template <typename T>
    // T mapLightState(const ara::adi::sensoritf::LightStatus& lightState) const;

    osi3::MovingObject_VehicleClassification_LightState_BrakeLightState mapBrakeLightState(
        const ara::adi::sensoritf::LightStatus& lightState) const;

    osi3::MovingObject_VehicleClassification_LightState_IndicatorState mapIndicatorLightState(
        const ara::adi::sensoritf::LightStatus& lightState,
        const ara::adi::sensoritf::LightType& lightType) const;

    osi3::MovingObject_VehicleClassification_LightState_GenericLightState mapGenericLightState(
        const ara::adi::sensoritf::LightStatus& lightState) const;
};

#endif  // ADI_OSI_ADAPTER_MOVINGOBJECT_POTENTIALLYMOVINGOBJECTS_MAPPING_HPP_
