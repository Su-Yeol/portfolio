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

#include "cmApp09_activity.hpp"

#include <cstdlib>
#include <cstring>
#include <stdint.h>

using namespace ara::log;
// using apd::testsuite::cm::SpecificErrorsErrc;

cmApp09Activity::cmApp09Activity()
{
    this->cmService5Skeleton = new cmService5Imp(ara::com::InstanceIdentifier(VSOMEIPCONFIG::AP_SERVICE5_INSTANCE_ID));
}

cmApp09Activity::~cmApp09Activity()
{
    delete this->cmService5Skeleton;
}

void cmApp09Activity::offerCmService5()
{
    if (not isOfferCmService5) {
        this->isOfferCmService5 = true;
        this->cmService5Skeleton->OfferService();
        m_logger_ctx3.LogInfo() << "cmApp09Activity: Offer cmService 1";
    } else {
        m_logger_ctx3.LogInfo() << "cmApp09Activity:Logic Error: cmService5 is already offered";
    }
}

void cmApp09Activity::stopOfferCmService5()
{
    if (isOfferCmService5) {
        this->isOfferCmService5 = false;
        this->cmService5Skeleton->StopOfferService();
        m_logger_ctx3.LogInfo() << "cmApp09Activity: stop Offered cmService5";
    } else {
        m_logger_ctx3.LogInfo() << "cmApp09Activity:Logic Error: cmService5 is not currently offered";
    }
}

void cmApp09Activity::init()
{
    m_logger_ctx3.LogDebug() << "enter init()";
    offerCmService5();
    enableService5ProcessingRequests();
}

void cmApp09Activity::deinit()
{
    m_logger_ctx3.LogDebug() << "enter deinit()";
    stopOfferCmService5();
    disableService5ProcessingRequests();
}

void cmApp09Activity::enableService5ProcessingRequests()
{
    this->isService5ProcessingRequests = true;
    m_logger_ctx3.LogInfo() << "cmApp09Activity: enable cmService5 ProcessingRequests";
}

void cmApp09Activity::disableService5ProcessingRequests()
{
    this->isService5ProcessingRequests = false;
    m_logger_ctx3.LogInfo() << "cmApp09Activity: disable cmService5 ProcessingRequests";
}

void cmApp09Activity::act()
{
    if (isOfferCmService5 && isService5ProcessingRequests) {
        if (send_count < 10) {
            m_logger_ctx3.LogInfo() << "cmApp09Activity: act()";
            auto result = this->cmService5Skeleton->Event5.Send(1);
            send_count++;
            if (result) {
                m_logger_ctx3.LogInfo() << "cmApp09Activity: sent Event data";
            } else {
                m_logger_ctx3.LogInfo() << "cmApp09Activity: sent Event failed";
            }
        }
    }
}
