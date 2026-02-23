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

#ifndef ARA_UCM_PKGMGR_ECU_REPROGRAMMING_SEQUENCE_H_
#define ARA_UCM_PKGMGR_ECU_REPROGRAMMING_SEQUENCE_H_

#include <atomic>
#include "ara/core/string.h"

#include "ara/core/promise.h"
#include "ara/core/string.h"
#include "ara/log/logger.h"

#include "prog_conf.h"
#include "flash_data.h"
#include "uds_interface.h"

class EcuReprogrammingSequence
{

public:
    EcuReprogrammingSequence(std::shared_ptr<PduApiHandle>& pduApiHandle,
        std::unique_ptr<ProgConf> progConf,
        ara::core::String path);
    /**
     * Destructor
     */
    virtual ~EcuReprogrammingSequence(){};

    /** \brief Returns progress of reprogramming sequence
     *
     *  \return int of progress
     */
    virtual uint8_t GetProgress() = 0;

    /// @brief Initialize underlying communication layer
    /// @return future with result of Init
    ara::core::Future<void> Init();

    /** \brief Installs given flash file
     *
     *  \return future with result of install
     */
    virtual ara::core::Future<void> Install() = 0;

    /** \brief Activates given flash file
     *
     *  \return future with result of install
     */
    virtual ara::core::Future<void> Activate() = 0;

    /** \brief Verifies the reqrogramming sequence
     *
     *  \return future with result of verify
     */
    virtual ara::core::Future<void> Verify() = 0;

    /** \brief Starts rollback of reprogramming
     *
     *  \return future with result of rollback
     */
    virtual ara::core::Future<void> Rollback() = 0;

    ara::core::String GetEcuSwName();

    ara::core::String GetEcuSwVersion();

protected:
    EcuReprogrammingSequence(EcuReprogrammingSequence&&) = default;
    EcuReprogrammingSequence(EcuReprogrammingSequence const&) = default;

    /// @brief Download
    ara::core::Future<void> downloadFirmware(FlashData& flashData);
    ara::core::Future<void> eraseRoutine(FlashData& flashData);
    bool getFlashFileData(ara::core::String filePath, FlashData& flashData);
    ara::core::Future<void> doProgramming(const ProgConf::FileList& filePathList);
    ara::core::Future<void> doReadVersion(ara::core::String udsCommandName);
    ara::core::Future<void> doDependencyCheck();
    ara::core::Future<void> doEcuReset();
    ara::core::Future<void> enterProgrammingSession();
    std::unique_ptr<ProgConf> progConf_;
    ara::core::String pathToFiles_;
    UdsInterface udsInterface_;
    std::atomic<uint8_t> progress_;
    ara::log::Logger& logger_{ara::log::CreateLogger("ECUR", "EcuReprogramming", ara::log::LogLevel::kVerbose)};
};

#endif  // ARA_UCM_PKGMGR_ECU_REPROGRAMMING_SEQUENCE_H_
