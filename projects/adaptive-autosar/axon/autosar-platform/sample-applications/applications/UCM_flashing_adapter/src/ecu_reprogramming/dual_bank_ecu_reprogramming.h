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

#ifndef ARA_UCM_PKGMGR_DUAL_BANK_ECU_PROGRAMMING_H_
#define ARA_UCM_PKGMGR_DUAL_BANK_ECU_PROGRAMMING_H_

#include "ara/core/promise.h"

#include "ecu_reprogramming_sequence.h"

class DualBankEcuReprogramming : public EcuReprogrammingSequence
{
public:
    DualBankEcuReprogramming(std::shared_ptr<PduApiHandle>& pduApiHandle,
        std::unique_ptr<ProgConf> progConf,
        ara::core::String path)
        : EcuReprogrammingSequence(pduApiHandle, std::move(progConf), path){};

    /** \brief Returns progress of reprogramming sequence
     *
     *  \return int of progress
     */
    uint8_t GetProgress() override;

    /** \brief Installs given flash file
     *
     *  \return future with result of install
     */
    ara::core::Future<void> Install() override;

    /** \brief Activates given flash file
     *
     *  \return future with result of install
     */
    ara::core::Future<void> Activate() override;

    /** \brief Verifies the reqrogramming sequence
     *
     *  \return future with result of verify
     */
    ara::core::Future<void> Verify() override;

    /** \brief Starts rollback of reprogramming
     *
     *  \return future with result of rollback
     */
    ara::core::Future<void> Rollback() override;
};

#endif  // ARA_UCM_PKGMGR_DUAL_BANK_ECU_PROGRAMMING_H_
