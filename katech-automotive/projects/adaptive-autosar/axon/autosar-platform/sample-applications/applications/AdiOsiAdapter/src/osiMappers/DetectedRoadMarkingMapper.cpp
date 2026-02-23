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

#include "osiMappers/DetectedRoadMarkingMapper.h"
#include "osi3/osi_roadmarking.pb.h"
#include "osi3/osi_sensordata.pb.h"

osi3::DetectedRoadMarking DetectedRoadMarkingMapper::mapDetectedRoadMaking(
    const ara::adi::sensoritf::RoadMarking& roadMarking,
    const ara::adi::sensoritf::ColourModelType& colourModelType) const
{
    osi3::DetectedRoadMarking detectedRoadMarking{};

    osi3::DetectedItemHeader* detectedItemHeader{detectedRoadMarking.mutable_header()};
    *detectedItemHeader = detectedItemHeaderMapper.mapToDetectedItemHeader(roadMarking.RoadMarkingsStatus);

    auto base{detectedRoadMarking.mutable_base()};
    *base = mapToBaseStationary(roadMarking.RoadMarkingsPolylines);

    osi3::BaseStationary* baseError{detectedRoadMarking.mutable_base_rmse()};
    *baseError = mapToBaseStationaryRsme(roadMarking.RoadMarkingsPolylines);

    addingCandidates(roadMarking, detectedRoadMarking);

    osi3::ColorDescription* colorDescription{detectedRoadMarking.mutable_color_description()};
    auto colourTone = getTheColourToneWithTheBestClassification(roadMarking.RoadMarkingsInformation);
    *colorDescription = mapToColorDesription(colourModelType, colourTone);

    return detectedRoadMarking;
}

osi3::BaseStationary DetectedRoadMarkingMapper::mapToBaseStationaryRsme(
    const ara::adi::sensoritf::Polylines& polylines) const
{
    osi3::BaseStationary base{};

    // simple solution: take the first element of the first polyline and use it for the base position
    // more complex solutions are possible
    if (polylines.ValidPolylinesList.size() == 0) {
        return {};
    }

    if (polylines.ValidPolylinesList.at(0).ValidVerticesList.size() == 0) {
        return {};
    }

    auto positionErrorOsi{base.mutable_position()};
    auto positionError = polylines.ValidPolylinesList.at(0).ValidVerticesList.at(0).VertexPointError;
    *positionErrorOsi = mapPoint3DError2Osi(positionError);

    return base;
}

void DetectedRoadMarkingMapper::addingCandidates(const ara::adi::sensoritf::RoadMarking& roadMarking,
    osi3::DetectedRoadMarking& detectedRoadMarking) const
{
    auto classifications = roadMarking.RoadMarkingsInformation.ValidRoadMarkingClassificationsList;
    if (classifications.size() != roadMarking.RoadMarkingsInformation.NumberOfValidRoadMarkingClassifications) {
        m_logger.LogWarn() << "Warning: NumberOfValidRoadMarkingClassifications has a wrong value!";
    }

    for (auto it = classifications.begin(); it != classifications.end(); ++it) {
        auto candidate = detectedRoadMarking.add_candidate();

        candidate->set_probability(static_cast<double>(it->RoadMarkingTypeConfidence));

        // Traffic Sign class inside RoadMarking should be repeated in osi. For now setting sign class to 0
        osi3::RoadMarking_Classification* osiClassification{candidate->mutable_classification()};
        *osiClassification = mapToClassification(*it, 0);
    }
}

ara::adi::sensoritf::ColourTone DetectedRoadMarkingMapper::getTheColourToneWithTheBestClassification(
    const ara::adi::sensoritf::RoadMarkingsInformation& roadMarkingInformation) const
{
    ara::adi::sensoritf::ColourTone colourTone;

    auto list = roadMarkingInformation.ValidRoadMarkingClassificationsList;

    if (list.size() != roadMarkingInformation.NumberOfValidRoadMarkingClassifications) {
        m_logger.LogWarn() << "Warning: NumberOfValidRoadMarkingClassifications has a wrong value!";
    }

    auto best = std::max_element(list.begin(),
        list.end(),
        [](const ara::adi::sensoritf::RoadMarkingClassification& a,
            const ara::adi::sensoritf::RoadMarkingClassification& b) {
            return a.RoadMarkingTypeConfidence < b.RoadMarkingTypeConfidence;
        });

    return best->ColourTone;
}
