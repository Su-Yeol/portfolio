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

#ifndef LINE_SENSOR_APP_H_
#define LINE_SENSOR_APP_H_

#include <atomic>  // SIGTERM
#include <memory>  // unique_ptr

// ara
#include "apd/picar/linesensorinterface_skeleton.h"
#include "ara/log/logger.h"  // Logger CreateLogger LogLevel

#include "line_detector.h"

namespace apd
{
namespace picar
{
namespace linesensor
{

class LineSensorApp
{
public:
    LineSensorApp() = default;

    LineSensorApp(const LineSensorApp&) = delete;
    LineSensorApp& operator=(const LineSensorApp&) = delete;
    LineSensorApp(LineSensorApp&&) = delete;
    LineSensorApp& operator=(LineSensorApp&&) = delete;

    /// @brief A method represents the lifecycle of sm application.
    /// Shall be called from main() function (return app.Execute())
    ///
    /// @return Execution status
    ///(could be used as a return value from main() function)
    int Execute();

private:
    /// @brief A method to be executed at initialize step.
    /// @return Initialize success status (Was it successful or not)
    bool Init();
    /// @brief A method to register Terminate Signal
    /// @return register success status
    bool RegisterSigTermHandler();

    /// @brief A method to be executed at runtime.
    void Run();
    /// @brief A method to wait until getting TERM signal
    void WaitUntilTermination();

    /// @brief Performs actions that are required during app termination
    void Terminate();
    /// @brief Close pipe ends for selfpipe.
    void CloseSelfPipe();

private:
    /// @brief Index for reading end of the self pipe
    static constexpr int PipeReadingEnd_ = 0;

    /// @brief Index for writing end of the self pipe
    static constexpr int PipeWritingEnd = 1;

    /// @brief File descriptors for pipe ends (used by self pipe trick)
    static int selfpipe[2];

    /// @brief An atomic<bool> flag, which is used for exiting after SIGTERM
    /// caught
    static std::atomic_bool exitRequested_;

    /// @brief SIGTERM signal handler
    /// This function is used to handle SIGTERM signals.
    ///
    /// @param sig the signal, which is sent to application
    static void SigTermHandler(int sig);

private:
    /// @brief Pointer to UpdateRequest service skeleton
    std::unique_ptr<apd::picar::skeleton::LineSensorInterfaceSkeleton> lineSensorService_;

    /// @brief A reference to logger, which is used to log app lifetime info
    ara::log::Logger& log_{
        ara::log::CreateLogger("LNSA", "LineSensor APP runtime context", ara::log::LogLevel::kVerbose)};

    LineDetector sensor_;
};

}  // namespace linesensor
}  // namespace picar
}  // namespace apd

#endif  // LINE_SENSOR_APP_H_
