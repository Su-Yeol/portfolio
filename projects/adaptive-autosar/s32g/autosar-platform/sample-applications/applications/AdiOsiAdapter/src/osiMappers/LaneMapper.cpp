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

#include "osiMappers/LaneMapper.h"

#include "osi3/osi_sensordata.pb.h"

osi3::Lane LaneMapper::mapLane(const ara::adi::sensoritf::RoadObjectInterface& roadObjectInterface) const
{
    osi3::Lane osiLane{};

    // Identifier Mapping
    // osiLane.set_allocated_id(roadObjectInterface.RoadSurfaceObjectList.);

    // External Reference Mapping
    // osiLane.add_source_reference();

    // Classification Mapping
    osi3::Lane_Classification osiLaneClassification = mapLaneClassification(roadObjectInterface.RoadSurfaceObjectList);
    osiLane.set_allocated_classification(&osiLaneClassification);

    return osiLane;
}

osi3::Lane_Classification LaneMapper::mapLaneClassification(const ara::adi::sensoritf::RoadSurface& roadSurface) const
{
    osi3::Lane_Classification osiLaneClassification{};

    // NA

    // osiLaneClassification.add_centerline();
    // osiLaneClassification.add_free_lane_boundary_id();
    // osiLaneClassification.add_lane_pairing();
    // osiLaneClassification.add_left_adjacent_lane_id();
    // osiLaneClassification.add_left_lane_boundary_id();
    // osiLaneClassification.add_right_adjacent_lane_id();
    // osiLaneClassification.add_right_lane_boundary_id();

    osi3::Lane_Classification_RoadCondition osiRoadcondition{};
    // ara::adi::sensoritf::RoadConditionClassificationType roadConditionClass =
    // roadSurface.ValidRoadSurfaceConditionClassifications.
    //    at(index).RoadConditionType;
    // switch (roadConditionClass)
    //{
    // case ara::adi::sensoritf::RoadConditionClassificationType::kDry:
    //{
    //    osiRoadcondition = osi3::Lane_Classification_RoadCondition::
    //    break;
    //}
    // default:
    //    break;
    //}

    // Road condition values NA; enum type are not required by OSI

    // osiRoadcondition.set_surface_freezing_point();
    // osiRoadcondition.set_surface_ice();

    osiRoadcondition.set_surface_roughness(static_cast<double>(roadSurface.RoadSurfaceRoughness));
    // osiRoadcondition.set_surface_temperature();

    // osiRoadcondition.set_surface_texture();
    // osiRoadcondition.set_surface_water_film();

    osiLaneClassification.set_allocated_road_condition(&osiRoadcondition);

    // NA
    // osiLaneClassification.set_centerline_is_driving_direction();

    // NA
    // osiLaneClassification.set_is_host_vehicle_lane();

    // NA
    // osi3::Lane_Classification_Subtype osiLaneClassSubtype{};
    // osiLaneClassification.set_subtype();

    osi3::Lane_Classification_Type osiLaneClassType = mapLaneClassificationType(roadSurface.RoadType);
    osiLaneClassification.set_type(osiLaneClassType);

    return osiLaneClassification;
}

osi3::Lane_Classification_Type LaneMapper::mapLaneClassificationType(
    const ara::adi::sensoritf::RoadType& roadType) const
{
    switch (roadType) {
    case ara::adi::sensoritf::RoadType::kHighway: {
        return osi3::Lane_Classification_Type::Lane_Classification_Type_TYPE_INTERSECTION;
    }

    case ara::adi::sensoritf::RoadType::kCity: {
        return osi3::Lane_Classification_Type::Lane_Classification_Type_TYPE_DRIVING;
    }

    case ara::adi::sensoritf::RoadType::kUnknown: {
        return osi3::Lane_Classification_Type::Lane_Classification_Type_TYPE_UNKNOWN;
    }

    case ara::adi::sensoritf::RoadType::kOffRoad: {
        return osi3::Lane_Classification_Type::Lane_Classification_Type_TYPE_NONDRIVING;
    }

    case ara::adi::sensoritf::RoadType::kRural: {
        return osi3::Lane_Classification_Type::Lane_Classification_Type_TYPE_DRIVING;
    }

    default: {
        m_logger.LogWarn() << "Warning: Unknown type for ara::adi::sensoritf::RoadType.";

        return osi3::Lane_Classification_Type::Lane_Classification_Type_TYPE_UNKNOWN;
    } break;
    }
}
