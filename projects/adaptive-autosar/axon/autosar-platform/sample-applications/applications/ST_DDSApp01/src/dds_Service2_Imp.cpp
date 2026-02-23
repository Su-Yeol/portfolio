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

#include "dds_Service2_Imp.hpp"

#include <stdint.h>
#include <cstdlib>
#include <cstring>

using namespace ara::log;

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Service2 log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

void ddsService2Imp::SendEventC(uint8_t i)
{
    // allocate sample
    auto allocation_result = DDSService2Skeleton::EventC.Allocate();
    if (!allocation_result) {
        GetLogger().LogError() << "EventC allocation failed with error: " << allocation_result.Error();
    } else {
        auto l_eventC = std::move(allocation_result).Value();
        i = 5;
        l_eventC->active = true;
        l_eventC->objectVector.push_back(i);

        // send sample
        auto send_result = DDSService2Skeleton::EventC.Send(std::move(l_eventC));
        if (send_result) {
            GetLogger().LogInfo() << "EventC sent";
        } else {
            GetLogger().LogError() << "EventC.Send failed with error: " << send_result.Error();
        }
    }
}

void ddsService2Imp::offerDdsService2()
{
    if (not isOfferDdsService2) {
        this->isOfferDdsService2 = true;
        DDSService2Skeleton::OfferService();
        GetLogger().LogInfo() << "ddsApp01Activity : Offer DDSService 2";
    } else {
        GetLogger().LogInfo() << "ddsApp01Activity : Error ddsService2 is already offered";
    }
}

void ddsService2Imp::stopOfferDdsService2()
{
    if (isOfferDdsService2) {
        this->isOfferDdsService2 = false;
        DDSService2Skeleton::StopOfferService();
        GetLogger().LogInfo() << "ddsApp01Activity : Stop Offer DDSService 2";
    } else {
        GetLogger().LogInfo() << "ddsApp01Activity : Error ddsService2 is not  offered";
    }
}
