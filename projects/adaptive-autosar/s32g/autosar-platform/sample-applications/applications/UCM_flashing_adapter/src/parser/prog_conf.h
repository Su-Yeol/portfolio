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

#ifndef ARA_UCM_PKGMGR_PROG_CONF_H_
#define ARA_UCM_PKGMGR_PROG_CONF_H_

#include "ara/core/string.h"
#include "ara/core/map.h"
#include "ara/core/vector.h"

enum class ReprogrammingSequenceType : uint8_t
{
    SingleBankEcu,
    DualBankEcu,
    UnknownEcuType = 255u
};
/// @brief Datatype holding UDS command as byte vector and the corresponding
///        expected positive responds
struct UdsCommand
{
    ara::core::Vector<uint8_t> command;
    ara::core::Vector<uint8_t> positiveResponse;
};

/// @brief Configuration for reprogramming sequence
class ProgConf
{
public:
    typedef ara::core::Map<ara::core::String, UdsCommand> UdsCommandMap;

    typedef ara::core::Vector<ara::core::String> FileList;
    ProgConf(FileList& flashFiles,
        FileList& rollbackFlashFiles,
        ReprogrammingSequenceType reprogrammingSequenceType,
        UdsCommandMap& udsCommandMap,
        uint32_t canReqId,
        uint32_t canRespId,
        ara::core::String ecuSwName_,
        ara::core::String ecuSwVersion_);

    ProgConf(FileList& flashFiles,
        ReprogrammingSequenceType reprogrammingSequenceType,
        UdsCommandMap& udsCommandMap,
        uint32_t canReqId,
        uint32_t canRespId,
        ara::core::String ecuSwName_,
        ara::core::String ecuSwVersion_);

    /// @brief Get the type of the reprogramming sequence
    /// @return the type of the reprogramming sequence
    ReprogrammingSequenceType GetReprogrammingSequenceType() const;

    /// @brief Get the file list of the flash files
    /// @return the list of flash files
    const FileList& getFlashFiles() const;

    /// @brief Get the file list of the rollback flash files
    /// @return the list of rollback flash files
    const FileList& getRollbackFlashFiles() const;

    /// @brief Gets a uds command by its name. The name is the key in the json file
    /// @param udsCommandName name of the command to get
    /// @param udsCommand refernce holding the command if found
    /// @return true if found, otherwise false
    bool getUdsCommandByName(ara::core::String udsCommandName, UdsCommand& udsCommand) const;

    /// @brief Get the request CAN id
    /// @return request CAN id
    uint32_t getCanRequestId() const;

    /// @brief Get the response CAN id
    /// @return response CAN id
    uint32_t getCanResponseId() const;

    /// @brief Get the ECU sw name
    /// @return ECU sw name
    ara::core::String GetEcuSwName() const;

    /// @brief Get the ECU sw version
    /// @return ECU sw version
    ara::core::String GetEcuSwVersion() const;

private:
    /// @brief File path list of all flash files mentioned in json config
    FileList flashFiles_;
    /// @brief File path list of all rollback flash files mentioned in json config
    FileList rollbackFlashFiles_;
    /// @brief The type of the reporgramming sequence
    ReprogrammingSequenceType reprogrammingSequenceType_;
    /// @brief Map holding udsCommandName as key and corresponding udsCommand as value
    UdsCommandMap udsCommandMap_;
    /// @brief The CAN request ID read from config file
    uint32_t canReqId_ = 0;
    /// @brief The CAN respond ID read from config file
    uint32_t canRespId_ = 0u;
    /// @brief name of the ecu software
    ara::core::String ecuSwName_;
    /// @brief version of the ecu software
    ara::core::String ecuSwVersion_;
};

#endif  // ARA_UCM_PKGMGR_PROG_CONF_H_
