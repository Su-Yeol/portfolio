// // // --------------------------------------------------------------------------
// // // |              _    _ _______     .----.      _____         _____        |
// // // |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
// // // |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
// // // |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
// // // |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
// // // |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
// // // |                              . _ _  .                                  |
// // // --------------------------------------------------------------------------
// // //
// // // All Rights Reserved.
// // // Any use of this source code is subject to a license agreement with the
// // // AUTOSAR development cooperation.
// // // More information is available at www.autosar.org.
// // //
// // // Disclaimer
// // //
// // // This work (specification and/or software implementation) and the material
// // // contained in it, as released by AUTOSAR, is for the purpose of information
// // // only. AUTOSAR and the companies that have contributed to it shall not be
// // // liable for any use of the work.
// // //
// // // The material contained in this work is protected by copyright and other
// // // types of intellectual property rights. The commercial exploitation of the
// // // material contained in this work requires a license to such intellectual
// // // property rights.
// // //
// // // This work may be utilized or reproduced without any modification, in any
// // // form or by any means, for informational purposes only. For any other
// // // purpose, no part of the work may be utilized or reproduced, in any form
// // // or by any means, without permission in writing from the publisher.
// // //
// // // The work has been developed for automotive applications only. It has
// // // neither been developed, nor tested for non-automotive applications.
// // //
// // // The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// // // --------------------------------------------------------------------------

// // #include "ara/tsync/synchronized_time_base_provider.h"
// // #include "ara/tsync/timestamp.h"
// // #include "ara/core/instance_specifier.h"
// // #include "ara/core/initialization.h"
// // #include "ara/exec/execution_client.h"
// // #include "ara/log/logger.h"
// // #include <chrono>
// // #include <cstdlib>
// // #include <csignal>
// // #include <atomic>
// // #include <thread>

// // namespace
// // {
// // // Atomic flag for exit after SIGTERM caught
// // std::atomic_bool continueExecution{true};

// // void SigTermHandler(int signal)
// // {
// //     if (signal == SIGTERM) {
// //         // Set atomic exit flag
// //         continueExecution = false;
// //     }
// // }

// // bool RegisterSigTermHandler()
// // {
// //     struct sigaction sa;
// //     sa.sa_handler = SigTermHandler;
// //     sa.sa_flags = 0;
// //     sigemptyset(&sa.sa_mask);
// //     // Register signal handler
// //     if (sigaction(SIGTERM, &sa, nullptr) == -1) {
// //         // Could not register a SIGTERM signal handler
// //         return false;
// //     }
// //     return true;
// // }
// // }  // namespace

// // int main()
// // {
// //     if (!ara::core::Initialize()) {
// //         // No interaction with ARA is possible here since initialization failed
// //         return EXIT_FAILURE;
// //     }

// //     ara::exec::ExecutionClient exec_client;
// //     exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

// //     auto& logger = ara::log::CreateLogger("TSP1", "TimeSyncProvider Context 1", ara::log::LogLevel::kVerbose);

// //     if (!RegisterSigTermHandler()) {
// //         logger.LogError() << "Unable to register signal handler";
// //     }

// //     // Register a Synchronized Master Time Base with ARA::TSYNC
// //     ara::tsync::SynchronizedTimeBaseProvider GPSClock(ara::core::InstanceSpecifier("GPS"));

// //     bool gpsClockToggle = false;

// //     // Toggle the GPS clock every 10 seconds
// //     while (continueExecution) {
// //         logger.LogInfo() << "TimeSyncProvider is alive";
// //         if (gpsClockToggle) {
// //             GPSClock.SetTime(ara::tsync::Timestamp(std::chrono::seconds(654321)));
// //             gpsClockToggle = false;
// //         } else {
// //             GPSClock.SetTime(ara::tsync::Timestamp(std::chrono::seconds(123456)));
// //             gpsClockToggle = true;
// //         }

// //         logger.LogInfo() << "GPS clock:" << GPSClock.GetCurrentTime().time_since_epoch().count();
// //         logger.LogInfo() << "GPS clock toggled";

// //         std::this_thread::sleep_for(std::chrono::milliseconds(10000));
// //     }

// //     logger.LogInfo() << "TimeSyncProvider has exited its main loop, about to terminate";

// //     if (!ara::core::Deinitialize()) {
// //         // No interaction with ARA is possible here since some ARA resources can be destroyed already
// //         return EXIT_FAILURE;
// //     }

// //     return EXIT_SUCCESS;
// // }

#include "ara/tsync/synchronized_time_base_provider.h"
#include "ara/tsync/timestamp.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/initialization.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"

#include <chrono>
#include <cstdlib>
#include <csignal>
#include <atomic>
#include <thread>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>

// PHC
// #include <linux/ptp_clock.h>
// #include <sys/ioctl.h>
// #include <fcntl.h>
// #include <unistd.h>

// #ifndef CLOCKFD
// #    define CLOCKFD 3
// #endif
// #ifndef FD_TO_CLOCKID
// #    define FD_TO_CLOCKID(fd) ((clockid_t)((((unsigned long)~(fd)) << 3) | CLOCKFD))
// #endif

namespace
{
std::atomic_bool g_continue{true};

void SigTermHandler(int sig)
{
    if (sig == SIGTERM)
        g_continue = false;
}

bool RegisterSigTermHandler()
{
    struct sigaction sa;
    sa.sa_handler = SigTermHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    return (sigaction(SIGTERM, &sa, nullptr) != -1);
}

std::string ToLocalTimeString(std::chrono::nanoseconds ns)
{
    using namespace std::chrono;
    const auto secs = duration_cast<seconds>(ns);
    std::time_t t = secs.count();

    const char* cur_tz = std::getenv("TZ");
    if (!cur_tz || std::strlen(cur_tz) == 0) {
        setenv("TZ", "Asia/Seoul", 1);
        tzset();
    }

    std::tm tm{};
#if defined(__unix__)
    localtime_r(&t, &tm);
#else
    tm = *std::localtime(&t);
#endif

    char buf[40];
    std::snprintf(buf,
                  sizeof(buf),
                  "%04d-%02d-%02d.%02d:%02d:%02d",
                  tm.tm_year + 1900,
                  tm.tm_mon + 1,
                  tm.tm_mday,
                  tm.tm_hour,
                  tm.tm_min,
                  tm.tm_sec);

    char tzbuf[8] = {0};
    strftime(tzbuf, sizeof(tzbuf), "%Z", &tm);

    std::string out(buf);
    out += " ";
    out += tzbuf;
    return out;
}

// // /dev/ptpN에서 현재 PHC 시간을 읽어 ns로 반환
// bool ReadPhcNowNs(const char* ptp_dev,
//                   ara::log::Logger& logger,
//                   std::chrono::nanoseconds& out_ns)
// {
//     int fd = open(ptp_dev, O_RDONLY | O_CLOEXEC);
//     if (fd < 0) {
//         logger.LogError() << "[CLK Provider] PHC open failed: " << ptp_dev
//                           << " errno=" << errno << " (" << std::strerror(errno) << ")";
//         return false;
//     }

//     clockid_t clkid = FD_TO_CLOCKID(fd);
//     struct timespec ts{};
//     int rc = clock_gettime(clkid, &ts);
//     int saved_errno = errno;
//     close(fd);

//     if (rc < 0) {
//         logger.LogError() << "[CLK Provider] clock_gettime(PHC) failed on " << ptp_dev
//                           << " errno=" << saved_errno << " (" << std::strerror(saved_errno) << ")";
//         return false;
//     }

//     // PHC epoch == Unix epoch
//     out_ns = std::chrono::seconds(ts.tv_sec) + 
//                 std::chrono::nanoseconds(ts.tv_nsec);
//     return true;
// }

}  // namespace

// int main()
// {
//     using namespace std::chrono;

//     if (!ara::core::Initialize()) {
//         return EXIT_FAILURE;
//     }

//     ara::exec::ExecutionClient exec_client;
//     exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

//     auto& logger = ara::log::CreateLogger(
//         "TSP1",
//         "TimeSyncProvider (PHC → TSYNC)",
//         ara::log::LogLevel::kVerbose);

//     if (!RegisterSigTermHandler()) {
//         logger.LogError() << "[CLK Provider] Unable to register SIGTERM handler";
//     }

//     // TSYNC 시간 베이스 제공자 ("PHC" logical time base)
//     ara::tsync::SynchronizedTimeBaseProvider PHCClock(
//         ara::core::InstanceSpecifier("PHC"));

//     // PHC 디바이스 선택: 환경변수 PTP_DEVICE 없으면 /dev/ptp3
//     const char* ptp_dev = std::getenv("PTP_DEVICE");
//     if (!ptp_dev || std::strlen(ptp_dev) == 0) {
//         ptp_dev = "/dev/ptp3";   // eth0 PHC 예시
//     }
//     logger.LogInfo() << "[CLK Provider] Using PHC device: " << ptp_dev;
//     logger.LogInfo() << "[CLK Provider] Mode: PHC → TSYNC";

//     const auto period = milliseconds(20);  // 50 Hz 업데이트

//     while (g_continue) {
//         std::chrono::nanoseconds phc_ns;

//         if (ReadPhcNowNs(ptp_dev, logger, phc_ns)) {
//             // 1) PHC 시간을 TSYNC logical time base 로 공급
//             auto res = PHCClock.SetTime(ara::tsync::Timestamp(phc_ns));
//             if (!res.HasValue()) {
//                 logger.LogError()
//                     << "[CLK Provider] SetTime(PHC) failed for TSYNC";
//             }

//             logger.LogInfo()
//                 << "[CLK Provider] PHC raw = "
//                 << phc_ns.count() << " ns"
//                 << " (" << ToLocalTimeString(phc_ns) << ")";
//         } else {
//             logger.LogError()
//                 << "[CLK Provider] Failed to read PHC: " << ptp_dev
//                 << " (keeping previous TSYNC clock value)";
//         }

//         // 2) 현재 TSYNC PHCClock 값 확인 (optional debug)
//         auto cur_tp = PHCClock.GetCurrentTime();
//         auto cur_ns = duration_cast<nanoseconds>(cur_tp.time_since_epoch());
//         logger.LogInfo()
//             << "[CLK Provider] TSYNC(PHC) clock = "
//             << cur_ns.count() << " ns"
//             << " (" << ToLocalTimeString(cur_ns) << ")";

//         std::this_thread::sleep_for(period);
//     }

//     logger.LogInfo()
//         << "[CLK Provider] TimeSyncProvider main loop exited; terminating";

//     if (!ara::core::Deinitialize()) {
//         return EXIT_FAILURE;
//     }

//     return EXIT_SUCCESS;
// }


int main()
{
    using namespace std::chrono;

    if (!ara::core::Initialize()) {
        return EXIT_FAILURE;
    }

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    auto& logger
        = ara::log::CreateLogger("TSP1", "TimeSyncProvider ", ara::log::LogLevel::kVerbose);

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "[CLK Provider] Unable to register SIGTERM handler";
    }

    // Provider라는 이름의 TSYNC 시간베이스를 "system clock"으로 제공
    ara::tsync::SynchronizedTimeBaseProvider Provider(ara::core::InstanceSpecifier("PHC"));

    const auto period = milliseconds(10);

    while (g_continue) {
        // 1) system clock 현재 시각(ns since Unix epoch)
        auto sys_now_ns =
            duration_cast<nanoseconds>(system_clock::now().time_since_epoch());

        // 2) TSYNC(=Provider)에 system clock 값을 그대로 세팅
        auto res = Provider.SetTime(ara::tsync::Timestamp(sys_now_ns));
        if (!res.HasValue()) {
            logger.LogError() << "[CLK Provider] SetTime() failed";
        }

        // 3) 방금 SetTime 한 system clock 값만 로그로 출력
        logger.LogInfo()
            << "[CLK Provider] SYS clock set to TSYNC: "
            << sys_now_ns.count() << " ns"
            << " (" << ToLocalTimeString(sys_now_ns) << ")";

        std::this_thread::sleep_for(period);
    }

    logger.LogInfo() << "[CLK Provider] TimeSyncProvider main loop exited; terminating";

    if (!ara::core::Deinitialize()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}