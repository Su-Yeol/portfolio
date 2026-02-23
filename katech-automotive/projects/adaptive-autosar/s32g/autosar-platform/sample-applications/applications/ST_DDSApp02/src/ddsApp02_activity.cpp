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

#include "ddsApp02_activity.hpp"

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

using namespace ara::log;

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Activity Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

ddsApp02Activity::ddsApp02Activity()
{
    this->ddsService1Proxy = new ddsService1Imp();
    this->ddsService3Proxy = new ddsService3Imp();
    this->ddsService4Proxy = new ddsService4Imp();
}

void ddsApp02Activity::init()
{
    std::int16_t DDSApp02_port = 16002;
    tcpServer = new ddsApp02TcpServer(DDSApp02_port);
}

void ddsApp02Activity::act()
{
    GetLogger().LogInfo() << "ddsApp02 alive";
    std::string request;

    bool retRun = tcpServer->Run(request);
    if (retRun == true) {
        GetLogger().LogInfo() << "Read Tester Request";
        Command(request);
    }
}
void ddsApp02Activity::deinit()
{
    ddsService1Proxy->StopFindService1();
    ddsService3Proxy->StopFindService3();
    ddsService4Proxy->StopFindService4();
}

void ddsApp02Activity::Command(std::string request)
{
    GetLogger().LogInfo() << "ddsApp02 service Command";

    // cast string to int
    int reqNum = atoi(request.c_str());
    GetLogger().LogInfo() << "request:" << reqNum;

    std::string response;
    bool ret;

    switch (reqNum) {
    case DDSAPP02RequestServices::FIND_DDSSERVICE1: {
        GetLogger().LogInfo() << "find DDS Service1";
        ddsService1Proxy->StartFindService1();

        break;
    }
    case DDSAPP02RequestServices::FIND_DDSSERVICE3: {
        GetLogger().LogInfo() << "find DDS Service3";
        ddsService3Proxy->StartFindService3();

        break;
    }
    case DDSAPP02RequestServices::FIND_DDSSERVICE4: {
        GetLogger().LogInfo() << "find DDS Service4";
        ddsService4Proxy->StartFindService4();

        break;
    }
    case DDSAPP02RequestServices::CALL_DDSSERVICE4: {
        GetLogger().LogInfo() << "Call DDS Service4 Method";
        ddsService4Proxy->syncMethodACall();
        ddsService4Proxy->syncMethodBCall();

        break;
    }
    case DDSAPP02RequestServices::STOP_DDSSERVICE1: {
        GetLogger().LogInfo() << "StopFind DDS Service1";
        ddsService1Proxy->StopFindService1();

        break;
    }
    case DDSAPP02RequestServices::STOP_DDSSERVICE3: {
        GetLogger().LogInfo() << "StopFind DDS Service3";
        ddsService3Proxy->StopFindService3();

        break;
    }
    case DDSAPP02RequestServices::STOPSENDING_DDSSERVICE1: {
        GetLogger().LogInfo() << "StopSending DDS Service1";
        ddsService1Proxy->StopSendingService1();

        break;
    }
    case DDSAPP02RequestServices::SUBSCRIBE_DDSSERVICE1: {
        GetLogger().LogInfo() << "Subscribe DDS Service1";
        if (nullptr != this->ddsService1Proxy->service1_proxy) {

            ddsService1Proxy->SetEventAHandler();
            ddsService1Proxy->SetEventBHandler();
        }

        break;
    }
    case DDSAPP02RequestServices::SUBSCRIBE_DDSSERVICE3: {
        GetLogger().LogInfo() << "Subscribe DDS Service3";
        ddsService3Proxy->FieldASubscription();

        break;
    }
    case DDSAPP02RequestServices::GETVALUE_DDSSERVICE3: {
        GetLogger().LogInfo() << "GetValue DDS Service3";
        ddsService3Proxy->FieldGetter();

        break;
    }
    case DDSAPP02RequestServices::SETVALUE_DDSSERVICE3: {
        GetLogger().LogInfo() << "SetValue DDS Service3";
        ddsService3Proxy->FieldSetter();

        break;
    }
    case TESTER_REQUESTS::REQUEST_RESULTS_TC1: {
        GetLogger().LogInfo() << "Send Tester Request Results";
        // RequestResult;
        response = ddsService1Proxy->SendRequestResult1();
        ret = tcpServer->WriteString(response);
        GetLogger().LogInfo() << "Response:" << response;
        if (ret != true) {
            GetLogger().LogInfo() << "DdsApp02TcpServer : not send result ";
        }

        break;
    }
    case TESTER_REQUESTS::REQUEST_RESULTS_TC2_1: {
        GetLogger().LogInfo() << "Send Tester Request Results";
        // RequestResult;
        response = ddsService1Proxy->SendRequestResult2();
        GetLogger().LogInfo() << "Response:" << response;
        ret = tcpServer->WriteString(response);
        if (ret != true) {
            GetLogger().LogInfo() << "DdsApp02TcpServer : not send result ";
        }

        break;
    }
    case TESTER_REQUESTS::REQUEST_RESULTS_TC2_2: {
        GetLogger().LogInfo() << "Send Tester Request Results";
        // RequestResult;
        response = ddsService1Proxy->SendRequestResult2_2();
        GetLogger().LogInfo() << "Response:" << response;
        ret = tcpServer->WriteString(response);
        if (ret != true) {
            GetLogger().LogInfo() << "DdsApp02TcpServer : not send result ";
        }

        break;
    }
    case TESTER_REQUESTS::REQUEST_RESULTS_TC3: {
        GetLogger().LogInfo() << "Send Tester Request Results";
        // RequestResult;
        response = ddsService3Proxy->SendRequestResult3();
        GetLogger().LogInfo() << "Response:" << response;
        ret = tcpServer->WriteString(response);
        if (ret != true) {
            GetLogger().LogInfo() << "DdsApp02TcpServer : not send result ";
        }

        break;
    }
    case TESTER_REQUESTS::REQUEST_RESULTS_TC4: {
        GetLogger().LogInfo() << "Send Tester Request Results";
        // RequestResult;
        response = ddsService4Proxy->SendRequestResult4();
        GetLogger().LogInfo() << "Response:" << response;
        ret = tcpServer->WriteString(response);
        if (ret != true) {
            GetLogger().LogInfo() << "DdsApp02TcpServer : not send result ";
        }

        break;
    }
    default: {
        std::cout << "DdsApp02TcpServer: Request does not exist  not defined" << std::endl;
        GetLogger().LogInfo() << "ddsApp02TcpServer: Request does not exist not defined";
    }
    }
}
