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

///////////////////////////////////////////////////////////////////////
// Activity specific implementation, skeleton can be generated from the model
// Discovery of services and sending/receiving of data according
// to the communication API
///////////////////////////////////////////////////////////////////////

#include "osiAdiAdapter.h"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

//#include "ara/com/sample/radar_skeleton.h"
#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/vector.h"

const ara::core::InstanceSpecifier ROSportSpecifier{"OsiAdiAdapter/OsiAdiAdapter/OsiAdiAdapter_PPort"};
const ara::core::InstanceSpecifier SOSportSpecifier{"OsiAdiAdapter/OsiAdiAdapter/OsiAdiAdapter_PPort2"};
const ara::core::InstanceSpecifier PMOSportSpecifier{"OsiAdiAdapter/OsiAdiAdapter/OsiAdiAdapter_PPort3"};
const ara::core::InstanceSpecifier CFSportSpecifier{"OsiAdiAdapter/OsiAdiAdapter/OsiAdiAdapter_PPort4"};
const ara::core::InstanceSpecifier CDSportSpecifier{"OsiAdiAdapter/OsiAdiAdapter/OsiAdiAdapter_PPort5"};
const ara::core::InstanceSpecifier UDSportSpecifier{"OsiAdiAdapter/OsiAdiAdapter/OsiAdiAdapter_PPort6"};
const ara::core::InstanceSpecifier UFSportSpecifier{"OsiAdiAdapter/OsiAdiAdapter/OsiAdiAdapter_PPort7"};
const ara::core::InstanceSpecifier LDSportSpecifier{"OsiAdiAdapter/OsiAdiAdapter/OsiAdiAdapter_PPort8"};
const ara::core::InstanceSpecifier RDSportSpecifier{"OsiAdiAdapter/OsiAdiAdapter/OsiAdiAdapter_PPort9"};

OsiAdiAdapter::OsiAdiAdapter()
{
    m_logger_.LogInfo() << "Start OsiAdiAdapter";
    InitializeServiceInterfaces();
}

void OsiAdiAdapter::Act()
{
    ptrCameraDetectionsServiceImp_->SendEvent();
    ptrRoadObjectsServiceImp_->SendEvent();
    ptrCameraFeaturesServiceImp_->SendEvent();
    ptrPMOServiceImp_->SendEvent();
    ptrRadarDetectionsServiceImp_->SendEvent();
    ptrLidarDetectionsServiceImp_->SendEvent();
    ptrStaticObjectsServiceImp_->SendEvent();
    ptrUltrasonicDetectionsServiceImp_->SendEvent();
    ptrUltrasonicFeaturesServiceImp_->SendEvent();
}

bool OsiAdiAdapter::InitializeServiceInterfaces()
{
    ara::com::InstanceIdentifierContainer CDSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(CDSportSpecifier);
    ara::com::InstanceIdentifierContainer ROSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(ROSportSpecifier);
    ara::com::InstanceIdentifierContainer CFSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(CFSportSpecifier);
    ara::com::InstanceIdentifierContainer PMOSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(PMOSportSpecifier);
    ara::com::InstanceIdentifierContainer RDSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(RDSportSpecifier);
    ara::com::InstanceIdentifierContainer LDSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(LDSportSpecifier);
    ara::com::InstanceIdentifierContainer SOSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(SOSportSpecifier);
    ara::com::InstanceIdentifierContainer UDSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(UDSportSpecifier);
    ara::com::InstanceIdentifierContainer UFSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(UFSportSpecifier);

    if (CDSinstanceIDs.empty() || ROSinstanceIDs.empty() || CFSinstanceIDs.empty() || PMOSinstanceIDs.empty()
        || RDSinstanceIDs.empty() || LDSinstanceIDs.empty() || SOSinstanceIDs.empty() || UDSinstanceIDs.empty()
        || UFSinstanceIDs.empty()) {
        m_logger_.LogError() << "At least one InstanceIdentifiers not resolved from provided InstanceSpecifier -- "
                                "InitializeServiceInterfaces failed";
        return false;
    } else {
        // TODO: Remove this logging
        for (auto const& instanceId : CDSinstanceIDs) {
            m_logger_.LogInfo() << "CD Service Instance offered:" << instanceId.ToString();
        }
    }

    // Create services
    ptrCameraDetectionsServiceImp_ = std::make_unique<CameraDetectionsServiceImp>(
        CDSinstanceIDs[0]);  // Can use instanceSpec also, need to change definition for that
    ptrRoadObjectsServiceImp_ = std::make_unique<RoadObjectsServiceImp>(ROSinstanceIDs[0]);
    ptrCameraFeaturesServiceImp_ = std::make_unique<CameraFeaturesServiceImp>(CFSinstanceIDs[0]);
    ptrPMOServiceImp_ = std::make_unique<PMOServiceImp>(PMOSinstanceIDs[0]);
    ptrRadarDetectionsServiceImp_ = std::make_unique<RadarDetectionsServiceImp>(RDSinstanceIDs[0]);
    ptrLidarDetectionsServiceImp_ = std::make_unique<LidarDetectionsServiceImp>(LDSinstanceIDs[0]);
    ptrStaticObjectsServiceImp_ = std::make_unique<StaticObjectsServiceImp>(SOSinstanceIDs[0]);
    ptrUltrasonicDetectionsServiceImp_ = std::make_unique<UltrasonicDetectionsServiceImp>(UDSinstanceIDs[0]);
    ptrUltrasonicFeaturesServiceImp_ = std::make_unique<UltrasonicFeaturesServiceImp>(UFSinstanceIDs[0]);

    if (!ptrCameraDetectionsServiceImp_) {
        m_logger_.LogError() << "Failed to connect to the CameraDetectionsServiceInterface -- Init failed";
        return false;
    }
    if (!ptrRoadObjectsServiceImp_) {
        m_logger_.LogError() << "Failed to connect to the RoadObjectsServiceInterface -- Init failed";
        return false;
    }
    if (!ptrCameraFeaturesServiceImp_) {
        m_logger_.LogError() << "Failed to connect to the CameraFeaturesServiceInterface -- Init failed";
        return false;
    }
    if (!ptrPMOServiceImp_) {
        m_logger_.LogError() << "Failed to connect to the PotentiallyMovingObjectsService -- Init failed";
        return false;
    }
    if (!ptrRadarDetectionsServiceImp_) {
        m_logger_.LogError() << "Failed to connect to the RadarDetectionsService -- Init failed";
        return false;
    }
    if (!ptrLidarDetectionsServiceImp_) {
        m_logger_.LogError() << "Failed to connect to the LidarDetectionsService -- Init failed";
        return false;
    }
    if (!ptrStaticObjectsServiceImp_) {
        m_logger_.LogError() << "Failed to connect to the StaticObjectsService -- Init failed";
        return false;
    }
    if (!ptrUltrasonicDetectionsServiceImp_) {
        m_logger_.LogError() << "Failed to connect to the UltrasonicDetectionsService -- Init failed";
        return false;
    }
    if (!ptrUltrasonicFeaturesServiceImp_) {
        m_logger_.LogError() << "Failed to connect to the UltrasonicFeaturesService -- Init failed";
        return false;
    }

    m_logger_.LogInfo() << "OsiAdiAdapter::InitializeServiceInterfaces() -- start OfferService()";
    ptrCameraDetectionsServiceImp_->InitializeService();
    ptrRoadObjectsServiceImp_->InitializeService();
    ptrCameraFeaturesServiceImp_->InitializeService();
    ptrPMOServiceImp_->InitializeService();
    ptrRadarDetectionsServiceImp_->InitializeService();
    ptrLidarDetectionsServiceImp_->InitializeService();
    ptrStaticObjectsServiceImp_->InitializeService();
    ptrUltrasonicDetectionsServiceImp_->InitializeService();
    ptrUltrasonicFeaturesServiceImp_->InitializeService();

    return true;
}
