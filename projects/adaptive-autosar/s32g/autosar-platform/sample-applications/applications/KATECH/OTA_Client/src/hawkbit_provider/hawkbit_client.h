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

#ifndef HAWKBIT_CLIENT_H_
#define HAWKBIT_CLIENT_H_

#include <chrono>
#include <mutex>

#include "ara/rest/client_types.h"
#include "ara/rest/client.h"
#include "ara/rest/pointer.h"

#include "hawkbit_types.h"
#include "hawkbit_config_file.h"
#include "json_helper.h"
#include "vpackage_management_app.h"

namespace apd
{
namespace ucm
{
namespace ota
{
namespace hawkbitClient
{

class HawkbitClient
{
public:
    explicit HawkbitClient(
        apd::ucm::ota::VehiclePackageManagementApp<ara::ucm::proxy::VehiclePackageManagementProxy>& app);
    ~HawkbitClient() = default;

    /**
     * \brief Stop the Server asynchronously
     * \param config configuration of hawkbit client
     * \return true if successful initialization was possbile
     */
    bool Init(const ConfigClass& config);

    /**
     * \brief Start the hawkbit client service
     * \return true if successfully started
     */
    bool StartServiceSync();

    /**
     * \brief Function stops service
     */
    void StopServiceSync();

private:
    /**
     * \brief Initialize the necessary configuration and variables to run the hawkbit client.
     *
     *
     * \param config to the config object
     */
    void HawkbitInit(const ConfigClass& config);

    /**
     * \brief Function launches the callback that connects to the server in a specific time interval
     */
    void HawkbitStartServiceSync();

    /**
     * \brief Callbacks that send progress messages
     *        Note: These callbacks are not yet called.
     */
    bool HawkbitProgress(const ara::rest::String& msg);

    /**
     * \brief Callback that connects to the server
     */
    void HawkbitPullCallback();

    /**
     * \brief Helper function to build URL
     * \param path string of the url which has to be transformed to ara-rest URI
     * \return Uri of the given url string
     */
    ara::rest::Uri BuildApiURL(const ara::rest::String& path);

    /**
     * \brief Make REST request.
     *
     * \param method HTTP Method ex.: GET
     * \param url URL used in HTTP REST request
     * \param request shows that we will do a request
     * \param jsonRequestBody REST request body. If NULL, no body is sent.
     * \param response shows that we expect a response
     * \param jsonResponseParser REST response
     * \return HTTP Status code (Standard codes: 200 = OK, 524 = Operation timed out, 401 = Authorization needed, 403 =
     * Authentication failed )
     */
    int RestRequest(const ara::rest::RequestMethod& method,
        const ara::rest::Uri& url,
        bool request,
        ara::rest::Pointer<ara::rest::ogm::Object>& restRequest,
        bool response,
        ara::rest::Pointer<ara::rest::ogm::Object>& restResponse);
    /**
     * \brief Identify the ourself to the server
     * \return true if identification was successful
     */
    bool Identify();

    /**
     * \brief Send Feedback to hawkBit.
     * \param url URL used in HTTP REST request
     * \param id id of feedback
     * \param detail details of feedback
     * \param finished finished field of feedback
     * \param execution execution field of feedback
     * \return true if feedback was successful
     */
    bool Feedback(const ara::rest::Uri& url,
        const ara::rest::String& id,
        const ara::rest::String& detail,
        const ara::rest::String& finished,
        const ara::rest::String& execution);
    /**
     * \brief Send progress feedback to hawkBit.
     * \param url URL used in HTTP REST request
     * \param id id of feedback
     * \param progress progress message of feedback
     * \param detail details of feedback
     * \return true if feedback was successful
     */
    bool FeedbackProgress(const ara::rest::Uri& url,
        const ara::rest::String& id,
        int progress,
        const ara::rest::String& detail);

    /**
     * \brief This function interprets the request
     * \param requestRoot json containing deployment information provided by hawkbit server
     * \return true, if deployment information was process successfully
     */
    bool ProcessDeployment(ara::rest::Pointer<ara::rest::ogm::Object>& requestRoot);

    /**
     * \brief Thread function to download given chunks
     * \param chunks Hawkbit chunk specifications which have to be downloaded
     */
    void DownloadAndDeploy(std::shared_ptr<std::queue<std::unique_ptr<HawkbitChunkSpecification>>> chunks);

    /**
     * \brief Download software to file.
     *
     * \param downloadUrl URL to Software bundle
     * \param file File the software should be written to.
     * \param filesize Expected file size
     * \param checkSum Calculated checkSum
     * \param httpCode Return location for the httpCode, can be NULL
     * \return true if request was successful
     */
    bool GetBinary(const ara::rest::Uri& downloadUrl,
        const ara::rest::String& file,
        ara::rest::String& checkSum,
        int& httpCode);

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

    bool Deploy(ara::rest::Uri& feedbackURL, NewSoftwareDeploymentData& deploymentData);

    /** \brief Cleans given directory by deleting all files
     *
     * \param  pathToDir directory to be cleaned
     */
    void CleanDirectory(std::string pathToDir);

    std::chrono::seconds sleepTime_;
    std::thread serviceThread_;

    /**
     * @brief reference of hawkbit client configuration
     */
    const ConfigClass* config_ = nullptr;
    /**
     * @brief ara-rest client instance
     */
    ara::rest::Client restClient_;
    /**
     * @brief interval in seconds to check for new software
     */
    std::chrono::seconds hawkbitIntervalCheckSec_ = DEFAULT_SLEEP_TIME_SEC;
    /**
     * @brief flag for shutdown
     */
    std::atomic_bool continueExecution_{true};
    std::unique_ptr<ara::rest::String> actionId_;
    bool installSuccessSoftware_ = false;
    ara::log::Logger& logger_;

    /**
     * @brief Vehicle Package Management service interface
     */
    apd::ucm::ota::VehiclePackageManagementApp<ara::ucm::proxy::VehiclePackageManagementProxy>& serviceApp_;
};

}  // namespace hawkbitClient
}  // namespace ota
}  // namespace ucm
}  // namespace apd

#endif  // HAWKBIT_CLIENT_H_
