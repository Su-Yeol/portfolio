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

#include "dds_Service5_Imp.hpp"
#include <cstdlib>
#include <cstring>
#include <stdint.h>

using apd::shared::Position;

using namespace ara::log;

inline LogStream& operator<<(LogStream& out, const Position& value)
{
    return (out << "x, y, z (" << value.x << "," << value.y << "," << value.z << ")");
}

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Service3 Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

auto ddsService5Imp::MethodC(const Position& target_position) -> decltype(DDSService5Skeleton::MethodC(target_position))
{
    GetLogger().LogInfo() << "METHODS: "
                          << "CarPosition MethodC call";
    ddsService5Imp::MethodCOutput output;

    output.effective_position = target_position;
    output.success = true;
    // Set the promise value.
    decltype(DDSService5Skeleton::MethodC(target_position))::PromiseType promise;
    promise.set_value(std::move(output));
    return promise.get_future();
}

void ddsService5Imp::MethodD(const String& text)
{
    GetLogger().LogInfo() << "METHODS: " << text;
}

void ddsService5Imp::offerDdsService5()
{
    if (not isOfferDdsService5) {
        this->isOfferDdsService5 = true;
        DDSService5Skeleton::OfferService();
        GetLogger().LogInfo() << "ddsApp05Activity : Offer DDSService 5";
    } else {
        GetLogger().LogInfo() << "ddsApp05Activity : Error ddsService1 is already offered";
    }
}

void ddsService5Imp::stopOfferDdsService5()
{
    if (isOfferDdsService5) {
        this->isOfferDdsService5 = false;
        GetLogger().LogInfo() << "ddsApp05Activity : stop Offer DDSService 5";
        DDSService5Skeleton::StopOfferService();
    } else {
        GetLogger().LogInfo() << "ddsApp05Activity : Error ddsService4 is not offered";
    }
}
