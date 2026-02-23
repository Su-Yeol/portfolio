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

#include "osiMappers/GeneralHeaderMapper.h"

namespace
{
const std::uint32_t kColorToneLuvValues = 3;
const std::uint32_t kColorToneLuminance = 0;
const std::uint32_t kColorToneU = 1;
const std::uint32_t kColorToneV = 2;
const std::uint32_t kColorToneHSVValues = 3;
const std::uint32_t kColorToneHue = 0;
const std::uint32_t kColorToneSaturation = 1;
const std::uint32_t kColorToneValue = 2;
const std::uint32_t kColorToneGreyValues = 1;
const std::uint32_t kColorToneGrey = 0;
const std::uint32_t kColorToneRgbValues = 3;
const std::uint32_t kColorToneRed = 0;
const std::uint32_t kColorToneGreen = 1;
const std::uint32_t kColorToneBlue = 2;
}  // namespace

osi3::Vector3d GeneralHeaderMapper::mapPoint3D2Osi(const ara::adi::sensoritf::Point3D& point) const
{
    osi3::Vector3d vector;

    vector.set_x(point.x);
    vector.set_y(point.y);
    vector.set_z(point.z);

    return vector;
}

osi3::Vector3d GeneralHeaderMapper::mapPoint3DError2Osi(const ara::adi::sensoritf::Point3DError& point) const
{
    osi3::Vector3d vector;

    vector.set_x(point.xError);
    vector.set_y(point.yError);
    vector.set_z(point.zError);

    return vector;
}

osi3::Orientation3d GeneralHeaderMapper::mapOrientation3D2Osi(
    const ara::adi::sensoritf::Orientation3D& orientation) const
{
    osi3::Orientation3d vector;

    vector.set_pitch(orientation.Pitch);
    vector.set_roll(orientation.Roll);
    vector.set_yaw(orientation.Yaw);

    return vector;
}

osi3::Orientation3d GeneralHeaderMapper::mapToOrientation3dError(
    const ara::adi::sensoritf::Orientation3DError& orientation) const
{
    osi3::Orientation3d vector;

    vector.set_pitch(orientation.PitchError);
    vector.set_roll(orientation.RollError);
    vector.set_yaw(orientation.YawError);

    return vector;
}

osi3::Dimension3d GeneralHeaderMapper::mapDimension3D2Osi(const ara::adi::sensoritf::DimensionBox& dimension) const
{
    osi3::Dimension3d vector;

    vector.set_length(dimension.Length);
    vector.set_width(dimension.Width);
    vector.set_height(dimension.Height);

    return vector;
}

osi3::Dimension3d GeneralHeaderMapper::mapToDimension3dError(
    const ara::adi::sensoritf::DimensionBoxError& dimError) const
{
    osi3::Dimension3d vector;

    vector.set_length(dimError.Length);
    vector.set_width(dimError.Width);
    vector.set_height(dimError.Height);

    return vector;
}

osi3::InterfaceVersion GeneralHeaderMapper::mapInterfaceVersion(
    const ara::adi::sensoritf::InterfaceVersionID& interfaceVersion) const
{
    m_logger.LogInfo() << "mapInterfaceVersion";
    osi3::InterfaceVersion osiInterfaceVersion;
    // TODO note: map uint32 to unit8
    osiInterfaceVersion.set_version_major(interfaceVersion.InterfaceVersionIDMajor);
    osiInterfaceVersion.set_version_minor(interfaceVersion.InterfaceVersionIDMinor);
    osiInterfaceVersion.set_version_patch(interfaceVersion.InterfaceVersionIDPatch);

    return osiInterfaceVersion;
}

osi3::ColorDescription GeneralHeaderMapper::mapToColorDesription(
    const ara::adi::sensoritf::ColourModelType& colorModelType,
    const ara::adi::sensoritf::ColourTone& colorTone) const
{
    osi3::ColorDescription colorDescription;

    switch (colorModelType) {
    case ara::adi::sensoritf::ColourModelType::kGrey: {
        if (colorTone.ValidColourModel.size() != kColorToneGreyValues) {
            m_logger.LogWarn() << "Warning: validColorModel size: " << colorTone.ValidColourModel.size()
                               << "dose not fit to ColourModelType::kGrey size: " << kColorToneGreyValues
                               << ". return empty";
            break;
        }

        osi3::ColorGrey* grey = colorDescription.mutable_grey();
        grey->set_grey(static_cast<double>(colorTone.ValidColourModel.at(kColorToneGrey)));
        // colorDescription.set_allocated_grey(&grey);
    } break;

    case ara::adi::sensoritf::ColourModelType::kHSV: {
        if (colorTone.ValidColourModel.size() != kColorToneHSVValues) {
            m_logger.LogWarn() << "Warning: validColorModel size: " << colorTone.ValidColourModel.size()
                               << "dose not fit to ColourModelType::kHSV size: " << kColorToneHSVValues
                               << ". return empty";
            break;
        }

        osi3::ColorHSV* hsv = colorDescription.mutable_hsv();
        hsv->set_hue(static_cast<double>(colorTone.ValidColourModel.at(kColorToneHue)));
        hsv->set_saturation(static_cast<double>(colorTone.ValidColourModel.at(kColorToneSaturation)));
        hsv->set_value(static_cast<double>(colorTone.ValidColourModel.at(kColorToneValue)));

    } break;

    case ara::adi::sensoritf::ColourModelType::kLUV: {
        if (colorTone.ValidColourModel.size() != kColorToneLuvValues) {
            m_logger.LogWarn() << "Warning: validColorModel size: " << colorTone.ValidColourModel.size()
                               << "dose not fit to ColourModelType::kLUV size: " << kColorToneLuvValues
                               << ". return empty";
            break;
        }
        osi3::ColorLUV* luv = colorDescription.mutable_luv();
        luv->set_luminance(static_cast<double>(colorTone.ValidColourModel.at(kColorToneLuminance)));
        luv->set_u(static_cast<double>(colorTone.ValidColourModel.at(kColorToneU)));
        luv->set_v(static_cast<double>(colorTone.ValidColourModel.at(kColorToneV)));

    } break;

    case ara::adi::sensoritf::ColourModelType::kRGB: {
        if (colorTone.ValidColourModel.size() != kColorToneRgbValues) {
            m_logger.LogWarn() << "Warning: validColorModel size: " << colorTone.ValidColourModel.size()
                               << "dose not fit to ColourModelType::kRGB size: " << kColorToneRgbValues
                               << ". return empty";
            break;
        }

        osi3::ColorRGB* rgb = colorDescription.mutable_rgb();
        rgb->set_red(static_cast<double>(colorTone.ValidColourModel.at(kColorToneRed)));
        rgb->set_green(static_cast<double>(colorTone.ValidColourModel.at(kColorToneGreen)));
        rgb->set_blue(static_cast<double>(colorTone.ValidColourModel.at(kColorToneBlue)));

    } break;

    case ara::adi::sensoritf::ColourModelType::kColourList: {
        m_logger.LogInfo() << "Info: This type is not supported yet."
                           << "return empty";
    } break;

    default: {
        m_logger.LogWarn() << "Warning: Unknown type for "
                              "ara::adi::sensoritf::ColourModelType."
                           << "return empty";
    } break;
    }

    return colorDescription;
}

osi3::Timestamp GeneralHeaderMapper::mapToTimestamp(const uint64_t timestamp) const
{
    m_logger.LogInfo() << "mapToTimestamp";
    osi3::Timestamp osiTimestamp;

    osiTimestamp.set_seconds(timestamp);

    //
    // osiTimestamp.set_nanos(...);

    return osiTimestamp;
}
