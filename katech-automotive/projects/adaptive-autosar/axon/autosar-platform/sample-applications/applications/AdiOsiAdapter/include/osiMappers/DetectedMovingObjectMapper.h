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

#ifndef ADI_OSI_ADAPTER_DETECTEDMOVINGOBJECT_POTENTIALLYMOVINGOBJECTS_MAPPING_HPP_
#define ADI_OSI_ADAPTER_DETECTEDMOVINGOBJECT_POTENTIALLYMOVINGOBJECTS_MAPPING_HPP_

#include "ara/log/logger.h"

#include "osi3/osi_detectedobject.pb.h"
#include "osi3/osi_sensordata.pb.h"

#include "ara/adi/sensoritf/impl_type_potentiallymovingobjectinterface.h"

#include "MovingObjectMapper.h"
#include "DetectedItemHeaderMapper.h"

using osiDMORefPt = osi3::DetectedMovingObject_ReferencePoint;
using adiDMORefPt = ara::adi::sensoritf::ReferencePoint;

class DetectedMovingObjectMapper : public MovingObjectMapper
{
public:
    DetectedMovingObjectMapper() = default;

public:
    osi3::DetectedMovingObject mapDetectedMovingObject(
        const ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const;

private:
    osi3::BaseMoving mapBaseRmse(
        const ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const;

    static std::map<adiDMORefPt, osiDMORefPt> referencePointMap;
    osi3::DetectedMovingObject_ReferencePoint mapReferencePoint(
        const ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const;

    osi3::DetectedMovingObject_MovementState mapMovementState(
        const ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject) const;

    osi3::DetectedMovingObject_CandidateMovingObject mapDMOCandidate(
        const ara::adi::sensoritf::ValidPotentiallyMovingObject& potentiallyMovingObject,
        const size_t& classIndex) const;

private:
    ara::log::Logger& m_logger{ara::log::CreateLogger("DMOM", "...", ara::log::LogLevel::kVerbose)};
    DetectedItemHeaderMapper detectedItemHeaderMapper;
};

#endif  // ADI_OSI_ADAPTER_DETECTEDMOVINGOBJECT_POTENTIALLYMOVINGOBJECTS_MAPPING_HPP_
