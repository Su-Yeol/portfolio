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

#include "adiMappers/GeneralElementsMapper.h"
#include "osi3/osi_sensordata.pb.h"

Point3D GeneralElementsMapper::mapPositiontoADI(const osi3::Vector3d& osiPosition) const
{
    Point3D adiPosition{};

    if (osiPosition.has_x())
        adiPosition.x = static_cast<float>(osiPosition.x());

    if (osiPosition.has_y())
        adiPosition.y = static_cast<float>(osiPosition.y());

    if (osiPosition.has_z())
        adiPosition.z = static_cast<float>(osiPosition.z());

    return adiPosition;
}

Point2D GeneralElementsMapper::mapPosition2DtoADI(const osi3::Vector3d& osiPosition) const
{
    Point2D adiPosition{};

    if (osiPosition.has_x())
        adiPosition.x = static_cast<float>(osiPosition.x());

    if (osiPosition.has_y())
        adiPosition.y = static_cast<float>(osiPosition.y());

    return adiPosition;
}

Position3DSpheric GeneralElementsMapper::mapPositiontoADI(const osi3::Spherical3d& osiPosition) const
{

    Position3DSpheric adiPosition{};

    if (osiPosition.has_azimuth())
        adiPosition.azimuth = static_cast<float>(osiPosition.azimuth());

    if (osiPosition.has_elevation())
        adiPosition.elevation = static_cast<float>(osiPosition.elevation());

    if (osiPosition.has_distance())
        adiPosition.distance = static_cast<float>(osiPosition.distance());

    return adiPosition;
}

Point3DError GeneralElementsMapper::mapPositionErrtoADI(const osi3::Vector3d& osiPositionError) const
{

    Point3DError adiPositionError{};

    if (osiPositionError.has_x())
        adiPositionError.xError = static_cast<float>(osiPositionError.x());

    if (osiPositionError.has_y())
        adiPositionError.yError = static_cast<float>(osiPositionError.y());

    if (osiPositionError.has_z())
        adiPositionError.zError = static_cast<float>(osiPositionError.z());

    return adiPositionError;
}

Point2DError GeneralElementsMapper::mapPosition2DErrtoADI(const osi3::Vector3d& osiPositionError) const
{
    Point2DError adiPositionError{};

    if (osiPositionError.has_x())
        adiPositionError.xError = static_cast<float>(osiPositionError.x());

    if (osiPositionError.has_y())
        adiPositionError.yError = static_cast<float>(osiPositionError.y());

    return adiPositionError;
}

Position3DSphericError GeneralElementsMapper::mapPositionErrtoADI(const osi3::Spherical3d& osiPositionError) const
{
    Position3DSphericError adiPositionSpherical{};

    if (osiPositionError.has_azimuth())
        adiPositionSpherical.azimuth = static_cast<float>(osiPositionError.azimuth());

    if (osiPositionError.has_elevation())
        adiPositionSpherical.elevation = static_cast<float>(osiPositionError.elevation());

    if (osiPositionError.has_distance())
        adiPositionSpherical.distance = static_cast<float>(osiPositionError.distance());

    return adiPositionSpherical;
}

Orientation3D GeneralElementsMapper::mapOrientationtoADI(const osi3::Orientation3d& osiOrientation) const
{

    Orientation3D adiOrientation{};

    if (osiOrientation.has_yaw())
        adiOrientation.Yaw = static_cast<float>(osiOrientation.yaw());

    if (osiOrientation.has_roll())
        adiOrientation.Roll = static_cast<float>(osiOrientation.roll());

    if (osiOrientation.has_pitch())
        adiOrientation.Pitch = static_cast<float>(osiOrientation.pitch());

    return adiOrientation;
}

Orientation3DError GeneralElementsMapper::mapOrientationErrortoADI(const osi3::Orientation3d& osiOrientationError) const
{

    Orientation3DError adiOrientationErr{};

    if (osiOrientationError.has_yaw())
        adiOrientationErr.YawError = static_cast<float>(osiOrientationError.yaw());

    if (osiOrientationError.has_roll())
        adiOrientationErr.RollError = static_cast<float>(osiOrientationError.roll());

    if (osiOrientationError.has_pitch())
        adiOrientationErr.PitchError = static_cast<float>(osiOrientationError.pitch());

    return adiOrientationErr;
}

DimensionBox GeneralElementsMapper::mapDimensiontoADI(const osi3::Dimension3d& osiDimension) const
{

    DimensionBox adidimBbox{};

    if (osiDimension.has_length())
        adidimBbox.Length = static_cast<float>(osiDimension.length());

    if (osiDimension.has_width())
        adidimBbox.Width = static_cast<float>(osiDimension.width());

    if (osiDimension.has_height())
        adidimBbox.Height = static_cast<float>(osiDimension.height());

    return adidimBbox;
}

DimensionBoxError GeneralElementsMapper::mapToDimension3dError(const osi3::Dimension3d& osiDimensionErr) const
{

    DimensionBoxError adidimBboxErr{};

    if (osiDimensionErr.has_length())
        adidimBboxErr.Length = static_cast<float>(osiDimensionErr.length());

    if (osiDimensionErr.has_width())
        adidimBboxErr.Width = static_cast<float>(osiDimensionErr.width());

    if (osiDimensionErr.has_height())
        adidimBboxErr.Height = static_cast<float>(osiDimensionErr.height());

    return adidimBboxErr;
}

InterfaceVersionID GeneralElementsMapper::mapInterfaceVersion(const osi3::InterfaceVersion& osiInterfaceVersion) const
{

    InterfaceVersionID adiInterfaceVerId{};

    if (osiInterfaceVersion.has_version_major())
        adiInterfaceVerId.InterfaceVersionIDMajor = static_cast<u_int8_t>(osiInterfaceVersion.version_major());

    if (osiInterfaceVersion.has_version_minor())
        adiInterfaceVerId.InterfaceVersionIDMinor = static_cast<u_int8_t>(osiInterfaceVersion.version_minor());

    if (osiInterfaceVersion.has_version_patch())
        adiInterfaceVerId.InterfaceVersionIDPatch = static_cast<u_int8_t>(osiInterfaceVersion.version_patch());

    return adiInterfaceVerId;
}

uint64_t GeneralElementsMapper::mapToTimestamp(const osi3::Timestamp& osiTimestamp) const
{

    uint64_t adiTimestamp{};

    adiTimestamp = osiTimestamp.seconds();

    return adiTimestamp;
}

InterfaceHeader GeneralElementsMapper::mapInterfaceHeader(
    const osi3::DetectedEntityHeader& osiDetectedEntityHeader) const
{

    InterfaceHeader interfaceHeader{};

    ara::adi::sensoritf::InformationInterface adiInformationInterface{};

    if (osiDetectedEntityHeader.has_cycle_counter())
        adiInformationInterface.CycleCounter = osiDetectedEntityHeader.cycle_counter();

    if (osiDetectedEntityHeader.has_data_qualifier()) {
        osi3::DetectedEntityHeader_DataQualifier osiDataQualifier = osiDetectedEntityHeader.data_qualifier();

        switch (osiDataQualifier) {
        case osi3::DetectedEntityHeader_DataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_OTHER: {
            adiInformationInterface.DataQualifier = DataQualifier::kUnknown;

            break;
        }
        case osi3::DetectedEntityHeader_DataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_BLINDNESS: {
            adiInformationInterface.DataQualifier = DataQualifier::kReduceInCoverage;

            break;
        }
        case osi3::DetectedEntityHeader_DataQualifier::
            DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_NOT_AVAILABLE: {
            adiInformationInterface.DataQualifier = DataQualifier::kNotAvailable;

            break;
        }
        case osi3::DetectedEntityHeader_DataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE: {
            adiInformationInterface.DataQualifier = DataQualifier::kNormal;

            break;
        }
        case osi3::DetectedEntityHeader_DataQualifier::
            DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_AVAILABLE_REDUCED: {
            adiInformationInterface.DataQualifier = DataQualifier::kReduceInCoverage;

            break;
        }
        case osi3::DetectedEntityHeader_DataQualifier::
            DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_TEMPORARY_AVAILABLE: {
            adiInformationInterface.DataQualifier = DataQualifier::kReduceInViewAndPerformance;

            break;
        }
        case osi3::DetectedEntityHeader_DataQualifier::DetectedEntityHeader_DataQualifier_DATA_QUALIFIER_UNKNOWN: {
            adiInformationInterface.DataQualifier = DataQualifier::kUnknown;

            break;
        }
        default: {
            m_ge_logger.LogError() << "Error: mapInterfaceHeader: DataQualifier"
                                      "--> Unknown enum entry: ";
            adiInformationInterface.DataQualifier = DataQualifier::kUnknown;
        }
        }
    }

    // TODO
    // adiInformationInterface.InterfaceID = osiSensorData.
    // adiInformationInterface.NumberOfValidServingSensors
    // InformationInterface.TimeStamp = osiDetectedEntityHeader.measurement_time().seconds();
    // adiInformationInterface.ValidServingSensors
    // adiInformationInterface.VersionId

    // Interface Extension
    // TODO: Color mapping definitions different for ADI and OSI here
    // osi3::ColorDescription osiColorDescription = osiSensorData.moving_object(0).color_description();
    //
    // if(osiColorDescription.has_grey()) {
    //    interfaceHeader.InterfaceExtension.ColourModelType = ColourModelType::kGrey;
    //}
    // else if(osiColorDescription.has_hsv()) {
    //    interfaceHeader.InterfaceExtension.ColourModelType = ColourModelType::kHSV;
    //}
    // else if(osiColorDescription.has_luv()) {
    //    interfaceHeader.InterfaceExtension.ColourModelType = ColourModelType::kLUV;
    //}
    // else if(osiColorDescription.has_rgb()) {
    //    interfaceHeader.InterfaceExtension.ColourModelType = ColourModelType::kRGB;
    //}
    // else {
    //    // TODO Warning
    //}

    // interfaceHeader.InterfaceExtension.InformationAmbiguityDomain

    // Sensor Calibration

    // interfaceHeader.SensorCalibration.CoorectionPosLimitMax;
    // interfaceHeader.SensorCalibration.SensorOrientationCorrection
    // interfaceHeader.SensorCalibration.SensorOrientationCorrectionError
    // interfaceHeader.SensorCalibration.SensorOriginPointCorrection
    // interfaceHeader.SensorCalibration.SensorOriginPointCorrectionError

    // SensorClusterInformation

    // interfaceHeader.SensorClusterInformation.NumberOfValidSensors
    // interfaceHeader.SensorClusterInformation.SensorIDReferenceList

    // SensorPose

    // interfaceHeader.SensorPose.SensorOrientation
    // interfaceHeader.SensorPose.SensorOrientationError
    // interfaceHeader.SensorPose.SensorOrientationErrorVector
    // interfaceHeader.SensorPose.SensorOriginPoint
    // interfaceHeader.SensorPose.SensorOriginPointError
    // interfaceHeader.SensorPose.SensorOriginPointErrorVector

    // SensorSurrounding

    // interfaceHeader.SensorSurrounding.VanishingPoint
    // interfaceHeader.SensorSurrounding.VanishingPointError

    // VehicleCoordinateSystem
    // interfaceHeader.VehicleCoordinateSystem

    interfaceHeader.InformationInterface = adiInformationInterface;

    return interfaceHeader;
}

// ColourTone GeneralElementsMapper::mapToColorToneADI(const osi3::ColorDescription& osiColorDescription) const {
//    ColourTone adiColourTone{};
//
//    osiColorDescription.
//    adiColourTone.ValidColourModel.push_back(static_cast<float>(osiColorDescription.cmyk().cyan()));
//
//
//}
