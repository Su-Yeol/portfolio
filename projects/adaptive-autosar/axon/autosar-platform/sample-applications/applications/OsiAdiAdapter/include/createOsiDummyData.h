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

#ifndef OSIDUMMYDATA_H_
#define OSIDUMMYDATA_H_

#include "osi3/osi_sensordata.pb.h"
#include <gtest/gtest.h>
#include <ara/log/logger.h>

struct osiVector3DValue
{
    double x, y, z;
};
struct osiOrientation3DValue
{
    double r, p, y;
};
struct osiDimension3DValue
{
    double h, l, w;
};
struct osiSpherical3DValue
{
    double a, d, e;
};
struct osiBaseMovingValue
{
    osiVector3DValue osiAccValue, osiPosValue, osiVelValue;
    osiDimension3DValue osiDimValue;
    osiOrientation3DValue osiOrientValue;
};
struct osiBaseStationaryValue
{
    osiVector3DValue osiPosValue;
    osiDimension3DValue osiDimValue;
    osiOrientation3DValue osiOrientValue;
};
struct osiMountingPoseValue
{
    osiVector3DValue osiMountingPosition;
    osiOrientation3DValue osiMountingOrientation;
};
struct osiSensorHeaderValues
{
    osiVector3DValue osiPosition, osiPositionRmse;
    osiOrientation3DValue osiOrientation, osiOrientationRmse;
    int osiSensorId;
    uint64_t osiCycleCounter;
    osi3::SensorDetectionHeader_DataQualifier osiSDataQualifier;
    uint32_t numDetections;
};
struct osiVersionValue
{
    uint32_t major, minor, patch;
};
struct osiDetectedItemHeaderValue
{

    int osiGroundtruthID;
    int osiSensorID;
    int osiTrackingID;
    double osiExistProb;
    osi3::DetectedItemHeader_MeasurementState osiDIHMeasurement;
};
struct osiDetectedEntityHeaderValue
{

    uint64_t osiCycleCounter;
    osi3::DetectedEntityHeader_DataQualifier osiDataQualifier;
};

osi3::Vector3d setVector3D(const osiVector3DValue& osiVector3DValue);
osi3::Orientation3d setOrientation3D(const osiOrientation3DValue& osiOrientation3DValue);
osi3::Dimension3d setDimension3D(const osiDimension3DValue& osiDimension3DValue);
osi3::Spherical3d setSpherical3D(const osiSpherical3DValue& osiSpherical3DValue);

osi3::DetectedLane createDetectedLaneData();
osi3::DetectedItemHeader setDetectedItemHeader(const osiDetectedItemHeaderValue& osiDetectedItemHeaderValue);
osi3::BaseMoving setBaseMoving(const osiBaseMovingValue& osiBaseMovingValue);
osi3::BaseStationary setBaseStationary(const osiBaseStationaryValue& osiBaseStationaryValue);
osi3::DetectedLaneBoundary createDetectedLaneBoundary();
osi3::DetectedMovingObject createMovingObject();
osi3::DetectedRoadMarking createRoadMarking();
osi3::DetectedStationaryObject createStationaryObject();
osi3::DetectedTrafficLight createTrafficLight();
osi3::DetectedTrafficSign createTrafficSign();
osi3::MountingPosition createMountingPosition(const osiMountingPoseValue& osiMountPoseValue);
osi3::CameraDetection createCameraDetection();
osi3::SensorDetectionHeader createSensorHeader(const osiSensorHeaderValues& sensorHeaderValues);
osi3::DetectedEntityHeader createDetectedEntityHeader(const osiDetectedEntityHeaderValue& osiDetectedEntityHeaderValue);
osi3::InterfaceVersion createInterfaceVersion(const osiVersionValue& osiInterfaceVer);
osi3::LogicalDetection createLogicalDetection();
osi3::SensorData createOsiDummyData();

#endif
