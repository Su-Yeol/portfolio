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
#include "ara/adi/sensoritf/impl_type_generallandmark.h"
#include "osi3/osi_object.pb.h"
#include "osiMappers/StationaryObjectMapper.h"

// TODO
// osi3::StationaryObject StationaryObjectMapper::mapStationaryObject(
//        const ara::adi::sensoritf::GeneralLandmark& generalLandmark) const
//{
//    // Map Identifier
//}

osi3::BaseStationary StationaryObjectMapper::mapBaseStationary(
    const ara::adi::sensoritf::GeneralLandmark& generalLandmark) const
{
    osi3::BaseStationary osiBase{};

    osi3::Dimension3d* osiDimension{osiBase.mutable_dimension()};
    auto dimError{generalLandmark.GeneralLandmarksBoundingBox.BoxDimension};
    *osiDimension = mapDimension3D2Osi(dimError);

    osi3::Vector3d* osiPosition{osiBase.mutable_position()};
    auto posError{generalLandmark.GeneralLandmarksPos.Position};
    *osiPosition = mapPoint3D2Osi(posError);

    osi3::Orientation3d* osiOrientation{osiBase.mutable_orientation()};
    auto orientError{generalLandmark.GeneralLandmarksPos.Orientation};
    *osiOrientation = mapOrientation3D2Osi(orientError);

    return osiBase;
}

osi3::StationaryObject_Classification StationaryObjectMapper::mapClassification(
    const ::ara::adi::sensoritf::GeneralLandmarkClassification& generalLandmarkClassification) const
{
    osi3::StationaryObject_Classification osiStationaryObjClassification{};

    // Map type
    osi3::StationaryObject_Classification_Type osiStationaryObjClassType
        = mapOsiStationaryObjClassType(generalLandmarkClassification.GeneralLandMarkType);
    osiStationaryObjClassification.set_type(osiStationaryObjClassType);

    // TODO: Unavailable material, color, density mapping
    return osiStationaryObjClassification;
}

osi3::StationaryObject_Classification_Type StationaryObjectMapper::mapOsiStationaryObjClassType(
    const ::ara::adi::sensoritf::GeneralLandmarkClassificationType& generalLandmarkClassType) const
{
    osi3::StationaryObject_Classification_Type osiStationaryObjClassType{};
    switch (generalLandmarkClassType) {
    case ::ara::adi::sensoritf::GeneralLandmarkClassificationType::kBeacon: {
        osiStationaryObjClassType = osi3::StationaryObject_Classification_Type ::
            StationaryObject_Classification_Type_TYPE_CONSTRUCTION_SITE_ELEMENT;
        break;
    }
    case ::ara::adi::sensoritf::GeneralLandmarkClassificationType::kBridge: {
        osiStationaryObjClassType
            = osi3::StationaryObject_Classification_Type ::StationaryObject_Classification_Type_TYPE_BRIDGE;
        break;
    }
    case ::ara::adi::sensoritf::GeneralLandmarkClassificationType::kCone: {
        osiStationaryObjClassType
            = osi3::StationaryObject_Classification_Type ::StationaryObject_Classification_Type_TYPE_PYLON;
        break;
    }
    case ::ara::adi::sensoritf::GeneralLandmarkClassificationType::kLampPost: {
        osiStationaryObjClassType
            = osi3::StationaryObject_Classification_Type ::StationaryObject_Classification_Type_TYPE_EMITTING_STRUCTURE;
        break;
    }
    case ::ara::adi::sensoritf::GeneralLandmarkClassificationType::kVerticalStructure: {
        osiStationaryObjClassType
            = osi3::StationaryObject_Classification_Type ::StationaryObject_Classification_Type_TYPE_VERTICAL_STRUCTURE;
        break;
    }
    case ::ara::adi::sensoritf::GeneralLandmarkClassificationType::kOverheadObject: {
        osiStationaryObjClassType
            = osi3::StationaryObject_Classification_Type ::StationaryObject_Classification_Type_TYPE_OVERHEAD_STRUCTURE;
        break;
    }
    case ::ara::adi::sensoritf::GeneralLandmarkClassificationType::kRectangularStructure: {
        osiStationaryObjClassType = osi3::StationaryObject_Classification_Type ::
            StationaryObject_Classification_Type_TYPE_RECTANGULAR_STRUCTURE;
        break;
    }
    case ::ara::adi::sensoritf::GeneralLandmarkClassificationType::kReflector: {
        osiStationaryObjClassType = osi3::StationaryObject_Classification_Type ::
            StationaryObject_Classification_Type_TYPE_REFLECTIVE_STRUCTURE;
        break;
    }
    case ::ara::adi::sensoritf::GeneralLandmarkClassificationType::kUnknown: {
        osiStationaryObjClassType
            = osi3::StationaryObject_Classification_Type ::StationaryObject_Classification_Type_TYPE_UNKNOWN;
        break;
    }
    default: {
        m_logger.LogError() << "Error: mapOsiStationaryObjClassType: "
                               "Unknown enum entry: "
                            << static_cast<std::uint8_t>(generalLandmarkClassType);
        osiStationaryObjClassType
            = osi3::StationaryObject_Classification_Type::StationaryObject_Classification_Type_TYPE_UNKNOWN;
        break;
    }
    }
    return osiStationaryObjClassType;
}
