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

#ifndef SAMPLE_CALCULATOR_SOURCE_ADDINTERFACEIMPL_H_
#define SAMPLE_CALCULATOR_SOURCE_ADDINTERFACEIMPL_H_

#include "ara/diag/example/addinterface_skeleton.h"

namespace ara
{
namespace diag
{
namespace sample_application
{
namespace calculator
{

class Calculator;  // forward declaration

/**
 * \brief ara::com interface implementation for AddInterface
 */
class AddInterfaceImpl : public ara::diag::example::skeleton::AddInterfaceSkeleton
{
    using Skeleton = ara::diag::example::skeleton::AddInterfaceSkeleton;

public:
    AddInterfaceImpl(const ara::com::InstanceIdentifier& instance_id, Calculator* calculator)
        : Skeleton(instance_id)
        , calculator_(calculator)
    { }
    virtual ~AddInterfaceImpl()
    { }

protected:
    Calculator* calculator_;

public:
    auto AddMethod() -> decltype(Skeleton::AddMethod()) override;
};

}  // namespace calculator
}  // namespace sample_application
}  // namespace diag
}  // namespace ara

#endif  // SAMPLE_CALCULATOR_SOURCE_ADDINTERFACEIMPL_H_
