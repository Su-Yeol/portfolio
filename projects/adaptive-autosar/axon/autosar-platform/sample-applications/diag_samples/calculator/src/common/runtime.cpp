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

#include "runtime.h"

#include <unistd.h>
#include <csignal>
#include <exception>
#include <iostream>
#include <memory>
#include <thread>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>  // based on boost v1.58.0

#include "exception/errno_exception.h"

namespace ara
{
namespace diag
{

using namespace ara::log;

std::mutex Runtime::instance_mutex_;
std::unique_ptr<Runtime> Runtime::instance_{nullptr};

Runtime::Runtime()
    : log_(CreateLogger("DM", "DM context"))
{
    if (-1 == pipe(selfpipe_)) {
        throw ara::diag::exception::ErrnoException(errno);
    }
}

Runtime::~Runtime()
{
    // Stop signal handling
    signal(SIGTERM, SIG_DFL);

    close(selfpipe_[0]);
    close(selfpipe_[1]);
}

void Runtime::HandleSignal(int signum)
{
    if (SIGTERM == signum) {
        log_.LogInfo() << "Caught SIGTERM!";
        exit_requested_ = true;
        if (-1 == write(selfpipe_[1], "\0", 1)) {
            throw ara::diag::exception::ErrnoException(errno);
        }
    }
}

void Runtime::SignalHandler(int signum)
{
    Runtime::GetInstance().HandleSignal(signum);
}

void Runtime::WaitUntilTermination()
{
    while (!exit_requested_) {
        int buffer;
        // Blocking read on selfpipe_
        ssize_t bytesRead(read(selfpipe_[0], &buffer, sizeof(int)));
        if (bytesRead > 0) {
            // selfpipe_ is only used to signal an exit request.
            exit_requested_ = true;
        } else {
            throw ara::diag::exception::ErrnoException(errno);
        }
    }
}

Runtime& Runtime::GetInstance()
{
    return *instance_.get();
}

boost::property_tree::ptree Runtime::loadConfiguration(const char* PathToConfigFile)
{
    boost::property_tree::ptree config;

    boost::property_tree::read_json(PathToConfigFile, config, std::locale());

    return config;
}

} /* namespace diag */
} /* namespace ara */
