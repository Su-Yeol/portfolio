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

#include "pdu_api_handle.h"

bool PduApiHandle::init()
{
    if (PDUConstruct(nullptr, nullptr)) {
        logger_.LogError() << "PDUConstruct() failed";
        return false;
    }

    PDU_MODULE_ITEM* pModuleIdList = nullptr;
    if (PDUGetModuleIds(&pModuleIdList)) {
        logger_.LogError() << "PDUGetModuleIds() failed";
        return false;
    }

    hMod_ = pModuleIdList->pModuleData[0].hMod;

    // Destroying the Module Item
    if (PDUDestroyItem(reinterpret_cast<PDU_ITEM*>(pModuleIdList))) {
        logger_.LogError() << "PDUDestroyItem() failed";
        return false;
    }

    // Connecting the first detected module
    if (PDUModuleConnect(hMod_)) {
        logger_.LogError() << "PDUModuleConnect() failed";
        return false;
    }

    logger_.LogInfo() << "PduApiHandle init successfully";
    isInit_ = true;
    return true;
}

PduApiHandle::~PduApiHandle()
{
    UNUM32 retVal = PDUDestruct();
    logger_.LogInfo() << "PDUDestruct() API called and returned" << ara::log::HexFormat(retVal);
}

UNUM32 PduApiHandle::getModuleHandle() const
{
    return hMod_;
}

bool PduApiHandle::isInit() const
{
    return isInit_;
}
