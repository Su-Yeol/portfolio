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

#ifndef APD_UCM_OTA_SAMPLE_DATA_TRANSFER_H_
#define APD_UCM_OTA_SAMPLE_DATA_TRANSFER_H_

#include "ara/vucm/vehiclepackagemanagement_proxy.h"
#include "ara/log/logger.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include "ara/core/vector.h"

namespace apd
{
namespace ucm
{
namespace ota
{

template <class VehiclePackageManagementService>
class DataTransfer
{
public:
    /// @brief Initializes this object with a reference to the PackageManagement
    /// service
    ///
    /// @param vehiclePackageManagementService The instance of the PackageManagement
    /// service to be used
    explicit DataTransfer(const std::shared_ptr<VehiclePackageManagementService>& vehiclePackageManagementService)
        : vehiclePackageManagementService_(vehiclePackageManagementService)
        , blockSize_(0)
        , currentSession_(0)
        , logger_(ara::log::CreateLogger("DT", "DataTransfer", ara::log::LogLevel::kVerbose))
    { }

    ~DataTransfer() = default;

    /// @brief Initializes a new session
    ///
    /// @param size The size of the data that should be transmitted
    ///
    /// @return Boolean indicating whether the session was successfully initiated
    bool InitSession(uint64_t size);

    /// @brief Initializes a new session
    ///
    /// @param name The name of the data that should be transmitted
    ///
    /// @return Boolean indicating whether the session was successfully initiated
    bool InitSession(const ara::core::String& name);

    /// @brief Transfers data to UCM sending the provided collection of bytes
    /// to UCM, possibly by using multiple smaller data blocks accoring to the
    /// configured block size.
    ///
    /// @param data The data that should be sent to UCM
    ///
    /// @return Boolean indicating whether the data was successfully sent
    bool Transfer(const ara::core::Vector<uint8_t>& data);

    /// @brief Exits the current session
    ///
    /// @return Boolean indicating whether this session was successfully exited
    bool ExitSession();

    /// @brief Returns the session id of the current session.
    ///
    /// @return The id of the current session
    ara::ucm::TransferIdType GetTransferId() const;

private:
    /// @brief The instance of the PackageManagement service used
    std::shared_ptr<VehiclePackageManagementService> vehiclePackageManagementService_;

    /// @brief The configured block size for transmitting data
    uint32_t blockSize_;

    /// @brief The id of the current transfer
    ara::ucm::TransferIdType currentSession_;

    /// @brief For logging errors during transferring.
    ara::log::Logger& logger_;
};

}  // namespace ota
}  // namespace ucm
}  // namespace apd

#endif  // APD_UCM_OTA_SAMPLE_DATA_TRANSFER_H_
