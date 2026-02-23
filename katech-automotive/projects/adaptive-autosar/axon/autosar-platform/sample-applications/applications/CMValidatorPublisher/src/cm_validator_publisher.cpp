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

#include "ara/com/sample/cmvalidatorpublisher_skeleton.h"
#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"

#include "cm_validator_publisher_impl.h"
#include "cm_validator_publisher_act.h"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <thread>

using namespace ara::log;

using ara::com::sample::SpecificErrorsErrc;
using ara::com::ComErrorDomainErrc;
using ara::com::sample::skeleton::fields::Field1;

namespace cmvalidatorpublisher
{
auto CMValidatorPublisherImp::MethodsCapability() -> decltype(Skeleton::MethodsCapability())
{
    CMValidatorPublisherImp::MethodsCapabilityOutput output;

    methodsCapabilityCount_++;

    loggerCtxMc_.LogInfo() << kMethodsHeader << "MethodsCapability called " << methodsCapabilityCount_;

    if ((methodsCapabilityCount_ % 10) == 0) {
        // Emulate error condition -- MethodsCapability took too long
        std::this_thread::sleep_for(std::chrono::seconds(2));
        output.result = false;
        loggerCtxMc_.LogWarn() << kMethodsHeader << "MethodsCapability took very long time, there is something wrong";
    } else {
        // Calibration was successful, set result to true.
        output.result = true;
        loggerCtxMc_.LogInfo() << kMethodsHeader << "MethodsCapability was successful";
    }

    // Set the promise value (or error in case calibration went wrong).
    decltype(Skeleton::MethodsCapability())::PromiseType promise;
    if (!output.result) {
        loggerCtxMc_.LogWarn() << kErrorsHeader << "Configuration string was invalid.";
        promise.SetError(ara::core::ErrorCode(SpecificErrorsErrc::kInvalidConfigString));
    } else {
        promise.set_value(std::move(output));
    }

    return promise.get_future();
}

void CMValidatorPublisherImp::Echo(const String& text)
{
    loggerCtxFfmc_.LogInfo() << kMethodsHeader << text;
}

void CMValidatorPublisherImp::ProcessRequests()
{
    while (!finished_) {
        std::chrono::time_point<std::chrono::system_clock> deadline
            = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
        auto request_finished = ProcessNextMethodCall();
        if (request_finished.wait_until(deadline) != ara::core::future_status::ready) {
            loggerCtxImp_.LogFatal() << "Request timeout";
        } else {
            if (!finished_) {
                std::this_thread::sleep_until(deadline);
            }
        }
    }
}

CMValidatorPublisherActivity::~CMValidatorPublisherActivity()
{
    delete skeleton_;
}

CMValidatorPublisherActivity::CMValidatorPublisherActivity()
    : skeleton_(nullptr)
    , randEng_(rd_())
    , ud_0_100_(0, 100)
    , ud_0_50_(0, 50)
{
    logger_.LogInfo() << "Start CMValidatorPublisherActivity";

    ara::core::InstanceSpecifier instanceSpec{"CMValidatorPublisher/CMValidatorPublisher/CMValidatorPublisher_PPort"};
    logger_.LogInfo() << "Port In Executable Ref:" << instanceSpec.ToString();

    skeleton_ = new CMValidatorPublisherImp(instanceSpec, ara::com::MethodCallProcessingMode::kPoll);

    // The instance id resolution is not mandatory for service creation (but could be an option)
    // here it's intended to list ids for the offered service instances
    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(instanceSpec);
    for (auto const& instanceId : instanceIDs) {
        logger_.LogInfo() << "Service Instance offered:" << instanceId.ToString();
    }

    logger_.LogDebug() << "object address" << this;
    logger_.LogInfo() << "Finish CMValidatorPublisherActivity";
}

ara::core::Future<ara::com::sample::skeleton::fields::Field1::value_type> CMValidatorPublisherActivity::GetField1()
{
    ara::core::Promise<ara::com::sample::skeleton::fields::Field1::value_type> promise;
    // Field value is always accessible as per current spec
    field_1_ = ud_0_100_(randEng_);
    loggerCtxField_.LogInfo() << kFieldsHeader << "Getting the field1 value :" << field_1_;
    promise.set_value(std::move(field_1_));
    return promise.get_future();
}

ara::core::Future<uint32_t> CMValidatorPublisherActivity::SetField1(uint32_t field)
{
    ara::core::Promise<uint32_t> promise;
    switch (internal_state_for_update_rate_set_handler_) {
    // field setters indicates all errors by keeping previous value of field as per current spec
    case internalStates::kReady: {
        field_1_ = field;
        loggerCtxField_.LogInfo() << kFieldsHeader << "Setting the field1 value to " << field_1_;
        promise.set_value(std::move(field_1_));
        internal_state_for_update_rate_set_handler_ = internalStates::kNotReady;
        break;
    }
    case internalStates::kNotReady: {
        loggerCtxField_.LogInfo() << kFieldsHeader << "Simulating unreadiness, keeping previous value " << field_1_;
        promise.set_value(std::move(field_1_));
        internal_state_for_update_rate_set_handler_ = internalStates::kReady;
        break;
    }
    }
    return promise.get_future();
}

void CMValidatorPublisherActivity::Init()
{
    logger_.LogDebug() << "Init() enter";

    // Init cached version of the field1.
    this->field_1_ = 0;

    // Register Field Getters.
    auto register_get_result
        = skeleton_->Field1.RegisterGetHandler(std::bind(&CMValidatorPublisherActivity::GetField1, this));
    if (register_get_result) {
        loggerCtxField_.LogInfo() << kFieldsHeader << "Field1 get handler successful";
    } else {
        loggerCtxField_.LogError() << kFieldsHeader << "Field1 get handler with error: " << register_get_result.Error();
    }

    // Register Field Setters.
    auto register_set_result = skeleton_->Field1.RegisterSetHandler(
        std::bind(&CMValidatorPublisherActivity::SetField1, this, std::placeholders::_1));
    if (register_set_result) {
        loggerCtxField_.LogInfo() << kFieldsHeader << "Field1 set handler successful";
    } else {
        loggerCtxField_.LogError() << kFieldsHeader << "Field1 set handler with error: " << register_set_result.Error();
    }

    // // Initialize Fields Values before Offering Service.
    skeleton_->Field1.Update(0);
    skeleton_->Field2.Update(0);
    skeleton_->OfferService();

    logger_.LogDebug() << "Init() exit";
}

void CMValidatorPublisherActivity::Act()
{
    // Allocate and send events
    SendEvents();

    // Update Field values and send notifications
    UpdateFields();
}

void CMValidatorPublisherActivity::UpdateFields()
{
    logger_.LogInfo() << kFieldsHeader << "CMValidatorPublisher updateFields started";

    static uint8_t i = 0;

    if (i % 5 == 0) {
        field_1_ = ud_0_100_(randEng_);
        auto update_result = skeleton_->Field1.Update(field_1_);
        if (update_result) {
            loggerCtxField_.LogInfo() << kFieldsHeader << "Field1 updated " << field_1_;
        } else {
            loggerCtxField_.LogError() << kFieldsHeader << "Field1.Update failed with error: " << update_result.Error();
        }
    }

    std::uint16_t distance = ud_0_50_(randEng_);
    auto update_result = skeleton_->Field2.Update(distance);
    if (update_result) {
        loggerCtxField_.LogInfo() << kFieldsHeader << "Field2 updated " << distance;
    } else {
        loggerCtxField_.LogError() << kFieldsHeader << "Field2.Update failed with error: " << update_result.Error();
    }
    i++;
}

void CMValidatorPublisherActivity::SendEvents()
{
    loggerCtxEvent_.LogInfo() << kEventsHeader << "CMValidatorPublisher sendEvents started";

    static uint8_t i = 0;

    // allocate sample
    auto allocation_result = skeleton_->Event1.Allocate();
    if (!allocation_result) {
        loggerCtxEvent_.LogError() << kEventsHeader
                                   << "Event1 allocation failed with error: " << allocation_result.Error();
    } else {
        auto l_sampleEvent1 = std::move(allocation_result).Value();

        // write data
        if (i % 5 == 0) {
            l_sampleEvent1->active = true;
        }
        l_sampleEvent1->objectVector.push_back(0x10);
        l_sampleEvent1->objectVector.push_back(0x20);
        l_sampleEvent1->objectVector.push_back(0x30);
        l_sampleEvent1->objectVector.push_back(0x40);
        l_sampleEvent1->objectVector.push_back(i);

        // send sample
        auto send_result = skeleton_->Event1.Send(std::move(l_sampleEvent1));
        if (send_result) {
            loggerCtxEvent_.LogInfo() << kEventsHeader << "Event1 sent";
        } else {
            loggerCtxEvent_.LogError() << kEventsHeader << "Event1.Send failed with error: " << send_result.Error();
        }
    }

    auto pb_allocation_result = skeleton_->Event2.Allocate();
    if (!pb_allocation_result) {
        loggerCtxEvent_.LogError() << kEventsHeader
                                   << "Event2 allocation failed with error: " << pb_allocation_result.Error();
    } else {
        auto l_sampleEvent2 = std::move(pb_allocation_result).Value();

        if (i % 5 != 0) {
            l_sampleEvent2->active = true;
        }
        l_sampleEvent2->objectVector.push_back(255 - i);

        // FIX for possible threading problem in vSomeIP which led to SEGFAULT
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // send sample
        auto send_result = skeleton_->Event2.Send(std::move(l_sampleEvent2));
        if (send_result) {
            loggerCtxEvent_.LogInfo() << kEventsHeader << "Event2 sent";
        } else {
            loggerCtxEvent_.LogError() << kEventsHeader << "Event2.Send failed with error: " << send_result.Error();
        }
    }
    i++;
}
}  // namespace cmvalidatorpublisher
