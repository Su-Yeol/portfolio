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

#ifndef ADCM_IMPL_TYPE_CAN_DATA_OBJECTS_H_
#define ADCM_IMPL_TYPE_CAN_DATA_OBJECTS_H_

#include "adcm/impl_type_laneinfostruct.h"
#include <cstdint>
namespace adcm
{

struct can_data_Objects
{
    double WheelSpeed_FL;
    double WheelSpeed_FR;
    double WheelSpeed_RL;
    double WheelSpeed_RR;
    double ClusterSpeed;
    std::uint8_t GearState;
    double LateralAcceleration;
    double LongitudinalAcceleration;
    double Angular_Acceleration;
    std::uint8_t Turn_Signal;
    double SteeringWheelAngle;
    std::int32_t ControlSteering;
    ::adcm::LaneInfoStruct LaneInfo;
    bool AutoDrivingMode;
    std::uint8_t Emergency_Light;
    bool No_Autonomous_Driving;
    std::uint32_t WheelTick_FL;
    std::uint32_t WheelTick_FR;
    std::uint32_t WheelTick_RL;
    std::uint32_t WheelTick_RR;
    double Accel_Pedal;
    double Brake_Pedal;
    double RPM;

    using IsEnumerableTag = void;
    template <typename F>
    void enumerate(F& fun)
    {
        fun(this->WheelSpeed_FL);
        fun(this->WheelSpeed_FR);
        fun(this->WheelSpeed_RL);
        fun(this->WheelSpeed_RR);
        fun(this->ClusterSpeed);
        fun(this->GearState);
        fun(this->LateralAcceleration);
        fun(this->LongitudinalAcceleration);
        fun(this->Angular_Acceleration);
        fun(this->Turn_Signal);
        fun(this->SteeringWheelAngle);
        fun(this->ControlSteering);
        fun(this->LaneInfo);
        fun(this->AutoDrivingMode);
        fun(this->Emergency_Light);
        fun(this->No_Autonomous_Driving);
        fun(this->WheelTick_FL);
        fun(this->WheelTick_FR);
        fun(this->WheelTick_RL);
        fun(this->WheelTick_RR);
        fun(this->Accel_Pedal);
        fun(this->Brake_Pedal);
        fun(this->RPM);
    }
};
}  // namespace adcm

#endif  // ADCM_IMPL_TYPE_CAN_DATA_OBJECTS_H_
