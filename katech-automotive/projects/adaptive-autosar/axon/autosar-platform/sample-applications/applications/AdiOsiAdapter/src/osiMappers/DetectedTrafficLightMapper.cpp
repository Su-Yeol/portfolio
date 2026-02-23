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

#include "osiMappers/DetectedTrafficLightMapper.h"

#include "osi3/osi_trafficlight.pb.h"
#include "osi3/osi_sensordata.pb.h"

osi3::DetectedTrafficLight DetectedTrafficLightMapper::mapDetectedTrafficLight(
    const ara::adi::sensoritf::TrafficLight& trafficLight,
    size_t trafficLightSpotIndex) const
{
    osi3::DetectedTrafficLight detectedTrafficLight;
    ara::adi::sensoritf::TrafficLightSpot tlSpot{
        trafficLight.TrafficLightsSpots.ValidTrafficLightSpotList.at(trafficLightSpotIndex)};

    // ToDo: Item Header Mapping with SubObjectStatus argument type

    // osi3::DetectedItemHeader detectedItemHeader =
    // detectedItemHeaderMapper.mapObjectstatus(trafficLightSpot.TrafficLightsSpotsStatus);
    // detectedTrafficLight.set_allocated_header(&detectedItemHeader);

    // base Mapping
    osi3::BaseStationary* base{detectedTrafficLight.mutable_base()};
    *base = mapBaseStationary(trafficLight, trafficLightSpotIndex);

    // base rmse Mapping
    osi3::BaseStationary* baseError{detectedTrafficLight.mutable_base_rmse()};
    ara::adi::sensoritf::Point3DError pointError = tlSpot.TrafficLightsSpotsPosition.PositionObjectLevelError;

    osi3::Vector3d* positionError{baseError->mutable_position()};
    *positionError = mapPoint3DError2Osi(pointError);

    // ColorDescription Mapping
    // TODO

    // CandidateTrafficLight Mapping
    osi3::DetectedTrafficLight_CandidateTrafficLight* ptrTLcandidate;

    auto numTlShapeClass{tlSpot.TrafficLightsSpotsInformation.ValidLightShapeClassificationList.size()};

    if (numTlShapeClass == 0) {
        return {};
    }

    if (numTlShapeClass != tlSpot.TrafficLightsSpotsInformation.NumberOfValidLightShapeClassifications) {

        m_logger.LogWarn() << "Warning: Mismatch between "
                              "NumberOfValidLightShapeClassifications: "
                           << tlSpot.TrafficLightsSpotsInformation.NumberOfValidLightShapeClassifications
                           << " and ValidTrafficLightSpotList.size(): "
                           << tlSpot.TrafficLightsSpotsInformation.ValidLightShapeClassificationList.size();
    }

    for (uint8_t i = 0; i < numTlShapeClass; i++) {
        ptrTLcandidate = detectedTrafficLight.add_candidate();
        auto tlClassification = ptrTLcandidate->mutable_classification();
        *tlClassification = mapClassification(tlSpot, i);

        ::ara::adi::sensoritf::ProbabilityPercentage candidateProbability
            = tlSpot.TrafficLightsSpotsInformation.ValidLightShapeClassificationList.at(i)
                  .LightShapeClassificationTypeConfidence;
        ptrTLcandidate->set_probability(static_cast<double>(candidateProbability));
    }

    return detectedTrafficLight;
}
