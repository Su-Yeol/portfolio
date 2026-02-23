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

#ifndef ADI_OSI_ADAPTER_ULTRASONICLOGICALDETECTIONDATA_MAPPER_HPP_
#define ADI_OSI_ADAPTER_ULTRASONICLOGICALDETECTIONDATA_MAPPER_HPP_

#include "ara/log/logger.h"

#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_datarecording.pb.h"

#include "ara/adi/sensoritf/impl_type_ultrasonicfeatureinterface.h"

#include "GeneralHeaderMapper.h"

class UltrasonicLogicalDetectionDataMapper : public GeneralHeaderMapper
{
public:
    UltrasonicLogicalDetectionDataMapper() = default;

public:
    osi3::LogicalDetectionData mapLogicalDetectionData(
        const ara::adi::sensoritf::UltrasonicFeatureInterface& ultrasonicFeatureInterface) const;

protected:
    osi3::LogicalDetectionDataHeader mapLogicalDetectionDataHeader(
        const ara::adi::sensoritf::UltrasonicFeatureInterface& ultrasonicFeatureInterface) const;
    osi3::LogicalDetection mapLogicalDetection(
        const ara::adi::sensoritf::UltrasonicFeatureInterface& ultrasonicFeatureInterface,
        const size_t& index) const;

private:
    ara::log::Logger& m_logger{ara::log::CreateLogger("ULDD", "...", ara::log::LogLevel::kVerbose)};
    osi3::Vector3d map2DVelocity2Osi(const ara::adi::sensoritf::Point2D& point) const;
    osi3::Vector3d map2DVelocityError2Osi(const ara::adi::sensoritf::Point2DError& pointError) const;
};

#endif  // ADI_OSI_ADAPTER_ULTRASONICLOGICALDETECTIONDATA_MAPPER_HPP_
