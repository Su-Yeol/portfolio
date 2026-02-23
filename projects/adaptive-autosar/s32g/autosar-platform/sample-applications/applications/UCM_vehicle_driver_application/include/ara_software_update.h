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

#ifndef APD_UCM_VDA_SAMPLE_ARA_SOFTWARE_UPDATE_H_
#define APD_UCM_VDA_SAMPLE_ARA_SOFTWARE_UPDATE_H_

#include "ara/vucm/vehicledriverapplicationinterface_proxy.h"

struct AraSoftwareUpdate
{
    ara::ucm::SwClusterInfoVectorType swClusterInfoVec;
    int approvalId_;
};

struct progressInformationStruct
{
    ara::vucm::CampaignStateType campaignState_;
    uint8_t processProgress = 0;
    uint8_t transferProgress = 0;
};

struct packageInformationStruct
{
    ara::vucm::SwPackageDescVectorType swPackageDescVec;
    ara::vucm::VehiclePackageDescriptionType vehPackageDecVec;
    bool approvalRequired_ = true;
    ara::vucm::VehicleConditionCollectionType vehicleConditions_;
};

struct campaignHistoryStruct
{
    ara::vucm::VehicleDriverApplicationInterface::GetCampaignHistoryOutput campaignHistoryInfo_;
    std::uint64_t timestampGE = 0;
    std::uint64_t timestampLT = 0;
};

#endif  // APD_UCM_VDA_SAMPLE_ARA_SOFTWARE_UPDATE_H_
