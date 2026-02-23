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

/// @file
/// @brief Class to access file

#ifndef IAM_TEST_APP_FILE_ACCESSOR_H_
#define IAM_TEST_APP_FILE_ACCESSOR_H_

#include "ara/core/instance_specifier.h"

#include "ucm_accessor_skeleton.h"

namespace apd
{
namespace iamtest
{

/// @brief Wrapper class to access file via DemoFC which is placed within ARA
///
/// The APIs provided by this class are comparable with the level of ara APIs to
/// access resources provided by the Adaptive AUTOSAR stack.
class UcmAccessor
{
public:
    /// @brief Request file access
    ///
    /// @param instanceSpecifier AUTOSAR model element to access
    void AccessUcm(const ara::core::InstanceSpecifier& instanceSpecifier);

private:
    /// @brief Object to send request via IPC to access resource of DemoFC (file access)
    UcmAccessorSkeleton mUcmAccessorSkeleton;
};

}  // namespace iamtest
}  // namespace apd

#endif  // IAM_TEST_APP_FILE_ACCESSOR_H_
