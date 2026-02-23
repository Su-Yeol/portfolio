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

#include "osiMappers/DetectedEntityHeaderMapper.h"

osi3::DetectedEntityHeader DetectedEntityHeaderMapper::mapDetectedEntityHeader(
    const ara::adi::sensoritf::InformationInterface& informationInterface) const
{
    osi3::DetectedEntityHeader detectedHeader;

    detectedHeader.set_data_qualifier(mapDataQualifier2Osi(informationInterface.DataQualifier));

    detectedHeader.set_cycle_counter(informationInterface.CycleCounter);

    osi3::Timestamp* time{detectedHeader.mutable_measurement_time()};
    *time = mapToTimestamp(informationInterface.TimeStamp);

    return detectedHeader;
}

osi3::DetectedEntityHeader_DataQualifier DetectedEntityHeaderMapper::mapDataQualifier2Osi(
    const ara::adi::sensoritf::DataQualifier& dataQualifier) const
{
    using OsiDataQualifier = osi3::DetectedEntityHeader_DataQualifier;
    osi3::DetectedEntityHeader_DataQualifier osiDataqualifier{
        OsiDataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN};

    switch (dataQualifier) {
    case ara::adi::sensoritf::DataQualifier::kNotAvailable: {
        osiDataqualifier = OsiDataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE;
    } break;
    case ara::adi::sensoritf::DataQualifier::kInvalid: {
        m_logger.LogWarn() << "No perfect mapping was possible: DataQualifier::kInvalid "
                              "-> DataQualifier::DATA_QUALIFIER_NOT_AVAILABLE;";
        osiDataqualifier = OsiDataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE;
    } break;
    case ara::adi::sensoritf::DataQualifier::kNormal: {
        osiDataqualifier = OsiDataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE;
    } break;
    case ara::adi::sensoritf::DataQualifier::kReduceInCoverage: {
        osiDataqualifier = OsiDataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_BLINDNESS;
    } break;
    case ara::adi::sensoritf::DataQualifier::kReduceInPerformance: {
        osiDataqualifier = OsiDataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED;
    } break;
    case ara::adi::sensoritf::DataQualifier::kReduceInViewAndPerformance: {
        m_logger.LogInfo() << "No perfect mapping was possible: DataQualifier::kReduceInViewAndPerformance "
                           << "-> DataQualifier::DATA_QUALIFIER_AVAILABLE_REDUCED";
        osiDataqualifier = OsiDataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED;
    } break;
    case ara::adi::sensoritf::DataQualifier::kTestMode: {
        m_logger.LogInfo() << "No perfect mapping was possible: DataQualifier::kTestMode "
                              "-> DataQualifier::DATA_QUALIFIER_OTHER";
        osiDataqualifier = OsiDataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_OTHER;
    } break;
    case ara::adi::sensoritf::DataQualifier::kUnknown: {
        osiDataqualifier = OsiDataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN;
    } break;
    default: {
        m_logger.LogError() << "Error: mapDataQualifier: Unknown enum entry: "
                            << static_cast<std::uint8_t>(dataQualifier);
    } break;
    }

    return osiDataqualifier;
}
