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

// std
#include <chrono>
#include <sstream>
#include <cstdlib>
// boost
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/program_options.hpp"
// ara
#include "ara/com/internal/runtime.h"
#include "ara/core/string.h"
#include "ara/exec/execution_client.h"
#include "ara/log/logger.h"
#include "ara/log/ifc/logging.h"
#include "ara/ucm/impl_type_transferidtype.h"
#include "ara/core/initialization.h"
// local
#include "find_service.h"

using ara::ucm::proxy::PackageManagementProxy;
using ara::core::Vector;

constexpr int FAILURE = 1;
constexpr int SUCCESS = 0;

// Function fills provided variables with data loaded from command line.
// In case it failed to do so, it returns false, otherwise it returns true.
static bool readProgramOptions(int ac, char** av, uint32_t& blockSize, uint32_t& totalSize, uint32_t& timeoutInSeconds)
{
    try {

        boost::program_options::options_description desc("Allowed options", 140);
        desc.add_options()  //
            ("help", "Show this help and exit")  //
            ("timeout",
                boost::program_options::value<uint32_t>(&timeoutInSeconds)->default_value(10),
                "The default wait time for the UCM service.")  //
            ("block-size",
                boost::program_options::value<uint32_t>(&blockSize)->default_value(1024 * 1024),
                "Size of single block to be transferred with one TransferData() call.")  //
            ("total-size",
                boost::program_options::value<uint32_t>(&totalSize)->default_value(10 * 1024 * 1024),
                "Total size of a single packet to be transferred");

        boost::program_options::variables_map vm;
        boost::program_options::store(boost::program_options::parse_command_line(ac, av, desc), vm);

        // in case help is requested, it is shown and no values are returned
        if (vm.count("help")) {
            std::cout << desc << "\n";
            return false;
        }

        // update values and inform on success
        boost::program_options::notify(vm);
        return true;

    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "Exception of unknown type!\n";
    }

    return false;  // return no values by default
}

static ara::core::Optional<ara::ucm::TransferIdType> TestTransferStart(ara::log::Logger& log,
    apd::ucm::pkgmgrsample::ServiceProxyPtr& service,
    uint32_t totalSize)
{
    const auto start = std::chrono::high_resolution_clock::now();

    ara::core::Future<ara::ucm::PackageManagement::TransferStartOutput> transferStartFuture
        = service->TransferStart(totalSize);
    const auto transferStartResult = transferStartFuture.GetResult();
    const auto finish = std::chrono::high_resolution_clock::now();

    if (!transferStartResult) {
        log.LogError() << "TransferStart failed:" << transferStartResult.Error().Message();

        return {};
    }

    const ara::ucm::TransferIdType transferId = transferStartResult.Value().id;

    log.LogInfo() << "TransferStart time:" << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
                  << "Created transferId" << transferId << "to send" << totalSize << "bytes";

    return {transferId};
}

static void TestTransferData(ara::log::Logger& log,
    apd::ucm::pkgmgrsample::ServiceProxyPtr& service,
    const ara::core::Vector<uint8_t>& blockToSend,
    ara::ucm::TransferIdType transferId,
    uint32_t totalSize,
    uint32_t blockSize)
{
    const auto start = std::chrono::high_resolution_clock::now();

    for (uint32_t counter = 1; counter <= totalSize / blockSize; counter++) {
        auto transferDataFuture = service->TransferData(transferId, blockToSend, counter);
        const auto transferDataResult = transferDataFuture.GetResult();
        if (!transferDataResult) {
            log.LogInfo() << "TransferData failed on block" << counter;
            //     // Uncomment if you want extreme verbosity on for every sent chunk:
            // } else {
            //     log.LogInfo() << "TransferData sent" << blockSize << "bytes in block" << counter;
        }
    }
    if (totalSize % blockSize != 0) {
        // Maybe an unnecessary precision for the send data length, but whatever, let's be correct:
        ara::core::Vector<uint8_t> rest(totalSize % blockSize);
        auto transferDataFuture = service->TransferData(transferId, rest, totalSize / blockSize + 1);
        const auto transferDataResult = transferDataFuture.GetResult();
        if (!transferDataResult) {
            log.LogInfo() << "TransferData failed on block" << totalSize / blockSize + 1;
            //     // Uncomment if you want extreme verbosity on for every sent chunk:
            // } else {
            //     log.LogInfo() << "TransferData sent" << totalSize % blockSize << "in block" << totalSize / blockSize
            //     + 1;
        }
    }

    const auto finish = std::chrono::high_resolution_clock::now();

    std::ostringstream stream;
    stream.precision(4);
    stream << 1000.0 / 1024 * totalSize / 1024
            / std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
    ara::core::String speedStr(stream.str());

    log.LogInfo() << "TransferData time:" << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
                  << "at" << speedStr << "MB/s. Sent" << totalSize << "bytes in"
                  << totalSize / blockSize + (totalSize % blockSize == 0 ? 0 : 1) << "blocks";
}

static void TestTransferExit(ara::log::Logger& log,
    apd::ucm::pkgmgrsample::ServiceProxyPtr& service,
    ara::ucm::TransferIdType transferId)
{
    const auto start = std::chrono::high_resolution_clock::now();

    const auto transferExitResult = service->TransferExit(transferId).GetResult();

    const auto finish = std::chrono::high_resolution_clock::now();

    if (!transferExitResult) {
        log.LogError() << "TransferExit time:" << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
                       << "for transferId" << transferId << "with error:" << transferExitResult.Error().Message();
    } else {
        log.LogInfo() << "TransferExit time:" << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start)
                      << "for transferId" << transferId;
    }
}

static void TestDeleteTransfer(ara::log::Logger& log,
    apd::ucm::pkgmgrsample::ServiceProxyPtr& service,
    ara::ucm::TransferIdType transferId)
{
    const auto start = std::chrono::high_resolution_clock::now();

    ara::core::Future<ara::ucm::PackageManagement::GetSwPackagesOutput> getSwPackagesFuture = service->GetSwPackages();
    const ara::ucm::PackageManagement::GetSwPackagesOutput getSwPackagesOutput = getSwPackagesFuture.get();

    ara::core::Future<void> deleteTransferFuture = service->DeleteTransfer(transferId);
    const auto result = deleteTransferFuture.GetResult();
    if (!result) {
        log.LogError() << "DeleteTransfer of transferId" << transferId << ":" << result.Error().Message();
        // ignore the error
    }

    const auto finish = std::chrono::high_resolution_clock::now();

    log.LogInfo() << "DeleteTransfer for transferId" << transferId
                  << "time:" << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
}

int main(int ac, char** av)
{
    if (!ara::core::Initialize()) {
        std::cerr << "core initialization failed" << std::endl;
        return EXIT_FAILURE;
    }

    uint32_t totalSize, blockSize, timeoutInSeconds;

    int mainResult = FAILURE;
    if (!readProgramOptions(ac, av, blockSize, totalSize, timeoutInSeconds)) {
        return mainResult;
    }

    ara::core::Vector<uint8_t> blockToSend(blockSize);
    for (uint32_t i = 0; i < blockSize; i++) {
        blockToSend[i] = rand();
    }

    // report application state
    ara::exec::ExecutionClient exec_client;
    exec_client.ReportExecutionState(ara::exec::ExecutionState::kRunning);

    auto& log = ara::log::CreateLogger("MAIN", "MAIN context", ara::log::LogLevel::kVerbose);
    log.LogInfo() << "Starting Transfer Test Application";

    auto foundService = apd::ucm::pkgmgrsample::FindPackageManagementService(2);

    if (!foundService.has_value()) {
        log.LogError() << "Transfer Test Application did not find the PackageManagement service";
        return FAILURE;
    }
    apd::ucm::pkgmgrsample::ServiceProxyPtr service = *foundService;

    const auto start = std::chrono::high_resolution_clock::now();

    const auto transferIdOptional = TestTransferStart(log, service, totalSize);
    if (transferIdOptional) {
        const auto transferId = *transferIdOptional;
        TestTransferData(log, service, blockToSend, transferId, totalSize, blockSize);
        TestTransferExit(log, service, transferId);
        TestDeleteTransfer(log, service, transferId);

        const auto finish = std::chrono::high_resolution_clock::now();

        log.LogInfo() << "Total time:" << std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
        mainResult = SUCCESS;
    }

    if (!ara::core::Deinitialize()) {
        std::cerr << "core deinitialization failed" << std::endl;
        return EXIT_FAILURE;
    }

    return mainResult;
}
