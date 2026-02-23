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

#include "dds_Service1_Imp.hpp"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <thread>

using namespace ara::log;

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Service1 Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

void ddsService1Imp::SendEventA(uint8_t i)
{
    i = 10;
    // allocate
    auto allocation_result = DDSService1Skeleton::EventA.Allocate();
    if (!allocation_result) {
        GetLogger().LogError() << "EventA allocation failed with error: " << allocation_result.Error();
    } else {
        auto l_eventA = std::move(allocation_result).Value();
        // write data
        if (i % 5 == 0) {
            l_eventA->active = true;
        }
        l_eventA->objectVector.push_back(i);

        // send
        auto send_result = DDSService1Skeleton::EventA.Send(std::move(l_eventA));
        if (send_result) {
            GetLogger().LogInfo() << "EventA sent";
        } else {
            GetLogger().LogError() << "EventA.Send failed with error: " << send_result.Error();
        }
    }
}

void ddsService1Imp::SendEventB(uint8_t i)
{
    i = 5;
    // allocate
    auto allocation_result = DDSService1Skeleton::EventB.Allocate();
    if (!allocation_result) {
        GetLogger().LogError() << "EventB allocation failed with error: " << allocation_result.Error();
    } else {
        auto l_eventB = std::move(allocation_result).Value();

        if (i % 5 != 0) {
            l_eventB->active = true;
        }
        l_eventB->objectVector.push_back(255 - i);

        // FIX for possible threading problem in vSomeIP which led to SEGFAULT
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // send sample
        auto send_result = DDSService1Skeleton::EventB.Send(std::move(l_eventB));
        if (send_result) {
            GetLogger().LogInfo() << "EventB sent";
        } else {
            GetLogger().LogError() << "EventB.Send failed with error: " << send_result.Error();
        }
    }
}

void ddsService1Imp::offerDdsService1()
{
    if (not isOfferDdsService1) {
        this->isOfferDdsService1 = true;
        DDSService1Skeleton::OfferService();
        GetLogger().LogInfo() << "ddsApp01Activity : Offer DDSService 1";
    } else {
        GetLogger().LogInfo() << "ddsApp01Activity : Error ddsService1 is already offered";
    }
}

void ddsService1Imp::stopOfferDdsService1()
{
    if (isOfferDdsService1) {
        this->isOfferDdsService1 = false;
        GetLogger().LogInfo() << "ddsApp01Activity : stop Offer DDSService 1";
        DDSService1Skeleton::StopOfferService();
    } else {
        GetLogger().LogInfo() << "ddsApp01Activity : Error ddsService1 is not offered";
    }
}
