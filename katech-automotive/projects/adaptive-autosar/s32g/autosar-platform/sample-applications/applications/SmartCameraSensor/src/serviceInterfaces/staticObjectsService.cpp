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

#include "serviceInterfaces/staticObjectsService.h"

#include <thread>

StaticObjectsService::StaticObjectsService(ara::com::InstanceIdentifier instance_id)
    : SOSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&StaticObjectsService::ProcessRequests, this)
{ }

StaticObjectsService::~StaticObjectsService()
{
    m_finished_ = true;
    m_worker_.join();
}

void StaticObjectsService::InitializeService()
{
    m_logger_.LogInfo() << "Initialize StaticObjects Service";
    OfferService();
}

void StaticObjectsService::SendEvent()
{
    // allocate sample
    auto allocation_result = StaticObjectInterfaceEvent.Allocate();
    if (!allocation_result) {
        m_logger_.LogError() << "StaticObjectInterfaceEvent allocation failed with error: "
                             << allocation_result.Error();
        return;
    }

    auto l_sampleStaticObjects = std::move(allocation_result).Value();
    l_sampleStaticObjects = std::move(createStaticObjects(std::move(l_sampleStaticObjects)));

    // send sample
    auto send_result = StaticObjectInterfaceEvent.Send(std::move(l_sampleStaticObjects));
    if (send_result) {
        m_logger_.LogInfo() << "StaticObjectInterfaceEvent sent";
    } else {
        m_logger_.LogError() << "StaticObjectInterfaceEvent.Send failed with error: " << send_result.Error();
    }
}

auto StaticObjectsService::StaticObjectsCapability() -> decltype(SOSSkeleton::StaticObjectsCapability())
{
    m_logger_.LogInfo() << "Call StaticObjectsService::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.1.3 SObjectsService Capability Vector
    const bool kIsInterfaceIdOptional{true};  // Bit: 1
    const bool kIsCycleCounterOptional{true};  // Bit: 2
    const bool kIsInterfaceCycleTimeOptional{true};  // Bit: 3
    // ...
    // 155 entries

    ara::core::Vector<bool> capVector{
        kIsInterfaceIdOptional, kIsCycleCounterOptional, kIsInterfaceCycleTimeOptional
        // TODO
        // ...
    };

    StaticObjectsService::StaticObjectsCapabilityOutput output;
    output.capVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor StaticObjectsService capability: " << *it;
    }
    decltype(SOSSkeleton::StaticObjectsCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void StaticObjectsService::ProcessRequests()
{
    while (!m_finished_) {
        std::chrono::time_point<std::chrono::system_clock> deadline
            = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
        auto request_finished = ProcessNextMethodCall();
        if (request_finished.wait_until(deadline) != ara::core::future_status::ready) {
            m_logger_.LogFatal() << "Request took too long :S";
        } else {
            if (!m_finished_) {
                std::this_thread::sleep_until(deadline);
            }
        }
    }
}

StaticObjectsService::ptrStaticObjectsInterface StaticObjectsService::createStaticObjects(
    StaticObjectsService::ptrStaticObjectsInterface pStaticObjectsInterface)
{
    // Fill the interface with dummy data
    DummyDataGenerator generator;
    pStaticObjectsInterface->StaticObjectInterfaceHeader = generator.getInterfaceHeader();

    // GeneralLandmarks (optional)
    ara::adi::sensoritf::GeneralLandmarks generalLandmarks;
    generalLandmarks.RecognizedGeneralLandmarksCapability = 3;  // (optional)
    generalLandmarks.RecognizedGeneralLandmarksStatus
        = ara::adi::sensoritf::RecognizedStatus::kPreLimits;  // 0x01 // (optional)
    generalLandmarks.NumberOfValidGeneralLandmarks = 10;
    ara::adi::sensoritf::GeneralLandmarkVector landmarkVector = ara::adi::sensoritf::GeneralLandmarkVector();
    ara::adi::sensoritf::GeneralLandmark landmark;
    ara::adi::sensoritf::ObjectStatus status;
    status.ExistenceProbabilityObjectLevel = 0.4;
    status.ObjectID = 5;
    status.GroupingObjectID = 7;  // (optional)
    status.Age = 123456;
    status.NumberOfValidObservationsObjectLevel = 8;  // (optional)
    ara::adi::sensoritf::ValidObservationVector observationVector = ara::adi::sensoritf::ValidObservationVector();
    ara::adi::sensoritf::ValidObservation landmarkObservation;
    landmarkObservation.TimeStampReferenceObjectLevel = 123456;
    landmarkObservation.ObservationStatusObjectLevel = ara::adi::sensoritf::ObservationStatus::kUnknown;
    observationVector.push_back(landmarkObservation);
    status.ValidObservations = observationVector;  // (optional)
    status.TrackQuality = 12;  // (optional)
    status.MeasurementStatusObjectLevel = ara::adi::sensoritf::MeasurementStatus::kPredicted;
    landmark.GeneralLandmarksStatus = status;
    ara::adi::sensoritf::GeneralLandmarksInformation information;
    landmark.GeneralLandmarksInformation = information;
    ara::adi::sensoritf::GeneralLandmarksPosition position;
    landmark.GeneralLandmarksPos = position;
    ara::adi::sensoritf::BoundingBox boundingBox;
    landmark.GeneralLandmarksBoundingBox = boundingBox;  // (optional)
    landmarkVector.push_back(landmark);
    generalLandmarks.GeneralLandmarksList = landmarkVector;
    pStaticObjectsInterface->StaticObjectGeneralLandmarks = generalLandmarks;  // (optional)

    ara::adi::sensoritf::TrafficSigns trafficSigns;
    trafficSigns.RecognizedTrafficSignsCapability = 12;  // (optional)
    trafficSigns.RecognizedTrafficSignsStatus = ara::adi::sensoritf::RecognizedStatus::kPreLimits;  // (optional)
    trafficSigns.NumberOfValidTrafficSigns = 16;
    ara::adi::sensoritf::ValidTrafficSignVector signVector = ara::adi::sensoritf::ValidTrafficSignVector();
    ara::adi::sensoritf::TrafficSign sign;
    ara::adi::sensoritf::ObjectStatus signStatus;
    signStatus.ExistenceProbabilityObjectLevel = 0.8;
    signStatus.ObjectID = 3;
    signStatus.GroupingObjectID = 1;  // (optional)
    signStatus.Age = 123456;
    signStatus.NumberOfValidObservationsObjectLevel = 8;  // (optional)
    ara::adi::sensoritf::ValidObservationVector trafficObservationVector
        = ara::adi::sensoritf::ValidObservationVector();
    ara::adi::sensoritf::ValidObservation trafficObservation;
    trafficObservation.TimeStampReferenceObjectLevel = 123456;
    trafficObservation.ObservationStatusObjectLevel = ara::adi::sensoritf::ObservationStatus::kObservationFalse;
    trafficObservationVector.push_back(trafficObservation);
    signStatus.ValidObservations = trafficObservationVector;  // (optional)
    signStatus.TrackQuality = 10;  // (optional)
    signStatus.MeasurementStatusObjectLevel = ara::adi::sensoritf::MeasurementStatus::kMeasured;
    sign.TrafficSignsStatus = signStatus;

    ara::adi::sensoritf::TrafficSignsInformation signInformation;
    signInformation.NumberOfValidSignClassifications = 3;
    ara::adi::sensoritf::ValidMainSignClassificationVector mainClassificationVector
        = ara::adi::sensoritf::ValidMainSignClassificationVector();
    ara::adi::sensoritf::MainSignClassification mainClassification;
    mainClassification.MainSignClassificationType = ara::adi::sensoritf::SignClassificationType::kSpeedLimitSign;
    mainClassification.SignClassificationTypeConfidence = 0.9;
    mainClassification.SignValue = 5.1;
    mainClassification.MSignUnit = ara::adi::sensoritf::SignValueUnit::kMetricTon;
    mainClassification.SignState = ara::adi::sensoritf::SignState::kPartlyOutOfService;
    mainClassification.MSignGeometry = ara::adi::sensoritf::SignGeometry::kOctagon;  // (optional)
    mainClassificationVector.push_back(mainClassification);
    signInformation.ValidMainSignClassificationsList = mainClassificationVector;
    signInformation.NumberOfValidLaneRelevanceClassifications = 6;
    ara::adi::sensoritf::ValidLaneRelevanceClassificationVector laneClassificationVector
        = ara::adi::sensoritf::ValidLaneRelevanceClassificationVector();
    ara::adi::sensoritf::LaneRelevanceClassification laneClassification;
    laneClassification.LaneRelevanceClassificationType
        = ara::adi::sensoritf::LaneRelevanceClassificationType::kOnTrackAndNextLaneRight;
    laneClassification.LaneRelevanceClassificationTypeConfidence = 0.6;
    laneClassificationVector.push_back(laneClassification);
    signInformation.ValidLaneRelevanceClassificationList = laneClassificationVector;
    sign.TrafficSignsInformation = signInformation;

    ara::adi::sensoritf::ColourTone signColour;
    signColour.ValidColourModel.push_back(6);
    signColour.ColourToneConfidenceObjectLevel = 0.9;  // (optional)
    sign.ColourTone = signColour;

    ara::adi::sensoritf::TrafficSignsPosition signPosition;
    ara::adi::sensoritf::Point3D signPoint;
    signPoint.x = 3;
    signPoint.y = 2;
    signPoint.z = 1;  // (optional)
    signPosition.Position = signPoint;
    ara::adi::sensoritf::Point3DError signPointError;
    signPointError.xError = 3;
    signPointError.yError = 2;
    signPointError.zError = 1;  // (optional)
    signPosition.PositionError = signPointError;
    sign.TrafficSignsPosition = signPosition;

    ara::adi::sensoritf::TrafficSignsSupplementarySigns supplementSigns;
    supplementSigns.NumberOfValidTrafficSupplementarySigns = 7;
    ara::adi::sensoritf::TrafficSignsSupplementarySignVector supplementVector
        = ara::adi::sensoritf::TrafficSignsSupplementarySignVector();
    ara::adi::sensoritf::TrafficSignsSupplementarySign suppSign;
    ara::adi::sensoritf::SubObjectStatus suppStatus;
    suppStatus.ExistenceProbabilityObjectLevel = 0.8;
    suppStatus.Age = 123456;
    suppStatus.NumberOfValidObservationsObjectLevel = 34;  // (optional)
    ara::adi::sensoritf::ValidObservationVector suppObservationVector = ara::adi::sensoritf::ValidObservationVector();
    ara::adi::sensoritf::ValidObservation suppObservation;
    suppObservation.TimeStampReferenceObjectLevel = 234567;
    suppObservation.ObservationStatusObjectLevel = ara::adi::sensoritf::ObservationStatus::kObservationTrue;
    suppObservationVector.push_back(suppObservation);
    suppStatus.ValidObservations = suppObservationVector;  // (optional)
    suppStatus.TrackQuality = 14;  // (optional)
    suppStatus.MeasurementStatusObjectLevel = ara::adi::sensoritf::MeasurementStatus::kMeasured;
    suppSign.TrafficSignsSupplementarySignsStatus = suppStatus;

    ara::adi::sensoritf::TrafficSignsSupplementarySignsInformation suppInformation;
    suppInformation.NumberOfValidSupplementarySignClassifications = 3;
    ara::adi::sensoritf::ValidSupplementarySignClassificationVector suppSignClassificationVector
        = ara::adi::sensoritf::ValidSupplementarySignClassificationVector();
    ara::adi::sensoritf::SupplementarySignClassification suppSignClassification;
    suppSignClassification.SupplementarySignClassificationType
        = ara::adi::sensoritf::SupplementarySignClassificationType::kFrost;
    suppSignClassification.SupplementarySignClassificationTypeConfidence = 0.4;
    suppSignClassification.SSignValue = 6.5;
    suppSignClassification.SSignUnit = ara::adi::sensoritf::SignValueUnit::kKilometre;
    suppSignClassification.SignState = ara::adi::sensoritf::SignState::kOutOfView;
    suppSignClassificationVector.push_back(suppSignClassification);
    suppInformation.ValidSupplementarySignClassifications = suppSignClassificationVector;
    suppSign.TrafficSignsSupplementarySignsInformation = suppInformation;

    ara::adi::sensoritf::ColourTone suppColour;
    suppColour.ValidColourModel.push_back(78);
    suppColour.ColourToneConfidenceObjectLevel = 0.76;  // (optional)
    suppSign.TrafficSignsSupplementarySignsColourTone = suppColour;

    ara::adi::sensoritf::TrafficSignsSupplementarySignsPosition suppPosition;
    suppPosition.SSRelativePosition = ara::adi::sensoritf::RelativePosition::kRight;
    suppPosition.RelativePosOrder = 8;
    suppSign.TrafficSignsSupplementarySignsPos = suppPosition;

    supplementVector.push_back(suppSign);
    supplementSigns.SSignList = supplementVector;
    sign.TrafficSignsSupplementarySigns = supplementSigns;

    signVector.push_back(sign);
    trafficSigns.TrafficSignsList = signVector;
    pStaticObjectsInterface->StaticObjectTrafficSigns = trafficSigns;  // (optional)

    ara::adi::sensoritf::TrafficLights trafficLights;
    trafficLights.RecognizedTrafficLightsCapability = 45;  // (optional)
    trafficLights.RecognizedTrafficLightsStatus = ara::adi::sensoritf::RecognizedStatus::kLimited;  // (optional)
    trafficLights.NumberOfValidTrafficLights = 6;
    ara::adi::sensoritf::ValidTrafficLightVector lightVector = ara::adi::sensoritf::ValidTrafficLightVector();
    ara::adi::sensoritf::TrafficLight light;
    ara::adi::sensoritf::ObjectStatus lightStatus;
    lightStatus.ExistenceProbabilityObjectLevel = 0.9;
    lightStatus.ObjectID = 9;
    lightStatus.GroupingObjectID = 4;  // (optional)
    lightStatus.Age = 123456;
    lightStatus.NumberOfValidObservationsObjectLevel = 2;  // (optional)
    ara::adi::sensoritf::ValidObservationVector lightObservationVector = ara::adi::sensoritf::ValidObservationVector();
    ara::adi::sensoritf::ValidObservation lightObservation;
    lightObservation.TimeStampReferenceObjectLevel = 123456;
    lightObservation.ObservationStatusObjectLevel = ara::adi::sensoritf::ObservationStatus::kUnknown;
    lightObservationVector.push_back(lightObservation);
    lightStatus.ValidObservations = lightObservationVector;  // (optional)
    lightStatus.TrackQuality = 3;  // (optional)
    lightStatus.MeasurementStatusObjectLevel = ara::adi::sensoritf::MeasurementStatus::kNew;
    light.TrafficLightsStatus = lightStatus;

    ara::adi::sensoritf::TrafficLightsInformation lightInformation;
    lightInformation.NumberOfValidStructureLightClassifications = 5;
    ara::adi::sensoritf::ValidStructureLightClassificationsVector lightClassificationVector
        = ara::adi::sensoritf::ValidStructureLightClassificationsVector();
    ara::adi::sensoritf::StructureLightClassification lightClassification;
    lightClassification.StructureLightClassificationType
        = ara::adi::sensoritf::StructureLightClassificationType::kHorizontal3;
    lightClassification.StructureLightClassificationTypeConfidence = 0.4;
    lightClassificationVector.push_back(lightClassification);
    lightInformation.ValidStructureLightClassificationsList = lightClassificationVector;
    light.StructureLightsInformation = lightInformation;

    ara::adi::sensoritf::TrafficLightsPosition lightPosition;
    ara::adi::sensoritf::Point3D lightPoint;
    lightPoint.x = 1;
    lightPoint.y = 2;
    lightPoint.z = 3;  // (optional)
    lightPosition.PositionObjectLevel = lightPoint;
    ara::adi::sensoritf::Point3DError lightPointError;
    lightPointError.xError = 0.1;
    lightPointError.yError = 0.2;
    lightPointError.zError = 0.3;  // (optional)
    lightPosition.PositionObjectLevelError = lightPointError;
    ara::adi::sensoritf::Orientation3D lightOrientation;
    lightOrientation.Pitch = 4;
    lightOrientation.Roll = 5;
    lightOrientation.Yaw = 6;
    lightPosition.Orientation = lightOrientation;
    ara::adi::sensoritf::Orientation3DError lightOrientationError;  // (optional)
    lightOrientationError.PitchError = 0.4;
    lightOrientationError.RollError = 0.5;
    lightOrientationError.YawError = 0.6;
    lightPosition.OrientationError = lightOrientationError;  // (optional)
    lightPosition.ReferencePoint = ara::adi::sensoritf::ReferencePoint::kMidsideLeftBottom;  // (optional)
    lightPosition.MinimumVisibilityDistance = 45;
    light.TrafficLightsPosition = lightPosition;

    ara::adi::sensoritf::BoundingBox lightBoundingBox;
    ara::adi::sensoritf::DimensionBox lightDimension;
    lightDimension.Width = 0.9;
    lightDimension.Length = 9.8;
    lightDimension.Height = 0.1;  // (optional)
    lightBoundingBox.BoxDimension = lightDimension;
    ara::adi::sensoritf::DimensionBoxError lightDimensionError;
    lightDimensionError.Width = 9;
    lightDimensionError.Length = 0.98;
    lightDimensionError.Height = 1;  // (optional)
    lightBoundingBox.BoxError = lightDimensionError;  // (optional)
    light.TrafficLightsBoundingBox = lightBoundingBox;  // (optional)

    ara::adi::sensoritf::TrafficLightSpots lightSpots;
    lightSpots.TotalNumberOfTrafficLightSpots = 1;  // (optional)
    lightSpots.TotalNumberOfTrafficLightSpotsConfidence = 0.9;  // (optional)
    lightSpots.NumberOfValidTrafficLightSpots = 9;
    ara::adi::sensoritf::TrafficLightSpotVector lightSpotVector = ara::adi::sensoritf::TrafficLightSpotVector();
    ara::adi::sensoritf::TrafficLightSpot lightSpot;

    ara::adi::sensoritf::SubObjectStatus spotStatus;
    spotStatus.ExistenceProbabilityObjectLevel = 0.1;
    spotStatus.Age = 123456;
    spotStatus.NumberOfValidObservationsObjectLevel = 2;  // (optional)
    ara::adi::sensoritf::ValidObservationVector spotObservationVector = ara::adi::sensoritf::ValidObservationVector();
    ara::adi::sensoritf::ValidObservation spotObservation;
    spotObservation.TimeStampReferenceObjectLevel = 234567;
    spotObservation.ObservationStatusObjectLevel = ara::adi::sensoritf::ObservationStatus::kObservationFalse;
    spotObservationVector.push_back(spotObservation);
    spotStatus.ValidObservations = spotObservationVector;  // (optional)
    spotStatus.TrackQuality = 9;  // (optional)
    spotStatus.MeasurementStatusObjectLevel = ara::adi::sensoritf::MeasurementStatus::kPredictedOccluded;
    lightSpot.TrafficLightsSpotsStatus = spotStatus;

    ara::adi::sensoritf::TrafficLightsSpotsInformation spotInformation;
    spotInformation.NumberOfValidLightShapeClassifications = 5;
    ara::adi::sensoritf::LightShapeClassificationVector spotClassificationVector
        = ara::adi::sensoritf::LightShapeClassificationVector();
    ara::adi::sensoritf::LightShapeClassification spotClassification;
    spotClassification.LightShapeClassificationType
        = ara::adi::sensoritf::LightShapeClassificationType::kArrowStraightAheadLeft;
    spotClassification.LightShapeClassificationTypeConfidence = 0.7;
    spotClassification.LightShapeValue = 4;  // (optional)
    spotClassificationVector.push_back(spotClassification);
    spotInformation.ValidLightShapeClassificationList = spotClassificationVector;
    lightSpot.TrafficLightsSpotsInformation = spotInformation;

    ara::adi::sensoritf::TrafficLightsSpotsColour spotColour;
    spotColour.NumberOfValidColourClassifications = 4;
    ara::adi::sensoritf::ColourClassificationVector spotColourClassificationVector
        = ara::adi::sensoritf::ColourClassificationVector();
    ara::adi::sensoritf::ColourClassification spotColourClassification;
    spotColourClassification.ColourClassificationType = ara::adi::sensoritf::ColourClassificationType::kRed;
    spotColourClassification.ColourClassificationTypeConfidence = 0.77;
    spotColourClassificationVector.push_back(spotColourClassification);
    spotColour.ValidColourClassificationVectorList = spotColourClassificationVector;
    spotColour.NumberOfValidLightModeClassifications = 7;
    ara::adi::sensoritf::LightModeClassificationVector spotModeClassificationVector
        = ara::adi::sensoritf::LightModeClassificationVector();
    ara::adi::sensoritf::LightModeClassification spotModeClassification;
    spotModeClassification.LightModeClassificationType = ara::adi::sensoritf::LightModeClassificationType::kCounting;
    spotModeClassification.LightModeClassificationTypeConfidence = 0.2;
    spotModeClassificationVector.push_back(spotModeClassification);
    spotColour.ValidLightModeClassificationVectorList = spotModeClassificationVector;
    lightSpot.TrafficLightsSpotsColour = spotColour;

    ara::adi::sensoritf::TrafficLightsSpotsPosition spotPosition;
    ara::adi::sensoritf::Point3D spotPoint;
    spotPoint.x = 1;
    spotPoint.y = 2;
    spotPoint.z = 3;  // (optional)
    spotPosition.PositionObjectLevel = spotPoint;
    ara::adi::sensoritf::Point3DError spotPointError;
    spotPointError.xError = 0.1;
    spotPointError.yError = 0.2;
    spotPointError.zError = 0.3;  // (optional)
    spotPosition.PositionObjectLevelError = spotPointError;  // (optional)
    spotPosition.NumberOfValidLaneRelevanceClassifications = 17;
    ara::adi::sensoritf::ValidLaneRelevanceClassificationVector laneRelevanceClassificationVector
        = ara::adi::sensoritf::ValidLaneRelevanceClassificationVector();
    ara::adi::sensoritf::LaneRelevanceClassification laneRelevanceClassification;
    laneRelevanceClassification.LaneRelevanceClassificationType
        = ara::adi::sensoritf::LaneRelevanceClassificationType::kMostRightLane;
    laneRelevanceClassification.LaneRelevanceClassificationTypeConfidence = 0.4;
    laneRelevanceClassificationVector.push_back(laneRelevanceClassification);
    spotPosition.ValidLaneRelevanceClassificationList = laneRelevanceClassificationVector;
    lightSpot.TrafficLightsSpotsPosition = spotPosition;

    lightSpotVector.push_back(lightSpot);
    lightSpots.ValidTrafficLightSpotList = lightSpotVector;
    light.TrafficLightsSpots = lightSpots;

    lightVector.push_back(light);
    trafficLights.TrafficLightList = lightVector;
    pStaticObjectsInterface->StaticObjectTrafficLights = trafficLights;  // (optional)

    return pStaticObjectsInterface;
}
