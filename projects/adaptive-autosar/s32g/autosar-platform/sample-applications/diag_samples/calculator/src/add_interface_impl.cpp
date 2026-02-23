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

#include "add_interface_impl.h"

#include <ara/log/logger.h>
#include <ara/com/exception.h>

#include "calculator.h"
#include "underflow_addition_exception.h"
#include "overflow_addition_exception.h"

namespace ara
{
namespace diag
{
namespace sample_application
{
namespace calculator
{

auto AddInterfaceImpl::AddMethod() -> decltype(Skeleton::AddMethod())
{
    decltype(Skeleton::AddMethod())::PromiseType promise;
    ara::diag::example::AddInterface::AddMethodOutput result;
    try {
        calculator_->Add();
        promise.set_value(result);
    } catch (OverflowAdditionException& e) {
        logger().LogError() << "Overflow Exception caught in Calculator-App.";
        result.Lhs = e.GetLHS();
        result.Rhs = e.GetRHS();
        promise.set_value(result);
        promise.SetError(ara::diag::example::DiagExampleErrc::kCalculationOverflow);
    } catch (UnderflowAdditionException& e) {
        logger().LogError() << "Underflow Exception caught in Calculator-App.";
        result.Lhs = e.GetLHS();
        result.Rhs = e.GetRHS();
        promise.set_value(result);
        promise.SetError(ara::diag::example::DiagExampleErrc::kCalculationUnderflow);
    }
    return promise.get_future();
}

}  // namespace calculator
}  // namespace sample_application
}  // namespace diag
}  // namespace ara
