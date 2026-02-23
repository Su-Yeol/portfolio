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

#include <gtest/gtest.h>
#include <gtest/internal/gtest-port.h>
#include <gmock/gmock.h>
#include <string.h>

#include "createOsiDummyData.h"
#include "osi3/osi_sensordata.pb.h"

#include "adiMappers/CameraDetectionsMapper.h"
#include "adiMappers/CameraFeaturesMapper.h"
#include "adiMappers/LidarDetectionsMapper.h"
#include "adiMappers/PMObjectsMapper.h"
#include "adiMappers/RadarDetectionsMapper.h"
#include "adiMappers/RoadObjectsMapper.h"
#include "adiMappers/StaticObjectsMapper.h"
#include "adiMappers/UltrasonicDetectionsMapper.h"
#include "adiMappers/UltrasonicFeaturesMapper.h"

using namespace ara::adi::sensoritf;

class test_osi_adi_mapper : public ::testing::Test
{

protected:
    virtual void SetUp()
    {
        testOsiData = createOsiDummyData();
        adiCamDetInterface = testcameraDetectionMapper.mapCameraDetectionsInterface(testOsiData);
        adiCamFeatureInterface = testCameraFeaturesMapper.mapCameraFeatureInterface(testOsiData);
        adiLidarDetectionsInterface = testLidarDetectionsMapper.mapLidarDetectionsInterface(testOsiData);
        adiRadarDetectionsInterface = testRadarDetectionsMapper.mapRadarDetectionsInterface(testOsiData);
        adiRoadObjectInterface = testRoadObjectsMapper.mapRoadObjects(testOsiData);
        adiStaticObjectInterface = testStaticObjectsMapper.mapStaticObjectInterface(testOsiData);
        adiPotentiallyMovingObjectInterface = testPMObjectsMapper.mapPMObjectInterface(testOsiData);
        adiUltrasonicDetectionsInterface = testUltrasonicDetectionsMapper.mapUltrasonicDetectionsInterface(testOsiData);
        adiUltrasonicFeatureInterface = testUltrasonicFeaturesMapper.mapUltrasonicFeatureInterface(testOsiData);
    }

    virtual void TearDown()
    { }

    osi3::SensorData testOsiData;

    CameraDetectionsMapper testcameraDetectionMapper;
    CameraFeaturesMapper testCameraFeaturesMapper;
    LidarDetectionsMapper testLidarDetectionsMapper;
    RadarDetectionsMapper testRadarDetectionsMapper;
    RoadObjectsMapper testRoadObjectsMapper;
    StaticObjectMapper testStaticObjectsMapper;
    PMObjectMapper testPMObjectsMapper;
    UltrasonicDetectionsMapper testUltrasonicDetectionsMapper;
    UltrasonicFeaturesMapper testUltrasonicFeaturesMapper;

    CameraDetectionsInterface adiCamDetInterface;
    CameraFeatureInterface adiCamFeatureInterface;
    LidarDetectionsInterface adiLidarDetectionsInterface;
    RadarDetectionsInterface adiRadarDetectionsInterface;
    RoadObjectInterface adiRoadObjectInterface;
    StaticObjectInterface adiStaticObjectInterface;
    PotentiallyMovingObjectInterface adiPotentiallyMovingObjectInterface;
    UltrasonicDetectionsInterface adiUltrasonicDetectionsInterface;
    UltrasonicFeatureInterface adiUltrasonicFeatureInterface;
};

TEST_F(test_osi_adi_mapper, test_cameradetection_sensorID)
{

    auto adiCamInterfaceHeader = adiCamDetInterface.CameraDetectionInterfaceHeader;
    auto adiCamSensorID = adiCamInterfaceHeader.SensorClusterInformation.SensorIDReferenceList.at(0);
    EXPECT_THAT(2, adiCamSensorID);
    RecordProperty("adiCamSensorID: ", adiCamSensorID);
}

TEST_F(test_osi_adi_mapper, test_cameradetection_sensorOrientPitch)
{

    auto adiCamInterfaceHeader = adiCamDetInterface.CameraDetectionInterfaceHeader;
    auto adiMountingPose = adiCamInterfaceHeader.SensorPose;
    auto adiCamSensorOrientPitch = adiMountingPose.SensorOrientation.Pitch;
    EXPECT_FLOAT_EQ(3.66f, adiCamSensorOrientPitch);

    RecordProperty("adiCamSensorOrientPitch: ", std::to_string(adiCamSensorOrientPitch));
}

TEST_F(test_osi_adi_mapper, test_cameradetection_sensorOrientYaw)
{

    auto adiCamInterfaceHeader = adiCamDetInterface.CameraDetectionInterfaceHeader;
    auto adiMountingPose = adiCamInterfaceHeader.SensorPose;
    auto adiCamSensorOrientYaw = adiMountingPose.SensorOrientation.Yaw;
    EXPECT_FLOAT_EQ(2.5f, adiCamSensorOrientYaw);

    RecordProperty("adiCamSensorOrientYaw: ", std::to_string(adiCamSensorOrientYaw));
}

TEST_F(test_osi_adi_mapper, test_cameradetection_camShapeAmbiguityID)
{

    std::vector<uint16_t> defaultValuesCamShapeAmbiguityId;
    defaultValuesCamShapeAmbiguityId.push_back(1);

    for (std::size_t index = 0; index < adiCamDetInterface.ValidCameraDetectionList.size(); index++) {
        auto adiCameShapeInfo = adiCamDetInterface.ValidCameraDetectionList.at(index).CameraShapeInformation;
        auto adiCamShapeAmbiguityId = adiCameShapeInfo.ShapeAmbiguityID;
        EXPECT_EQ(defaultValuesCamShapeAmbiguityId.at(index), adiCamShapeAmbiguityId);
        RecordProperty("adiCamShapeAmbiguityId: ", adiCamShapeAmbiguityId);
    }
}

TEST_F(test_osi_adi_mapper, test_cameradetection_camShapePoints)
{

    std::vector<uint16_t> defaultValuesCamShapePoints;
    defaultValuesCamShapePoints.push_back(500);

    for (std::size_t index = 0; index < adiCamDetInterface.ValidCameraDetectionList.size(); index++) {
        auto adiCamDetShapePoints = adiCamDetInterface.ValidCameraDetectionList.at(index)
                                        .CameraShapePoints.NumberOfValidShapePointsDetectionLevel;
        EXPECT_EQ(defaultValuesCamShapePoints.at(index), adiCamDetShapePoints);
        RecordProperty("adiCamDetShapePoints: ", adiCamDetShapePoints);
    }
}

TEST_F(test_osi_adi_mapper, test_cameradetection_camShapeType)
{

    std::vector<ShapeClassificationType> defaultValuesCamShapeType;
    defaultValuesCamShapeType.push_back(ShapeClassificationType::kVehicle);

    for (std::size_t index = 0; index < adiCamDetInterface.ValidCameraDetectionList.size(); index++) {
        auto adiCameShapeInfo = adiCamDetInterface.ValidCameraDetectionList.at(index).CameraShapeInformation;

        for (std::size_t adiShapeClassIndex = 0;
             adiShapeClassIndex < adiCameShapeInfo.ValidShapeClassificationsList.size();
             adiShapeClassIndex++) {
            auto adiCamShapeClassType
                = adiCameShapeInfo.ValidShapeClassificationsList.at(index).ShapeClassificationTypeDetectionLevel;
            EXPECT_THAT(defaultValuesCamShapeType.at(index), adiCamShapeClassType);
            RecordProperty("adiCamShapeClassType: ", "ShapeClassificationType::kVehicle");
        }
    }
}

TEST_F(test_osi_adi_mapper, test_cameradetection_camShapePointProbability)
{

    std::vector<float> defaultValuesCamShapePointProb;
    defaultValuesCamShapePointProb.push_back(0.86f);

    for (std::size_t index = 0; index < adiCamDetInterface.ValidCameraDetectionList.size(); index++) {
        auto adiCamDetShapePoints = adiCamDetInterface.ValidCameraDetectionList.at(index).CameraShapePoints;
        for (std::size_t pointIndex = 0; pointIndex < adiCamDetShapePoints.ValidShapePointsDetectionLevelList.size();
             pointIndex++) {
            auto adiCamDetShapePointProb = adiCamDetShapePoints.ValidShapePointsDetectionLevelList.at(pointIndex)
                                               .PointExistenceProbabilityDetectionLevel;
            EXPECT_FLOAT_EQ(defaultValuesCamShapePointProb.at(index), adiCamDetShapePointProb);
            RecordProperty("adiCamDetShapePointProb: ", std::to_string(adiCamDetShapePointProb));
        }
    }
}

TEST_F(test_osi_adi_mapper, test_camerafeature_infoInterfaceVersionMajor)
{
    auto adiCameraFeatureInterfaceHeader = adiCamFeatureInterface.CameraFeatureInterfaceHeader;
    auto adiCamFeatureInterfaceVer
        = adiCameraFeatureInterfaceHeader.InformationInterface.VersionID.InterfaceVersionIDMajor;
    EXPECT_THAT(1, adiCamFeatureInterfaceVer);
    RecordProperty("adiCamFeatureInterfaceVer: ", adiCamFeatureInterfaceVer);
}

TEST_F(test_osi_adi_mapper, test_camerafeature_infoDataQualifier)
{
    auto adiCameraFeatureInterfaceHeader = adiCamFeatureInterface.CameraFeatureInterfaceHeader;
    auto adiCamFeatureDataQualifier = adiCameraFeatureInterfaceHeader.InformationInterface.DataQualifier;
    EXPECT_THAT(DataQualifier::kNormal, adiCamFeatureDataQualifier);
    RecordProperty("adiCamFeatureDataQualifier: ", "DataQualifier::kNormal");
}

TEST_F(test_osi_adi_mapper, test_camerafeature_numValidDetections)
{
    auto adiNumberOfValidFeatures = adiCamFeatureInterface.NumberOfValidFeatures;
    EXPECT_THAT(4, adiNumberOfValidFeatures);
    RecordProperty("adiNumberOfValidFeatures: ", adiNumberOfValidFeatures);
}

TEST_F(test_osi_adi_mapper, test_camerafeature_position)
{
    auto adiValidCameraFeatureVector = adiCamFeatureInterface.ValidCameraFeaturesList;

    std::vector<Point3D> adiPoint;
    Point3D adiPoint3D;
    adiPoint3D.x = 25.95f;
    adiPoint3D.y = 89.34f;
    adiPoint3D.z = 0.0f;
    adiPoint.push_back(adiPoint3D);

    for (std::size_t index = 0; index < adiValidCameraFeatureVector.size(); index++) {

        ShapePoints adiShapePoints = adiValidCameraFeatureVector.at(index).CameraFeaturesShapePoints;
        for (std::size_t numPointsIndex = 0; numPointsIndex < adiShapePoints.ValidShapePointsList.size();
             numPointsIndex++) {
            ShapePoint adiShapePoint = adiShapePoints.ValidShapePointsList.at(numPointsIndex);
            EXPECT_FLOAT_EQ(adiPoint.at(numPointsIndex).x, adiShapePoint.Position.x);

            EXPECT_FLOAT_EQ(adiPoint.at(numPointsIndex).y, adiShapePoint.Position.y);

            EXPECT_FLOAT_EQ(adiPoint.at(numPointsIndex).z, adiShapePoint.Position.z);

            RecordProperty("adiShapePointsPosition: ",
                std::to_string(adiPoint.at(numPointsIndex).x) + ", " + std::to_string(adiPoint.at(numPointsIndex).y)
                    + ", " + std::to_string(adiPoint.at(numPointsIndex).z));
        }
    }
}

TEST_F(test_osi_adi_mapper, test_camerafeature_existenceProb)
{
    auto adiValidCameraFeatureVector = adiCamFeatureInterface.ValidCameraFeaturesList;
    std::vector<float> defaultCamFeatureProb;
    defaultCamFeatureProb.push_back(94.23f);
    for (std::size_t index = 0; index < adiValidCameraFeatureVector.size(); index++) {
        auto adiCFExistenceProbability
            = adiValidCameraFeatureVector.at(index).CameraFeaturesStatus.ExistenceProbabilityFeatureLevel;
        EXPECT_FLOAT_EQ(defaultCamFeatureProb.at(index), adiCFExistenceProbability);
    }
}

TEST_F(test_osi_adi_mapper, test_lidardetection_sensorID)
{

    auto adiLidarInterfaceHeader = adiLidarDetectionsInterface.LidarDetectionInterfaceHeader;
    auto adiLidSensorID = adiLidarInterfaceHeader.SensorClusterInformation.SensorIDReferenceList.at(0);
    EXPECT_THAT(4, adiLidSensorID);
    RecordProperty("adiLidSensorID: ", adiLidSensorID);
}

TEST_F(test_osi_adi_mapper, test_lidardetection_sensorOrientPitch)
{

    auto adiLidarInterfaceHeader = adiLidarDetectionsInterface.LidarDetectionInterfaceHeader;
    auto adiMountingPose = adiLidarInterfaceHeader.SensorPose;
    auto adiLidSensorOrientPitch = adiMountingPose.SensorOrientation.Pitch;
    EXPECT_FLOAT_EQ(3.59f, adiLidSensorOrientPitch);

    RecordProperty("adiLidSensorOrientPitch: ", std::to_string(adiLidSensorOrientPitch));
}

TEST_F(test_osi_adi_mapper, test_lidardetection_sensorOrientYaw)
{

    auto adiLidarInterfaceHeader = adiLidarDetectionsInterface.LidarDetectionInterfaceHeader;
    auto adiMountingPose = adiLidarInterfaceHeader.SensorPose;
    auto adiLidarSensorOrientYaw = adiMountingPose.SensorOrientation.Yaw;
    EXPECT_FLOAT_EQ(2.5f, adiLidarSensorOrientYaw);

    RecordProperty("adiLidarSensorOrientYaw: ", std::to_string(adiLidarSensorOrientYaw));
}

TEST_F(test_osi_adi_mapper, test_lidardetection_lidarDetFreeSpaceProbability)
{

    std::vector<float> defaultLidarDetFreeSpaceProbability;
    defaultLidarDetFreeSpaceProbability.push_back(0.54f);

    for (std::size_t index = 0; index < adiLidarDetectionsInterface.ValidLidarDetectionsList.size(); index++) {

        auto adiLidarDetFreeSpaceProbability = adiLidarDetectionsInterface.ValidLidarDetectionsList.at(index)
                                                   .LidarDetectionsInformation.FreeSpaceProbability;
        EXPECT_EQ(defaultLidarDetFreeSpaceProbability.at(index), adiLidarDetFreeSpaceProbability);
        RecordProperty("adiLidarDetFreeSpaceProbability: ", std::to_string(adiLidarDetFreeSpaceProbability));
    }
}

TEST_F(test_osi_adi_mapper, test_lidardetection_lidarDetReflectivity)
{

    std::vector<float> defaultValuesLidarDetReflectivity;
    defaultValuesLidarDetReflectivity.push_back(1.45f);

    for (std::size_t index = 0; index < adiLidarDetectionsInterface.ValidLidarDetectionsList.size(); index++) {

        auto adiLidarDetReflectivity
            = adiLidarDetectionsInterface.ValidLidarDetectionsList.at(index).LidarDetectionsInformation.Reflectivity;
        EXPECT_EQ(defaultValuesLidarDetReflectivity.at(index), adiLidarDetReflectivity);
        RecordProperty("adiLidarDetReflectivity: ", std::to_string(adiLidarDetReflectivity));
    }
}

TEST_F(test_osi_adi_mapper, test_lidardetection_lidarDetObjectID)
{

    std::vector<uint16_t> defaultValuesLidarDetObjectID;
    defaultValuesLidarDetObjectID.push_back(1);

    for (std::size_t index = 0; index < adiLidarDetectionsInterface.ValidLidarDetectionsList.size(); index++) {

        auto adiLidarDetObjectID
            = adiLidarDetectionsInterface.ValidLidarDetectionsList.at(index).LidarDetectionStatus.ObjectID;
        EXPECT_EQ(defaultValuesLidarDetObjectID.at(index), adiLidarDetObjectID);
        RecordProperty("adiLidarDetObjectID: ", adiLidarDetObjectID);
    }
}

TEST_F(test_osi_adi_mapper, test_radardetection_radarDetObjectID)
{

    std::vector<uint16_t> defaultValuesRadarDetObjectID;
    defaultValuesRadarDetObjectID.push_back(3);

    for (std::size_t index = 0; index < adiRadarDetectionsInterface.ValidRadarDetectionsList.size(); index++) {

        auto adiRadarDetObjectID
            = adiRadarDetectionsInterface.ValidRadarDetectionsList.at(index).RadarDetectionsStatus.ObjectID;
        EXPECT_EQ(defaultValuesRadarDetObjectID.at(index), adiRadarDetObjectID);
        RecordProperty("adiRadarDetObjectID: ", adiRadarDetObjectID);
    }
}

TEST_F(test_osi_adi_mapper, test_radardetection_radarExistenceProbabilityDetectionLevel)
{

    std::vector<float> defaultValuesRadarExistenceProbabilityDetectionLevel;
    defaultValuesRadarExistenceProbabilityDetectionLevel.push_back(0.95f);

    for (std::size_t index = 0; index < adiRadarDetectionsInterface.ValidRadarDetectionsList.size(); index++) {

        auto adiRadarExistenceProbabilityDetectionLevel = adiRadarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                                              .RadarDetectionsStatus.ExistenceProbabilityDetectionLevel;
        EXPECT_FLOAT_EQ(
            defaultValuesRadarExistenceProbabilityDetectionLevel.at(index), adiRadarExistenceProbabilityDetectionLevel);
        RecordProperty(
            "adiRadarExistenceProbabilityDetectionLevel: ", std::to_string(adiRadarExistenceProbabilityDetectionLevel));
    }
}

TEST_F(test_osi_adi_mapper, test_radardetection_radarRadarCrossSection)
{

    std::vector<float> defaultValuesRadarRadarCrossSection;
    defaultValuesRadarRadarCrossSection.push_back(7);

    for (std::size_t index = 0; index < adiRadarDetectionsInterface.ValidRadarDetectionsList.size(); index++) {

        auto adiRadarRadarCrossSection = adiRadarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                             .RadarDetectionsInformation.RadarCrossSection;
        EXPECT_FLOAT_EQ(defaultValuesRadarRadarCrossSection.at(index), adiRadarRadarCrossSection);
        RecordProperty("adiRadarRadarCrossSection: ", std::to_string(adiRadarRadarCrossSection));
    }
}

TEST_F(test_osi_adi_mapper, test_radardetection_radarDetSNR)
{

    std::vector<float> defaultValuesRadarSNR;
    defaultValuesRadarSNR.push_back(1.0f);

    for (std::size_t index = 0; index < adiRadarDetectionsInterface.ValidRadarDetectionsList.size(); index++) {

        auto adiRadarSNR = adiRadarDetectionsInterface.ValidRadarDetectionsList.at(index)
                               .RadarDetectionsInformation.SignalToNoiseRatioDetectionLevel;
        EXPECT_FLOAT_EQ(defaultValuesRadarSNR.at(index), adiRadarSNR);
        RecordProperty("adiRadarSNR: ", std::to_string(adiRadarSNR));
    }
}

TEST_F(test_osi_adi_mapper, test_radardetection_radarDetClassType)
{

    std::vector<DetectionClassificationType> defaultValuesRadarDetClassType;
    defaultValuesRadarDetClassType.push_back(DetectionClassificationType::kUnknown);

    for (std::size_t index = 0; index < adiRadarDetectionsInterface.ValidRadarDetectionsList.size(); index++) {

        auto adiRadarDetClassList = adiRadarDetectionsInterface.ValidRadarDetectionsList.at(index)
                                        .RadarDetectionsInformation.ValidDetectionClassificationList;

        for (std::size_t numDetClassIndex = 0; numDetClassIndex < adiRadarDetClassList.size(); numDetClassIndex++) {
            auto adiRadarDetClassType = adiRadarDetClassList.at(numDetClassIndex).DetectionClassificationType;
            EXPECT_EQ(defaultValuesRadarDetClassType.at(index), adiRadarDetClassType);
            RecordProperty("adiRadarDetClassType: ", "DetectionClassificationType::kUnknown");
        }
    }
}

TEST_F(test_osi_adi_mapper, test_radardetection_radarDetPos)
{

    std::vector<Position3DSpheric> defaultValuesRadarDetPos;
    Position3DSpheric defaultPose{};
    defaultPose.azimuth = 4.56;
    defaultPose.distance = 10.45;
    defaultPose.elevation = 0.5;
    defaultValuesRadarDetPos.push_back(defaultPose);

    for (std::size_t index = 0; index < adiRadarDetectionsInterface.ValidRadarDetectionsList.size(); index++) {

        auto adiRadarDetPos
            = adiRadarDetectionsInterface.ValidRadarDetectionsList.at(index).RadarDetectionsPosition.DetectionPosition;

        EXPECT_EQ(defaultValuesRadarDetPos.at(index).azimuth, adiRadarDetPos.azimuth);
        EXPECT_EQ(defaultValuesRadarDetPos.at(index).elevation, adiRadarDetPos.elevation);
        EXPECT_EQ(defaultValuesRadarDetPos.at(index).distance, adiRadarDetPos.distance);
        RecordProperty("adiRadarDetPosAzimuth: ", std::to_string(adiRadarDetPos.azimuth));
        RecordProperty("adiRadarDetPosElevation: ", std::to_string(adiRadarDetPos.elevation));
        RecordProperty("adiRadarDetPosDistance: ", std::to_string(adiRadarDetPos.distance));
    }
}

TEST_F(test_osi_adi_mapper, test_potentiallymovingobjects_CycleCounter)
{
    auto adiPMOInterfaceHeader = adiPotentiallyMovingObjectInterface.PotentiallyMovingObjectInterfaceHeader;
    auto adiPMOCycleCounter = adiPMOInterfaceHeader.InformationInterface.CycleCounter;
    EXPECT_EQ(25, adiPMOCycleCounter);
    RecordProperty("adiPMOCycleCounter: ", adiPMOCycleCounter);
}

TEST_F(test_osi_adi_mapper, test_potentiallymovingobjects_DataQualifier)
{
    auto adiPMOInterfaceHeader = adiPotentiallyMovingObjectInterface.PotentiallyMovingObjectInterfaceHeader;
    auto adiPMODataQualifier = adiPMOInterfaceHeader.InformationInterface.DataQualifier;
    EXPECT_EQ(DataQualifier::kReduceInCoverage, adiPMODataQualifier);
    RecordProperty("adiPMODataQualifier: ", "DataQualifier::kReduceInCoverage");
}

TEST_F(test_osi_adi_mapper, test_potentiallymovingobjects_BoundingBoxExtent)
{
    std::vector<DimensionBox> defaultValuesBBox;
    DimensionBox defaultDim{};
    defaultDim.Height = 4.0f;
    defaultDim.Width = 4.0f;
    defaultDim.Length = 4.0f;
    defaultValuesBBox.push_back(defaultDim);
    auto adiPMOValidList
        = adiPotentiallyMovingObjectInterface.PotentiallyMovingObjectList.ValidPotentiallyMovingObjects;
    for (std::size_t index = 0; index < adiPMOValidList.size(); index++) {
        auto adiPMOBBox = adiPMOValidList.at(index).PotentiallyMovingObjectsBoundingBox.BoundingBoxExtent;
        EXPECT_FLOAT_EQ(defaultValuesBBox.at(index).Height, adiPMOBBox.Height);
        EXPECT_FLOAT_EQ(defaultValuesBBox.at(index).Width, adiPMOBBox.Width);
        EXPECT_FLOAT_EQ(defaultValuesBBox.at(index).Length, adiPMOBBox.Length);

        RecordProperty("adiPMOBBox Height: ", std::to_string(adiPMOBBox.Height));
        RecordProperty("adiPMOBBox Width: ", std::to_string(adiPMOBBox.Width));
        RecordProperty("adiPMOBBox Length: ", std::to_string(adiPMOBBox.Length));
    }
}

TEST_F(test_osi_adi_mapper, test_potentiallymovingobjects_Acceleration)
{
    std::vector<Point3D> defaultValuesAcc;
    Point3D Acc{};
    Acc.x = 3.4;
    Acc.y = 3.9;
    Acc.z = 4.4;
    defaultValuesAcc.push_back(Acc);
    auto adiPMOValidList
        = adiPotentiallyMovingObjectInterface.PotentiallyMovingObjectList.ValidPotentiallyMovingObjects;
    for (std::size_t index = 0; index < adiPMOValidList.size(); index++) {
        auto adiPMOAcc = adiPMOValidList.at(index).PotentiallyMovingObjectsDynamics.Acceleration;
        EXPECT_FLOAT_EQ(defaultValuesAcc.at(index).x, adiPMOAcc.x);
        EXPECT_FLOAT_EQ(defaultValuesAcc.at(index).y, adiPMOAcc.y);
        EXPECT_FLOAT_EQ(defaultValuesAcc.at(index).z, adiPMOAcc.z);

        RecordProperty("adiPMOAcc x: ", std::to_string(adiPMOAcc.x));
        RecordProperty("adiPMOAcc y: ", std::to_string(adiPMOAcc.y));
        RecordProperty("adiPMOAcc z: ", std::to_string(adiPMOAcc.z));
    }
}

TEST_F(test_osi_adi_mapper, test_potentiallymovingobjects_NumberOfValidLights)
{
    std::vector<LightType> adiLightTypes;
    LightType adiLightType = LightType::kLeftFlashLight;
    adiLightTypes.push_back(adiLightType);

    auto adiPMOValidList
        = adiPotentiallyMovingObjectInterface.PotentiallyMovingObjectList.ValidPotentiallyMovingObjects;
    for (std::size_t index = 0; index < adiPMOValidList.size(); index++) {
        auto adiPMONumLightsList
            = adiPMOValidList.at(index).PotentiallyMovingObjectsLights.PotentiallyMovingObjectsLightList;
        for (std::size_t lightIndex = 0; lightIndex < adiPMONumLightsList.size(); lightIndex++) {
            auto adiPMOLightType = adiPMONumLightsList.at(lightIndex).PMOLightType;
            EXPECT_THAT(adiLightTypes.at(lightIndex), adiPMOLightType);
            RecordProperty("adiPMOLightType: ", "LightType::kLeftFlashLight");
        }
    }
}

TEST_F(test_osi_adi_mapper, test_staticobjects_NumberOfValidTrafficLights)
{
    auto adiTrafficLightList = adiStaticObjectInterface.StaticObjectTrafficLights.TrafficLightList;
    for (std::size_t index = 0; index < adiTrafficLightList.size(); index++) {
        auto adiTLNumTrafficLights = adiTrafficLightList.at(index).TrafficLightsSpots.NumberOfValidTrafficLightSpots;
        EXPECT_THAT(1, adiTLNumTrafficLights);
        RecordProperty("adiTLNumTrafficLights: ", adiTLNumTrafficLights);
    }
}

TEST_F(test_osi_adi_mapper, test_staticobjects_LightModeClassificationType)
{
    auto adiTrafficLightList = adiStaticObjectInterface.StaticObjectTrafficLights.TrafficLightList;
    LightModeClassificationType defaultValueLightClass = LightModeClassificationType::kCounting;

    for (std::size_t index = 0; index < adiTrafficLightList.size(); index++) {
        auto adiTLValidTrafficLights = adiTrafficLightList.at(index).TrafficLightsSpots.ValidTrafficLightSpotList;
        for (std::size_t trafficLightIndex = 0; trafficLightIndex < adiTLValidTrafficLights.size();
             trafficLightIndex++) {
            auto adiTLClass = adiTLValidTrafficLights.at(trafficLightIndex)
                                  .TrafficLightsSpotsColour.ValidLightModeClassificationVectorList;

            for (std::size_t adiTLClassIndex = 0; adiTLClassIndex < adiTLClass.size(); adiTLClassIndex++) {
                EXPECT_THAT(defaultValueLightClass, adiTLClass.at(adiTLClassIndex).LightModeClassificationType);
                RecordProperty("adiTLightModeClassificationType: ", "LightModeClassificationType::kCounting");
            }
        }
    }
}

TEST_F(test_osi_adi_mapper, test_staticobjects_LightShapeClassificationType)
{
    auto adiTrafficLightList = adiStaticObjectInterface.StaticObjectTrafficLights.TrafficLightList;
    std::vector<LightShapeClassificationType> defaultValueLightShapeClassList;
    LightShapeClassificationType defaultValueLightShapeClass = LightShapeClassificationType::kArrowLeft;
    defaultValueLightShapeClassList.push_back(defaultValueLightShapeClass);

    for (std::size_t index = 0; index < adiTrafficLightList.size(); index++) {
        auto adiTLValidTrafficLights = adiTrafficLightList.at(index).TrafficLightsSpots.ValidTrafficLightSpotList;
        for (std::size_t trafficLightIndex = 0; trafficLightIndex < adiTLValidTrafficLights.size();
             trafficLightIndex++) {
            auto adiTLLightShapeClass = adiTLValidTrafficLights.at(trafficLightIndex)
                                            .TrafficLightsSpotsInformation.ValidLightShapeClassificationList;

            for (std::size_t adiTLClassIndex = 0; adiTLClassIndex < adiTLLightShapeClass.size(); adiTLClassIndex++) {
                EXPECT_THAT(defaultValueLightShapeClassList.at(adiTLClassIndex),
                    adiTLLightShapeClass.at(adiTLClassIndex).LightShapeClassificationType);
                RecordProperty("adiTLightShapeClassificationType: ", "LightShapeClassificationType::kArrowLeft");
            }
        }
    }
}

TEST_F(test_osi_adi_mapper, test_staticobjects_TrafficSignUnit)
{
    auto adiTrafficSignsList = adiStaticObjectInterface.StaticObjectTrafficSigns.TrafficSignsList;
    std::vector<SignValueUnit> defaultValueSignValueUnitList;
    SignValueUnit defaultSignValueUnit = SignValueUnit::kMeter;
    defaultValueSignValueUnitList.push_back(defaultSignValueUnit);

    for (std::size_t index = 0; index < adiTrafficSignsList.size(); index++) {
        auto adiTSValidMainSignClassificationsList
            = adiTrafficSignsList.at(index).TrafficSignsInformation.ValidMainSignClassificationsList;
        for (std::size_t trafficLightIndex = 0; trafficLightIndex < adiTSValidMainSignClassificationsList.size();
             trafficLightIndex++) {
            auto adiTSMSignUnit = adiTSValidMainSignClassificationsList.at(trafficLightIndex).MSignUnit;
            EXPECT_THAT(defaultValueSignValueUnitList.at(trafficLightIndex), adiTSMSignUnit);
            RecordProperty("adiTSMSignUnit: ", "SignValueUnit::kMeter");
        }
    }
}

TEST_F(test_osi_adi_mapper, test_staticobjects_TrafficSignState)
{
    auto adiTrafficSignsList = adiStaticObjectInterface.StaticObjectTrafficSigns.TrafficSignsList;
    std::vector<SignState> defaultValueSignStateList;
    SignState defaultSignState = SignState::kStatic;
    defaultValueSignStateList.push_back(defaultSignState);

    for (std::size_t index = 0; index < adiTrafficSignsList.size(); index++) {
        auto adiTSValidMainSignClassificationsList
            = adiTrafficSignsList.at(index).TrafficSignsInformation.ValidMainSignClassificationsList;
        for (std::size_t trafficLightIndex = 0; trafficLightIndex < adiTSValidMainSignClassificationsList.size();
             trafficLightIndex++) {
            auto adiTSMSignState = adiTSValidMainSignClassificationsList.at(trafficLightIndex).SignState;
            EXPECT_THAT(defaultValueSignStateList.at(trafficLightIndex), adiTSMSignState);
            RecordProperty("adiTSMSignState: ", "SignState::kStatic");
        }
    }
}

TEST_F(test_osi_adi_mapper, test_staticobjects_TrafficSignValue)
{
    auto adiTrafficSignsList = adiStaticObjectInterface.StaticObjectTrafficSigns.TrafficSignsList;
    std::vector<float> defaultValueSignList;
    float defaultSignValue = 30.0f;
    defaultValueSignList.push_back(defaultSignValue);

    for (std::size_t index = 0; index < adiTrafficSignsList.size(); index++) {
        auto adiTSValidMainSignClassificationsList
            = adiTrafficSignsList.at(index).TrafficSignsInformation.ValidMainSignClassificationsList;
        for (std::size_t trafficLightIndex = 0; trafficLightIndex < adiTSValidMainSignClassificationsList.size();
             trafficLightIndex++) {
            auto adiTSMSignValue = adiTSValidMainSignClassificationsList.at(trafficLightIndex).SignValue;
            EXPECT_FLOAT_EQ(defaultValueSignList.at(trafficLightIndex), adiTSMSignValue);
            RecordProperty("adiTSMSignValue: ", "SignState::kFullOutOfService");
        }
    }
}

TEST_F(test_osi_adi_mapper, test_roadobjects_RoadBoundaryType)
{
    std::vector<RoadBoundaryType> defaultRoadBoundaryType;
    RoadBoundaryType adiRoadBoundaryType = RoadBoundaryType::kCurb;
    defaultRoadBoundaryType.push_back(adiRoadBoundaryType);

    auto adiRoadBoundaryList = adiRoadObjectInterface.RoadBoundariesObjectList.RoadBoundaryList;
    for (std::size_t index = 0; index < adiRoadBoundaryList.size(); index++) {
        auto adiRBClassList
            = adiRoadBoundaryList.at(index).RoadBoundariesInformation.ValidRoadBoundaryClassificationsList;

        for (std::size_t adiRBClassIndex = 0; adiRBClassIndex < adiRBClassList.size(); adiRBClassIndex++) {
            EXPECT_EQ(defaultRoadBoundaryType.at(adiRBClassIndex), adiRBClassList.at(adiRBClassIndex).RoadBoundaryType);
            RecordProperty("adiRoadBoundaryType: ", "RoadBoundaryType::kCurb");
        }
    }
}

TEST_F(test_osi_adi_mapper, test_roadobjects_RoadBoundaryTypeConfidence)
{
    std::vector<float> defaultRoadBoundaryTypeConf;
    float adiRoadBoundaryTypeConf = 0.94f;
    defaultRoadBoundaryTypeConf.push_back(adiRoadBoundaryTypeConf);

    auto adiRoadBoundaryList = adiRoadObjectInterface.RoadBoundariesObjectList.RoadBoundaryList;
    for (std::size_t index = 0; index < adiRoadBoundaryList.size(); index++) {
        auto adiRBClassList
            = adiRoadBoundaryList.at(index).RoadBoundariesInformation.ValidRoadBoundaryClassificationsList;
        for (std::size_t adiRBClassIndex = 0; adiRBClassIndex < adiRBClassList.size(); adiRBClassIndex++) {
            EXPECT_EQ(defaultRoadBoundaryTypeConf.at(index), adiRBClassList.at(index).RoadBoundaryTypeConfidence);
            RecordProperty(
                "adiRoadBoundaryTypeConfidence: ", std::to_string(adiRBClassList.at(index).RoadBoundaryTypeConfidence));
        }
    }
}

TEST_F(test_osi_adi_mapper, test_roadobjects_RoadMarkingMeasurementStatus)
{

    auto adiRoadMarkingList = adiRoadObjectInterface.RoadMarkingObjectList.ValidRoadMarkings;
    for (std::size_t index = 0; index < adiRoadMarkingList.size(); index++) {
        auto adiRMMeasurementStatusObjectLevel
            = adiRoadMarkingList.at(index).RoadMarkingsStatus.MeasurementStatusObjectLevel;

        EXPECT_THAT(MeasurementStatus::kMeasured, adiRMMeasurementStatusObjectLevel);
        RecordProperty("adiRMMeasurementStatusObjectLevel: ", "MeasurementStatus::kMeasured");
    }
}

TEST_F(test_osi_adi_mapper, test_roadobjects_RoadMarkingExistenceProb)
{

    auto adiRoadMarkingList = adiRoadObjectInterface.RoadMarkingObjectList.ValidRoadMarkings;
    for (std::size_t index = 0; index < adiRoadMarkingList.size(); index++) {
        auto adiRMProbability = adiRoadMarkingList.at(index).RoadMarkingsStatus.ExistenceProbabilityObjectLevel;

        EXPECT_FLOAT_EQ(0.89, adiRMProbability);
        RecordProperty("adiRMProbability: ", std::to_string(adiRMProbability));
    }
}

TEST_F(test_osi_adi_mapper, test_roadobjects_RoadMarkingVerticeHeight)
{

    auto adiRoadMarkingList = adiRoadObjectInterface.RoadMarkingObjectList.ValidRoadMarkings;
    for (std::size_t index = 0; index < adiRoadMarkingList.size(); index++) {
        auto adiRMValidPolylinesList = adiRoadMarkingList.at(index).RoadMarkingsPolylines.ValidPolylinesList;

        for (std::size_t markingIndex = 0; markingIndex < adiRMValidPolylinesList.size(); markingIndex++) {
            auto adiRMVerticeList = adiRMValidPolylinesList.at(markingIndex).ValidVerticesList;

            for (std::size_t verticeIndex = 0; verticeIndex < adiRMVerticeList.size(); verticeIndex++) {
                EXPECT_FLOAT_EQ(0.02f, adiRMVerticeList.at(verticeIndex).HeightVertex);
                RecordProperty(
                    "adiRoadMarkingVerticeHeight: ", std::to_string(adiRMVerticeList.at(verticeIndex).HeightVertex));
            }
        }
    }
}

TEST_F(test_osi_adi_mapper, test_roadobjects_RoadMarkingVerticePoint)
{

    auto adiRoadMarkingList = adiRoadObjectInterface.RoadMarkingObjectList.ValidRoadMarkings;
    for (std::size_t index = 0; index < adiRoadMarkingList.size(); index++) {
        auto adiRMValidPolylinesList = adiRoadMarkingList.at(index).RoadMarkingsPolylines.ValidPolylinesList;

        for (std::size_t markingIndex = 0; markingIndex < adiRMValidPolylinesList.size(); markingIndex++) {
            auto adiRMVerticeList = adiRMValidPolylinesList.at(markingIndex).ValidVerticesList;

            for (std::size_t verticeIndex = 0; verticeIndex < adiRMVerticeList.size(); verticeIndex++) {
                EXPECT_FLOAT_EQ(4.5, adiRMVerticeList.at(verticeIndex).VertexPoint.x);
                EXPECT_FLOAT_EQ(3.5, adiRMVerticeList.at(verticeIndex).VertexPoint.y);
                EXPECT_FLOAT_EQ(0, adiRMVerticeList.at(verticeIndex).VertexPoint.z);

                RecordProperty("adiRoadMarkingVerticePointx: ", adiRMVerticeList.at(verticeIndex).VertexPoint.x);
                RecordProperty("adiRoadMarkingVerticePointy: ", adiRMVerticeList.at(verticeIndex).VertexPoint.y);
                RecordProperty("adiRoadMarkingVerticePointz: ", adiRMVerticeList.at(verticeIndex).VertexPoint.z);
            }
        }
    }
}

TEST_F(test_osi_adi_mapper, test_ultrasonicDetection_distance)
{

    std::vector<float> defaultUltrasonDetsDistances;
    defaultUltrasonDetsDistances.push_back(35.4f);

    auto adiValidUltrasonicDetectionList = adiUltrasonicDetectionsInterface.ValidUltrasonicDetectionList;
    for (std::size_t index = 0; index < adiValidUltrasonicDetectionList.size(); index++) {
        auto adiValidUltrasonicDetectionDistance
            = adiValidUltrasonicDetectionList.at(index).UltrasonicDetectionsPosition.Distance;
        EXPECT_FLOAT_EQ(defaultUltrasonDetsDistances.at(index), adiValidUltrasonicDetectionDistance);
        RecordProperty("adiValidUltrasonicDetectionDistance: ", std::to_string(adiValidUltrasonicDetectionDistance));
    }
}

TEST_F(test_osi_adi_mapper, test_ultrasonicDetection_ExistenceProb)
{

    std::vector<float> defaultUltrasonicDetectionProb;
    defaultUltrasonicDetectionProb.push_back(0.923f);

    auto adiValidUltrasonicDetectionList = adiUltrasonicDetectionsInterface.ValidUltrasonicDetectionList;
    for (std::size_t index = 0; index < adiValidUltrasonicDetectionList.size(); index++) {
        auto adiValidUltrasonicDetectionProb
            = adiValidUltrasonicDetectionList.at(index).UltrasonicDetectionStatus.ExistenceProbabilityDetectionLevel;
        EXPECT_FLOAT_EQ(defaultUltrasonicDetectionProb.at(index), adiValidUltrasonicDetectionProb);
        RecordProperty("adiValidUltrasonicDetectionProb: ", std::to_string(adiValidUltrasonicDetectionProb));
    }
}

TEST_F(test_osi_adi_mapper, test_ultrasonicdetection_ultrasonicDetObjectID)
{

    std::vector<uint16_t> defaultValuesUltrasonicDetObjectID;
    defaultValuesUltrasonicDetObjectID.push_back(5);

    for (std::size_t index = 0; index < adiUltrasonicDetectionsInterface.ValidUltrasonicDetectionList.size(); index++) {

        auto adiUltrasonicDetObjectID = adiUltrasonicDetectionsInterface.ValidUltrasonicDetectionList.at(index)
                                            .UltrasonicDetectionStatus.ObjectID;
        EXPECT_EQ(defaultValuesUltrasonicDetObjectID.at(index), adiUltrasonicDetObjectID);
        RecordProperty("adiUltrasonicDetObjectID: ", adiUltrasonicDetObjectID);
    }
}

TEST_F(test_osi_adi_mapper, test_ultrasonicDetection_DataQualifier)
{

    auto adiValidUltrasonicDetectionDataQualifier
        = adiUltrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader.InformationInterface.DataQualifier;

    EXPECT_THAT(DataQualifier::kNormal, adiValidUltrasonicDetectionDataQualifier);
    RecordProperty("adiValidUltrasonicDetectionDataQualifier: ", "DataQualifier::kNormal");
}

TEST_F(test_osi_adi_mapper, test_ultrasonicDetection_CycleCounter)
{

    auto adiValidUltrasonicDetectionCycleCounter
        = adiUltrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader.InformationInterface.CycleCounter;

    EXPECT_THAT(27, adiValidUltrasonicDetectionCycleCounter);
    RecordProperty("adiValidUltrasonicDetectionCycleCounter: ", adiValidUltrasonicDetectionCycleCounter);
}

TEST_F(test_osi_adi_mapper, test_ultrasonicDetection_VersionID)
{

    auto adiValidUltrasonicDetectionVersionID
        = adiUltrasonicDetectionsInterface.UltrasonicDetectionsInterfaceHeader.InformationInterface.VersionID;

    EXPECT_THAT(1, adiValidUltrasonicDetectionVersionID.InterfaceVersionIDMajor);
    EXPECT_THAT(0, adiValidUltrasonicDetectionVersionID.InterfaceVersionIDMinor);
    EXPECT_THAT(0, adiValidUltrasonicDetectionVersionID.InterfaceVersionIDPatch);
    RecordProperty(
        "adiValidUltrasonicDetectionVersionIDMajor: ", adiValidUltrasonicDetectionVersionID.InterfaceVersionIDMajor);
    RecordProperty(
        "adiValidUltrasonicDetectionVersionIDMinor: ", adiValidUltrasonicDetectionVersionID.InterfaceVersionIDMinor);
    RecordProperty(
        "adiValidUltrasonicDetectionVersionIDPatch: ", adiValidUltrasonicDetectionVersionID.InterfaceVersionIDPatch);
}

TEST_F(test_osi_adi_mapper, test_ultrasonicFeature_VersionID)
{

    auto adiValidUltrasonicFeatureVersionID
        = adiUltrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface.VersionID;

    EXPECT_THAT(1, adiValidUltrasonicFeatureVersionID.InterfaceVersionIDMajor);
    EXPECT_THAT(1, adiValidUltrasonicFeatureVersionID.InterfaceVersionIDMinor);
    EXPECT_THAT(0, adiValidUltrasonicFeatureVersionID.InterfaceVersionIDPatch);
    RecordProperty(
        "adiValidUltrasonicFeatureVersionIDMajor: ", adiValidUltrasonicFeatureVersionID.InterfaceVersionIDMajor);
    RecordProperty(
        "adiValidUltrasonicFeatureVersionIDMinor: ", adiValidUltrasonicFeatureVersionID.InterfaceVersionIDMinor);
    RecordProperty(
        "adiValidUltrasonicFeatureVersionIDPatch: ", adiValidUltrasonicFeatureVersionID.InterfaceVersionIDPatch);
}

TEST_F(test_osi_adi_mapper, test_ultrasonicFeature_DataQualifier)
{

    auto adiUltrasonicFeatureInterfaceDataQualifier
        = adiUltrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface.DataQualifier;

    EXPECT_THAT(DataQualifier::kNormal, adiUltrasonicFeatureInterfaceDataQualifier);
    RecordProperty("adiUltrasonicFeatureInterfaceDataQualifier: ", "DataQualifier::kNormal");
}

TEST_F(test_osi_adi_mapper, test_ultrasonicFeature_SesnorId)
{

    auto adiUltrasonicFeatureSensorId
        = adiUltrasonicFeatureInterface.UltrasonicFeatureInterfaceHeader.InformationInterface.ValidServingSensors.at(0);

    EXPECT_THAT(3, adiUltrasonicFeatureSensorId);
    RecordProperty("adiUltrasonicFeatureSensorId: ", adiUltrasonicFeatureSensorId);
}

TEST_F(test_osi_adi_mapper, test_ultrasonicFeature_Velocity)
{

    std::vector<Point2D> defaultUltrasonicFeatureVelocity;
    Point2D defaultVel;
    defaultVel.x = 19;
    defaultVel.y = 12;
    defaultUltrasonicFeatureVelocity.push_back(defaultVel);
    auto adiValidUltrasonicFeaturesList = adiUltrasonicFeatureInterface.ValidUltrasonicFeaturesList;

    for (std::size_t index = 0; index < adiValidUltrasonicFeaturesList.size(); index++) {
        auto adiValidSegmentPointsList
            = adiValidUltrasonicFeaturesList.at(index).UltrasonicFeaturesSegmentPoints.ValidSegmentPointsList;

        for (std::size_t UFSegPointIndex = 0; UFSegPointIndex < adiValidSegmentPointsList.size(); UFSegPointIndex++) {
            auto adiUltrasonicFeatureVelocity = adiValidSegmentPointsList.at(UFSegPointIndex).VelocityUltrasonic;
            EXPECT_FLOAT_EQ(defaultUltrasonicFeatureVelocity.at(UFSegPointIndex).x, adiUltrasonicFeatureVelocity.x);
            EXPECT_FLOAT_EQ(defaultUltrasonicFeatureVelocity.at(UFSegPointIndex).y, adiUltrasonicFeatureVelocity.y);

            RecordProperty("adiUltrasonicFeatureVelocity.x: ", std::to_string(adiUltrasonicFeatureVelocity.x));
            RecordProperty("adiUltrasonicFeatureVelocity.y: ", std::to_string(adiUltrasonicFeatureVelocity.y));
        }
    }
}

TEST_F(test_osi_adi_mapper, test_ultrasonicFeature_Position)
{

    std::vector<Point3D> defaultUltrasonicFeaturePosition;
    Point3D defaultVel;
    defaultVel.x = 25.95;
    defaultVel.y = 89.34;
    defaultVel.z = 0;
    defaultUltrasonicFeaturePosition.push_back(defaultVel);
    auto adiValidUltrasonicFeaturesList = adiUltrasonicFeatureInterface.ValidUltrasonicFeaturesList;

    for (std::size_t index = 0; index < adiValidUltrasonicFeaturesList.size(); index++) {
        auto adiValidSegmentPointsList
            = adiValidUltrasonicFeaturesList.at(index).UltrasonicFeaturesSegmentPoints.ValidSegmentPointsList;

        for (std::size_t UFSegPointIndex = 0; UFSegPointIndex < adiValidSegmentPointsList.size(); UFSegPointIndex++) {
            auto adiUltrasonicFeaturePosition = adiValidSegmentPointsList.at(UFSegPointIndex).Position;
            EXPECT_FLOAT_EQ(defaultUltrasonicFeaturePosition.at(UFSegPointIndex).x, adiUltrasonicFeaturePosition.x);
            EXPECT_FLOAT_EQ(defaultUltrasonicFeaturePosition.at(UFSegPointIndex).y, adiUltrasonicFeaturePosition.y);
            EXPECT_FLOAT_EQ(defaultUltrasonicFeaturePosition.at(UFSegPointIndex).z, adiUltrasonicFeaturePosition.z);

            RecordProperty("adiUltrasonicFeaturePosition.x: ", std::to_string(adiUltrasonicFeaturePosition.x));
            RecordProperty("adiUltrasonicFeaturePosition.y: ", std::to_string(adiUltrasonicFeaturePosition.y));
            RecordProperty("adiUltrasonicFeaturePosition.z: ", std::to_string(adiUltrasonicFeaturePosition.z));
        }
    }
}

TEST_F(test_osi_adi_mapper, test_ultrasonicFeature_PositionErr)
{

    std::vector<Point3DError> defaultUltrasonicFeaturePositionErr;
    Point3DError defaultPosErr;
    defaultPosErr.xError = 0.9f;
    defaultPosErr.yError = 0.8f;
    defaultPosErr.zError = 0.7f;
    defaultUltrasonicFeaturePositionErr.push_back(defaultPosErr);
    auto adiValidUltrasonicFeaturesList = adiUltrasonicFeatureInterface.ValidUltrasonicFeaturesList;

    for (std::size_t index = 0; index < adiValidUltrasonicFeaturesList.size(); index++) {
        auto adiValidSegmentPointsList
            = adiValidUltrasonicFeaturesList.at(index).UltrasonicFeaturesSegmentPoints.ValidSegmentPointsList;

        for (std::size_t UFSegPointIndex = 0; UFSegPointIndex < adiValidSegmentPointsList.size(); UFSegPointIndex++) {
            auto adiUltrasonicFeaturePositionErr = adiValidSegmentPointsList.at(UFSegPointIndex).PositionError;
            EXPECT_FLOAT_EQ(
                defaultUltrasonicFeaturePositionErr.at(UFSegPointIndex).xError, adiUltrasonicFeaturePositionErr.xError);
            EXPECT_FLOAT_EQ(
                defaultUltrasonicFeaturePositionErr.at(UFSegPointIndex).yError, adiUltrasonicFeaturePositionErr.yError);
            EXPECT_FLOAT_EQ(
                defaultUltrasonicFeaturePositionErr.at(UFSegPointIndex).zError, adiUltrasonicFeaturePositionErr.zError);

            RecordProperty(
                "adiUltrasonicFeaturePositionErr.xErr: ", std::to_string(adiUltrasonicFeaturePositionErr.xError));
            RecordProperty(
                "adiUltrasonicFeaturePositionErr.yErr: ", std::to_string(adiUltrasonicFeaturePositionErr.yError));
            RecordProperty(
                "adiUltrasonicFeaturePositionErr.zErr: ", std::to_string(adiUltrasonicFeaturePositionErr.zError));
        }
    }
}

int main()
{
    testing::FLAGS_gtest_output = "xml:/mnt/native-docker-build/myMapperTestReports/myMapperTestReport.xml";
    testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}
