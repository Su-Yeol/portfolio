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

#include "calculator.h"

#include <csignal>
#include <unistd.h>
#include <arpa/inet.h>

#include "calculator_error_code.h"
#include "overflow_addition_exception.h"
#include "underflow_addition_exception.h"

namespace ara
{
namespace diag
{
namespace sample_application
{
namespace calculator
{

Calculator::Calculator()
    : input_1_(0)
    , input_2_(0)
    , output_(0)
    , service_input_(ara::com::InstanceIdentifier("SOME/IP:1402") /* instanceId */)
    , service_output_(ara::com::InstanceIdentifier("SOME/IP:1402") /* instanceId */)
    , service_method_(ara::com::InstanceIdentifier("SOME/IP:1402") /* instanceId */, this)
    , service_method_arg_(ara::com::InstanceIdentifier("SOME/IP:1401") /* instanceId */, this)
    , log_(ara::log::CreateLogger("CALX", "calculator context", ara::log::LogLevel::kVerbose))
{ }

Calculator::~Calculator()
{ }

void Calculator::Main()
{
    // Setup the modules
    Initialize();

    // Run, if we weren't asked to quit in the meantime
    if (!exit_requested_) {
        Run();
    } else {
        log_.LogInfo() << "Skipping Run()";
    }

    // Wait until a shutdown is signaled
    {
        std::unique_lock<std::mutex> shutdown_lock{shutdown_mutex_};
        while (!exit_requested_) {
            shutdown_condvar_.wait(shutdown_lock);
        }
    }

    // Perform the shutdown
    Shutdown();
}

Calculator::Calculator_IO_Datatype Calculator::CheckedAdd(Calculator::Calculator_IO_Datatype a,
    Calculator::Calculator_IO_Datatype b)
{
    if (a > 0 && b > std::numeric_limits<Calculator_IO_Datatype>::max() - a) {
        throw OverflowAdditionException(a, b);
    }
    if (a < 0 && b < std::numeric_limits<Calculator_IO_Datatype>::min() - a) {
        throw UnderflowAdditionException(a, b);
    }
    return a + b;
}

Calculator::Calculator_IO_Datatype Calculator::Add()
{
    return AddArg(0);
}

Calculator::Calculator_IO_Datatype Calculator::AddArg(const Calculator::Calculator_IO_Datatype& arg)
{
    output_ = CheckedAdd(CheckedAdd(input_1_, input_2_), arg);
    return output_;
}

void Calculator::Initialize()
{
    sigset_t signals;
    sigfillset(&signals);
    pthread_sigmask(SIG_SETMASK, &signals, NULL);

    // set handler functions
    service_input_.input1.RegisterGetHandler(std::bind(&Calculator::HandleInput1Get, this));
    service_input_.input2.RegisterGetHandler(std::bind(&Calculator::HandleInput2Get, this));
    service_output_.OutputValue.RegisterGetHandler(std::bind(&Calculator::HandleOutputGet, this));

    service_input_.input1.RegisterSetHandler(std::bind(&Calculator::SetInput1, this, std::placeholders::_1));
    service_input_.input2.RegisterSetHandler(std::bind(&Calculator::SetInput2, this, std::placeholders::_1));

    // Initialize fields
    service_input_.input1.Update(0);
    service_input_.input2.Update(0);
    service_output_.OutputValue.Update(0);

    // Offer services
    service_input_.OfferService();
    service_output_.OfferService();
    service_method_.OfferService();
    service_method_arg_.OfferService();
}

void Calculator::Run()
{
    while (!exit_requested_) {
        log_.LogVerbose() << "DiagnosticCalculator: input_1_: " << input_1_ << " input_2_: " << input_2_
                          << " output_: " << output_;
        sleep(5);
    }
}

void Calculator::Shutdown()
{
    service_input_.StopOfferService();
    service_output_.StopOfferService();
    service_method_.StopOfferService();
    service_method_arg_.StopOfferService();
}
void Calculator::SignalHandler(int /*signum*/)
{ }
void Calculator::HandleSignal(int signum)
{
    if (SIGTERM == signum) {
        log_.LogInfo() << "Caught SIGTERM!";
        exit_requested_ = true;
    }
}

}  // namespace calculator
}  // namespace sample_application
}  // namespace diag
}  // namespace ara
