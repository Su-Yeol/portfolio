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

#ifndef CMAPP02_DATA_IDENTIFIER_H_
#define CMAPP02_DATA_IDENTIFIER_H_

#include <memory>

#include <ara/com/types.h>
#include "ara/diag/dataidentifier_skeleton.h"
#include "cmService1_activity.hpp"

class CMApp02DataIdentifier : public ara::diag::skeleton::DataIdentifierSkeleton
{
    using Skeleton = ara::diag::skeleton::DataIdentifierSkeleton;

    private:
        std::shared_ptr<cmService1Activity> cmService1ActivityPtr;

    public:
         explicit CMApp02DataIdentifier(ara::com::InstanceIdentifier);
         ~CMApp02DataIdentifier(void);
         auto Write(const ByteArray& dataRecord) -> decltype(Skeleton::Write(dataRecord)) override;
         auto Read() -> decltype(Skeleton::Read()) override;
};

#endif /* CMAPP02_DATA_IDENTIFIER_H_ */
