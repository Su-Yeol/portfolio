// // --------------------------------------------------------------------------
// // |              _    _ _______     .----.      _____         _____        |
// // |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
// // |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
// // |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
// // |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
// // |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
// // |                              . _ _  .                                  |
// // --------------------------------------------------------------------------
// //
// // All Rights Reserved.
// // Any use of this source code is subject to a license agreement with the
// // AUTOSAR development cooperation.
// // More information is available at www.autosar.org.
// //
// // Disclaimer
// //
// // This work (specification and/or software implementation) and the material
// // contained in it, as released by AUTOSAR, is for the purpose of information
// // only. AUTOSAR and the companies that have contributed to it shall not be
// // liable for any use of the work.
// //
// // The material contained in this work is protected by copyright and other
// // types of intellectual property rights. The commercial exploitation of the
// // material contained in this work requires a license to such intellectual
// // property rights.
// //
// // This work may be utilized or reproduced without any modification, in any
// // form or by any means, for informational purposes only. For any other
// // purpose, no part of the work may be utilized or reproduced, in any form
// // or by any means, without permission in writing from the publisher.
// //
// // The work has been developed for automotive applications only. It has
// // neither been developed, nor tested for non-automotive applications.
// //
// // The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// // --------------------------------------------------------------------------

// #include "ara/tsync/synchronized_time_base_consumer.h"
// #include "ara/tsync/timestamp.h"
// #include "ara/tsync/synchronized_time_base_status.h"
// #include "ara/core/instance_specifier.h"
// #include "ara/core/initialization.h"
// #include "ara/exec/execution_client.h"
// #include "ara/log/logger.h"
// #include <chrono>
// #include <cstdlib>
// #include <csignal>
// #include <atomic>
// #include <thread>

// namespace
// {
// // Atomic flag for exit after SIGTERM caught
// std::atomic_bool continueExecution{true};

// void SigTermHandler(int signal)
// {
//     if (signal == SIGTERM) {
//         // Set atomic exit flag
//         continueExecution = false;
//     }
// }

// bool RegisterSigTermHandler()
// {
//     struct sigaction sa;
//     sa.sa_handler = SigTermHandler;
//     sa.sa_flags = 0;
//     sigemptyset(&sa.sa_mask);
//     // Register signal handler
//     if (sigaction(SIGTERM, &sa, nullptr) == -1) {
//         // Could not register a SIGTERM signal handler
//         return false;
//     }
//     return true;
// }
// }  // namespace

// int main()
// {
//     if (!ara::core::Initialize()) {
//         // No interaction with ARA is possible here since initialization failed
//         return EXIT_FAILURE;
//     }

//     ara::exec::ExecutionClient exec_client;
//     exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

//     auto& logger = ara::log::CreateLogger("TSC1", "TimeSyncConsumer Context 1", ara::log::LogLevel::kVerbose);

//     if (!RegisterSigTermHandler()) {
//         logger.LogError() << "Unable to register signal handler";
//     }

//     // Register a Synchronized Slave Time Base with ARA::TSYNC
//     ara::tsync::SynchronizedTimeBaseConsumer GPSClock(ara::core::InstanceSpecifier("GPS"));

//     // Get Time With Status
//     ara::tsync::SynchronizedTimeBaseStatus GPSClockSyncStatus = GPSClock.GetTimeWithStatus();
//     logger.LogInfo() << "GPS clock Sync Status:" << unsigned(GPSClockSyncStatus.GetSynchronizationStatus());
//     logger.LogInfo() << "GPS clock Sync Status Creation Time:"
//                      << GPSClockSyncStatus.GetCreationTime().time_since_epoch().count();

//     // Display the GPS clock value every second
//     while (continueExecution) {
//         logger.LogInfo() << "TimeSyncConsumer is alive";
//         logger.LogInfo() << "GPS clock:" << GPSClock.GetCurrentTime().time_since_epoch().count();
//         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     }

//     logger.LogInfo() << "TimeSyncConsumer has exited its main loop, about to terminate";

//     if (!ara::core::Deinitialize()) {
//         // No interaction with ARA is possible here since some ARA resources can be destroyed already
//         return EXIT_FAILURE;
//     }

//     return EXIT_SUCCESS;
// }

#include "ara/tsync/synchronized_time_base_consumer.h"
#include "ara/tsync/synchronized_time_base_status.h"
#include "ara/tsync/timestamp.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/initialization.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <ctime>
#include <string>
#include <cstdint>
#include <cmath>

// PHC
#include <linux/ptp_clock.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#ifndef CLOCKFD
#    define CLOCKFD 3
#endif

#ifndef FD_TO_CLOCKID
#    define FD_TO_CLOCKID(fd) ((clockid_t)((((unsigned long)~(fd)) << 3) | CLOCKFD))
#endif

namespace
{
using namespace std::chrono;

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

    char ts[64]{};
    strftime(ts, sizeof(ts), "%Y-%m-%d.%H:%M:%S %Z", &tm);
    return std::string(ts);
}

// APD 24-11: 실제 헤더에 정의된 값만 사용 (kSynchronized)
const char* SyncStatusToStr(ara::tsync::SynchronizationStatus s)
{
    using SS = ara::tsync::SynchronizationStatus;
    if (s == SS::kSynchronized)
        return "SYNCHRONIZED";
    return "OTHER";
}

struct JitterStats
{
    std::uint64_t n{0};
    long double mean{0.0L};
    long double M2{0.0L};  // 분산 계산용 누적 값 (Welford)
    std::int64_t maxAbs{0};  // 최대 절대값 (worst-case jitter)

    long double compensated_mean = 0.0L;
    std::uint64_t compensated_count = 0;

    void AddSample(std::int64_t x_ns)
    {
        ++n;
        long double x = static_cast<long double>(x_ns);

        long double delta = x - mean;
        mean += delta / static_cast<long double>(n);
        long double delta2 = x - mean;
        M2 += delta * delta2;

        auto absx = (x_ns >= 0) ? x_ns : -x_ns;
        if (absx > maxAbs) {
            maxAbs = absx;
        }
    }

    bool Ready(std::uint64_t minSamples) const
    {
        return n >= minSamples;
    }

    void LogAndReset(ara::log::Logger& logger, const char* tag)
    {
        if (n < 2) {
            return;
        }

        long double var = M2 / static_cast<long double>(n - 1);
        long double stddev = std::sqrt(var);
        long double rms = std::sqrt(var + mean * mean);

        logger.LogInfo() << "[CLK Consumer][JITTER][" << tag << "] "
                         << "samples=" << n << ", mean=" << static_cast<std::int64_t>(std::llround(mean)) << " ns"
                         << ", stddev=" << static_cast<std::int64_t>(std::llround(stddev)) << " ns"
                         << ", rms=" << static_cast<std::int64_t>(std::llround(rms)) << " ns"
                         << ", max_abs=" << maxAbs << " ns";

        // 다음 윈도우를 위해 초기화
        n = 0;
        mean = 0.0L;
        M2 = 0.0L;
        maxAbs = 0;
    }
};

}  // namespace

int main()
{
    using namespace std::chrono;

    if (!ara::core::Initialize()) {
        return EXIT_FAILURE;
    }

    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    auto& logger = ara::log::CreateLogger(
        "TSC1", "TimeSyncConsumer (PHC TSYNC vs SYS, with jitter stats)", ara::log::LogLevel::kVerbose);

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "[CLK Consumer] Unable to register SIGTERM handler";
    }

    // TSYNC logical time base (예: "PHC" 글로벌 시간)
    ara::tsync::SynchronizedTimeBaseConsumer Consumer(ara::core::InstanceSpecifier("PHC"));
    {
        auto st = Consumer.GetTimeWithStatus();
        logger.LogInfo() << "[CLK Consumer] Initial SyncStatus=" << unsigned(st.GetSynchronizationStatus()) << " ("
                         << SyncStatusToStr(st.GetSynchronizationStatus()) << ")"
                         << ", created_at_ns=" << st.GetCreationTime().time_since_epoch().count();
    }

    const char* ptp_dev = "/dev/ptp3";
    int phc_fd = open(ptp_dev, O_RDONLY | O_CLOEXEC);
    if (phc_fd < 0) {
        logger.LogError() << "[CLK Consumer] Failed to open PHC device: " << ptp_dev;
        ara::core::Deinitialize();
        return EXIT_FAILURE;
    }

    clockid_t phc_clkid = FD_TO_CLOCKID(phc_fd);
    logger.LogInfo() << "[CLK Consumer] Using PHC device: " << ptp_dev;

    // phc2sys 의 offset 통계와 동일한 개념으로 사용
    JitterStats jitter_raw;

    // phc2sys 도 stats_max_count 로 window 크기를 설정함
    const std::uint64_t JITTER_WINDOW_SAMPLES = 1000;  // 필요하면 줄여도 됨

    while (g_continue) {
        // 1) TSYNC(PHC) 현재 시각 (Adaptive TSYNC 글로벌 시간)
        auto st = Consumer.GetTimeWithStatus();
        auto tsync_now_tp = Consumer.GetCurrentTime();
        auto tsync_now_ns = duration_cast<nanoseconds>(tsync_now_tp.time_since_epoch());

        // 2) SYS CLOCK NOW
        auto sys_now_ns = duration_cast<nanoseconds>(system_clock::now().time_since_epoch());

        // 3) RAW offset (TSYNC - SYS)
        auto diff_vs_sys = tsync_now_ns - sys_now_ns;
        std::int64_t diff_ns = diff_vs_sys.count();

        // jitter update
        jitter_raw.AddSample(diff_ns);

        long double cur_mean_ld = jitter_raw.mean;
        std::int64_t cur_mean_ns = static_cast<std::int64_t>(std::llround(cur_mean_ld));

        std::int64_t compensated_ns = diff_ns - cur_mean_ns;

        jitter_raw.compensated_count++;
        jitter_raw.compensated_mean
            += (static_cast<long double>(compensated_ns) - jitter_raw.compensated_mean) / jitter_raw.compensated_count;

        std::int64_t compensated_mean_ns = static_cast<std::int64_t>(std::llround(jitter_raw.compensated_mean));

        if (jitter_raw.Ready(JITTER_WINDOW_SAMPLES)) {
            jitter_raw.LogAndReset(logger, "RAW");
        }

        logger.LogInfo() << "[CLK Consumer] diff(TSYNC - SYS) = " << diff_ns << " ns (comp=" << compensated_ns << " ns)"
                         << " | avg=" << compensated_mean_ns << " ns"
                         << " | samples=" << jitter_raw.compensated_count
                         << " | status=" << unsigned(st.GetSynchronizationStatus()) << " ("
                         << SyncStatusToStr(st.GetSynchronizationStatus()) << ")";

        if (st.GetSynchronizationStatus() != ara::tsync::SynchronizationStatus::kSynchronized) {
            logger.LogWarn() << "[CLK Consumer] TSYNC not synchronized (status="
                             << unsigned(st.GetSynchronizationStatus()) << ")";
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    close(phc_fd);

    logger.LogInfo() << "[CLK Consumer] TimeSyncConsumer main loop exited; terminating";

    if (!ara::core::Deinitialize()) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
