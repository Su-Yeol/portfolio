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

#include "serviceInterfaces/roadObjectsService.h"

#include <thread>

RoadObjectsService::RoadObjectsService(ara::com::InstanceIdentifier instance_id)
    : ROSSkeleton(instance_id, ara::com::MethodCallProcessingMode::kPoll)
    ,  // Check if kEvent
    m_worker_(&RoadObjectsService::ProcessRequests, this)
{ }

RoadObjectsService::~RoadObjectsService()
{
    m_finished_ = true;
    m_worker_.join();
}

void RoadObjectsService::InitializeService()
{
    m_logger_.LogInfo() << "Initialize RoadObjects Service";
    OfferService();
}

void RoadObjectsService::SendEvent()
{
    // allocate sample
    auto allocation_result = RoadObjectInterfaceEvent.Allocate();
    if (!allocation_result) {
        m_logger_.LogError() << "RoadObjectInterfaceEvent allocation failed with error: " << allocation_result.Error();
        return;
    }

    auto l_sampleRoadObjects = std::move(allocation_result).Value();
    l_sampleRoadObjects = std::move(createRoadObjects(std::move(l_sampleRoadObjects)));

    // send sample
    auto send_result = RoadObjectInterfaceEvent.Send(std::move(l_sampleRoadObjects));
    if (send_result) {
        m_logger_.LogInfo() << "RoadObjectInterfaceEvent sent";
    } else {
        m_logger_.LogError() << "RoadObjectInterfaceEvent.Send failed with error: " << send_result.Error();
    }
}

auto RoadObjectsService::RoadObjectsCapability() -> decltype(ROSSkeleton::RoadObjectsCapability())
{
    m_logger_.LogInfo() << "Call RoadObjectsService::Capability";

    // Specification of Sensor Interfaces
    // AUTOSAR AP R21-11
    // 10.1.2 RObjectsService Capability Vector
    const bool kIsInterfaceIdOptional{true};  // Bit: 1
    const bool kIsCycleCounterOptional{true};  // Bit: 2
    const bool kIsInterfaceCycleTimeOptional{true};  // Bit: 3
    // ...
    // 172 entries

    ara::core::Vector<bool> capVector{
        kIsInterfaceIdOptional, kIsCycleCounterOptional, kIsInterfaceCycleTimeOptional
        // TODO
        // ...
    };

    RoadObjectsService::RoadObjectsCapabilityOutput output;
    output.capVector.assign(capVector.begin(), capVector.end());

    for (auto it = capVector.begin(); it != capVector.end(); it++) {
        m_logger_.LogDebug() << "Smart Camera Sensor RoadObjectsService capability: " << *it;
    }
    decltype(ROSSkeleton::RoadObjectsCapability())::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

// ProcessNextMethodCall requires base class
void RoadObjectsService::ProcessRequests()
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

RoadObjectsService::ptrRoadObjectsInterface RoadObjectsService::createRoadObjects(
    RoadObjectsService::ptrRoadObjectsInterface pRoadObjectsInterface)
{
    // Fill the interface wit dummy data
    DummyDataGenerator generator;
    pRoadObjectsInterface->RoadObjectInterfaceHeader = generator.getInterfaceHeader();

    ara::adi::sensoritf::RoadSurface roadSurface;
    roadSurface.RoadType = ara::adi::sensoritf::RoadType::kOffRoad;
    roadSurface.NumberOfValidRoadSurfaceClassifications = 3;
    ara::adi::sensoritf::RoadSurfaceClassificationsVector surfaceClassificationVector
        = ara::adi::sensoritf::RoadSurfaceClassificationsVector();
    ara::adi::sensoritf::RoadSurfaceClassification surfaceClassification;
    surfaceClassification.SurfaceType = ara::adi::sensoritf::RoadSurfaceClassificationType::kRomanRoad;
    surfaceClassification.RoadSurfaceClassificationTypeConfidence = 0.2;
    surfaceClassificationVector.push_back(surfaceClassification);
    roadSurface.ValidRoadSurfaceClassifications = surfaceClassificationVector;
    roadSurface.RoadSurfaceRoughness = 9.1;  // (optional)
    roadSurface.NumberOfValidRoadSurfaceConditionClassifications = 2;  // (optional)
    ara::adi::sensoritf::RoadSurfaceConditionClassificationsVector conditionClassificationVector
        = ara::adi::sensoritf::RoadSurfaceConditionClassificationsVector();
    ara::adi::sensoritf::RoadSurfaceConditionClassification conditionClassification;
    conditionClassification.RoadConditionType = ara::adi::sensoritf::RoadConditionClassificationType::kSnow;
    conditionClassification.RoadSurfaceConditionClassificationTypeConfidence = 0.8;
    conditionClassificationVector.push_back(conditionClassification);
    roadSurface.ValidRoadSurfaceConditionClassifications = conditionClassificationVector;
    roadSurface.TrackQuality = 66;  // (optional)
    roadSurface.PMOMeasurementStatusObjectLevel
        = ara::adi::sensoritf::MeasurementStatus::kPartiallyMeasured;  // (optional)
    pRoadObjectsInterface->RoadSurfaceObjectList = roadSurface;  // (optional)

    ara::adi::sensoritf::RoadMarkings roadMarkings;
    roadMarkings.RecognizedRoadMarkingsCapability = 2;  // (optional)
    roadMarkings.RecognizedRoadMarkingsStatus = ara::adi::sensoritf::RecognizedStatus::kUnknown;  // (optional)
    roadMarkings.NumberOfValidRoadMarkings = 16;
    ara::adi::sensoritf::RoadMarkingVector roadMarkingVector;
    ara::adi::sensoritf::RoadMarking marking;

    ara::adi::sensoritf::ObjectStatus markingStatus;
    markingStatus.ExistenceProbabilityObjectLevel = 0.5;
    markingStatus.ObjectID = 3;
    markingStatus.GroupingObjectID = 34;  // (optional)
    markingStatus.Age = 123456;
    markingStatus.NumberOfValidObservationsObjectLevel = 34;  // (optional)
    ara::adi::sensoritf::ValidObservationVector markingObservationVector
        = ara::adi::sensoritf::ValidObservationVector();
    ara::adi::sensoritf::ValidObservation markingObservation;
    markingObservation.TimeStampReferenceObjectLevel = 234567;
    markingObservation.ObservationStatusObjectLevel = ara::adi::sensoritf::ObservationStatus::kObservationFalse;
    markingObservationVector.push_back(markingObservation);
    markingStatus.ValidObservations = markingObservationVector;  // (optional)
    markingStatus.TrackQuality = 2;  // (optional)
    markingStatus.MeasurementStatusObjectLevel = ara::adi::sensoritf::MeasurementStatus::kMeasured;
    marking.RoadMarkingsStatus = markingStatus;

    ara::adi::sensoritf::RoadMarkingsInformation markingInformation;
    markingInformation.NumberOfValidRoadMarkingClassifications = 2;
    ara::adi::sensoritf::RoadMarkingClassificationVector markingClassificationVector
        = ara::adi::sensoritf::RoadMarkingClassificationVector();
    ara::adi::sensoritf::RoadMarkingClassification markingClassification;
    markingClassification.RoadMarkingType = ara::adi::sensoritf::RoadMarkingType::kDoubleLineSolid;
    markingClassification.RoadMarkingTypeConfidence = 0.3;
    markingClassification.RoadObjectLaneAssociation
        = ara::adi::sensoritf::RoadObjectLaneAssociation::kEgoLeft1Lane;  // (optional)
    markingClassification.RoadObjectLaneAssociationConfidence = 0.4;  // (optional)
    markingClassification.ArrowOrientation = 45.1;  // (optional)
    markingClassification.ArrowDirect = ara::adi::sensoritf::ArrowDirection::kTurningPointLeft;  // (optional)
    markingClassification.NumberOfValidSignClassifications = 34;  // (optional)
    ara::adi::sensoritf::ValidSignClassificationVector signClassificationVector
        = ara::adi::sensoritf::ValidSignClassificationVector();
    ara::adi::sensoritf::SignClassification signClassification;
    signClassification.SignClassificationType
        = ara::adi::sensoritf::SignClassificationType::kHeightLimitSign;  // (optional)
    signClassification.SignClassificationTypeConfidence = 0.7;  // (optional)
    signClassification.SignValue = 3.3;  // (optional)
    signClassification.SignValueUnit = ara::adi::sensoritf::SignValueUnit::kMilePerHour;  // (optional)
    signClassification.SignState = ara::adi::sensoritf::SignState::kSwitchedOff;  // (optional)
    signClassificationVector.push_back(signClassification);
    markingClassification.ValidSignClassificationsList = signClassificationVector;  // (optional)
    ara::adi::sensoritf::ColourTone markingColour;
    markingColour.ValidColourModel.push_back(5);
    markingColour.ColourToneConfidenceObjectLevel = 0.4;  // (optional)
    markingClassification.ColourTone = markingColour;  // (optional)
    markingClassificationVector.push_back(markingClassification);
    markingInformation.ValidRoadMarkingClassificationsList = markingClassificationVector;
    marking.RoadMarkingsInformation = markingInformation;

    ara::adi::sensoritf::Polynomials markingPolynomials;
    markingPolynomials.NumberOfValidConnections = 3;
    ara::adi::sensoritf::ValidConnectionVector connectionVector = ara::adi::sensoritf::ValidConnectionVector();
    ara::adi::sensoritf::ValidConnection connection;
    connection.Type = ara::adi::sensoritf::ConnectionType::kInterconnection;
    connection.ConnectionID = 3;
    connectionVector.push_back(connection);
    markingPolynomials.ValidConnectionList = connectionVector;
    markingPolynomials.NumberOfValidPolynomials = 1;
    ara::adi::sensoritf::ValidPolynomialVector polynomialVector = ara::adi::sensoritf::ValidPolynomialVector();
    ara::adi::sensoritf::ValidPolynomial polynomial;
    ara::adi::sensoritf::PolynomialCoefficient coefficient;
    coefficient.C0 = 0.1;
    coefficient.C1 = 0.2;
    coefficient.C2 = 0.3;
    coefficient.C3 = 0.4;
    polynomial.PolynomialCoefficientY = coefficient;
    polynomial.PolynomialCoefficientZ = coefficient;
    ara::adi::sensoritf::PolynomialRangeX rangeX;
    rangeX.Start = 45;
    rangeX.End = 100;
    polynomial.PolynomialRange = rangeX;
    // @TODO fill optional polynomial types
    polynomialVector.push_back(polynomial);
    markingPolynomials.ValidPolynomialsList = polynomialVector;
    marking.RoadMarkingsPolynomials = markingPolynomials;  // (optional)

    ara::adi::sensoritf::Polylines markingPolylines;
    markingPolylines.NumberOfValidConnections = 1;
    markingPolylines.ValidConnectionList = connectionVector;
    markingPolylines.InterpolationMethod = ara::adi::sensoritf::PolylineInterpolationMethod::kSpline;
    markingPolylines.NumberOfValidPolylines = 4;
    ara::adi::sensoritf::ValidPolylineVector polylineVector = ara::adi::sensoritf::ValidPolylineVector();
    ara::adi::sensoritf::ValidPolyline polyline;
    polyline.NumberOfValidVertices = 3;
    ara::adi::sensoritf::ValidVerticeVector verticeVector = ara::adi::sensoritf::ValidVerticeVector();
    ara::adi::sensoritf::ValidVertice vertice;
    ara::adi::sensoritf::Point3D verticePoint;
    verticePoint.x = 1;
    verticePoint.y = 2;
    verticePoint.z = 3;
    vertice.VertexPoint = verticePoint;
    ara::adi::sensoritf::Point3DError verticePointError;
    verticePointError.xError = 1;
    verticePointError.yError = 2;
    verticePointError.zError = 3;
    vertice.VertexPointError = verticePointError;
    // @TODO fill in optional polyline types
    verticeVector.push_back(vertice);
    polyline.ValidVerticesList = verticeVector;
    polylineVector.push_back(polyline);
    markingPolylines.ValidPolylinesList = polylineVector;
    marking.RoadMarkingsPolylines = markingPolylines;  // (optional)

    roadMarkingVector.push_back(marking);
    roadMarkings.ValidRoadMarkings = roadMarkingVector;
    pRoadObjectsInterface->RoadMarkingObjectList = roadMarkings;  // (optional)

    ara::adi::sensoritf::RoadBoundaries roadBoundaries;
    roadBoundaries.RecognizedRoadBoundariesCapability = 23;  // (optional)
    roadBoundaries.RecognizedRoadBoundariesStatus = ara::adi::sensoritf::RecognizedStatus::kPreLimits;  // (optional)
    roadBoundaries.NumberOfValidRoadBoundaries = 2;
    ara::adi::sensoritf::RoadBoundaryVector boundaryVector = ara::adi::sensoritf::RoadBoundaryVector();
    ara::adi::sensoritf::ValidRoadBoundary boundary;
    boundary.RoadBoundariesStatus = markingStatus;
    ara::adi::sensoritf::RoadBoundariesInformation boundaryInformation;
    boundaryInformation.NumberOfValidRoadBoundaryClassifications = 2;
    ara::adi::sensoritf::ValidRoadBoundaryClassificationVector boundaryClassificationVector;
    ara::adi::sensoritf::ValidRoadBoundaryClassification boundaryClassification;
    boundaryClassification.RoadBoundaryType = ara::adi::sensoritf::RoadBoundaryType::kGuardrail;
    boundaryClassification.RoadBoundaryTypeConfidence = 0.4;
    boundaryClassification.RoadObjectLaneAssociation = ara::adi::sensoritf::RoadObjectLaneAssociation::kRightRoadEdge;
    boundaryClassification.RoadObjectLaneAssociationConfidence = 0.5;
    ara::adi::sensoritf::ColourTone boundaryColour;
    boundaryColour.ValidColourModel.push_back(99);
    boundaryColour.ColourToneConfidenceObjectLevel = 0.9;  // (optional)
    boundaryClassification.ColourTone = boundaryColour;  // (optional)
    boundaryClassificationVector.push_back(boundaryClassification);
    boundaryInformation.ValidRoadBoundaryClassificationsList = boundaryClassificationVector;
    boundary.RoadBoundariesInformation = boundaryInformation;
    boundary.RoadBoundariesPolynomials = markingPolynomials;
    boundary.RoadBoundariesPolylines = markingPolylines;
    // @TODO fill in own boundary data
    boundaryVector.push_back(boundary);
    roadBoundaries.RoadBoundaryList = boundaryVector;
    pRoadObjectsInterface->RoadBoundariesObjectList = roadBoundaries;  // (optional)

    return pRoadObjectsInterface;
}
