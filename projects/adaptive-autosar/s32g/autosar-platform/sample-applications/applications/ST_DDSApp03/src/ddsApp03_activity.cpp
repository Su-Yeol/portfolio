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

#include "ddsApp03_activity.hpp"

#include <stdint.h>

#include <iomanip>
#include <cstdlib>
#include <exception>
#include <cassert>
#include <iostream>
#include <stdexcept>

// includes for used services

#include "ara/com/e2e/e2e_types.h"
#include "ara/com/e2e_helper.h"

#include "ara/core/instance_specifier.h"

#include "ara/log/logger.h"
#include "apd/testsuite/cmdds/ddsservice5_proxy.h"
using namespace ara::log;

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Activity Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

ddsApp03Activity::ddsApp03Activity()
{
    this->ddsService2Proxy = new ddsService2Imp();
    this->ddsService3Proxy = new ddsService3Imp();
    this->ddsService5Proxy = new ddsService5Imp();
}

void ddsApp03Activity::init()
{
    GetLogger().LogInfo() << "init() enter";

    std::int16_t DDSApp03_port = 16003;
    tcpServer = new ddsApp03TcpServer(DDSApp03_port);

    GetLogger().LogInfo() << "init() exit";
}

void ddsApp03Activity::act()
{
    GetLogger().LogInfo() << "ddsApp03 alive";

    std::string request;

    bool retRun = tcpServer->Run(request);
    if (retRun == true) {
        GetLogger().LogInfo() << "Read Tester Request";
        Command(request);
    }
}

void ddsApp03Activity::deinit()
{
    ddsService2Proxy->StopFindService2();
    ddsService3Proxy->StopFindService3();
    ddsService5Proxy->StopFindService5();
}

void ddsApp03Activity::Command(std::string request)
{
    GetLogger().LogInfo() << "ddsApp03 service Command";
    bool ret;
    std::string response;

    // cast string to int
    int reqNum = atoi(request.c_str());
    GetLogger().LogInfo() << "request:" << reqNum;

    switch (reqNum) {
    case DDSAPP03RequestServices::FIND_DDSSERVICE2: {
        GetLogger().LogInfo() << "Find DDS Service2";
        ddsService2Proxy->StartFindService2();

        break;
    }
    case DDSAPP03RequestServices::FIND_DDSSERVICE3: {
        GetLogger().LogInfo() << "Find DDS Service3";
        ddsService3Proxy->StartFindService3();

        break;
    }
    case DDSAPP03RequestServices::FIND_DDSSERVICE5: {
        GetLogger().LogInfo() << "Find DDS Service5";
        ddsService5Proxy->StartFindService5();

        break;
    }
    case DDSAPP03RequestServices::CALL_DDSSERVICE5: {
        GetLogger().LogInfo() << "Call DDS Service5";
        ddsService5Proxy->asyncMethodCCall();
        ddsService5Proxy->asyncMethodDCall();

        break;
    }
    case DDSAPP03RequestServices::STOP_DDSSERVICE2: {
        GetLogger().LogInfo() << "StopFind DDS Service2";
        ddsService2Proxy->StopFindService2();

        break;
    }
    case DDSAPP03RequestServices::STOP_DDSSERVICE3: {
        GetLogger().LogInfo() << "StopFind DDS Service3";
        ddsService3Proxy->StopFindService3();

        break;
    }
    case DDSAPP03RequestServices::STOPSENDING_DDSSERVICE2: {
        GetLogger().LogInfo() << "StopSending DDS Service2";
        ddsService2Proxy->StopSendingService2();

        break;
    }
    case DDSAPP03RequestServices::SUBSCRIBE_DDSSERVICE2: {
        GetLogger().LogInfo() << "Subscribe DDS Service2";
        if (nullptr != this->ddsService2Proxy->service2_proxy) {

            ddsService2Proxy->SetEventCHandler();
        }
        break;
    }
    case DDSAPP03RequestServices::SUBSCRIBE_DDSSERVICE3: {
        GetLogger().LogInfo() << "Subscribe DDS Service3";
        ddsService3Proxy->FieldASubscription();

        break;
    }
    case DDSAPP03RequestServices::GETVALUE_DDSSERVICE3: {
        GetLogger().LogInfo() << "GetValue DDS Service3";
        ddsService3Proxy->FieldGetter();

        break;
    }
    case DDSAPP03RequestServices::SETVALUE_DDSSERVICE3: {
        GetLogger().LogInfo() << "SetValue DDS Service3";
        ddsService3Proxy->FieldSetter();

        break;
    }
    case TESTER_REQUESTS::REQUEST_RESULTS_TC1: {
        GetLogger().LogInfo() << "Send Tester Request Results";
        response = ddsService2Proxy->SendRequestResult1();
        ret = tcpServer->WriteString(response);
        GetLogger().LogInfo() << "Response:" << response;
        if (ret != true) {
            GetLogger().LogInfo() << "DdsApp02TcpServer :not send result";
        }

        break;
    }
    case TESTER_REQUESTS::REQUEST_RESULTS_TC2_1: {
        GetLogger().LogInfo() << "Send Tester Request Results";
        response = ddsService2Proxy->SendRequestResult2();
        ret = tcpServer->WriteString(response);
        GetLogger().LogInfo() << "Response:" << response;
        if (ret != true) {
            GetLogger().LogInfo() << "DdsApp02TcpServer :not send result";
        }

        break;
    }
    case TESTER_REQUESTS::REQUEST_RESULTS_TC2_2: {
        GetLogger().LogInfo() << "Send Tester Request Results";
        response = ddsService2Proxy->SendRequestResult2_2();
        ret = tcpServer->WriteString(response);
        GetLogger().LogInfo() << "Response:" << response;
        if (ret != true) {
            GetLogger().LogInfo() << "DdsApp02TcpServer :not send result";
        }

        break;
    }
    case TESTER_REQUESTS::REQUEST_RESULTS_TC3: {
        GetLogger().LogInfo() << "Send Tester Request Results";
        response = ddsService3Proxy->SendRequestResult3();
        ret = tcpServer->WriteString(response);
        GetLogger().LogInfo() << "Response:" << response;
        if (ret != true) {
            GetLogger().LogInfo() << "DdsApp02TcpServer :not send result";
        }

        break;
    }
    case TESTER_REQUESTS::REQUEST_RESULTS_TC4: {
        GetLogger().LogInfo() << "Send Tester Request Results";
        response = ddsService5Proxy->SendRequestResult4();
        ret = tcpServer->WriteString(response);
        GetLogger().LogInfo() << "Response:" << response;
        if (ret != true) {
            GetLogger().LogInfo() << "DdsApp02TcpServer :not send result";
        }

        break;
    }
    default: {
        std::cout << "DdsApp03TcpServer: Request does not exist  not defined" << std::endl;
        GetLogger().LogInfo() << "ddsApp03TcpServer: Request does not exist not defined";
    }
    }
}
