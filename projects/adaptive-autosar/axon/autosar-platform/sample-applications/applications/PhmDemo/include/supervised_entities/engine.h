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

#ifndef PHM_DEMO_SUPERVISED_ENTITIES_ENGINE_H_
#define PHM_DEMO_SUPERVISED_ENTITIES_ENGINE_H_

#include "ara/phm/supervised_entity.h"

/// @brief generated header files for Supervised Entities.
///
/// @uptrace{SWS_PHM_01002, 3d279b8453e23bfc6ffaa99ba98eb1996eefcf0b}
/// @uptrace{SWS_PHM_01020, 43786a6a7e8bee537b99ea2cd6f78de271311d14}
namespace ara
{
namespace phm
{

/// @brief Namespace for Checkpoints
///
/// @uptrace{SWS_PHM_01005, 1da9414968907d7469a5c53c0af3eaf55bc1ca3a}
namespace supervised_entities
{

/// @brief Namespace for Checkpoints
///
/// @uptrace{SWS_PHM_00426, 7caaa0ac0c89f13e324691af9a5e4c5ff2c39d1d}
namespace engine
{

/// @brief definition of all health statuses of this SE
///
/// @uptrace{SWS_PHM_00424, 0edee1cce8094e111d27caae7fbae2196d3204c3}
/// @uptrace{SWS_PHM_00425, 2623c5634777411aafe4b54a4d7844781ca1b130}
enum class Checkpoints : Checkpoint
{
    kInitializing = 0U,
    kStartupTest = 1U,
    kInitializingFinished = 2U,
};

// InstanceSpecifier of the supervision entity engine
ara::core::InstanceSpecifier engineInstance{"exe0/RootSWCP_0/Comp_Lvl1/Comp_Lvl2/SwComponentPrototype_0/RPort_0"};

}  // namespace engine
}  // namespace supervised_entities
}  // namespace phm
}  // namespace ara

#endif  // PHM_DEMO_SUPERVISED_ENTITIES_ENGINE_H_
