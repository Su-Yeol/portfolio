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

#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>

#include <ara/log/logger.h>
#include "ara/core/instance_specifier.h"
#include "hp_processor.h"

#define NUM_WORKER 7

namespace apd
{
namespace wgres
{
namespace tra
{

HpProcessor::HpProcessor()
    : proxy_(0)
    , dataForExtraProcessing_(0)
    , periodicCheckerDone_(0)
    , periodicCheckerData_(0)
    , triggerFlag_(0)
    , triggerData_(0)
    , triggerDone_(0)
{ }

bool HpProcessor::FindService()
{
    logger_.LogInfo() << "HpProcessor - InitializeServiceInterfaces";

    ara::core::InstanceSpecifier portSpecifier{"RES_TimingRefApp/RES_TimingRefApp/cpapservicePort"};
    logger_.LogInfo() << "Port In Executable Ref:" << portSpecifier.ToString();

    auto instanceIDs = ara::com::runtime::ResolveInstanceIDs(portSpecifier);
    if (instanceIDs.empty()) {
        logger_.LogError() << "No InstanceIdentifiers resolved from provided InstanceSpecifier";
        return false;
    }

    logger_.LogInfo() << "Searching for Service Instance:" << instanceIDs[0].ToString();
    auto res = SensorActuatorServiceProxy::StartFindService(
        [this](ara::com::ServiceHandleContainer<SensorActuatorServiceProxy::HandleType> handles,
            ara::com::FindServiceHandle handler) { CallbackHandler(std::move(handles), handler); },
        portSpecifier);

    if (!res) {
        logger_.LogError() << "StartFindService failed with error: " << res.Error();
        return false;
    }
    return true;
}

void HpProcessor::Run()
{
    observerPeriodicChecker_ = std::make_unique<std::thread>([&]() { this->PeriodicChecker(10000); });
    observerTrigger_ = std::make_unique<std::thread>([&]() { this->Trigger(); });
}

void HpProcessor::Stop()
{
    if (observerPeriodicChecker_ != nullptr) {
        periodicCheckerDone_ = 1;
        observerPeriodicChecker_->join();
        observerPeriodicChecker_.reset();
    }

    if (observerTrigger_ != nullptr) {
        triggerDone_ = 1;
        // toDo set condVar
        observerTrigger_->join();
        observerTrigger_.reset();
    }
}

void HpProcessor::CallbackHandler(ara::com::ServiceHandleContainer<SensorActuatorServiceProxy::HandleType> handles,
    ara::com::FindServiceHandle handler)
{
    logger_.LogInfo() << "CallbackHandler was called. Found handles: " << handles.size();
    static int named = 0;
    if (named == 0) {
        named = 1;
        pthread_setname_np(pthread_self(), "cb_wgres_service");
    }

    if (handles.size() > 0) {
        for (auto handle : handles) {
            proxy_ = std::make_shared<SensorActuatorServiceProxy>(handle);

            if (proxy_ == nullptr) {
                logger_.LogWarn() << "CallbackHandler encountered nullptr as service handle.";
                continue;
            }

            logger_.LogInfo() << "Subscribing to speed event";
            if (!proxy_->sensorData.IsSubscribed()) {

                // Register event receive callback
                proxy_->sensorData.SetReceiveHandler([this]() { SensorDataEventHandler(); });

                // subscribe to event
                proxy_->sensorData.Subscribe(1);

            } else {
                logger_.LogInfo() << "Event already subscribed";
            }
        }
    }
    logger_.LogInfo() << "Leaving CallBackHandler.";
}

void HpProcessor::SensorDataEventHandler()
{
    static int named = 0;
    if (named == 0) {
        named = 1;
        pthread_setname_np(pthread_self(), "cb_wgres_sensor");
    }

    auto callback = [&](auto sample) {
        uint32_t mySensorData = static_cast<uint32_t>(*sample);
        ProcessSensorDataSample(mySensorData);
    };

    proxy_->sensorData.GetNewSamples(callback, 1);
}

void HpProcessor::ProcessSensorDataSample(uint32_t mySensorData)
{

    auto us
        = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    logger_.LogInfo() << us.count() << "DataReceived: ";
    logger_.LogInfo() << "Event handler received new sensor data: " << mySensorData;

    dataForExtraProcessing_ = mySensorData;

    Delay(1000, 2000);

    std::unique_ptr<std::thread> extraWorker[NUM_WORKER];

    for (int i = 0; i < NUM_WORKER; i++) {
        extraWorker[i] = std::make_unique<std::thread>([&](int index) { this->DoExtraProcessing(index); }, i);
    }

    for (int i = 0; i < NUM_WORKER; i++) {
        extraWorker[i]->join();
    }

    for (int i = 0; i < NUM_WORKER; i++) {
        extraWorker[i].reset();
    }

    Delay(10000, 20000);

    periodicCheckerData_ = mySensorData;
}

void HpProcessor::ProcessCallActionMethod()
{
    auto us
        = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    logger_.LogInfo() << us.count() << "Will call method with 10*" << triggerData_;

    auto future = proxy_->Action(10 * triggerData_);
    future.wait();
    auto result = future.GetResult();

    us = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch());
    logger_.LogInfo() << us.count() << "MethodCalled result=" << result.Value().return_value;
}

void HpProcessor::DoExtraProcessing(int id)
{
    char name[15] = "extra_worker_0";
    name[13] = id + '0';

    pthread_setname_np(pthread_self(), name);

    Delay(25000, 50000);
}

void HpProcessor::PeriodicChecker(int periodUS)
{

    pthread_setname_np(pthread_self(), "wgres_periodic");

    std::chrono::time_point<std::chrono::system_clock> nextPoint
        = std::chrono::system_clock::now() + std::chrono::microseconds(periodUS);

    uint32_t lastData = 0;

    while (periodicCheckerDone_ == 0) {
        std::this_thread::sleep_until(nextPoint);
        nextPoint = nextPoint + std::chrono::microseconds(periodUS);

        if (lastData != periodicCheckerData_) {
            lastData = periodicCheckerData_;

            auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::system_clock::now().time_since_epoch());
            logger_.LogInfo() << us.count() << "New sensor data in periodic check: " << periodicCheckerData_;

            Delay(13000, 23000);
            {
                std::lock_guard<std::mutex> lk(triggerMutex_);
                triggerData_ = periodicCheckerData_;
                triggerFlag_ = 1;
            }
            triggerCV_.notify_all();
        }
    }
}

void HpProcessor::Trigger()
{

    pthread_setname_np(pthread_self(), "wgres_trigger");

    while (triggerDone_ == 0) {
        // Wait for new data from PeriodicChecker
        {
            std::unique_lock<std::mutex> lk(triggerMutex_);
            triggerCV_.wait(lk, [this] { return triggerFlag_ == 1; });
            triggerFlag_ = 0;
        }

        auto us = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch());
        logger_.LogInfo() << us.count() << "Triggered for new data:" << triggerData_;

        Delay(1000, 2000);

        if (triggerData_ > 25) {
            ProcessCallActionMethod();
        }
    }
}

#define NOP10   \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop"); \
    asm("nop")

#define FACTOR_RASPI 23
#define FACTOR_RH3 13  // ToDo: Adjust to R-Car H3
#define FACTOR_US FACTOR_RASPI

void HpProcessor::Delay(int bcet, int wcet)
{
    // Generator and distribution have to be static, otherwiese
    // we always get the same values
    // static std::default_random_engine generator;
    // static std::uniform_int_distribution<int> distribution(bcet, wcet);

    // int valueUs = distribution(generator);

    int valueUs = wcet;

    unsigned int current = FACTOR_US / 10 * valueUs;

    for (unsigned int i = 0; i < current; ++i) {
        /* 500 nops */
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
        NOP10;
    }
}

} /* namespace tra */
} /* namespace wgres */
} /* namespace apd */
