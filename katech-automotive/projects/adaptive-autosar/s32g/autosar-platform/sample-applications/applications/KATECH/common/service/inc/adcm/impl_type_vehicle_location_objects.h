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

#ifndef ADCM_IMPL_TYPE_VEHICLE_LOCATION_OBJECTS_H_
#define ADCM_IMPL_TYPE_VEHICLE_LOCATION_OBJECTS_H_

#include "adcm/impl_type_gnssstruct.h"
#include "adcm/impl_type_imustruct.h"
#include <cstdint>
namespace adcm
{

struct vehicle_location_Objects
{
    float MeasurementTime;
    std::int64_t CurrentLane_id;
    double PositionX;
    double PositionY;
    double PositionZ;
    ::adcm::GNSSStruct GNSS;
    ::adcm::IMUStruct IMU;
    std::uint64_t GridID;
    std::uint64_t CellID;
    double Accuracy;
    std::uint8_t Fix_type;
    bool GPS_Status;

    using IsEnumerableTag = void;
    template <typename F>
    void enumerate(F& fun)
    {
        fun(this->MeasurementTime);
        fun(this->CurrentLane_id);
        fun(this->PositionX);
        fun(this->PositionY);
        fun(this->PositionZ);
        fun(this->GNSS);
        fun(this->IMU);
        fun(this->GridID);
        fun(this->CellID);
        fun(this->Accuracy);
        fun(this->Fix_type);
        fun(this->GPS_Status);
    }
};
}  // namespace adcm

#endif  // ADCM_IMPL_TYPE_VEHICLE_LOCATION_OBJECTS_H_
