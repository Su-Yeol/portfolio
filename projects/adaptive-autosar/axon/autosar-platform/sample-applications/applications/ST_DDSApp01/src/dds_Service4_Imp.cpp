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

#include "dds_Service4_Imp.hpp"

#include <cstdlib>
#include <cstring>
#include <thread>
#include <stdint.h>
#include <chrono>
#include <sys/time.h>

using apd::testsuite::cmdds::DDSVehicleState;
using apd::testsuite::cmdds::DDSFusionVariant;

using namespace ara::log;

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Service4 Log", ara::log::LogLevel::kInfo);
    return logger;
}

}  // namespace

auto ddsService4Imp::MethodA(const String& configuration, const DDSVehicleState& vehiclestate)
    -> decltype(DDSService4Skeleton::MethodA(configuration, vehiclestate))
{
    ddsService4Imp::MethodAOutput output;

    GetLogger().LogInfo() << "METHODS: "
                          << "CarState methodA call, vehiclestate is" << (std::uint16_t)vehiclestate;

    decltype(DDSService4Skeleton::MethodA(configuration, vehiclestate))::PromiseType promise;
    output.result = true;
    promise.set_value(std::move(output));
    return promise.get_future();
}

auto ddsService4Imp::MethodB(const String& configuration, const DDSFusionVariant& variant)
    -> decltype(DDSService4Skeleton::MethodB(configuration, variant))
{
    ddsService4Imp::MethodBOutput output;

    GetLogger().LogInfo() << "METHODS: "
                          << "variant methodB call,variant is" << (std::uint16_t)variant;

    decltype(DDSService4Skeleton::MethodB(configuration, variant))::PromiseType promise;
    output.result = true;
    promise.set_value(std::move(output));
    return promise.get_future();
}

void ddsService4Imp::offerDdsService4()
{
    if (not isOfferDdsService4) {
        this->isOfferDdsService4 = true;
        DDSService4Skeleton::OfferService();
        GetLogger().LogInfo() << "ddsApp04Activity : Offer DDSService 4";
    } else {
        GetLogger().LogInfo() << "ddsApp04Activity : Error ddsService1 is already offered";
    }
}

void ddsService4Imp::stopOfferDdsService4()
{
    if (isOfferDdsService4) {
        this->isOfferDdsService4 = false;
        GetLogger().LogInfo() << "ddsApp04Activity : stop Offer DDSService 4";
        DDSService4Skeleton::StopOfferService();
    } else {
        GetLogger().LogInfo() << "ddsApp04Activity : Error ddsService4 is not offered";
    }
}

void ddsService4Imp::ProcessRequests()
{
    while (!m_finished) {
        std::chrono::time_point<std::chrono::system_clock> deadline
            = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
        auto request_finished = ProcessNextMethodCall();
        if (request_finished.wait_until(deadline) != ara::core::future_status::ready) {
            GetLogger().LogFatal() << "Request took too long :Service1";
        } else {
            if (!m_finished) {
                std::this_thread::sleep_until(deadline);
            }
        }
    }
}
