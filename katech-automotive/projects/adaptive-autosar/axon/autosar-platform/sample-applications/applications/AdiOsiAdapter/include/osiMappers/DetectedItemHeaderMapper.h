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

#ifndef ADI_OSI_ADAPTER_DETECTEDITEMHEADER_MAPPING_HPP_
#define ADI_OSI_ADAPTER_DETECTEDITEMHEADER_MAPPING_HPP_

#include "ara/log/logger.h"

#include "osi3/osi_roadmarking.pb.h"
#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_datarecording.pb.h"

#include "ara/adi/sensoritf/impl_type_roadobjectinterface.h"

#include "GeneralHeaderMapper.h"

class DetectedItemHeaderMapper : public GeneralHeaderMapper
{
public:
    DetectedItemHeaderMapper() = default;

public:
    osi3::DetectedItemHeader mapToDetectedItemHeader(const ara::adi::sensoritf::ObjectStatus& objectStatus) const;
    osi3::DetectedItemHeader_MeasurementState mapMeasurementStatus(
        const ara::adi::sensoritf::MeasurementStatus& measurementStatus) const;

private:
    ara::log::Logger& m_logger{ara::log::CreateLogger("DIHM", "...", ara::log::LogLevel::kVerbose)};
};

#endif  // ADI_OSI_ADAPTER_DETECTEDITEMHEADER_MAPPING_HPP_
