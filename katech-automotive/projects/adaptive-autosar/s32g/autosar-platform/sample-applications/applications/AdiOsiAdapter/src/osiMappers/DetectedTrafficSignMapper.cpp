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

#include "osiMappers/DetectedTrafficSignMapper.h"

#include "osi3/osi_trafficsign.pb.h"
#include "osi3/osi_sensordata.pb.h"

osi3::DetectedTrafficSign DetectedTrafficSignMapper::mapDetectedTrafficSign(
    const ara::adi::sensoritf::TrafficSign& trafficSign) const
{
    osi3::DetectedTrafficSign osiDetectedTrafficSign;

    // Item Header Mapping
    osi3::DetectedItemHeader* osiDetectedItemHeader{osiDetectedTrafficSign.mutable_header()};
    *osiDetectedItemHeader = detectedItemHeaderMapper.mapToDetectedItemHeader(trafficSign.TrafficSignsStatus);

    // Main Sign Mapping
    osi3::DetectedTrafficSign_DetectedMainSign* osiDtsDetectedMainSign{osiDetectedTrafficSign.mutable_main_sign()};
    *osiDtsDetectedMainSign = mapDTSDetectedMainSign(trafficSign);

    // Supplementary Signs Mapping
    // osi3::DetectedTrafficSign_DetectedSupplementarySign* ptrOsiDtsDetectedSupplSign;
    std::size_t numDtsSupplementarySigns = trafficSign.TrafficSignsSupplementarySigns.SSignList.size();

    if (numDtsSupplementarySigns == 0) {
        return {};
    }

    if (numDtsSupplementarySigns != trafficSign.TrafficSignsSupplementarySigns.NumberOfValidTrafficSupplementarySigns) {

        m_logger.LogWarn() << "Warning: Mismatch between NumberOfValidTrafficSupplementarySigns: "
                           << trafficSign.TrafficSignsSupplementarySigns.NumberOfValidTrafficSupplementarySigns
                           << " and numDtsSupplementarySigns: "
                           << trafficSign.TrafficSignsSupplementarySigns.SSignList.size();
    }

    for (std::size_t i = 0; i < numDtsSupplementarySigns; i++) {
        osi3::DetectedTrafficSign_DetectedSupplementarySign* ptrOsiDtsDetectedSupplSign
            = osiDetectedTrafficSign.add_supplementary_sign();
        *ptrOsiDtsDetectedSupplSign = mapDTSDetectedSupplSign(trafficSign, i);
    }

    return osiDetectedTrafficSign;
}

osi3::DetectedTrafficSign_DetectedMainSign DetectedTrafficSignMapper::mapDTSDetectedMainSign(
    const ara::adi::sensoritf::TrafficSign& trafficSign) const
{
    osi3::DetectedTrafficSign_DetectedMainSign osiDtsDetectedMainSign;

    // Candidate Main Sign Mapping
    // osi3::DetectedTrafficSign_DetectedMainSign_CandidateMainSign* ptrOsiDtsCandidateMainSign;

    std::size_t numTsClassification = trafficSign.TrafficSignsInformation.ValidMainSignClassificationsList.size();

    if (numTsClassification == 0) {
        return {};
    }

    if (numTsClassification != trafficSign.TrafficSignsInformation.NumberOfValidSignClassifications) {

        m_logger.LogWarn() << "Warning: Mismatch between "
                              "NumberOfValidSignClassifications: "
                           << trafficSign.TrafficSignsInformation.NumberOfValidSignClassifications
                           << " and numTsClassification: "
                           << trafficSign.TrafficSignsInformation.ValidMainSignClassificationsList.size();
    }

    // repeated Candidate Main Sign
    for (std::size_t i = 0; i < numTsClassification; i++) {
        float tsMainSignProbability;
        // classification Mapping
        osi3::DetectedTrafficSign_DetectedMainSign_CandidateMainSign* ptrOsiDtsCandidateMainSign
            = osiDtsDetectedMainSign.add_candidate();

        osi3::TrafficSign_MainSign_Classification* osiTsMainSignClass{
            ptrOsiDtsCandidateMainSign->mutable_classification()};

        *osiTsMainSignClass = mapTrafficSignMainClassification(
            trafficSign.TrafficSignsInformation.ValidMainSignClassificationsList.at(i));

        // probability Mapping
        tsMainSignProbability = trafficSign.TrafficSignsInformation.ValidMainSignClassificationsList.at(i)
                                    .SignClassificationTypeConfidence;
        ptrOsiDtsCandidateMainSign->set_probability(static_cast<double>(tsMainSignProbability));
    }

    // Base Mapping
    osi3::BaseStationary* osiMainBaseStationary{osiDtsDetectedMainSign.mutable_base()};
    *osiMainBaseStationary = mapMainBaseStationary(trafficSign);

    // Base rmse Mapping
    osi3::BaseStationary* osiMainBaseRmse{osiDtsDetectedMainSign.mutable_base_rmse()};
    osi3::Vector3d* positionError{osiMainBaseRmse->mutable_position()};
    *positionError = mapPoint3DError2Osi(trafficSign.TrafficSignsPosition.PositionError);

    // Geometry Mapping
    // TODO: Change in osi required!
    // Geometry should be a part of CandidateMainSign class!

    // std::uint8_t tsMainSignGeometry = trafficSign.TrafficSignsInformation.ValidMainSignClassificationsList
    // osiDtsDetectedMainSign.set_geometry();

    return osiDtsDetectedMainSign;
}

osi3::DetectedTrafficSign_DetectedSupplementarySign DetectedTrafficSignMapper::mapDTSDetectedSupplSign(
    const ara::adi::sensoritf::TrafficSign& trafficSign,
    const size_t& index) const
{
    osi3::DetectedTrafficSign_DetectedSupplementarySign osiDtsDetectedSupplSign;

    // Map Candidate
    // osi3::DetectedTrafficSign_DetectedSupplementarySign_CandidateSupplementarySign* ptrOsiDtsCandidateSupplSign;
    std::size_t numTsSupplSignClass
        = trafficSign.TrafficSignsSupplementarySigns.SSignList.at(index)
              .TrafficSignsSupplementarySignsInformation.ValidSupplementarySignClassifications.size();

    if (numTsSupplSignClass == 0) {
        return {};
    }

    if (numTsSupplSignClass
        != trafficSign.TrafficSignsSupplementarySigns.SSignList.at(index)
               .TrafficSignsSupplementarySignsInformation.NumberOfValidSupplementarySignClassifications) {

        m_logger.LogWarn()
            << "Warning: Mismatch between NumberOfValidSupplementarySignClassifications: "
            << trafficSign.TrafficSignsSupplementarySigns.SSignList.at(index)
                   .TrafficSignsSupplementarySignsInformation.NumberOfValidSupplementarySignClassifications
            << " and numTsSupplSignClass: "
            << trafficSign.TrafficSignsSupplementarySigns.SSignList.at(index)
                   .TrafficSignsSupplementarySignsInformation.ValidSupplementarySignClassifications.size();
    }

    // repeated Candidate Main Sign
    for (std::size_t i = 0; i < numTsSupplSignClass; i++) {
        float tsSupplSignProbability;
        // Map classification
        osi3::DetectedTrafficSign_DetectedSupplementarySign_CandidateSupplementarySign* ptrOsiDtsCandidateSupplSign
            = osiDtsDetectedSupplSign.add_candidate();

        osi3::TrafficSign_SupplementarySign_Classification* osiTsSupplSignClass{
            ptrOsiDtsCandidateSupplSign->mutable_classification()};

        *osiTsSupplSignClass = mapTrafficSignSupplementaryClassification(
            trafficSign.TrafficSignsSupplementarySigns.SSignList.at(index)
                .TrafficSignsSupplementarySignsInformation.ValidSupplementarySignClassifications.at(i));

        // Map probability
        tsSupplSignProbability
            = trafficSign.TrafficSignsSupplementarySigns.SSignList.at(index)
                  .TrafficSignsSupplementarySignsInformation.ValidSupplementarySignClassifications.at(i)
                  .SupplementarySignClassificationTypeConfidence;
        ptrOsiDtsCandidateSupplSign->set_probability(static_cast<double>(tsSupplSignProbability));
    }

    // Map Base
    osi3::BaseStationary* osiSupplBaseStationary{osiDtsDetectedSupplSign.mutable_base()};
    *osiSupplBaseStationary = mapSupplementaryBaseStationary(trafficSign, index);

    // Map Base rmse
    // TODO: Suplementary Base Position Error unavailable. Mapping Same as Main Sign POsition error
    osi3::BaseStationary* osiMainBaseRmse{osiDtsDetectedSupplSign.mutable_base_rmse()};
    osi3::Vector3d* positionError{osiMainBaseRmse->mutable_position()};
    *positionError = mapPoint3DError2Osi(trafficSign.TrafficSignsPosition.PositionError);

    return osiDtsDetectedSupplSign;
}
