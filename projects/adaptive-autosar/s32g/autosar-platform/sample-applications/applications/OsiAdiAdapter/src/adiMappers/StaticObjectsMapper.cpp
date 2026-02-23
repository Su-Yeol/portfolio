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

#include "adiMappers/StaticObjectsMapper.h"
#include "osi3/osi_sensordata.pb.h"

#include <algorithm>

StaticObjectInterface StaticObjectMapper::mapStaticObjectInterface(osi3::SensorData osiSensorData)
{

    StaticObjectInterface adiStaticObjectInterface{};
    adiStaticObjectInterface.StaticObjectInterfaceHeader = mapSOInterfaceHeader(osiSensorData);
    adiStaticObjectInterface.StaticObjectGeneralLandmarks = mapGeneralLandmarks(osiSensorData);
    adiStaticObjectInterface.StaticObjectTrafficLights = trafficLightsMapper.mapTrafficLights(osiSensorData);
    adiStaticObjectInterface.StaticObjectTrafficSigns = trafficSignsMapper.mapTrafficSigns(osiSensorData);

    return adiStaticObjectInterface;
}

InterfaceHeader StaticObjectMapper::mapSOInterfaceHeader(osi3::SensorData osiSensorData)
{

    InterfaceHeader interfaceHeader{};

    // Information Interface
    if (osiSensorData.has_stationary_object_header()) {
        interfaceHeader = mapInterfaceHeader(osiSensorData.stationary_object_header());
    }

    return interfaceHeader;
}

GeneralLandmarks StaticObjectMapper::mapGeneralLandmarks(osi3::SensorData osiSensorData)
{

    GeneralLandmarks adiGeneralLandmarks{};
    GeneralLandmark adiGeneralLandmark{};
    BoundingBox bBox{};
    GeneralLandmarksPosition generalLandmarkPos{};
    ObjectStatus objectStatus{};
    GeneralLandmarksInformation landmarkInfo{};

    int osiStyObjSize = osiSensorData.stationary_object_size();

    for (int index = 0; index < osiStyObjSize; index++) {

        if (osiSensorData.stationary_object(index).has_base()) {

            if (osiSensorData.stationary_object(index).base().has_dimension()) {
                bBox.BoxDimension = mapDimensiontoADI(osiSensorData.stationary_object(index).base().dimension());
            }

            if (osiSensorData.stationary_object(index).base().has_orientation()) {
                generalLandmarkPos.Orientation
                    = mapOrientationtoADI(osiSensorData.stationary_object(index).base().orientation());
            }

            if (osiSensorData.stationary_object(index).base().has_position()) {
                generalLandmarkPos.Position
                    = mapPositiontoADI(osiSensorData.stationary_object(index).base().position());
            }
        }

        if (osiSensorData.stationary_object(index).has_base_rmse()) {

            if (osiSensorData.stationary_object(index).base_rmse().has_dimension()) {
                bBox.BoxError = mapToDimension3dError(osiSensorData.stationary_object(index).base_rmse().dimension());
            }

            if (osiSensorData.stationary_object(index).base_rmse().has_orientation()) {
                generalLandmarkPos.OrientationError
                    = mapOrientationErrortoADI(osiSensorData.stationary_object(index).base_rmse().orientation());
            }

            if (osiSensorData.stationary_object(index).base_rmse().has_position()) {
                generalLandmarkPos.PositionError
                    = mapPositionErrtoADI(osiSensorData.stationary_object(index).base_rmse().position());
            }
        }

        adiGeneralLandmark.GeneralLandmarksBoundingBox = bBox;

        adiGeneralLandmark.GeneralLandmarksPos = generalLandmarkPos;

        if (osiSensorData.stationary_object(index).has_header()) {
            objectStatus = objectStatusMapper.mapObjectStatus(osiSensorData.stationary_object(index).header());
        }

        adiGeneralLandmark.GeneralLandmarksStatus = objectStatus;

        auto numOsiStyObjectCandidates = osiSensorData.stationary_object(index).candidate_size();
        landmarkInfo.NumberOfValidGeneralLandmarkClassifications = numOsiStyObjectCandidates;

        landmarkInfo = mapLandmarkInfo(osiSensorData.stationary_object(index), numOsiStyObjectCandidates);
        adiGeneralLandmark.GeneralLandmarksInformation = landmarkInfo;
    }

    return adiGeneralLandmarks;
}

GeneralLandmarksInformation StaticObjectMapper::mapLandmarkInfo(const osi3::DetectedStationaryObject& osiDetectedStyObj,
    int numOsiStyObjectCandidates)
{

    osi3::StationaryObject_Classification_Type osiStyObjClassType{};
    ProbabilityPercentage landmarkProbability{};
    GeneralLandmarkClassificationType landmarkClassType{};
    GeneralLandmarksInformation landmarkInfo{};
    GeneralLandmarkClassification landmarkClass{};

    for (auto cand = 0; cand < numOsiStyObjectCandidates; cand++) {

        if (osiDetectedStyObj.candidate(cand).has_classification()) {
            if (osiDetectedStyObj.candidate(cand).classification().has_type()) {

                osiStyObjClassType = osiDetectedStyObj.candidate(cand).classification().type();

                switch (osiStyObjClassType) {
                case osi3::StationaryObject_Classification_Type::StationaryObject_Classification_Type_TYPE_BARRIER: {
                    landmarkClassType = GeneralLandmarkClassificationType::kBarrel;
                    break;
                }
                case osi3::StationaryObject_Classification_Type::StationaryObject_Classification_Type_TYPE_BRIDGE: {
                    landmarkClassType = GeneralLandmarkClassificationType::kBridge;
                    break;
                }
                case osi3::StationaryObject_Classification_Type::StationaryObject_Classification_Type_TYPE_BUILDING: {
                    // TODO Warning
                    landmarkClassType = GeneralLandmarkClassificationType::kRectangularStructure;
                    break;
                }
                case osi3::StationaryObject_Classification_Type::
                    StationaryObject_Classification_Type_TYPE_OVERHEAD_STRUCTURE: {
                    // TODO Warning
                    landmarkClassType = GeneralLandmarkClassificationType::kOverheadObject;
                    break;
                }
                case osi3::StationaryObject_Classification_Type::
                    StationaryObject_Classification_Type_TYPE_EMITTING_STRUCTURE: {
                    // TODO Warning
                    landmarkClassType = GeneralLandmarkClassificationType::kLampPost;
                    break;
                }
                case osi3::StationaryObject_Classification_Type::
                    StationaryObject_Classification_Type_TYPE_RECTANGULAR_STRUCTURE: {
                    landmarkClassType = GeneralLandmarkClassificationType::kRectangularStructure;
                    break;
                }
                case osi3::StationaryObject_Classification_Type::
                    StationaryObject_Classification_Type_TYPE_VERTICAL_STRUCTURE: {
                    landmarkClassType = GeneralLandmarkClassificationType::kVerticalStructure;
                    break;
                }
                case osi3::StationaryObject_Classification_Type::
                    StationaryObject_Classification_Type_TYPE_REFLECTIVE_STRUCTURE: {
                    landmarkClassType = GeneralLandmarkClassificationType::kReflector;
                    break;
                }

                default: {
                    m_so_logger.LogError() << "Error: mapLandmarkInfo: Unknown enum entry"
                                              "mapped to GeneralLandmarkClassificationType::kUnknown";
                    landmarkClassType = GeneralLandmarkClassificationType::kUnknown;
                    break;
                }
                }
            }
        }

        if (osiDetectedStyObj.candidate(cand).has_probability()) {
            landmarkProbability = osiDetectedStyObj.candidate(cand).probability();
        }

        landmarkClass.GeneralLandMarkType = landmarkClassType;
        landmarkClass.LandmarkClassProbability = landmarkProbability;

        landmarkInfo.LandmarkTypelist.push_back(landmarkClass);
    }

    return landmarkInfo;
}
