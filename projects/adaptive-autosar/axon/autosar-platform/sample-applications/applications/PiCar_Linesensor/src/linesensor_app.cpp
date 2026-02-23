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

#include <csignal>
#include <cstdint>
#include <unistd.h>
#include <thread>

// ara
#include "ara/com/instance_identifier.h"

// update request
#include "linesensor_service.h"

#include "linesensor_app.h"

namespace apd
{
namespace picar
{
namespace linesensor
{

int LineSensorApp::selfpipe[2] = {0, 0};

std::atomic_bool LineSensorApp::exitRequested_(false);

const ara::core::InstanceSpecifier URportSpecifier{"linesensor/LineSensorRoot/LineSensorPPort"};

void LineSensorApp::SigTermHandler(int sig)
{
    if (sig == SIGTERM) {
        // set atomic exit flag
        exitRequested_ = true;
        // write to self pipe
        auto ignored = write(selfpipe[PipeWritingEnd], "\0", 1);
        (void)ignored;
    }
}

int LineSensorApp::Execute()
{
    if (Init()) {
        Run();

        Terminate();
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

bool LineSensorApp::Init()
{
    ara::com::InstanceIdentifierContainer URinstanceIDs = ara::com::runtime::ResolveInstanceIDs(URportSpecifier);
    if (URinstanceIDs.empty()) {
        log_.LogError() << "No InstanceIdentifiers resolved from provided InstanceSpecifier";
        return false;
    }

    // Create a service
    lineSensorService_ = std::make_unique<LineSensorService>(URinstanceIDs[0]);

    if (!lineSensorService_) {
        log_.LogError() << "Failed to create ServiceInterface";
        return false;
    }

    if (!RegisterSigTermHandler()) {
        return false;
    }
    return true;
}

bool LineSensorApp::RegisterSigTermHandler()
{
    // Creating a pipe
    if (pipe(selfpipe) == -1) {
        // Could not create a pipe for performing a classic self pipe trick
        log_.LogError() << "Execution stopped. Unable to create a pipe";
        return false;
    }
    struct sigaction sa;
    sa.sa_handler = SigTermHandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    // register signal handler
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        // Could not register a SIGTERM signal handler
        log_.LogError() << "Execution stopped. Unable to register signal handler";
        CloseSelfPipe();
        return false;
    }
    return true;
}

void LineSensorApp::Run()
{
    lineSensorService_->OfferService();

    while (!exitRequested_) {
        auto sensorValue = sensor_.ReadAnalog();

        auto allocationResult = lineSensorService_->lineSensorEvent.Allocate();
        if (!allocationResult) {
            log_.LogError() << "Allocation failed: " << allocationResult.Error();
        } else {
            auto lineSample = std::move(allocationResult).Value();
            for (int i = 0; i < 5; i++) {
                lineSample->objectVector.push_back(sensorValue[i]);
            }
            auto sendResult = lineSensorService_->lineSensorEvent.Send(std::move(lineSample));
            if (!sendResult) {
                log_.LogError() << "LineSensor Event Send failed with error" << sendResult.Error();
            }
            log_.LogDebug() << "Line data " << sensorValue.at(0) << ", " << sensorValue.at(1) << ", "
                            << sensorValue.at(2) << ", " << sensorValue.at(3) << ", " << sensorValue.at(4);
        }
#ifdef PICAR
        std::this_thread::sleep_for(std::chrono::milliseconds(25));
#else
        // because of ITM, we need to reduce the update rate
        // otherwise we generate too many log messages (e2e failures)
        // and ITM will fail to execute within the timeout
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
#endif
    }
}

void LineSensorApp::WaitUntilTermination()
{
    // self pipe trick waiting
    while (!exitRequested_) {
        // reading from self pipe
        int buf;
        auto ignored = read(selfpipe[PipeReadingEnd_], &buf, sizeof(buf));
        (void)ignored;
    }
}

void LineSensorApp::Terminate()
{
    CloseSelfPipe();
}

void LineSensorApp::CloseSelfPipe()
{
    close(selfpipe[PipeReadingEnd_]);
    close(selfpipe[PipeWritingEnd]);
}

}  // namespace linesensor
}  // namespace picar
}  // namespace apd
