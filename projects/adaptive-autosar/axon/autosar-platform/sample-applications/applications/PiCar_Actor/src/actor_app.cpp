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

// ara
#include "ara/com/instance_identifier.h"

// update request
#include "actor_service.h"

#include "actor_app.h"

namespace apd
{
namespace picar
{
namespace actor
{

int ActorApp::selfpipe_[2] = {0, 0};
std::atomic_bool ActorApp::exitRequested_(false);

const ara::core::InstanceSpecifier URportSpecifier{"actor/ActorRoot/ActorPPort"};

void ActorApp::SigTermHandler(int sig)
{
    if (sig == SIGTERM) {
        // set atomic exit flag
        exitRequested_ = true;
        // write to self pipe
        auto ignored = write(selfpipe_[PipeWritingEnd_], "\0", 1);
        (void)ignored;
    }
}

int ActorApp::Execute()
{
    if (Init()) {
        log_.LogError() << "Execute called";
        Run();

        Terminate();
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
bool ActorApp::Init()
{
    ara::com::InstanceIdentifierContainer URinstanceIDs = ara::com::runtime::ResolveInstanceIDs(URportSpecifier);
    if (URinstanceIDs.empty()) {
        log_.LogError() << "No InstanceIdentifiers resolved from provided InstanceSpecifier";
        return false;
    }

    // Create a service
    actorService_ = std::make_unique<ActorService>(URinstanceIDs[0]);

    if (!actorService_) {
        log_.LogError() << "Failed to connect to the Actor ServiceInterface";
        return false;
    }

    if (!RegisterSigTermHandler()) {
        return false;
    }
    return true;
}

bool ActorApp::RegisterSigTermHandler()
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

void ActorApp::Run()
{
    actorService_->OfferService();
    WaitUntilTermination();
}

void ActorApp::WaitUntilTermination()
{
    // self pipe trick waiting
    while (!exitRequested_) {
        // reading from self pipe
        int buf;
        auto ignored = read(selfpipe_[PipeReadingEnd_], &buf, sizeof(buf));
        (void)ignored;
    }
}

void ActorApp::Terminate()
{
    CloseSelfPipe();
}

void ActorApp::CloseSelfPipe()
{
    close(selfpipe_[PipeReadingEnd_]);
    close(selfpipe_[PipeWritingEnd_]);
}

}  // namespace actor
}  // namespace picar
}  // namespace apd
