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

#include "ddsApp01_activity.hpp"

#include <stdint.h>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include "ara/log/logger.h"
#include "apd/testsuite/cmdds/ddsservice5_skeleton.h"
using namespace ara::log;

using apd::testsuite::cmdds::skeleton::fields::FieldA;

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "DDS Activity Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

ddsApp01Activity::ddsApp01Activity()
{
    ara::com::InstanceIdentifier service1_instance_id{"DDS:29"};
    ara::com::InstanceIdentifier service2_instance_id{"DDS:30"};
    ara::com::InstanceIdentifier service3_instance_id{"DDS:31"};
    ara::com::InstanceIdentifier service4_instance_id{"DDS:32"};
    ara::com::InstanceIdentifier service5_instance_id{"DDS:33"};

    this->ddsService1Skeleton = new ddsService1Imp(service1_instance_id, ara::com::MethodCallProcessingMode::kPoll);
    this->ddsService2Skeleton = new ddsService2Imp(service2_instance_id, ara::com::MethodCallProcessingMode::kPoll);
    this->ddsService3Skeleton = new ddsService3Imp(service3_instance_id, ara::com::MethodCallProcessingMode::kPoll);
    this->ddsService4Skeleton = new ddsService4Imp(service4_instance_id, ara::com::MethodCallProcessingMode::kPoll);
    this->ddsService5Skeleton = new ddsService5Imp(service5_instance_id, ara::com::MethodCallProcessingMode::kEvent);
}

ddsApp01Activity::~ddsApp01Activity()
{
    delete this->ddsService1Skeleton;
    delete this->ddsService2Skeleton;
    delete this->ddsService3Skeleton;
    delete this->ddsService4Skeleton;
    delete this->ddsService5Skeleton;
}

void ddsApp01Activity::init()
{
    GetLogger().LogInfo() << "enter init()";

    // Initialize Fields Values
    this->ddsService3Skeleton->FieldInitialization();

    std::int16_t DDSApp01_port = 16001;
    tcpServer = new ddsApp01TcpServer(DDSApp01_port);
}

void ddsApp01Activity::act()
{
    GetLogger().LogInfo() << "ddsApp01 active";
    std::string request;
    static uint8_t i = 0;

    bool retRun = tcpServer->Run(request);
    if (retRun == false) {
        GetLogger().LogError() << " Tcp Run Faild ";
    } else {
        GetLogger().LogInfo() << " Read Tcp Request" << request;
        Command(request);
    }

    i++;
}

void ddsApp01Activity::deinit()
{
    GetLogger().LogInfo() << "enter deinit()";
    this->ddsService1Skeleton->stopOfferDdsService1();
    this->ddsService2Skeleton->stopOfferDdsService2();
    this->ddsService3Skeleton->stopOfferDdsService3();
    this->ddsService4Skeleton->stopOfferDdsService4();
    this->ddsService5Skeleton->stopOfferDdsService5();
}

void ddsApp01Activity::Command(std::string request)
{
    GetLogger().LogInfo() << "ddsApp01 service Command";

    /* cast string to int */
    int reqNum = atoi(request.c_str());
    GetLogger().LogInfo() << "request:" << reqNum;

    static uint8_t i = 10;

    switch (reqNum) {
    case DDSAPP01RequestServices::OFFER_DDSSERVICE1: {
        GetLogger().LogInfo() << "Offer DDS Service1";
        ddsService1Skeleton->offerDdsService1();

        break;
    }
    case DDSAPP01RequestServices::OFFER_DDSSERVICE2: {
        GetLogger().LogInfo() << "Offer DDS Service2";
        ddsService2Skeleton->offerDdsService2();

        break;
    }
    case DDSAPP01RequestServices::OFFER_DDSSERVICE3: {
        GetLogger().LogInfo() << "Offer DDS Service3";
        ddsService3Skeleton->offerDdsService3();

        break;
    }
    case DDSAPP01RequestServices::OFFER_DDSSERVICE4: {
        GetLogger().LogInfo() << "Offer DDS Service4";
        ddsService4Skeleton->offerDdsService4();

        break;
    }
    case DDSAPP01RequestServices::OFFER_DDSSERVICE5: {
        GetLogger().LogInfo() << "Offer DDS Service5";
        ddsService5Skeleton->offerDdsService5();

        break;
    }
    case DDSAPP01RequestServices::STOP_DDSSERVICE1: {
        GetLogger().LogInfo() << "StopOffer DDS Service1";
        ddsService1Skeleton->stopOfferDdsService1();

        break;
    }
    case DDSAPP01RequestServices::STOP_DDSSERVICE2: {
        GetLogger().LogInfo() << "StopOffer DDS Service2";
        ddsService2Skeleton->stopOfferDdsService2();

        break;
    }
    case DDSAPP01RequestServices::STOP_DDSSERVICE3: {
        GetLogger().LogInfo() << "StopOffer DDS Service3";
        ddsService3Skeleton->stopOfferDdsService3();

        break;
    }
    case DDSAPP01RequestServices::STOP_DDSSERVICE4: {
        GetLogger().LogInfo() << "StopOffer DDS Service4";
        ddsService4Skeleton->stopOfferDdsService4();

        break;
    }
    case DDSAPP01RequestServices::STOP_DDSSERVICE5: {
        GetLogger().LogInfo() << "StopOffer DDS Service5";
        ddsService5Skeleton->stopOfferDdsService5();

        break;
    }
    case DDSAPP01RequestServices::SEND_DDSSERVICE1: {
        GetLogger().LogInfo() << "Send DDS Service1";
        ddsService1Skeleton->SendEventA(i);
        ddsService1Skeleton->SendEventB(i);

        break;
    }
    case DDSAPP01RequestServices::SEND_DDSSERVICE2: {
        GetLogger().LogInfo() << "Send DDS Service2";
        ddsService2Skeleton->SendEventC(i);

        break;
    }
    case DDSAPP01RequestServices::UPDATE_DDSSERVICE3: {
        GetLogger().LogInfo() << "Update DDS Service3";
        ddsService3Skeleton->UpdateFieldA();

        break;
    }
    default: {
        std::cout << "DdsApp01TcpServer: Request does not exist  not defined" << std::endl;
        GetLogger().LogInfo() << "ddsApp01TcpServer: Request does not exist not defined";
    }
    }
}
