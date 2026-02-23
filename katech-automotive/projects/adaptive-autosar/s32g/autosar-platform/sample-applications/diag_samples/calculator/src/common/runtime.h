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

#ifndef SOURCE_COMMON_RUNTIME_H_
#define SOURCE_COMMON_RUNTIME_H_

#include <boost/property_tree/ptree.hpp>  // based on boost v1.58.0

#include <atomic>
#include <exception>
#include <memory>
#include <mutex>
#include <thread>
#include "ara/core/vector.h"

#include <ara/log/logger.h>

namespace ara
{
namespace diag
{

/**
 *  \brief class Runtime is a singleton, which represents the running instance of the
 *  Dcm part of the diagnostics module.
 *  \details It basically provides methods for controlling the life cycle of the dcm
 *  part of diagnostics - that is: initializing, starting/running and stopping.
 */
class Runtime
{
protected:
    /**
     * \brief Flag to indicate that the Runtime object is initialized correctly.
     */
    bool initialized_{false};

    /**
     * \brief Flag to indicate whether this Application has been asked to terminate.
     */
    std::atomic_bool exit_requested_{false};

    /**
     * \brief Container to hold all Threads spawned by this object.
     */
    ara::core::Vector<std::thread> threads_{};

    /**
     * \brief A pair of pipes used by SignalHandler() to notify WaitSignal() of an incoming signal.
     */
    int selfpipe_[2]{};

public:
    Runtime();
    virtual ~Runtime();

    /**
     * \brief Performs one-time setup of the application.
     */
    virtual void Initialize() = 0;

    /**
     * \brief Executes the workload of the application.
     *
     * This method spawns threads for DEM::Main() and DCM::Main().
     */
    virtual void Run() = 0;

    /**
     * \brief Perform an orderly shutdown of the application.
     *
     * This method shuts down all communication and waits for all threads to terminate.
     */
    virtual void Shutdown() = 0;

    /**
     * \brief Return the Runtime instance.
     */
    static Runtime& GetInstance();

    /**
     * \brief Getter for the initialization status.
     */
    bool IsInitialized() const
    {
        return initialized_;
    }

    /**
     * \brief Blocks the application until it is explicitly asked to terminate.
     *
     * The blocking wait is implemented by a blocking wait on a pipe. HandleSignal() catches the SIGTERM signal and
     * converts it into a token sent via the pipe.
     */
    void WaitUntilTermination();

    /**
     * \brief Asks the application to load the configuration from the configuration file.
     *
     * \param PathToConfigFile path to the file location of the config file.
     *
     * @throws ParseError In case the config file could not be opened or the syntax of the config file is invalid.
     */
    static boost::property_tree::ptree loadConfiguration(const char* PathToConfigFile);

protected:
    /**
     * \brief Signal handler to catch signals sent to the process.
     *
     * This handler delegates to HandleSignal(int).
     */
    static void SignalHandler(int signum);

    /**
     * \brief Pointer to the instance of the AdaptiveAutosarApplication.
     *
     * Used by SignalHandler(int) to delegate to HandleSignal(int). Note: instance_ is a guard against misconfiguring
     * the signal handler. It is not an implementation of the singleton pattern!
     */
    static std::unique_ptr<Runtime> instance_;

    /**
     * \brief Mutex used to protect concurrent write access to instance_.
     *
     * Note that concurrent read is possible without protection by the mutex. The rationale is that instance_ is
     * conceptually write-once, i.e., it will be written once by MakeInstance() and is read-only from then on out.
     */
    static std::mutex instance_mutex_;

    /**
     * \brief Signal handler to catch signals sent to the AdaptiveAutosarApplication.
     *
     * This signal handler only handles SIGTERM, which asks the application to terminate.
     * It forwards the notification that a SIGTERM occurred using selfpipe.
     */
    void HandleSignal(int signum);

private:
    /*
     * Ensure that objects of this type are not copyable and not movable.
     */
    Runtime(Runtime&& other) = delete;
    Runtime(const Runtime& other) = delete;
    Runtime& operator=(Runtime&& other) = delete;
    Runtime& operator=(const Runtime& other) = delete;

    /*
     * Logging instance reference
     */
    ara::log::Logger& log_;
};

} /* namespace diag */
} /* namespace ara */

#endif  // SOURCE_COMMON_RUNTIME_H_
