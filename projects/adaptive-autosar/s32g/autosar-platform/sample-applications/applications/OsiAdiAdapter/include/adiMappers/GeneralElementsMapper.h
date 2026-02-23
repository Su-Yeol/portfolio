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

#ifndef OSI_ADI_ADAPTER_GENERALELEMENTS_MAPPER_HPP_
#define OSI_ADI_ADAPTER_GENERALELEMENTS_MAPPER_HPP_

#include "ara/log/logger.h"

#include "osi3/osi_sensordata.pb.h"

#include "ara/adi/sensoritf/impl_type_point3d.h"
#include "ara/adi/sensoritf/impl_type_point2d.h"
#include "ara/adi/sensoritf/impl_type_point3derror.h"
#include "ara/adi/sensoritf/impl_type_point2derror.h"
#include "ara/adi/sensoritf/impl_type_position3dspheric.h"
#include "ara/adi/sensoritf/impl_type_position3dsphericerror.h"
#include "ara/adi/sensoritf/impl_type_orientation3d.h"
#include "ara/adi/sensoritf/impl_type_dimensionbox.h"
#include "ara/adi/sensoritf/impl_type_dimensionboxerror.h"
#include "ara/adi/sensoritf/impl_type_informationinterface.h"
#include "ara/adi/sensoritf/impl_type_dataqualifier.h"
#include "ara/adi/sensoritf/impl_type_interfaceversionid.h"
#include "ara/adi/sensoritf/impl_type_informationsensorpose.h"
#include "ara/adi/sensoritf/impl_type_colourmodeltype.h"
#include "ara/adi/sensoritf/impl_type_colourtone.h"
#include "ara/adi/sensoritf/impl_type_interfaceheader.h"

using namespace ara::adi::sensoritf;

class GeneralElementsMapper
{

protected:
    Point3D mapPositiontoADI(const osi3::Vector3d& osiPosition) const;
    Point2D mapPosition2DtoADI(const osi3::Vector3d& osiPosition) const;
    Point3DError mapPositionErrtoADI(const osi3::Vector3d& osiPositionError) const;
    Point2DError mapPosition2DErrtoADI(const osi3::Vector3d& osiPositionError) const;
    Position3DSpheric mapPositiontoADI(const osi3::Spherical3d& osiPosition) const;
    Position3DSphericError mapPositionErrtoADI(const osi3::Spherical3d& osiPositionError) const;
    Orientation3D mapOrientationtoADI(const osi3::Orientation3d& osiOrientation) const;
    Orientation3DError mapOrientationErrortoADI(const osi3::Orientation3d& osiOrientationError) const;
    Point3D mapAccntoADI(const osi3::Vector3d& osiPosition) const;
    Point3DError mapAccnErrtoADI(const osi3::Vector3d& osiPositionError) const;
    DimensionBox mapDimensiontoADI(const osi3::Dimension3d& osiDimension) const;
    DimensionBoxError mapToDimension3dError(const osi3::Dimension3d& osiDimensionErr) const;
    InterfaceVersionID mapInterfaceVersion(const osi3::InterfaceVersion& osiInterfaceVersion) const;
    ColourTone mapToColorToneADI(const osi3::ColorDescription& osiColorDescription) const;
    uint64_t mapToTimestamp(const osi3::Timestamp& osiTimestamp) const;
    InterfaceHeader mapInterfaceHeader(const osi3::DetectedEntityHeader& osiDetectedEntityHeader) const;

private:
    ara::log::Logger& m_ge_logger{ara::log::CreateLogger("GEM", "...", ara::log::LogLevel::kVerbose)};
};

#endif  // OSI_ADI_ADAPTER_GENERALELEMENTS_MAPPER_HPP_
