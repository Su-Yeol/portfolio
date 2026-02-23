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

#include "prog_conf.h"

ProgConf::ProgConf(FileList& flashFiles,
    FileList& rollbackFlashFiles,
    ReprogrammingSequenceType reprogrammingSequenceType,
    UdsCommandMap& udsCommandMap,
    uint32_t canReqId,
    uint32_t canRespId,
    ara::core::String ecuSwName,
    ara::core::String ecuSwVersion)
    : flashFiles_(flashFiles)
    , rollbackFlashFiles_(rollbackFlashFiles)
    , reprogrammingSequenceType_(reprogrammingSequenceType)
    , udsCommandMap_(udsCommandMap)
    , canReqId_(canReqId)
    , canRespId_(canRespId)
    , ecuSwName_(ecuSwName)
    , ecuSwVersion_(ecuSwVersion)
{ }

ProgConf::ProgConf(FileList& flashFiles,
    ReprogrammingSequenceType reprogrammingSequenceType,
    UdsCommandMap& udsCommandMap,
    uint32_t canReqId,
    uint32_t canRespId,
    ara::core::String ecuSwName,
    ara::core::String ecuSwVersion)
    : flashFiles_(flashFiles)
    , reprogrammingSequenceType_(reprogrammingSequenceType)
    , udsCommandMap_(udsCommandMap)
    , canReqId_(canReqId)
    , canRespId_(canRespId)
    , ecuSwName_(ecuSwName)
    , ecuSwVersion_(ecuSwVersion)
{ }

const ProgConf::FileList& ProgConf::getFlashFiles() const
{
    return flashFiles_;
}

uint32_t ProgConf::getCanRequestId() const
{
    return canReqId_;
}

uint32_t ProgConf::getCanResponseId() const
{
    return canRespId_;
}

const ProgConf::FileList& ProgConf::getRollbackFlashFiles() const
{
    return rollbackFlashFiles_;
}

ReprogrammingSequenceType ProgConf::GetReprogrammingSequenceType() const
{
    return reprogrammingSequenceType_;
}

ara::core::String ProgConf::GetEcuSwName() const
{
    return ecuSwName_;
}

ara::core::String ProgConf::GetEcuSwVersion() const
{
    return ecuSwVersion_;
}

bool ProgConf::getUdsCommandByName(ara::core::String udsCommandName, UdsCommand& udsCommand) const
{
    auto iter = udsCommandMap_.find(udsCommandName);
    if (iter != udsCommandMap_.end()) {
        udsCommand = iter->second;
        return true;
    }
    return false;
}
