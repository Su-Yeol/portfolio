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

#ifndef ARA_UCM_PKGMGR_UPDATE_SERVER_H_
#define ARA_UCM_PKGMGR_UPDATE_SERVER_H_

#include <cstdint>
#include <limits>
#include <mutex>

#include "ara/core/map.h"
#include "ara/log/logger.h"
#include "apd/rest/routing.h"
#include "apd/rest/ogm/base.h"
#include "apd/rest/ogm/node.h"
#include "apd/rest/ogm/field.h"
#include "apd/rest/ogm/object.h"
#include "apd/rest/ogm/array.h"
#include "apd/rest/ogm/string.h"
#include "apd/rest/ogm/int.h"
#include "apd/rest/ogm/real.h"
#include "apd/rest/ogm/visit.h"
#include "apd/rest/server.h"
#include "ara/ucm/packagemanagement_common.h"

class UpdateServer
{

public:
    UpdateServer();

    /** \brief Rest server callback methods
     *
     */
    apd::rest::Route::Upshot UpdateListGetHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);
    apd::rest::Route::Upshot ActivateUpdateGetHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);
    apd::rest::Route::Upshot DownloadGetHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);
    apd::rest::Route::Upshot FeedbackUpdatePostHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);
    apd::rest::Route::Upshot FeedbackActivatePostHandler(const apd::rest::Route& route,
        const apd::rest::ServerRequest& request,
        apd::rest::ServerReply& reply);

    /** \brief Adds a SW package to the deployment
     *
     * \param id if of software package to add for deployment
     * \param fileName filename of sw package
     * \param pathToFile path to the sw package
     */
    void AddSoftwarePackage(ara::ucm::TransferIdType id,
        const ara::core::String& fileName,
        const ara::core::String& pathToFile);

    /** \brief Activate software packages on update client
     *
     */
    void ActivateSoftwarePackages();

    /** \brief Process software packages to update client
     *
     */
    void ProcessSoftwarePackages();

    /** \brief Rremove software packages deployment
     *
     */
    void RemoveSoftwarePackages();

    /** \brief Check for finished software processing on client
     *
     * \return true if finished
     */
    bool CheckForFinishedSwProcessing();

    /** \brief Check for finished software activation on client
     *
     * \return true if finished
     */
    bool CheckForFinishedSwActivation();

    /**
     * \brief Struct containing the file information
     */
    struct FileInformation
    {
        ara::core::String filename;  // name of software
        ara::core::String pathToFile;
        int64_t size;  // size of software bundle file
        ara::core::String sha1;  // sha1 checksum of software file
        ara::core::String downloadUrl;
    };

    /**
     * \brief Struct containing the deployment information needed to coordinate data providing to update client
     */
    struct DeploymentInformation
    {
        ara::core::Map<ara::ucm::TransferIdType, FileInformation> fileInformationMap;
        ara::core::String updateStatus;
        ara::core::String activateStatus;
        bool swProcessAvailable;
        bool swActivateAvailable;
    };

private:
    /** \brief Extracts ogm field of a given ogm object
     *
     * \param root ogm root object to extract from
     * \param path path to the field which has to be extracted
     * \return ogm field, nullptr if not found
     */
    apd::rest::Pointer<apd::rest::ogm::Field> JsonExtractValue(apd::rest::Pointer<apd::rest::ogm::Object>& root,
        const ara::core::String& path);

    /** \brief Reads status field of a given ogm object
     *
     * \param root ogm root object to search in
     * \param path path to the status field
     * \return content of status field. Returns empty string if status field was not found
     */
    ara::core::String GetStatus(apd::rest::Pointer<apd::rest::ogm::Object>& root, const ara::core::String& path);

    /** \brief Converts TransferIdType to string representation
     *
     * \param id id as TransferIdType
     * \param filename id as TransferIdType
     * \param fileInformation id as TransferIdType
     */
    void GenerateSwPackageUrls(ara::ucm::TransferIdType id,
        ara::core::String filename,
        FileInformation& fileInformation);

    /** \brief Converts TransferIdType to string representation
     *
     * \param id id as TransferIdType
     * \return Id in string representation
     */
    ara::core::String TransferIdToString(ara::ucm::TransferIdType id);

    /** \brief Converts id as string to TransferIdType
     *
     * \param idAsString Id in string representation
     * \return id as TransferIdType
     */
    ara::ucm::TransferIdType StringToTransferId(ara::core::String idAsString);

    /** \brief Sends default server reply to client with empty payload
     *
     * \param reply reply object to use for sending
     * \param httpCode http code to send to client
     *
     */
    void SendDefaultReply(apd::rest::ServerReply& reply, int httpCode);

    /**
     * \brief Mutex for access control of currentStatus
     */
    std::mutex accessDeploymentInformation_;

    /**
     * \brief member variable containing all informationen needed during deployment to update client
     */
    DeploymentInformation deploymentInformation_;

    /**
     * \brief Rest server instance
     */
    apd::rest::Server restServer_;

    ara::log::Logger& logger_{ara::log::CreateLogger("UPUS", "UpdateServer", ara::log::LogLevel::kVerbose)};
};

#endif  // ARA_UCM_PKGMGR_UPDATE_ADAPTER_H_
