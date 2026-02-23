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

#include "osiMappers/StaticObjectsMapping.h"
#include "osiMappers/GeneralHeaderMapper.h"
#include "osiMappers/TrafficLightMapper.h"
#include "osiMappers/TrafficSignMapper.h"

#include "osi3/osi_trafficlight.pb.h"
#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_common.pb.h"

#include <algorithm>

osi3::TrafficLight StaticObjectsMapping::createTrafficLightsOsiOutput(
    const ara::adi::sensoritf::StaticObjectInterface& staticObjectInterface) const
{
    osi3::TrafficLight osiTrafficLight;
    ara::adi::sensoritf::TrafficLight trafficLight
        = staticObjectInterface.StaticObjectTrafficLights.TrafficLightList.at(0);
    TrafficLightMapper trafficLightsMapping;
    osiTrafficLight = trafficLightsMapping.mapTrafficLight(trafficLight, 0);  // TODO 0 is wrong

    return osiTrafficLight;
}

osi3::TrafficSign StaticObjectsMapping::createTrafficSignOsiOutput(
    const ara::adi::sensoritf::StaticObjectInterface& staticObjectInterface) const
{
    osi3::TrafficSign osiTrafficSign;
    ara::adi::sensoritf::TrafficSign trafficSign
        = staticObjectInterface.StaticObjectTrafficSigns.TrafficSignsList.at(0);
    TrafficSignMapper trafficSignsMapping;
    osiTrafficSign = trafficSignsMapping.mapTrafficSign(trafficSign);

    return osiTrafficSign;
}
