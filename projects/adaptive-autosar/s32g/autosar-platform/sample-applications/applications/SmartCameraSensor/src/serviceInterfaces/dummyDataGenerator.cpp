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

#include "serviceInterfaces/dummyDataGenerator.h"

ara::adi::sensoritf::InterfaceHeader DummyDataGenerator::getInterfaceHeader()
{
    // optional structs can be set as whole but the elements can not be accessed
    // directly
    ara::adi::sensoritf::InterfaceHeader interfaceHeader = ara::adi::sensoritf::InterfaceHeader();

    ara::adi::sensoritf::InformationInterface informationInterface;
    informationInterface.VersionID.InterfaceVersionIDMajor = 1;
    informationInterface.VersionID.InterfaceVersionIDMinor = 2;
    informationInterface.VersionID.InterfaceVersionIDPatch = 3;
    informationInterface.NumberOfValidServingSensors = 3;
    informationInterface.ValidServingSensors.push_back(2);
    informationInterface.InterfaceID = ara::adi::sensoritf::InterfaceID::kPotentialMovingObject;  // (optional)
    informationInterface.TimeStamp = 123456;
    informationInterface.CycleCounter = 123;  // (optional)
    informationInterface.InterfaceCycleTime = 123456;  // (optional)
    informationInterface.InterfaceCycleTimeVariation = 4;  // (optional)
    informationInterface.DataQualifier = ara::adi::sensoritf::DataQualifier::kTestMode;
    interfaceHeader.InformationInterface = informationInterface;

    // optional
    interfaceHeader.VehicleCoordinateSystem = ara::adi::sensoritf::VehicleCoordinateSystemType::kRearAxle;

    // optional
    ara::adi::sensoritf::InformationSensorPose sensorPose;
    ara::adi::sensoritf::Point3D originPoint;
    originPoint.x = 1;
    originPoint.y = 2;
    originPoint.z = 3;
    ara::adi::sensoritf::Point3DError originPointError;
    originPointError.xError = 0.1;
    originPointError.yError = 0.2;
    originPointError.zError = 0.3;  // (optional)
    ara::adi::sensoritf::Point3DErrorVector originPointErrorVector;
    originPointErrorVector.xxError = 1;  // (optional)
    originPointErrorVector.xyError = 2;  // (optional)
    originPointErrorVector.xzError = 3;  // (optional)
    originPointErrorVector.yxError = 1;  // (optional)
    originPointErrorVector.yyError = 2;  // (optional)
    originPointErrorVector.yzError = 3;  // (optional)
    originPointErrorVector.zxError = 1;  // (optional)
    originPointErrorVector.zyError = 2;  // (optional)
    originPointErrorVector.zzError = 3;  // (optional)
    sensorPose.SensorOriginPoint = originPoint;
    sensorPose.SensorOriginPointError = originPointError;  // (optional)
    sensorPose.SensorOriginPointErrorVector = originPointErrorVector;  // (optional)
    ara::adi::sensoritf::Orientation3D orientation;
    orientation.Pitch = 1;
    orientation.Roll = 2;
    orientation.Yaw = 3;
    ara::adi::sensoritf::Orientation3DError orientationError;
    orientationError.PitchError = 0.1;
    orientationError.RollError = 0.2;
    orientationError.YawError = 0.3;
    ara::adi::sensoritf::Orientation3DErrorVector orientationErrorVector;
    orientationErrorVector.pitchpitchError = 1;  // (optional)
    orientationErrorVector.pitchrollError = 2;  // (optional)
    orientationErrorVector.pitchyawError = 3;  // (optional)
    orientationErrorVector.rollpitchError = 1;  // (optional)
    orientationErrorVector.rollrollError = 2;  // (optional)
    orientationErrorVector.rollyawError = 3;  // (optional)
    orientationErrorVector.yawpitchError = 1;  // (optional)
    orientationErrorVector.yawrollError = 2;  // (optional)
    orientationErrorVector.yawyawError = 3;  // (optional)
    sensorPose.SensorOrientation = orientation;
    sensorPose.SensorOrientationError = orientationError;  // (optional)
    sensorPose.SensorOrientationErrorVector = orientationErrorVector;  // (optional)
    interfaceHeader.SensorPose = sensorPose;  // (optional)

    // optional
    ara::adi::sensoritf::Calibration calibration;
    calibration.NoValidSensorCalibratableComponents = 1;
    ara::adi::sensoritf::CaliComponentInfo cci;
    cci.CaliComponent = ara::adi::sensoritf::SensorCalibratableComponent::kOnline;
    cci.CaliStatus = ara::adi::sensoritf::SensorCalibrationStatus::kRecalibrationNeededFull;
    cci.CaliProcessStatus = ara::adi::sensoritf::CalibrationProcessState::kRecalibrationNeededFull;  // (optional)
    calibration.ValidSensorCalibratableComponents.push_back(cci);
    calibration.SensorOriginPointCorrection = originPoint;  // (optional)
    calibration.SensorOriginPointCorrectionError = originPointError;  // (optional)
    ara::adi::sensoritf::SensorOriginTranslationCorrectionLimit sotcl;
    sotcl.xbegin = 0.1;
    sotcl.xend = 0.2;
    sotcl.ybegin = 0.1;
    sotcl.yend = 0.2;
    sotcl.zbegin = 0.1;  // (optional)
    sotcl.zend = 0.2;  // (optional)
    calibration.SensorOriginTranslationCorrectionLimitRange = sotcl;  // (optional)
    calibration.SensorOrientationCorrection = orientation;  // (optional)
    calibration.SensorOrientationCorrectionError = orientationError;  // (optional)
    calibration.CorrectionPosLimitMax = originPoint;  // (optional)
    ara::adi::sensoritf::SensorPoseAngleCorrectionLimit spacl;
    spacl.Yawbegin = 0.1;
    spacl.Yawend = 0.2;
    spacl.Pitchbegin = 0.1;
    spacl.Pitchend = 0.2;
    spacl.Rollbegin = 0.1;
    spacl.Rollend = 0.2;
    calibration.SensorPoseAngleCorrectionLimitRange = spacl;  // (optional)
    interfaceHeader.SensorCalibration = calibration;  // (optional)

    // optional
    ara::adi::sensoritf::SensorCluster cluster;
    cluster.NumberOfValidSensors = 2;
    cluster.SensorIDReferenceList.push_back(2);
    interfaceHeader.SensorClusterInformation = cluster;  // (optional)

    // optional
    ara::adi::sensoritf::InformationInterfaceExtension extension;
    extension.TrackingMotionModel = ara::adi::sensoritf::TrackingMotionModel::kConstantAcceleration;  // (optional)
    extension.MotionType = ara::adi::sensoritf::MotionType::kAbsoluteValues;
    extension.ColourModelType = ara::adi::sensoritf::ColourModelType::kHSV;  // (optional)
    ara::adi::sensoritf::InformationAmbiguityDomain ambiguityDomain;  // (optional)
    ara::adi::sensoritf::RadialVelocityAmbiguityDomain rvad;
    rvad.Begin = 0;
    rvad.End = 100;
    ara::adi::sensoritf::RangeAmbiguityDomain rad;
    rad.Begin = 2;
    rad.End = 5;
    ara::adi::sensoritf::AngleAzimuthAmbiguityDomain aaad;
    aaad.Begin = 7;
    aaad.End = 4.1;
    ara::adi::sensoritf::AngleElevationAmbiguityDomain aead;
    aead.Begin = 3.1;
    aead.End = 9.2;
    ambiguityDomain.VelocityAmbiguity = rvad;  // (optional)
    ambiguityDomain.RangeAmbiguity = rad;  // (optional)
    ambiguityDomain.AzimuthAmbiguity = aaad;  // (optional)
    ambiguityDomain.ElevationAmbiguity = aead;  // (optional)
    extension.InformationAmbiguityDomain = ambiguityDomain;  // (optional)
    extension.InterfaceApplicability = ara::adi::sensoritf::InterfaceApplicability::kPMOI;  // (optional)
    interfaceHeader.InterfaceExtension = extension;  // (optional)

    // optional
    ara::adi::sensoritf::InformationSensorSurrounding surrounding;
    ara::adi::sensoritf::VanishingPoint vp;
    vp.Azimuth = 0.5;
    vp.Elevation = 8.1;
    ara::adi::sensoritf::VanishingPointError vpe;
    vpe.Azimuth = 7;
    vpe.Elevation = 20.1;
    surrounding.VanishingPoint = vp;
    surrounding.VanishingPointError = vpe;  // (optional)
    interfaceHeader.SensorSurrounding = surrounding;  // (optional)

    return interfaceHeader;
}
