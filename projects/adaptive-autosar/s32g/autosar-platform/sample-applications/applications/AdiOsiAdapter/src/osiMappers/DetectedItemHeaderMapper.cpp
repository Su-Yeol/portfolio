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

#include "osiMappers/DetectedItemHeaderMapper.h"
#include "osi3/osi_roadmarking.pb.h"
#include "osi3/osi_sensordata.pb.h"

osi3::DetectedItemHeader DetectedItemHeaderMapper::mapToDetectedItemHeader(
    const ara::adi::sensoritf::ObjectStatus& objectStatus) const
{
    osi3::DetectedItemHeader detectedItemHeader;

    detectedItemHeader.set_age(static_cast<double>(objectStatus.Age));

    detectedItemHeader.set_existence_probability(static_cast<double>(objectStatus.ExistenceProbabilityObjectLevel));

    osi3::Identifier trackingID;
    trackingID.set_value(static_cast<std::uint64_t>(objectStatus.ObjectID));
    auto ptrTrackingID{detectedItemHeader.mutable_tracking_id()};
    *ptrTrackingID = trackingID;

    // TODO mayby the sensorID could be find in InformationInterface
    // detectedItemHeader.add_sensor_id()

    detectedItemHeader.set_measurement_state(mapMeasurementStatus(objectStatus.MeasurementStatusObjectLevel));

    return detectedItemHeader;
}

osi3::DetectedItemHeader_MeasurementState DetectedItemHeaderMapper::mapMeasurementStatus(
    const ara::adi::sensoritf::MeasurementStatus& measurementStatus) const
{
    switch (measurementStatus) {
    case ara::adi::sensoritf::MeasurementStatus::kInvalid: {
        m_logger.LogWarn() << "Warning: Map kInvalid to DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_UNKNOWN.";
        return osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_UNKNOWN;
    }
    case ara::adi::sensoritf::MeasurementStatus::kMeasured: {
        return osi3::DetectedItemHeader_MeasurementState::
            DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED;
    }
    case ara::adi::sensoritf::MeasurementStatus::kPredicted: {
        return osi3::DetectedItemHeader_MeasurementState::
            DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_PREDICTED;
    }
    case ara::adi::sensoritf::MeasurementStatus::kPartiallyMeasured: {
        return osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_OTHER;
    }
    case ara::adi::sensoritf::MeasurementStatus::kNew: {
        return osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_OTHER;
    }
    case ara::adi::sensoritf::MeasurementStatus::kPredictedOccluded: {
        m_logger.LogWarn()
            << "Warning: Map kPredictedOccluded to DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_UNKNOWN.";
        return osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_UNKNOWN;
    }
    default: {
        m_logger.LogWarn() << "Warning: Unknown type for ara::adi::sensoritf::MeasurementStatus."
                           << "return UNKNOWN";
        return osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_UNKNOWN;
    }
    }
}
