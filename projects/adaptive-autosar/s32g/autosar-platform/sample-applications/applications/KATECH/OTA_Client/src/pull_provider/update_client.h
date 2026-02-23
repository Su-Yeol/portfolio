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

#ifndef APD_UCM_OTA_UPDATE_CLIENT_H_
#define APD_UCM_OTA_UPDATE_CLIENT_H_

#include <chrono>
#include <mutex>

#include "ara/rest/client_types.h"
#include "ara/rest/client.h"
#include "ara/rest/pointer.h"

#include "provider_common_types.h"
#include "json_helper.h"
#include "vpackage_management_app.h"

namespace apd
{
namespace ucm
{
namespace ota
{
namespace updateClient
{

class UpdateClient
{
    const std::chrono::seconds UPDATE_CLIENT_INTERVAL_CHECK{60u};

public:
    UpdateClient(ara::core::String vin,
        apd::ucm::ota::VehiclePackageManagementApp<ara::ucm::proxy::VehiclePackageManagementProxy>& app);
    ~UpdateClient() = default;

    void StartServiceSync();
    void StopServiceSync();

private:
    /** \brief Main routine for checking for updates and do a deployment
     *
     */
    void ServiceRoutine();

    /** \brief Builds URL for getting available SW packages from update server
     *
     * \param path path to resource
     * \return url
     */
    ara::rest::Uri BuildGetSoftwarePackageURL(const ara::core::String& path);

    /** \brief Builds URL for posting vehicle package info to server
     *
     * \return url
     */
    ara::rest::Uri BuildPostVehiclePackageURL();

    /** \brief Builds URL for getting a SW package from update server
     *
     * \param path path to resource
     * \return url
     */
    ara::rest::Uri BuildGetReqSoftwarePackageURL(const ara::core::String& path);

    /** \brief Builds URL for getting a SW package from update server
     *
     * \param method method of http request
     * \param url url to requested resource
     * \param request true if request payload shall be send
     * \param restRequest the request payload
     * \param response true if response payload shall be received
     * \param restResponse the response payload
     * \return http code of request
     */
    int RestRequest(const ara::rest::RequestMethod& method,
        const ara::rest::Uri& url,
        bool request,
        ara::rest::Pointer<ara::rest::ogm::Object>& restRequest,
        bool response,
        ara::rest::Pointer<ara::rest::ogm::Object>& restResponse);

    /** \brief Prints received SW package info
     *
     * \param softwarePackage path to resource
     */
    void PrintSoftwarePackageInfo(SoftwarePackage& softwarePackage);

    /** \brief Gets a package from update server
     *
     * \param method method of http request
     * \param request true if request payload shall be send
     * \param restRequest the request payload
     * \param url url to requested resource
     * \param file path to file downloaded data shall be saved to
     * \param httpCode http status code of the request
     * \return true if successful
     */
    bool GetPackageFromServer(const ara::rest::RequestMethod& method,
        bool request,
        ara::rest::Pointer<ara::rest::ogm::Object>& restRequest,
        const ara::rest::Uri& downloadUrl,
        const ara::rest::String& file,
        int& httpCode);

    /** \brief Gets a required SW packages from UCM Master
     *
     * \param softwarePackageInformation available SW packages from update server
     * \param requiredSoftwarePackages SW packages required for download
     * \return true if successful
     */
    bool GetReqSwPackage(SoftwarePackageInformation& softwarePackageInformation,
        ara::ucm::SwNameVersionVectorType& requiredSoftwarePackages);

    /** \brief Gets SW packages from update server
     *
     * \param path path to resource
     * \param fileInformations reference to deployment file information
     * \param softwarePackageInformation SW packages available for download from update server
     * \param requiredSoftwarePackages SW packages required for download from update server
     * \return true if successful
     */
    bool GetSwPackagesFromUpdateServer(ara::rest::String path,
        std::vector<FileInformation>& fileInformations,
        SoftwarePackageInformation& softwarePackageInformation,
        ara::ucm::SwNameVersionVectorType& requiredSoftwarePackages);

    /** \brief Gets reduced vehicle package from update server
     *
     * \param path path to resource
     * \param fileInformations reference to deployment file information
     * \param requiredSoftwarePackages required SW packages for reduced vehicle package
     * \return true if successful
     */
    bool GetVehiclePackageFromUpdateServer(ara::rest::String path,
        FileInformation& fileInformations,
        ara::ucm::SwNameVersionVectorType& requiredSoftwarePackages);

    /** \brief Transfers vehicle package to UCM Master
     *
     * \param dataLocation location of vehicle package file
     * \param vehiclepackage vehicle package file name
     * \return true if successful
     */
    bool TransferVehiclePackage(std::string const& dataLocation, std::string const& vehiclepackage);

    /** \brief Transfers SW packages to UCM Master
     *
     * \param  dataLocation location of SW packages
     * \return true if successful
     */
    bool TransferSoftwarePackages(std::string const& dataLocation);

    /** \brief Starts download of data from update server
     *
     * \param deploymentData reference to deployment data which has to be filled
     * \param softwarePackageInformation available software
     * \param requiredSoftwarePackages required software packages
     * \return true if successful
     */
    bool GetDataFromServer(NewSoftwareDeploymentData& deploymentData,
        SoftwarePackageInformation& softwarePackageInformation,
        ara::ucm::SwNameVersionVectorType& requiredSoftwarePackages);

    bool Deploy(NewSoftwareDeploymentData& deploymentData);

    void CleanDirectory(std::string pathToDir);

    bool CreateDirectoryIfNotExist(ara::core::String pathToDir);
    /**
     * @brief ara-rest client instance
     */
    ara::rest::Client restClient_;
    /**
     * @brief interval in seconds to check for new software
     */
    std::chrono::seconds sleepTime_;
    /**
     * @brief flag for shutdown
     */
    std::atomic_bool continueExecution_{true};
    ara::log::Logger& logger_;
    std::thread serviceThread_;
    ara::core::String vin_;

    std::string tempDirectory_{"var"};

    /**
     * @brief Vehicle Package Management service interface
     */
    apd::ucm::ota::VehiclePackageManagementApp<ara::ucm::proxy::VehiclePackageManagementProxy>& serviceApp_;
};

}  // namespace updateClient
}  // namespace ota
}  // namespace ucm
}  // namespace apd

#endif  // APD_UCM_OTA_UPDATE_CLIENT_H_
