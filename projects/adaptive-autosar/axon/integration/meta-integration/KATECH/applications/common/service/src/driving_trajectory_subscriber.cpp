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

#include "driving_trajectory_subscriber.h"

#include <stdint.h>

#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>
#include <stdexcept>

// includes for used services
#include "katech/driving_trajectory_proxy.h"

#include "ara/com/e2exf/types.h"
#include "ara/com/e2e_helper.h"

#include "ara/core/instance_specifier.h"
#include "logger.h"
#include "etc/jsonParser.h"

using namespace std::chrono_literals;

namespace katech
{

DrivingTrajectory_Subscriber* DrivingTrajectory_Subscriber::mInstance = NULL;
DrivingTrajectory_Subscriber::DrivingTrajectory_Subscriber()
{
    m_proxy = nullptr;
    m_promise_waitEvent = nullptr;
    DEBUG("object address %p", static_cast<void*>(this));
}

void DrivingTrajectory_Subscriber::udpClientCallback(int port, std::shared_ptr<katech::etc::udpClient::udpBuffer> buffer)
{
    std::shared_ptr<driving_trajectory_Objects> temp;
    xDrivingTrajectory_UdpPacket* ptr_data;
    ptr_data = (xDrivingTrajectory_UdpPacket*)(&(*buffer)[0]);
    temp = std::make_shared<driving_trajectory_Objects>();

    temp->Drive_Mode = ptr_data->Drive_Mode;
    temp->Emergency_acceleration = ptr_data->Emergency_acceleration;
    temp->Target_speed = ptr_data->Target_speed;
    temp->Turn_Signal = ptr_data->Turn_Signal;
    temp->Position = ptr_data->Position;
    // packet.sizeof_trajectory = (data.TrajectoryPointVector.size() > 50)?50:data.TrajectoryPointVector.size();
    for(int i = 0; i < ptr_data->sizeof_trajectory; i++)
    {
    temp->TrajectoryPointVector.push_back(ptr_data->TrajectoryPointVector[i]);
    }

    // INFO("%d", temp->TrajectoryPointVector.size());
    mInstance->enQueue(temp);
}
void DrivingTrajectory_Subscriber::init(std::string instance)
{
    katech::Log::Info() << "enter DrivingTrajectory_Subscriber::init()";
    int json_read_result = katech::etc::JsonParser::getInstance()->loadJson("/etc/katech_global_config.json");

    if(json_read_result == 0) {
        katech::etc::JsonParser::getInstance()->getValue("communication.DrivingTrajectory.UDP", mEnableUDP);
        katech::etc::JsonParser::getInstance()->getValue("communication.DrivingTrajectory.Port", mUdp_port);
    }

    if(mEnableUDP) {
        mUdpClient = std::make_shared<katech::etc::udpClient>(udpClientCallback, mUdp_port);

    } else {
        ara::core::InstanceSpecifier portSpecifier = ara::core::InstanceSpecifier(instance.c_str());
        INFO("Port In Executable Ref: %s", (std::string(portSpecifier.ToString().data())).c_str());
        auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);

        if(instanceIDs.empty()) {
            ERROR("Unknow Reference %s", instance.c_str());
            throw std::runtime_error{"No InstanceIdentifiers resolved to provided InstanceSpecifier"};
        }

        INFO("Searching for Service Instance: %s", (std::string(instanceIDs[0].ToString().data())).c_str());
        Proxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<Proxy::HandleType> handles, ara::com::FindServiceHandle handler) {
            DrivingTrajectory_Subscriber::serviceAvailabilityCallback(std::move(handles), handler);
        },
        instanceIDs[0]);
        katech::Log::Info() << "exit DrivingTrajectory_Subscriber::init()";
    }

    mInstance = this;
}


void DrivingTrajectory_Subscriber::subscribe_drivingTrajectory()
{
    if(!m_proxy->drivingTrajectoryEvent.IsSubscribed()) {
        // m_proxy got initialized via callback.
        m_proxy->drivingTrajectoryEvent.SetReceiveHandler(
        [this]() {
            DrivingTrajectory_Subscriber::receivedCallback_driving_trajectory();
        });
        // subscribe to event
        m_proxy->drivingTrajectoryEvent.Subscribe(1);
        INFO("DrivingTrajectory_Subscriber::subscribe_drivingTrajectory() complete");
    }
}

void DrivingTrajectory_Subscriber::serviceAvailabilityCallback(
    ara::com::ServiceHandleContainer<Proxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    UNUSED(handler);

    for(auto it : handles) {
        INFO("Instance %s is available", (std::string(it.GetInstanceId().ToString().data())).c_str());
    }

    if(handles.size() > 0) {
        std::lock_guard<std::mutex> lock(m_proxy_mutex);

        if(nullptr == m_proxy) {
            m_proxy = std::make_shared<Proxy>(handles[0]);
            INFO("Created proxy to handle with instance: %s",
                 (std::string(m_proxy->GetHandle().GetInstanceId().ToString().data())).c_str());
            // Construct some handles (implementation-specific).
            auto first_handle = handles[0];
            auto aux_handle = first_handle;

            for(auto current_handle : handles) {
                // Call equality operator.
                INFO("Check handle::operator==: %d", static_cast<uint8_t>(aux_handle == current_handle));
                // Call copy assignment operator.
                aux_handle = current_handle;
                // Call less-than operator.
                INFO("Check handle::operator<: %d", static_cast<uint8_t>(aux_handle < first_handle));
            }

            INFO("subscribe drivingTrajectory service, fields");
            subscribe_drivingTrajectory();
        }
    }
}

void DrivingTrajectory_Subscriber::receivedCallback_driving_trajectory()
{
    // execute callback for every samples in the context of GetNewSamples
    std::shared_ptr<driving_trajectory_Objects> temp;
    auto callback = [&temp](auto sample) {
        VERBOSE("Callback: drivingTrajectoryEvent ");
        temp = std::make_shared<driving_trajectory_Objects>(*sample);
    };
    m_proxy->drivingTrajectoryEvent.GetNewSamples(callback);
    this->enQueue(temp);
}

int DrivingTrajectory_Subscriber::getQueueSize()
{
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
    int length;

    if(mIndex_head > mIndex_tail) {
        length = mIndex_head - mIndex_tail;

    } else {
        length = mIndex_tail - mIndex_head;
    }

    return length;
}

auto DrivingTrajectory_Subscriber::setTrigger()
{
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
    m_promise_waitEvent = std::make_shared<ara::core::Promise<bool>>();
    return m_promise_waitEvent->get_future();
}

bool DrivingTrajectory_Subscriber::waitEvent(int deadLine)
{
    if(isEventQueueEmpty()) {
        auto future_wait = setTrigger();
        future_wait.wait_for(std::chrono::milliseconds(deadLine));
    }

    return !isEventQueueEmpty();
}
void DrivingTrajectory_Subscriber::enQueue(std::shared_ptr<driving_trajectory_Objects> item)
{
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
    m_Queue_event[mIndex_head++] = item;
    mIndex_head %= QUEUE_LENGTH;

    if(mIndex_head == mIndex_tail) {
        WARNNING("Queue length is too high!! drop event!!!");
        mIndex_tail++;
        mIndex_tail %= QUEUE_LENGTH;
    }

    if(m_promise_waitEvent != nullptr) {
        m_promise_waitEvent->set_value(true);
        m_promise_waitEvent = nullptr;
    }
}

std::shared_ptr<driving_trajectory_Objects> DrivingTrajectory_Subscriber::deQueue()
{
    std::shared_ptr<driving_trajectory_Objects> temp;
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
    temp = m_Queue_event[mIndex_tail];
    mIndex_tail++;
    mIndex_tail %= QUEUE_LENGTH;
    return temp;
}

std::shared_ptr<driving_trajectory_Objects> DrivingTrajectory_Subscriber::peekQueue()
{
    std::shared_ptr<driving_trajectory_Objects> temp = NULL;
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);

    if(mIndex_head != mIndex_tail) {
        int index = mIndex_head - 1;

        if(index < 0) {
            index = QUEUE_LENGTH - 1;
        }

        temp = m_Queue_event[index];
    }

    return temp;
}

void DrivingTrajectory_Subscriber::flashQueue()
{
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);
    mIndex_head = 0;
    mIndex_tail = 0;
}

std::shared_ptr<driving_trajectory_Objects> DrivingTrajectory_Subscriber::getEvent()
{
    std::shared_ptr<driving_trajectory_Objects> result;
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);

    if(mIndex_head != mIndex_tail) {
        result = m_Queue_event[mIndex_tail];
        mIndex_tail++;
        mIndex_tail %= QUEUE_LENGTH;

    } else {
        result = nullptr;
    }

    return result;
}

std::shared_ptr<driving_trajectory_Objects> DrivingTrajectory_Subscriber::getEvent_Last()
{
    std::shared_ptr<driving_trajectory_Objects> temp = NULL;
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);

    if(mIndex_head != mIndex_tail) {
        int index = mIndex_head - 1;

        if(index < 0) {
            index = QUEUE_LENGTH - 1;
        }

        temp = m_Queue_event[index];
    }

    mIndex_head = 0;
    mIndex_tail = 0;
    return temp;
}

bool DrivingTrajectory_Subscriber::isEventQueueEmpty()
{
    std::lock_guard<std::mutex> guard(m_Mutex_eventQueue);

    if(mIndex_head != mIndex_tail) {
        return false;

    } else {
        return true;
    }
}

}  // namespace katech
