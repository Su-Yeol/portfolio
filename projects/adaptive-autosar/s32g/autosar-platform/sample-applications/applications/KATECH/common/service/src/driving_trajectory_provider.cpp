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

#include "driving_trajectory_provider.h"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <chrono>

#include "adcm/driving_trajectory_skeleton.h"
#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"
#include "logger.h"

#include "etc/jsonParser.h"

using namespace ara::log;
using ara::com::ComErrorDomainErrc;

// <<operator implementation for logging custom types
// similar handler is also available in fusion sources, so for real projects it make sense to
// have some common place where custom type log-handlers are provided.

namespace adcm
{

void DrivingTrajectoryImp::ProcessRequests()
{
    while(!m_finished) {
        std::chrono::time_point<std::chrono::system_clock> deadline
            = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
        auto request_finished = ProcessNextMethodCall();
#if defined(R19_11_1)

        if(request_finished.wait_until(deadline) != ara::core::future_status::kReady) {
#else

        if(request_finished.wait_until(deadline) != ara::core::future_status::ready) {
#endif
            FATAL("Request took too long :S");

        } else {
            if(!m_finished) {
                std::this_thread::sleep_until(deadline);
            }
        }
    }
}

DrivingTrajectory_Provider::DrivingTrajectory_Provider()
{
    DEBUG("object address : %p", static_cast<void*>(this));
}

DrivingTrajectory_Provider::~DrivingTrajectory_Provider()
{
    delete m_skeleton;
}


void DrivingTrajectory_Provider::init(std::string instance)
{
    adcm::Log::Info() << "enter DrivingTrajectory_Provider::init()";
    int json_read_result = adcm::etc::JsonParser::getInstance()->loadJson("/etc/adcm_global_config.json");

    if(json_read_result == 0) {
        adcm::etc::JsonParser::getInstance()->getValue("communication.DrivingTrajectory.UDP", mEnableUDP);
        adcm::etc::JsonParser::getInstance()->getValue("communication.DrivingTrajectory.Port", mUdp_Location_port);
        // adcm::etc::JsonParser::getInstance()->getValue("CarInfo.CPU_B_IP", mIP_CPU_B);
        adcm::etc::JsonParser::getInstance()->getValue("CarInfo.S32G_IP", mIP_S32G);
    }

    if(mEnableUDP) {
        // mList_udpSender.push_back(std::make_shared<adcm::etc::udpSender>(mIP_CPU_B, mUdp_Location_port));
        mList_udpSender.push_back(std::make_shared<adcm::etc::udpSender>(mIP_S32G, mUdp_Location_port));

    } else {
        ara::core::InstanceSpecifier instanceSpec = ara::core::InstanceSpecifier(instance.c_str());
        adcm::Log::Info() << "Port In Executable Ref:" << instanceSpec.ToString();
        m_skeleton = new DrivingTrajectoryImp(instanceSpec, ara::com::MethodCallProcessingMode::kPoll);
        // The instance id resolution is not mandatory for service creation (but could be an option)
        // here it's intended to list ids for the offered service instances
        auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(instanceSpec);

        for(auto const& instanceId : instanceIDs) {
            adcm::Log::Info() << "Service Instance offered:" << instanceId.ToString();
        }

        // Init cached version of the Update Rate field.
        m_skeleton->OfferService();
        adcm::Log::Info() << "exit DrivingTrajectory_Provider::init()";
    }
}

void DrivingTrajectory_Provider::send(driving_trajectory_Objects& data)
{
    if(mEnableUDP) {
        try {
            xDrivingTrajectory_UdpPacket packet;
            packet.Drive_Mode = data.Drive_Mode;
            packet.Emergency_acceleration = data.Emergency_acceleration;
            packet.Target_speed = data.Target_speed;
            packet.Turn_Signal = data.Turn_Signal;
            packet.Position = data.Position;
            packet.sizeof_trajectory = data.TrajectoryPointVector.size();
            for(int i = 0; i < packet.sizeof_trajectory; i++)
            {
                packet.TrajectoryPointVector[i] = data.TrajectoryPointVector[i];
            }
            unsigned char* ptr_sending = (unsigned char*)&packet;
            for(auto udpSender : mList_udpSender) {
                udpSender->sendBytes(ptr_sending, sizeof(packet));
                // INFO("send = %d / %d", sizeof(packet), packet.sizeof_trajectory);
            }

        } catch(std::exception e) {
            ERROR("Exeception : %s", e.what());
        }

    } else {
        try {
            auto allocation = m_skeleton->drivingTrajectoryEvent.Allocate();
            auto l_sampleData = std::move(allocation).Value();
            *l_sampleData = data;
            m_skeleton->drivingTrajectoryEvent.Send(std::move(l_sampleData));
            VERBOSE("[DrivingTrajectory] sent drivingTrajectoryEvent");

        } catch(const ara::com::Exception& e) {
            ERROR("Exeception : %s", e.what());
        }
    }
}

}  // namespace adcm
