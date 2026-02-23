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
// or by any means, without permission in writing to the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////
// Activity specific implementation, skeleton can be generated to the model
// Discovery of services and sending/receiving of data according
// to the communication API
///////////////////////////////////////////////////////////////////////

#include "local_map_subscriber.h"

#include <stdint.h>

#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>
#include <stdexcept>

// includes for used services
#include "katech/local_map_proxy.h"

#include "ara/com/e2exf/types.h"
#include "ara/com/e2e_helper.h"

#include "ara/core/instance_specifier.h"
#include "logger.h"

using namespace std::chrono_literals;

namespace katech
{

LocalMap_Subscriber::LocalMap_Subscriber()
{
    m_proxy = nullptr;
    m_promise_waitEvent = nullptr;
    DEBUG("object address %p", static_cast<void*>(this));
}

void LocalMap_Subscriber::init(std::string instance)
{
    katech::Log::Info() << "enter LocalMap_Subscriber::init()";
    ara::core::InstanceSpecifier portSpecifier = ara::core::InstanceSpecifier(instance.c_str());
    INFO("Port In Executable Ref: %s", (std::string(portSpecifier.ToString().data())).c_str());
    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);

    if (instanceIDs.empty()) {
        ERROR("Unknow Reference %s", instance.c_str());
        throw std::runtime_error{"No InstanceIdentifiers resolved to provided InstanceSpecifier"};
    }

    INFO("Searching for Service Instance: %s", (std::string(instanceIDs[0].ToString().data())).c_str());
    Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<Proxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            LocalMap_Subscriber::serviceAvailabilityCallback(std::move(handles), handler);
        },
        instanceIDs[0]);
    katech::Log::Info() << "exit LocalMap_Subscriber::init()";
}

std::shared_ptr<local_map_Objects> LocalMap_Subscriber::MapData(
    const std::uint64_t& grid_id, const std::uint64_t& cell_id, const std::uint64_t deadLine)
{
    auto mapData_future = m_proxy->GetMapData(grid_id, cell_id);
    mapData_future.wait_for(std::chrono::milliseconds(deadLine));
    auto result = mapData_future.GetResult();

    if (result.HasValue()) {

        auto mapdata_output = result.Value();
        katech::Log::Info() << "MapData method call successful";
        return std::make_shared<local_map_Objects>(mapdata_output.map_data);
    }else{
        katech::Log::Error() << "MapData method call fail...";
        return NULL;
    }
}

local_map_Objects LocalMap_Subscriber::getMapData()
{
    return mapData;
}

void LocalMap_Subscriber::subscribe_serviceFlag()
{
    if (!m_proxy->ServiceFlag.IsSubscribed()) {
        // m_proxy got initialized via callback.
        // Register event receive callback
        m_proxy->ServiceFlag.SetReceiveHandler([this]() { LocalMap_Subscriber::receivedCallback_serviceFlag(); });
        // subscribe to event
        m_proxy->ServiceFlag.Subscribe(1);
        INFO("LocalMap_Subscriber::subscribe_serviceFlag() done");
    }
}

void LocalMap_Subscriber::subscribe_localMap()
{
    if (!m_proxy->localMapEvent.IsSubscribed()) {
        // m_proxy got initialized via callback.
        m_proxy->localMapEvent.SetReceiveHandler([this]() { LocalMap_Subscriber::receivedCallback_localMap(); });
        // subscribe to event
        m_proxy->localMapEvent.Subscribe(1);
        INFO("LocalMap_Subscriber::subscribe_localMap() complete");
    }
}

void LocalMap_Subscriber::serviceAvailabilityCallback(ara::com::ServiceHandleContainer<Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    UNUSED(handler);

    for (auto it : handles) {
        INFO("Instance %s is available", (std::string(it.GetInstanceId().ToString().data())).c_str());
    }

    if (handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxy_mutex);

        if (nullptr == m_proxy) {
            m_proxy = std::make_shared<Proxy>(handles[0]);
            INFO("Created proxy to handle with instance: %s",
                (std::string(m_proxy->GetHandle().GetInstanceId().ToString().data())).c_str());
            // Construct some handles (implementation-specific).
            auto first_handle = handles[0];
            auto aux_handle = first_handle;

            for (auto current_handle : handles) {
                // Call equality operator.
                INFO("Check handle::operator==: %d", static_cast<uint8_t>(aux_handle == current_handle));
                // Call copy assignment operator.
                aux_handle = current_handle;
                // Call less-than operator.
                INFO("Check handle::operator<: %d", static_cast<uint8_t>(aux_handle < first_handle));
            }

            INFO("subscribe LocalMap service, fields");
            subscribe_localMap();
            subscribe_serviceFlag();
        }
    }
}

void LocalMap_Subscriber::receivedCallback_serviceFlag()
{
    m_proxy->ServiceFlag.GetNewSamples(
        [](auto sample) {
#if defined(R19_11_1)
            katech::Log::Verbose() << "Callback: ServiceFlag Field: "
                                 << static_cast<katech::proxy::local_map::fields::ServiceFlag::value_type>(*sample);
#else
            katech::Log::Verbose() << "Callback: ServiceFlag Field: "
                                 << static_cast<katech::proxy::fields::ServiceFlag::value_type>(*sample);
#endif
        },
        1);
}

void LocalMap_Subscriber::receivedCallback_localMap()
{
    // execute callback for every samples in the context of GetNewSamples
    std::shared_ptr<local_map_Objects> temp;
    auto callback = [&temp](auto sample) {
        VERBOSE("Callback: localMapEvent ");
        temp = std::make_shared<local_map_Objects>(*sample);
    };
    m_proxy->localMapEvent.GetNewSamples(callback);
    this->enQueue(temp);
}

int LocalMap_Subscriber::getQueueSize()
{
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
    return static_cast<int>(m_Queue_event.size());
}

auto LocalMap_Subscriber::setTrigger()
{
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
    m_promise_waitEvent = std::make_shared<ara::core::Promise<bool>>();
    return m_promise_waitEvent->get_future();
}

bool LocalMap_Subscriber::waitEvent(int deadLine)
{
    if (isEventQueueEmpty()) {
        auto future_wait = setTrigger();
        future_wait.wait_for(std::chrono::milliseconds(deadLine));
    }

    return !isEventQueueEmpty();
}

void LocalMap_Subscriber::enQueue(std::shared_ptr<local_map_Objects> item)
{
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
    m_Queue_event.push(item);

    if (static_cast<int>(m_Queue_event.size()) > 11) {
        WARNNING("Queue length is too high!!(%d)", static_cast<int>(m_Queue_event.size()));
    }

    if (m_promise_waitEvent != nullptr) {
        m_promise_waitEvent->set_value(true);
        m_promise_waitEvent = nullptr;
    }
}

std::shared_ptr<local_map_Objects> LocalMap_Subscriber::deQueue()
{
    std::shared_ptr<local_map_Objects> temp;
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
    temp = m_Queue_event.front();
    m_Queue_event.pop();
    return temp;
}

std::shared_ptr<local_map_Objects> LocalMap_Subscriber::getEvent()
{
    std::shared_ptr<local_map_Objects> result;

    if (isEventQueueEmpty()) {
        result = nullptr;

    } else {
        result = deQueue();
    }

    return result;
}

bool LocalMap_Subscriber::isEventQueueEmpty()
{
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);

    if (static_cast<int>(m_Queue_event.size()) != 0) {
        return false;

    } else {
        return true;
    }
}

String LocalMap_Subscriber::fieldGetter_ServiceFlag()
{
    auto getServiceFlagFuture = m_proxy->ServiceFlag.Get();
    auto rServiceFlag = getServiceFlagFuture.GetResult();

    if (rServiceFlag.HasValue()) {
        auto value = rServiceFlag.Value();
        VERBOSE("FIELDS: Current ServiceFlag is %s", value);
        return value;

    } else {
        // while field getters itself do not return errors, networking errors are still possible
        auto err = rServiceFlag.Error();
        ERROR("APPLICATION_ERROR Code: %d", err.Value());
        ERROR("APPLICATION_ERROR Message: %s", err.Message().data());
    }
}

#if defined(R19_11_1)
void LocalMap_Subscriber::fieldSetter(katech::proxy::local_map::fields::ServiceFlag::value_type& value)
#else
void LocalMap_Subscriber::fieldSetter(katech::proxy::fields::ServiceFlag::value_type& value)
#endif
{
    try {
        auto setResultFuture = m_proxy->ServiceFlag.Set(value);
        auto r = setResultFuture.GetResult();

        if (r.HasValue()) {
            auto valueWhichWasSet = r.Value();

            if ((valueWhichWasSet != value)) {
                VERBOSE("FIELDS[ServiceFlag]: Set was successful");

            } else {
                ERROR("FIELDS[ServiceFlag]: Set failed!!!");
            }

        } else {
            // while field setters itself do not return errors, networking errors are still possible
            auto err = r.Error();
            ERROR("APPLICATION_ERROR Code: %d", err.Value());
            ERROR("APPLICATION_ERROR Message: %s", err.Message().data());
        }

    } catch (const ara::com::Exception& e) {
        ERROR("Exeception : %s", e.what());
    }
}

}  // namespace katech
