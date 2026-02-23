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
/// @brief IAM PEP functionality to enforce access control

#ifndef DEMO_FC_PEP_H_
#define DEMO_FC_PEP_H_

#include "ara/core/string.h"
#include "ara/core/optional.h"  // TODO: Replace with result
#include "ara/core/result.h"
#include "ara/log/logger.h"
#include "ara/exec/find_process.h"
#include "ara/iam/ifc/grant/grant_demo_fc.h"
#include "ara/iam/ifc/grantqueryclient/grant_query_client_impl.h"

#include "intermediate_id.h"

namespace apd
{
namespace demofc
{

/// @brief This class implements the logic in which the Access Control Decision is enforced. It
/// communicates directly with the corresponding PDP to receive the Access Control Decision.
///
/// The uptrace to SWS_IAM_01001 was defined here because the PEP, as part of the Demo FC, will
/// be deployed as own process. Therefore, the process separation to the AA is guaranteed.
///
/// @uptrace{SWS_IAM_01001}
class PEP
{
public:
    /// @brief Checks the access rights for the requested action of the AA.
    ///
    /// @param fileResource  the Instance Specifier of the PortPrototype which links to the PortInterface
    /// @param intID  the intermediate ID of the POSIX process which requests the action
    /// @returns true if access was granted, false otherwise
    ///
    /// @uptrace{RS_IAM_00008} Access shall be denied by the PEP if the corresponding PDP is not available
    bool EnforceAccessControl(types::IntID intID, const ara::core::InstanceSpecifier& instanceSpecifier);

private:
    /// @brief Authenticate requesting Adaptive Application via EM.
    ///
    /// @param intID Intermediate ID as identifier of a POSIX process
    /// @returns optional if IFC API executed without error, nullopt if error occurs
    ///
    /// @uptrace{SWS_IAM_01002}
    ara::core::Optional<ara::exec::FindProcessResult> AuthenticateAdaptiveApplication(const types::IntID intID);

    /// @brief Checks the Grant of the modelled Process element.
    ///
    /// The access control will be enforced via IFC communication to the Access Manager.
    /// Therefore, the by the integrator acknowlegded capabilities (Grants) will be used within the
    /// Access Control List of the Access Manager to match against the requesting Grant as far the
    /// AA was authenticated correctly.
    ///
    /// @param grant  the type which contains Process and DemoFile instance specifier
    /// @returns true if access was granted, false if not granted
    bool HasGrant(const ara::iam::ifc::grant::GrantDemoFC& grantDemoFC);

    /// @brief Client to enforce request via PDP provided by the AccessManager.
    ara::iam::ifc::grantqueryclient::IAMGrantQueryClientImpl mGrantQueryClient;

    /// @brief logger used in this class
    ara::log::Logger& mLogger_ = ara::log::CreateLogger("PEP", "PEP context", ara::log::LogLevel::kInfo);
};

}  // namespace demofc
}  // namespace apd

#endif  // DEMO_FC_PEP_H_
