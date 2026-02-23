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

#ifndef SAMPLE_CALCULATOR_SOURCE_CALCULATOR_H_
#define SAMPLE_CALCULATOR_SOURCE_CALCULATOR_H_

#include <ara/log/logger.h>

#include "add_interface_impl.h"
#include "diag_add_arg_interface_impl.h"
#include "application.h"
#include "ara/diag/example/inputvalueinterface_skeleton.h"
#include "ara/diag/example/outputvalueinterface_skeleton.h"

#include "ara/core/future.h"
#include "ara/core/promise.h"

inline ara::log::Logger& logger()
{
    static ara::log::Logger& loggerInstance
        = ara::log::CreateLogger("CALC", "Calculator: Diagnostic example application");
    return loggerInstance;
}

namespace ara
{
namespace diag
{
namespace sample_application
{
namespace calculator
{

/**
 * \brief An example application class.
 *
 * The objects instance methods are provided via DidHandler and RoutineHandler implementations to
 * the DiagnosticCommunicationManager.
 */
class Calculator : public Application
{
public:
    void Initialize(void) override;
    void Run(void) override;
    void Shutdown(void) override;
    static void SignalHandler(int signum);
    void HandleSignal(int signum) override;

    typedef Calculator* Ptr;

    void Main();
    /**
     * \brief Datatype for the IO data identifier
     */
    typedef int32_t Calculator_IO_Datatype;

    Calculator();
    virtual ~Calculator();

    /**
     * \brief Executes addition of input_1_ and input_2_.
     */
    Calculator::Calculator_IO_Datatype Add();

    /**
     * \brief Executes addition of input_1_ and input_2_ and the argument of request.
     *
     * \param arg Argument to add.
     */
    Calculator::Calculator_IO_Datatype AddArg(const Calculator::Calculator_IO_Datatype& arg);

    /**
     * \brief Returns input_1_.
     */
    Calculator_IO_Datatype GetInput1()
    {
        return input_1_;
    }

    /**
     * \brief Returns input_2_.
     */
    Calculator_IO_Datatype GetInput2()
    {
        return input_2_;
    }

    /**
     * \brief Returns output.
     */
    Calculator_IO_Datatype GetOutput()
    {
        return output_;
    }

    /**
     * \brief Set input_1 value.
     *
     * \param input The input value.
     */
    ara::core::Future<Calculator_IO_Datatype> SetInput1(Calculator_IO_Datatype input)
    {
        input_1_ = input;
        ara::core::Promise<Calculator_IO_Datatype> promise;
        promise.set_value(std::move(input_1_));
        return promise.get_future();
    }

    /**
     * \brief Set input_2 value.
     *
     * \param input The input value.
     */
    ara::core::Future<Calculator_IO_Datatype> SetInput2(Calculator_IO_Datatype input)
    {
        input_2_ = input;
        ara::core::Promise<Calculator_IO_Datatype> promise;
        promise.set_value(std::move(input_2_));
        return promise.get_future();
    }

    /**
     * \brief Set output value.
     *
     * \param output The output value.
     */
    void SetOutput(Calculator_IO_Datatype output)
    {
        output_ = output;
    }

protected:
    /**
     * \brief This is the handler function for MW read interaction of input1
     * \uptrace{SWS_CM_00114}
     */
    ara::core::Future<ara::diag::example::skeleton::fields::input1::value_type> HandleInput1Get()
    {
        ara::core::Promise<ara::diag::example::skeleton::fields::input1::value_type> promise;
        promise.set_value(std::move(input_1_));
        return promise.get_future();
    }

    /**
     * \brief This is the handler function for MW read interaction of input2
     * \uptrace{SWS_CM_00114}
     */
    ara::core::Future<ara::diag::example::skeleton::fields::input2::value_type> HandleInput2Get()
    {
        ara::core::Promise<ara::diag::example::skeleton::fields::input2::value_type> promise;
        promise.set_value(std::move(input_2_));
        return promise.get_future();
    }

    /**
     * \brief This is the handler function for MW read interaction of output
     * \uptrace{SWS_CM_00114}
     */
    ara::core::Future<ara::diag::example::skeleton::fields::OutputValue::value_type> HandleOutputGet()
    {
        ara::core::Promise<ara::diag::example::skeleton::fields::OutputValue::value_type> promise;
        promise.set_value(std::move(output_));
        return promise.get_future();
    }

    /**
     * \brief Mutex used for shutdown_condvar_.
     */
    std::mutex shutdown_mutex_;

    /**
     * \brief Condition Variable to notify the DiagnosticManager that a shutdown was requested.
     */
    std::condition_variable shutdown_condvar_;

private:
    Calculator_IO_Datatype input_1_;
    Calculator_IO_Datatype input_2_;
    Calculator_IO_Datatype output_;

    /**
     * \brief Returns the sum of parameters \a a and \a b. Throws an ApplicationErrorException if sum
     * leads to over- or underflow.
     *
     * \param a The first summand.
     * \param b The second summand.
     */
    Calculator_IO_Datatype CheckedAdd(Calculator_IO_Datatype a, Calculator_IO_Datatype b);

    /**
     * \brief The implementation of the IO services.
     */
    ara::diag::example::skeleton::InputValueInterfaceSkeleton service_input_;

    /**
     * \brief The implementation of the IO services.
     */
    ara::diag::example::skeleton::OutputValueInterfaceSkeleton service_output_;

    /**
     * \brief The implementation of the method services.
     */
    AddInterfaceImpl service_method_;

    /**
     * \brief The implementation of the method services.
     */
    Diag_AddArgInterfaceImpl service_method_arg_;

    /*
     * Longging context instance
     */
    ara::log::Logger& log_;
};

}  // namespace calculator
}  // namespace sample_application
}  // namespace diag
}  // namespace ara

#endif  // SAMPLE_CALCULATOR_SOURCE_CALCULATOR_H_
