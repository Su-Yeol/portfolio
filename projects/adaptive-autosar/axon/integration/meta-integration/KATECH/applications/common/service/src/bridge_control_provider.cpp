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

#include "bridge_control_provider.h"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <chrono>

#include "katech/bridge_control_skeleton.h"
#include "ara/com/com_error_domain.h"
#include "ara/core/instance_specifier.h"
#include "logger.h"

using namespace ara::log;
using ara::com::ComErrorDomainErrc;

// <<operator implementation for logging custom types
// similar handler is also available in fusion sources, so for real projects it make sense to
// have some common place where custom type log-handlers are provided.

namespace katech
{

void BridgeControlImp::ProcessRequests()
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

BridgeControl_Provider::BridgeControl_Provider()
{
    DEBUG("object address : %p", static_cast<void*>(this));
}

BridgeControl_Provider::~BridgeControl_Provider()
{
    delete m_skeleton;
}

void BridgeControl_Provider::init(std::string instance)
{
    katech::Log::Info() << "enter BridgeControl_Provider::init()";
    // Find Service
    ara::core::InstanceSpecifier instanceSpec = ara::core::InstanceSpecifier(instance.c_str()); 
    INFO("Port In Executable Ref: %s", instanceSpec.ToString().data());

    // BridgeControlImp : AUTSOAR 코드 생성 툴(arxml->C++)에 의해 생성된 Skeleton 클래스
    m_skeleton = new BridgeControlImp(instanceSpec, ara::com::MethodCallProcessingMode::kPoll); 
    
    // The instance id resolution is not mandatory for service creation (but could be an option)
    // here it's intended to list ids for the offered service instances
    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(instanceSpec);

    for(auto const& instanceId : instanceIDs) {
        INFO("Service Instance offered: %s", instanceId.ToString().data());
    }

    m_skeleton->OfferService(); // 인스턴스를 외부에 서비스 제공자로 등록; Subscriber는 이 서비스에 연결 가능
    katech::Log::Info() << "exit BridgeControl_Provider::init()";
}

void BridgeControl_Provider::send(bridge_control_Objects& data)
{
    /* 
        bridge control objects 데이터를 Event 포트를 통해 전송
        AA의 Event Publish 방식 사용
    */
    try {
        auto allocation = m_skeleton->controlBridgeEvent.Allocate(); // 이벤트 포트의 샘플 메모리 할당
        auto l_sampleData = std::move(allocation).Value(); // 실제 데이터 포인터 추출 -> std::move()로 소유권 이전
        *l_sampleData = data;
        m_skeleton->controlBridgeEvent.Send(std::move(l_sampleData)); // 이벤트 전송
        DEBUG("sent");

    } catch(ara::com::Exception e) {
        ERROR("Exeception : %s", e.what());
    }
}

}// namespace katech
