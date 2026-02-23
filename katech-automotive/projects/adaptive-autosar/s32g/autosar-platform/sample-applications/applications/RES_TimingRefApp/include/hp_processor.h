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

#ifndef HPPROCESSOR_H_
#define HPPROCESSOR_H_

#include <thread>
#include <mutex>
#include <condition_variable>

#include "apd/wgres/tra/sensoractuatorservice_proxy.h"
#include <ara/log/logger.h>

namespace apd
{
namespace wgres
{
namespace tra
{

using apd::wgres::tra::proxy::SensorActuatorServiceProxy;

class HpProcessor
{
public:
    HpProcessor();
    bool FindService();
    void Run();
    void Stop();
    virtual ~HpProcessor() = default;

private:
    void CallbackHandler(ara::com::ServiceHandleContainer<SensorActuatorServiceProxy::HandleType> handles,
        ara::com::FindServiceHandle handler);
    void SensorDataEventHandler();
    void ProcessSensorDataSample(uint32_t data);
    void ProcessCallActionMethod();
    std::shared_ptr<SensorActuatorServiceProxy> proxy_;

    uint32_t dataForExtraProcessing_;
    void DoExtraProcessing(int id);

    void PeriodicChecker(int periodUS);
    std::unique_ptr<std::thread> observerPeriodicChecker_;
    int periodicCheckerDone_;
    uint32_t periodicCheckerData_;

    void Trigger();
    std::unique_ptr<std::thread> observerTrigger_;
    std::mutex triggerMutex_;
    std::condition_variable triggerCV_;
    int triggerFlag_;
    uint32_t triggerData_;
    int triggerDone_;

    void Delay(int bcet, int wcet);

private:
    /// @brief Logger for logging errors
    ara::log::Logger& logger_{ara::log::CreateLogger("THPP",
        "TRA High Performance Processor - Service Subscriber",
        ara::log::LogLevel::kVerbose)};
};

} /* namespace tra */
} /* namespace wgres */
} /* namespace apd */

#endif /* HPPROCESSOR_H_ */
