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

#ifndef APD_OTA_BACKEND_COMMON_TYPES_H_
#define APD_OTA_BACKEND_COMMON_TYPES_H_

#include "ara/core/vector.h"
#include "ara/core/string.h"

namespace apd
{
namespace ucm
{
namespace ota
{

/**
 * \brief Struct containing the file information
 */
struct FileInformation
{
    ara::core::String filename;  // name of software
    int64_t size;  // size of software bundle file
    ara::core::String sha1;  // sha1 checksum of software file
};

/**
 * \brief  Struct containing information of the new downloaded file.
 */
struct NewSoftwareDeploymentData
{
    ara::core::String filesLocation;  // path to files
    FileInformation vehiclePackage;  // name of vehicle package
    ara::core::Vector<FileInformation> swPackages;  // list of SW packages
};

}  // namespace ota
}  // namespace ucm
}  // namespace apd

#endif  // APD_OTA_BACKEND_COMMON_TYPES_H_
