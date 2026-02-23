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

#include "pep.h"
#include "ara/exec/find_process.h"

namespace apd
{
namespace demofc
{

bool PEP::EnforceAccessControl(types::IntID intID, const ara::core::InstanceSpecifier& instanceSpecifier)
{
    auto identityAA = AuthenticateAdaptiveApplication(intID);
    if (!identityAA.has_value()) {
        mLogger_.LogError() << "Authentication of AA failed. PID is not available. Access denied!";
        return false;
    }

    if (identityAA->isPlatformApplication) {
        mLogger_.LogInfo() << "Adaptive Platform application. Access control not required!";
        return true;
    }

    ara::iam::ifc::grant::GrantDemoFC accessGrant(identityAA->process, instanceSpecifier);
    mLogger_.LogInfo() << "DemoFCGrant:"
                       << "\tMetaModelProcess:" << accessGrant.GetProcess().ToString()
                       << "\tDemoFileInstanceSpecifier:" << accessGrant.GetDemoFile().ToString();
    auto accessControlDecision = HasGrant(accessGrant);

    return accessControlDecision;
}

ara::core::Optional<ara::exec::FindProcessResult> PEP::AuthenticateAdaptiveApplication(const types::IntID intID)
{
    auto findProcessResult = ara::exec::FindProcess(intID, &mLogger_).GetResult();
    if (!findProcessResult.HasValue()) {
        mLogger_.LogError() << "PEP IFC to EM - Error domain:" << findProcessResult.Error().Domain().Id()
                            << "With error:" << findProcessResult.Error().Message();
        return {};
    }

    if (!findProcessResult.Value().has_value()) {
        mLogger_.LogError() << "Process couldn't be identified!";
        return {};
    }

    mLogger_.LogInfo() << "Results of FindProcess():"
                       << "\tProcess:" << findProcessResult.Value()->process.ToString() << "\tIsPlatformApplication:"
                       << (findProcessResult.Value()->isPlatformApplication ? "true" : "false");

    return findProcessResult.Value();
}

bool PEP::HasGrant(const ara::iam::ifc::grant::GrantDemoFC& grantDemoFC)
{
    auto hasGrantResult = mGrantQueryClient.HasGrant(grantDemoFC);
    if (!hasGrantResult.HasValue()) {
        mLogger_.LogError() << "PEP IFC to PDP - Error domain:" << hasGrantResult.Error().Domain().Id()
                            << "With error:" << hasGrantResult.Error().Message();
        return false;
    }

    return hasGrantResult.Value();
}

}  // namespace demofc
}  // namespace apd
