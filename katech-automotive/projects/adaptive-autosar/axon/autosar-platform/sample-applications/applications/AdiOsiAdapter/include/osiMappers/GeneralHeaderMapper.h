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

#ifndef ADI_OSI_ADAPTER_GENERALHEADER_MAPPER_HPP_
#define ADI_OSI_ADAPTER_GENERALHEADER_MAPPER_HPP_

#include "ara/log/logger.h"

#include "osi3/osi_roadmarking.pb.h"
#include "osi3/osi_sensordata.pb.h"
#include "osi3/osi_datarecording.pb.h"

#include "ara/adi/sensoritf/impl_type_point3d.h"
#include "ara/adi/sensoritf/impl_type_orientation3d.h"
#include "ara/adi/sensoritf/impl_type_dimensionbox.h"
#include "ara/adi/sensoritf/impl_type_dimensionboxerror.h"
#include "ara/adi/sensoritf/impl_type_informationinterface.h"
#include "ara/adi/sensoritf/impl_type_dataqualifier.h"
#include "ara/adi/sensoritf/impl_type_interfaceversionid.h"
#include "ara/adi/sensoritf/impl_type_informationsensorpose.h"
#include "ara/adi/sensoritf/impl_type_colourmodeltype.h"
#include "ara/adi/sensoritf/impl_type_colourtone.h"

class GeneralHeaderMapper
{
private:
    ara::log::Logger& m_logger{ara::log::CreateLogger("GHM", "...", ara::log::LogLevel::kVerbose)};

protected:
    osi3::Vector3d mapPoint3D2Osi(const ara::adi::sensoritf::Point3D& point) const;
    osi3::Vector3d mapPoint3DError2Osi(const ara::adi::sensoritf::Point3DError& point) const;
    osi3::Orientation3d mapOrientation3D2Osi(const ara::adi::sensoritf::Orientation3D& orientation) const;
    osi3::Orientation3d mapToOrientation3dError(const ara::adi::sensoritf::Orientation3DError& orientation) const;
    osi3::Dimension3d mapDimension3D2Osi(const ara::adi::sensoritf::DimensionBox& dimension) const;
    osi3::Dimension3d mapToDimension3dError(const ara::adi::sensoritf::DimensionBoxError& dimension) const;
    osi3::InterfaceVersion mapInterfaceVersion(const ara::adi::sensoritf::InterfaceVersionID& interfaceVersion) const;
    osi3::ColorDescription mapToColorDesription(const ara::adi::sensoritf::ColourModelType& colorModelType,
        const ara::adi::sensoritf::ColourTone& colorTone) const;
    osi3::Timestamp mapToTimestamp(const uint64_t timestamp) const;
};

#endif  // ADI_OSI_ADAPTER_GENERALHEADER_MAPPER_HPP_
