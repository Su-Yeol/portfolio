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

#include "SmartCamera.h"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/vector.h"

const ara::core::InstanceSpecifier ROSportSpecifier{"SmartCameraSensor/SmartCameraSensor/SmartCameraSensor_PPort"};
const ara::core::InstanceSpecifier SOSportSpecifier{"SmartCameraSensor/SmartCameraSensor/SmartCameraSensor_PPort2"};
const ara::core::InstanceSpecifier PMOSportSpecifier{"SmartCameraSensor/SmartCameraSensor/SmartCameraSensor_PPort3"};
const ara::core::InstanceSpecifier CFSportSpecifier{"SmartCameraSensor/SmartCameraSensor/SmartCameraSensor_PPort4"};
const ara::core::InstanceSpecifier CDSportSpecifier{"SmartCameraSensor/SmartCameraSensor/SmartCameraSensor_PPort5"};
const ara::core::InstanceSpecifier UDSportSpecifier{"SmartCameraSensor/SmartCameraSensor/SmartCameraSensor_PPort6"};
const ara::core::InstanceSpecifier UFSportSpecifier{"SmartCameraSensor/SmartCameraSensor/SmartCameraSensor_PPort7"};
const ara::core::InstanceSpecifier LDSportSpecifier{"SmartCameraSensor/SmartCameraSensor/SmartCameraSensor_PPort8"};
const ara::core::InstanceSpecifier RDSportSpecifier{"SmartCameraSensor/SmartCameraSensor/SmartCameraSensor_PPort9"};

SmartCamera::SmartCamera()
{
    m_logger_.LogInfo() << "Start SmartCamera";
    InitializeServiceInterfaces();
}

void SmartCamera::Act()
{
    ptrRoadObjectsService_->SendEvent();
    ptrStaticObjectsService_->SendEvent();
    ptrPMObjectsService_->SendEvent();
    ptrCameraDetectionsService_->SendEvent();
    ptrUltrasonicDetectionsService_->SendEvent();
    ptrCameraFeaturesService_->SendEvent();
    ptrUltrasonicFeaturesService_->SendEvent();
    ptrLidarDetectionsService_->SendEvent();
    ptrRadarDetectionsService_->SendEvent();
}

bool SmartCamera::InitializeServiceInterfaces()
{
    ara::com::InstanceIdentifierContainer ROSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(ROSportSpecifier);
    ara::com::InstanceIdentifierContainer SOSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(SOSportSpecifier);
    ara::com::InstanceIdentifierContainer PMOSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(PMOSportSpecifier);
    ara::com::InstanceIdentifierContainer CDSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(CDSportSpecifier);
    ara::com::InstanceIdentifierContainer UDSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(UDSportSpecifier);
    ara::com::InstanceIdentifierContainer CFSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(CFSportSpecifier);
    ara::com::InstanceIdentifierContainer UFSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(UFSportSpecifier);
    ara::com::InstanceIdentifierContainer LDSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(LDSportSpecifier);
    ara::com::InstanceIdentifierContainer RDSinstanceIDs = ara::com::runtime::ResolveInstanceIDs(RDSportSpecifier);

    if (ROSinstanceIDs.empty() || SOSinstanceIDs.empty() || PMOSinstanceIDs.empty() || CDSinstanceIDs.empty()
        || CFSinstanceIDs.empty() || UFSinstanceIDs.empty() || LDSinstanceIDs.empty() || RDSinstanceIDs.empty()) {
        m_logger_.LogError() << "At least one InstanceIdentifiers not resolved from provided InstanceSpecifier -- "
                                "InitializeServiceInterfaces failed";
        return false;
    } else {
        // TODO: Remove this logging
        for (auto const& instanceId : ROSinstanceIDs) {
            m_logger_.LogInfo() << "RO Service Instance offered:" << instanceId.ToString();
        }
        for (auto const& instanceIdSO : SOSinstanceIDs) {
            m_logger_.LogInfo() << "SO Service Instance offered:" << instanceIdSO.ToString();
        }
    }

    // Create services
    ptrRoadObjectsService_ = std::make_unique<RoadObjectsService>(
        ROSinstanceIDs[0]);  // Can use instanceSpec also, need to change definition for that
    ptrStaticObjectsService_ = std::make_unique<StaticObjectsService>(SOSinstanceIDs[0]);
    ptrPMObjectsService_ = std::make_unique<PotentiallyMovingObjectsService>(PMOSinstanceIDs[0]);
    ptrCameraDetectionsService_ = std::make_unique<CameraDetectionsService>(CDSinstanceIDs[0]);
    ptrUltrasonicDetectionsService_ = std::make_unique<UltrasonicDetectionsService>(UDSinstanceIDs[0]);
    ptrCameraFeaturesService_ = std::make_unique<CameraFeaturesService>(CFSinstanceIDs[0]);
    ptrUltrasonicFeaturesService_ = std::make_unique<UltrasonicFeaturesService>(UFSinstanceIDs[0]);
    ptrLidarDetectionsService_ = std::make_unique<LidarDetectionsService>(LDSinstanceIDs[0]);
    ptrRadarDetectionsService_ = std::make_unique<RadarDetectionsService>(RDSinstanceIDs[0]);

    if (!ptrRoadObjectsService_) {
        m_logger_.LogError() << "Failed to connect to the RoadObjectsServiceInterface -- Init failed";
        return false;
    }
    if (!ptrStaticObjectsService_) {
        m_logger_.LogError() << "Failed to connect to the StaticObjectsServiceInterface -- Init failed";
        return false;
    }
    if (!ptrPMObjectsService_) {
        m_logger_.LogError() << "Failed to connect to the PMObjectsServiceInterface -- Init failed";
        return false;
    }
    if (!ptrCameraDetectionsService_) {
        m_logger_.LogError() << "Failed to connect to the CameraDetectionsServiceInterface -- Init failed";
        return false;
    }
    if (!ptrUltrasonicDetectionsService_) {
        m_logger_.LogError() << "Failed to connect to the UltrasonicDetectionsServiceInterface -- Init failed";
        return false;
    }
    if (!ptrCameraFeaturesService_) {
        m_logger_.LogError() << "Failed to connect to the CameraFeaturesServiceInterface -- Init failed";
        return false;
    }
    if (!ptrUltrasonicFeaturesService_) {
        m_logger_.LogError() << "Failed to connect to the UltrasonicFeaturesServiceInterface -- Init failed";
        return false;
    }

    if (!ptrLidarDetectionsService_) {
        m_logger_.LogError() << "Failed to connect to the LidarDetectionServiceInterface -- Init failed";
        return false;
    }

    if (!ptrRadarDetectionsService_) {
        m_logger_.LogError() << "Failed to connect to the RadarDetectionServiceInterface -- Init failed";
        return false;
    }

    m_logger_.LogInfo() << "SmartCameraApp::InitializeServiceInterfaces() -- start OfferService()";
    ptrRoadObjectsService_->InitializeService();
    ptrStaticObjectsService_->InitializeService();
    ptrPMObjectsService_->InitializeService();
    ptrCameraDetectionsService_->InitializeService();
    ptrUltrasonicDetectionsService_->InitializeService();
    ptrCameraFeaturesService_->InitializeService();
    ptrUltrasonicFeaturesService_->InitializeService();
    ptrLidarDetectionsService_->InitializeService();
    ptrRadarDetectionsService_->InitializeService();

    return true;
}
