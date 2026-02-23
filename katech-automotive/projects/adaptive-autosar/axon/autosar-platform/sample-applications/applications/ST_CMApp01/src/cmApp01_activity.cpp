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

#include "cmApp01_activity.hpp"
//#include "cm_Service1_Imp.hpp"

#include <cstdlib>
#include <cstring>
#include <stdint.h>

using namespace ara::log;
// using apd::testsuite::cm::SpecificErrorsErrc;

cmApp01Activity::cmApp01Activity()
{
    this->cmService1Skeleton = new cmService1Imp(ara::com::InstanceIdentifier(VSOMEIPCONFIG::AP_SERVICE1_INSTANCE_ID));
}

cmApp01Activity::~cmApp01Activity()
{
    delete this->cmService1Skeleton;
}

void cmApp01Activity::offerCmService1()
{
    if (not isOfferCmService1) {
        this->isOfferCmService1 = true;
        this->cmService1Skeleton->OfferService();
        m_logger_ctx3.LogInfo() << "cmApp01Activity: Offer cmService 1";
    } else {
        m_logger_ctx3.LogInfo() << "cmApp01Activity:Logic Error: cmService1 is already offered";
    }
}

void cmApp01Activity::stopOfferCmService1()
{
    if (isOfferCmService1) {
        this->isOfferCmService1 = false;
        this->cmService1Skeleton->StopOfferService();
        m_logger_ctx3.LogInfo() << "cmApp01Activity: stop Offered cmService1";
    } else {
        m_logger_ctx3.LogInfo() << "cmApp01Activity:Logic Error: cmService1 is not currently offered";
    }
}

void cmApp01Activity::init()
{
    m_logger_ctx3.LogDebug() << "enter init()";
    offerCmService1();
    enableService1ProcessingRequests();
}

void cmApp01Activity::deinit()
{
    m_logger_ctx3.LogDebug() << "enter deinit()";
    stopOfferCmService1();
    disableService1ProcessingRequests();
}

void cmApp01Activity::enableService1ProcessingRequests()
{
    this->isService1ProcessingRequests = true;
    m_logger_ctx3.LogInfo() << "cmApp01Activity: enable cmService1 ProcessingRequests";
}

void cmApp01Activity::disableService1ProcessingRequests()
{
    this->isService1ProcessingRequests = false;
    m_logger_ctx3.LogInfo() << "cmApp01Activity: disable cmService1 ProcessingRequests";
}

void cmApp01Activity::act()
{
    if (isOfferCmService1 && isService1ProcessingRequests) {
        m_logger_ctx3.LogInfo() << "cmApp01Activity: act()";
        auto result = this->cmService1Skeleton->Event1.Send(1);
        if (result) {
            m_logger_ctx3.LogInfo() << "cmApp01Activity: sent Event data";
        } else {
            m_logger_ctx3.LogInfo() << "cmApp01Activity: sent Event failed";
        }
    }
}
