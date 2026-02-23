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

#include "diagApp01_diag_service.hpp"
#include "diagApp01_utils.hpp"

DIAGApp01DiagService::DIAGApp01DiagService()
{ }

DIAGApp01DiagService::~DIAGApp01DiagService()
{
    if (skeletonDIAGApp01DataIdentifier) {
        delete this->skeletonDIAGApp01DataIdentifier;
        this->skeletonDIAGApp01DataIdentifier = nullptr;
    }
}

bool DIAGApp01DiagService::Init()
{
    bool rv = false;
    m_logger_dst2.LogDebug() << "DIAGApp01: Init()";

    try {
        // Make DataIdentifier service available to diagnostic manager.
        skeletonDIAGApp01DataIdentifier
            = new DIAGApp01DataIdentifier(ara::com::InstanceIdentifier(DIAGCONFIG::DIAGAPP01_DATASERVICE_INSTANCE_ID));
        skeletonDIAGApp01DataIdentifier->OfferService();
        m_logger_dst2.LogInfo() << "DIAGApp01: Offer DIAGService";
        rv = true;
    } catch (std::exception& e) {
        m_logger_dst2.LogInfo() << "DIAGApp01:Error Offer DIAGService";
        rv = false;
    }

    return rv;
}

bool DIAGApp01DiagService::DeInit()
{
    skeletonDIAGApp01DataIdentifier->StopOfferService();
    m_logger_dst2.LogInfo() << "DIAGApp01: Stop DIAGService";

    return true;
}
