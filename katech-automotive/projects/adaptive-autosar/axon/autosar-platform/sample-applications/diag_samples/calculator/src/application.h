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

#ifndef SAMPLE_CALCULATOR_SOURCE_APPLICATION_H_
#define SAMPLE_CALCULATOR_SOURCE_APPLICATION_H_

#include <atomic>
#include <memory>
#include <thread>
#include "ara/core/vector.h"

namespace ara
{
namespace diag
{
namespace sample_application
{
namespace calculator
{

/**
 * \brief
 */
class Application
{
public:
    Application() = default;
    virtual ~Application() = default;

    virtual void Initialize() = 0;
    virtual void Run() = 0;
    virtual void Shutdown() = 0;

    /**
     * \brief Getter for the initialization status.
     */
    bool IsInitialized() const
    {
        return initialized_;
    }

protected:
    /**
     * \brief Signal handler to catch signals sent to the AdaptiveAutosarApplication.
     *
     * This signal handler only handles SIGTERM, which asks the application to terminate.
     * It forwards the notification that a SIGTERM occurred using selfpipe.
     */
    virtual void HandleSignal(int signum) = 0;

    /**
     * \brief Signal handler to catch signals sent to the process.
     *
     * This handler delegates to HandleSignal(int).
     */
    static void SignalHandler(int /*signum*/)
    { }

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
};

}  // namespace calculator
}  // namespace sample_application
}  // namespace diag
}  // namespace ara

#endif  // SAMPLE_CALCULATOR_SOURCE_APPLICATION_H_
