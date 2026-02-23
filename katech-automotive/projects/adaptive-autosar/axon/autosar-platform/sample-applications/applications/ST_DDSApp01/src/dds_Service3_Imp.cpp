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

#include "dds_Service3_Imp.hpp"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <thread>

using namespace ara::log;

using apd::testsuite::cmdds::skeleton::fields::FieldA;

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Service3 Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

void ddsService3Imp::FieldInitialization()
{
    field_a = 0;
    GetLogger().LogInfo() << "RegisterGetHandler";

    // Register Field Getters
    DDSService3Skeleton::FieldA.RegisterGetHandler(std::bind(&ddsService3Imp::GetFieldA, this));
    GetLogger().LogInfo() << "RegisterSetHandler";

    // Register Field Setters
    DDSService3Skeleton::FieldA.RegisterSetHandler(std::bind(&ddsService3Imp::SetFieldA, this, std::placeholders::_1));

    // Update FieldA
    DDSService3Skeleton::FieldA.Update(0);
    GetLogger().LogInfo() << "Exit field Initialization";
}

ara::core::Future<apd::testsuite::cmdds::skeleton::fields::FieldA::value_type> ddsService3Imp::GetFieldA()
{
    ara::core::Promise<apd::testsuite::cmdds::skeleton::fields::FieldA::value_type> promise;
    // Field value is always accessible as per current spec
    GetLogger().LogInfo() << FIELDS_HEADER << "Getting the field A value :" << field_a;
    promise.set_value(std::move(field_a));
    return promise.get_future();
}

ara::core::Future<std::uint32_t> ddsService3Imp::SetFieldA(std::uint32_t field)
{
    ara::core::Promise<uint32_t> promise;
    field_a = field;
    GetLogger().LogInfo() << FIELDS_HEADER << "Setting the field A value to :" << field_a;
    promise.set_value(std::move(field_a));
    return promise.get_future();
}

void ddsService3Imp::UpdateFieldA()
{

    if ((update_count % 2) == 0) {
        field_a = 1234;
    } else {
        field_a = 5678;
    }

    update_count++;

    auto update_result = DDSService3Skeleton::FieldA.Update(field_a);
    if (update_result) {
        GetLogger().LogInfo() << " Field A update" << field_a;
    } else {
        GetLogger().LogInfo() << " Field A update failed with error" << update_result.Error();
    }
}

void ddsService3Imp::offerDdsService3()
{
    if (not isOfferDdsService3) {
        this->isOfferDdsService3 = true;
        DDSService3Skeleton::OfferService();
        GetLogger().LogInfo() << "ddsApp01Activity : Offer DDSService 3";
    } else {
        GetLogger().LogInfo() << "ddsApp01Activity : Error ddsService3 is already offered";
    }
}

void ddsService3Imp::stopOfferDdsService3()
{
    if (isOfferDdsService3) {
        this->isOfferDdsService3 = false;
        DDSService3Skeleton::StopOfferService();
        GetLogger().LogInfo() << "ddsApp01Activity : Stop Offer DDSService 3";
    } else {
        GetLogger().LogInfo() << "ddsApp01Activity : Error ddsService3 is not  offered";
    }
}

void ddsService3Imp::ProcessRequests()
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
