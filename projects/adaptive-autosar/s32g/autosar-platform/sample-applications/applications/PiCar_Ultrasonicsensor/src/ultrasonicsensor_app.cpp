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
#include <atomic>
#include <csignal>
#include <cstdint>
#include <unistd.h>
#include <thread>

// ara
#include "ara/com/instance_identifier.h"

// update request
#include "ultrasonicsensor_service.h"

#include "ultrasonicsensor_app.h"

#ifdef PICAR
#    include "ultrasonic_sensor.h"
#endif

namespace apd
{
namespace picar
{
namespace ultrasonicsensor
{

int UltraSonicSensorApp::selfpipe_[2] = {0, 0};
std::atomic_bool UltraSonicSensorApp::exitRequested_(false);

const ara::core::InstanceSpecifier URportSpecifier{"ultrasonicSensor/UltrasonicSensorRoot/UltraSonicSensorPPort"};

void UltraSonicSensorApp::SigTermHandler(int sig)
{
    if (sig == SIGTERM) {
        // set atomic exit flag
        exitRequested_ = true;
        // write to self pipe
        auto ignored = write(selfpipe_[PipeWritingEnd_], "\0", 1);
        (void)ignored;
    }
}

int UltraSonicSensorApp::Execute()
{
    if (Init()) {
        log_.LogError() << "Execute called";
        Run();

        Terminate();
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
bool UltraSonicSensorApp::Init()
{
    log_.LogError() << "Init() called";
    ara::com::InstanceIdentifierContainer URinstanceIDs = ara::com::runtime::ResolveInstanceIDs(URportSpecifier);
    if (URinstanceIDs.empty()) {
        log_.LogError() << "No InstanceIdentifiers resolved from provided InstanceSpecifier";
        return false;
    }

#ifdef PICAR
    sensor_ = UltrasonicSensor::CreateSensor();
#endif

    // Create a service
    ultraSonicSensorService_ = std::make_unique<UltraSonicSensorService>(URinstanceIDs[0]);

    if (!ultraSonicSensorService_) {
        log_.LogError() << "Failed to connect to the Actor ServiceInterface";
        return false;
    }

    if (!RegisterSigTermHandler()) {
        return false;
    }
    return true;
}

bool UltraSonicSensorApp::RegisterSigTermHandler()
{
    // Creating a pipe
    if (pipe(selfpipe_) == -1) {
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

void UltraSonicSensorApp::Run()
{
    long distance;
    ultraSonicSensorService_->OfferService();
    log_.LogInfo() << "UltraSonicSensor Service Offered.";

    while (!exitRequested_) {
#ifndef PICAR
        {
            // emulate an object, counts 100 to 1 and then repeats
            using namespace std::chrono;
            distance = 100 - time_point_cast<seconds>(steady_clock::now()).time_since_epoch().count() % 100;
        }
#else
        distance = sensor_->GetDistance();
#endif
        log_.LogInfo() << std::string("Distance: ") + std::to_string(distance) + ".";
        auto send_result = ultraSonicSensorService_->ultrasonicSensorEvent.Send(distance);
        if (!send_result) {
            log_.LogError() << "Ultrasonic Event Send failed with error" << send_result.Error();
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

void UltraSonicSensorApp::WaitUntilTermination()
{
    // self pipe trick waiting
    while (!exitRequested_) {
        // reading from self pipe
        int buf;
        auto ignored = read(selfpipe_[PipeReadingEnd_], &buf, sizeof(buf));
        (void)ignored;
    }
}

void UltraSonicSensorApp::Terminate()
{
    CloseSelfPipe();
}

void UltraSonicSensorApp::CloseSelfPipe()
{
    close(selfpipe_[PipeReadingEnd_]);
    close(selfpipe_[PipeWritingEnd_]);
}

}  // namespace ultrasonicsensor
}  // namespace picar
}  // namespace apd
