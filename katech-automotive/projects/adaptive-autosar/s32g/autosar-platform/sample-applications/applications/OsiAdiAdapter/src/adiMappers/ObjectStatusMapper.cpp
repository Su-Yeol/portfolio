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

#include "adiMappers/ObjectStatusMapper.h"

ObjectStatus ObjectStatusMapper::mapObjectStatus(const osi3::DetectedItemHeader& osiDetectedItemHeader)
{

    ObjectStatus adiObjectStatus{};

    if (osiDetectedItemHeader.has_age()) {
        adiObjectStatus.Age = static_cast<uint64_t>(osiDetectedItemHeader.age());
    }

    if (osiDetectedItemHeader.has_existence_probability()) {
        adiObjectStatus.ExistenceProbabilityObjectLevel
            = static_cast<float>(osiDetectedItemHeader.existence_probability());
    }

    if (osiDetectedItemHeader.has_measurement_state()) {
        auto osiMeasurementState = osiDetectedItemHeader.measurement_state();

        switch (osiMeasurementState) {
        case osi3::DetectedItemHeader_MeasurementState::
            DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED: {
            adiObjectStatus.MeasurementStatusObjectLevel = MeasurementStatus::kMeasured;
            break;
        }
        case osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_OTHER: {
            m_logger.LogWarn() << "In mapObjectStatus: No perfect mapping to MEASUREMENT_STATE_OTHER available."
                                  "Mapped to MeasurementStatus::kPartiallyMeasured";
            adiObjectStatus.MeasurementStatusObjectLevel = MeasurementStatus::kPartiallyMeasured;
            break;
        }
        case osi3::DetectedItemHeader_MeasurementState::
            DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_PREDICTED: {
            adiObjectStatus.MeasurementStatusObjectLevel = MeasurementStatus::kPredicted;
            break;
        }
        case osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_UNKNOWN: {
            adiObjectStatus.MeasurementStatusObjectLevel = MeasurementStatus::kInvalid;
            break;
        }
        default: {
            m_logger.LogError() << "Error: mapObjectStatus: DetectedItemHeader_MeasurementState"
                                   "--> Unknown enum entry";
            adiObjectStatus.MeasurementStatusObjectLevel = MeasurementStatus::kInvalid;
            break;
        }
        }
    }

    // TODO Check
    if (osiDetectedItemHeader.ground_truth_id(0).has_value()) {
        adiObjectStatus.ObjectID = static_cast<uint16_t>(osiDetectedItemHeader.ground_truth_id(0).value());
    }
    // adiObjectStatus.GroupingObjectID = osiDMOCandidate.

    // adiObjectStatus.NumberOfValidObservationsObjectLevel

    // adiObjectStatus.TrackQuality
    // adiObjectStatus.ValidObservations.at(0).ObservationStatusObjectLevel
    // adiObjectStatus.ValidObservations.at(0).TimeStampReferenceObjectLevel

    return adiObjectStatus;
}

SubObjectStatus ObjectStatusMapper::mapSubObjectStatus(const osi3::DetectedItemHeader& osiDetectedItemHeader)
{

    SubObjectStatus adiObjectStatus{};

    if (osiDetectedItemHeader.has_age()) {
        adiObjectStatus.Age = static_cast<uint64_t>(osiDetectedItemHeader.age());
    }

    if (osiDetectedItemHeader.has_existence_probability()) {
        adiObjectStatus.ExistenceProbabilityObjectLevel
            = static_cast<float>(osiDetectedItemHeader.existence_probability());
    }

    if (osiDetectedItemHeader.has_measurement_state()) {
        auto osiMeasurementState = osiDetectedItemHeader.measurement_state();

        switch (osiMeasurementState) {
        case osi3::DetectedItemHeader_MeasurementState::
            DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_MEASURED: {
            adiObjectStatus.MeasurementStatusObjectLevel = MeasurementStatus::kMeasured;
            break;
        }
        case osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_OTHER: {
            m_logger.LogWarn() << "In mapObjectStatus: No perfect mapping to MEASUREMENT_STATE_OTHER available."
                                  "Mapped to MeasurementStatus::kPartiallyMeasured";
            adiObjectStatus.MeasurementStatusObjectLevel = MeasurementStatus::kPartiallyMeasured;
            break;
        }
        case osi3::DetectedItemHeader_MeasurementState::
            DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_PREDICTED: {
            adiObjectStatus.MeasurementStatusObjectLevel = MeasurementStatus::kPredicted;
            break;
        }
        case osi3::DetectedItemHeader_MeasurementState::DetectedItemHeader_MeasurementState_MEASUREMENT_STATE_UNKNOWN: {
            adiObjectStatus.MeasurementStatusObjectLevel = MeasurementStatus::kInvalid;
            break;
        }
        default: {
            m_logger.LogError() << "Error: mapObjectStatus: DetectedItemHeader_MeasurementState"
                                   "--> Unknown enum entry";
            adiObjectStatus.MeasurementStatusObjectLevel = MeasurementStatus::kInvalid;
            break;
        }
        }
    }

    // TODO Check

    // adiObjectStatus.GroupingObjectID = osiDMOCandidate.

    // adiObjectStatus.NumberOfValidObservationsObjectLevel

    // adiObjectStatus.TrackQuality
    // adiObjectStatus.ValidObservations.at(0).ObservationStatusObjectLevel
    // adiObjectStatus.ValidObservations.at(0).TimeStampReferenceObjectLevel

    return adiObjectStatus;
}
