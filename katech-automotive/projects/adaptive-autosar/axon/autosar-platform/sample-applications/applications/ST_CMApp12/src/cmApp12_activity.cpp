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

#include "cmApp12_activity.hpp"

#include <cstdlib>
#include <cstring>
#include <stdint.h>

using namespace ara::log;
// using apd::testsuite::cm::SpecificErrorsErrc;

cmApp12Activity::cmApp12Activity()
{
    this->cmService6Skeleton = new cmService6Imp(ara::com::InstanceIdentifier(VSOMEIPCONFIG::AP_SERVICE6_INSTANCE_ID));
}

cmApp12Activity::~cmApp12Activity()
{
    delete this->cmService6Skeleton;
}

void cmApp12Activity::offerCmService6()
{
    if (not isOfferCmService6) {
        this->isOfferCmService6 = true;
        this->cmService6Skeleton->OfferService();
        m_logger_ctx3.LogInfo() << "cmApp12Activity: Offer cmService 6";
    } else {
        m_logger_ctx3.LogInfo() << "cmApp12Activity:Logic Error: cmService6 is already offered";
    }
}

void cmApp12Activity::stopOfferCmService6()
{
    if (isOfferCmService6) {
        this->isOfferCmService6 = false;
        this->cmService6Skeleton->StopOfferService();
        m_logger_ctx3.LogInfo() << "cmApp12Activity: stop Offered cmService6";
    } else {
        m_logger_ctx3.LogInfo() << "cmApp12Activity:Logic Error: cmService6 is not currently offered";
    }
}

void cmApp12Activity::init()
{
    m_logger_ctx3.LogDebug() << "enter init()";
    offerCmService6();
    enableService6ProcessingRequests();
}

void cmApp12Activity::deinit()
{
    m_logger_ctx3.LogDebug() << "enter deinit()";
    stopOfferCmService6();
    disableService6ProcessingRequests();
}

void cmApp12Activity::enableService6ProcessingRequests()
{
    this->isService6ProcessingRequests = true;
    m_logger_ctx3.LogInfo() << "cmApp12Activity: enable cmService6 ProcessingRequests";
}

void cmApp12Activity::disableService6ProcessingRequests()
{
    this->isService6ProcessingRequests = false;
    m_logger_ctx3.LogInfo() << "cmApp12Activity: disable cmService6 ProcessingRequests";
}

void cmApp12Activity::act()
{
    if (isOfferCmService6 && isService6ProcessingRequests) {
        m_logger_ctx3.LogInfo() << "cmApp12Activity: act()";
        auto result = this->cmService6Skeleton->Event6.Send(1);
        send_count++;
        if (result) {
            m_logger_ctx3.LogInfo() << "cmApp12Activity: sent Event data";
        } else {
            m_logger_ctx3.LogInfo() << "cmApp12Activity: sent Event failed";
        }
    }
}
