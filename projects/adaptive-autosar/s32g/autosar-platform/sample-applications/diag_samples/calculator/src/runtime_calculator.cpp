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

#include "runtime_calculator.h"

#include <unistd.h>
#include <csignal>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>

#include <boost/property_tree/json_parser.hpp>  // based on boost v1.58.0
#include <boost/property_tree/ptree.hpp>  // based on boost v1.58.0

#ifdef USE_ARA
#    include <ara/exec/execution_client.h>
#endif

#include "common/exception/errno_exception.h"

namespace ara
{
namespace diag
{
namespace sample_application
{
namespace calculator
{

static const char kPathToConfigFile[] = "etc/calculator_cfg.json";

RuntimeCalculator::RuntimeCalculator(const boost::property_tree::ptree& /*config*/)
{ }

/**
 * \brief Creates an instance of the Runtime.
 *
 * Fills the instance_ pointer and installs a signal handler. This method is threadsafe. It is guaranteed that at most
 * one Runtime instance is created, no matter how may times this Function is called in parallel.
 *
 * If instance_ != nullptr, this method does nothing.
 */
void RuntimeCalculator::MakeInstance()
{
    std::unique_lock<std::mutex> instance_lock(instance_mutex_);
    if (instance_ != nullptr) {
        // Another thread was quicker, Instance already exists.
        return;
    }
    // load the configuration
    boost::property_tree::ptree config;
    boost::property_tree::read_json(kPathToConfigFile, config, std::locale());
    // create a new instance
    RuntimeCalculator::instance_ = std::unique_ptr<RuntimeCalculator>{new RuntimeCalculator{config}};

    // Register handler for quit signal
    sighandler_t old_signalhandler{signal(SIGTERM, Runtime::SignalHandler)};
    // Verify whether signal handler was registered successfully. Otherwise, abort
    // the application with an error.
    if (SIG_ERR == old_signalhandler) {
        throw ara::diag::exception::ErrnoException(errno);
    }
}

/**
 * \brief Return the Runtime instance.
 */
RuntimeCalculator& RuntimeCalculator::GetInstance()
{
    if (instance_ == nullptr) {
        RuntimeCalculator::MakeInstance();
    }
    return *dynamic_cast<RuntimeCalculator*>(instance_.get());
}

/**
 * read in configuration information and initialize runtime from it.
 */
void RuntimeCalculator::Initialize()
{
#if !defined(__CYGWIN__)
    pthread_setname_np(pthread_self(), "diagnosticExampleApplicationCalculator");
#endif

    signal(SIGTERM, Calculator::SignalHandler);
    initialized_ = true;
}

/**
 * Start services. After this method has been finished successfully, dcm is:
 * - able to serve UDS diag request comming in from protocol adapters.
 * - able to handle handle diag requests initiated from local clients.
 */
void RuntimeCalculator::Run()
{
    threads_.reserve(1);
    threads_.emplace_back(&ara::diag::sample_application::calculator::Calculator::Main, &calculator_);
#if !defined(__CYGWIN__)
    pthread_setname_np(threads_[0].native_handle(), "Calculator Main");
#endif
#ifdef USE_ARA
    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);
#endif
}

/**
 * stops service handling (incoming and outgoing UDS requests). Method returns
 * as soon as services have been stopped.
 */
void RuntimeCalculator::Shutdown()
{
    for (std::thread& thread : threads_) {
        thread.join();
    }
}

}  // namespace calculator
}  // namespace sample_application
}  // namespace diag
}  // namespace ara
