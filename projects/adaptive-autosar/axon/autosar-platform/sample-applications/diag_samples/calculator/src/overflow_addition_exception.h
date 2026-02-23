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

#ifndef SAMPLE_CALCULATOR_SOURCE_OVERFLOW_ADDITION_EXCEPTION_H_
#define SAMPLE_CALCULATOR_SOURCE_OVERFLOW_ADDITION_EXCEPTION_H_

#include <exception>

#include "calculator.h"

namespace ara
{
namespace diag
{
namespace sample_application
{
namespace calculator
{

class OverflowAdditionException : public std::overflow_error
{
public:
    OverflowAdditionException(Calculator::Calculator_IO_Datatype lhs, Calculator::Calculator_IO_Datatype rhs)
        : std::overflow_error("Overflow when adding " + std::to_string(lhs) + " + " + std::to_string(rhs))
        , lhs_(lhs)
        , rhs_(rhs)
    { }

    Calculator::Calculator_IO_Datatype GetLHS() const
    {
        return lhs_;
    }
    Calculator::Calculator_IO_Datatype GetRHS() const
    {
        return rhs_;
    }

private:
    Calculator::Calculator_IO_Datatype lhs_;
    Calculator::Calculator_IO_Datatype rhs_;
};

}  // namespace calculator
}  // namespace sample_application
}  // namespace diag
}  // namespace ara

#endif  // SAMPLE_CALCULATOR_SOURCE_OVERFLOW_ADDITION_EXCEPTION_H_
