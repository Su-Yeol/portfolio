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

#ifndef HAWKBIT_TYPES_H_
#define HAWKBIT_TYPES_H_

#include <queue>
#include <boost/uuid/detail/sha1.hpp>

#include "provider_common_types.h"

namespace apd
{
namespace ucm
{
namespace ota
{
namespace hawkbitClient
{

constexpr char HAWKBIT_USERAGENT[] = "hawkbit-c-agent/1.0";
constexpr std::chrono::seconds DEFAULT_SLEEP_TIME_SEC(60 * 60);  // 1 hour

/**
 * \brief Struct containing the artifact specification
 */
struct HawkbitArtifactSpecification
{
    std::string downloadURL;  // download URL of software file
    std::string feedbackURL;  // URL status feedback should be sent to
    FileInformation fileInformation;  // file information, e.g. name, size
};

/**
 * \brief Struct containing the chunk specification
 */
struct HawkbitChunkSpecification
{
    std::string name;  // name of software
    std::string version;  // software version
    std::string part;  // module category
    std::queue<std::unique_ptr<HawkbitArtifactSpecification>> artifactsQueue;
};

}  // namespace hawkbitClient
}  // namespace ota
}  // namespace ucm
}  // namespace apd

#endif  // HAWKBIT_TYPES_H_
