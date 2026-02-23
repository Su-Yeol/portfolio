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

#include <boost/filesystem.hpp>  // boost::filesystem::exists boost::filesystem::create_directory
#include "apd/rest/serialize/serialize.h"

#include "update_client.h"
#include "json_helper.h"
#include "find_service.h"
#include "vpackage_management_app.h"

#include <thread>

using namespace apd::rest;
using namespace apd::ucm::ota;
using namespace ara::vucm::proxy;

namespace apd
{
namespace ucm
{
namespace ota
{
namespace updateClient
{

UpdateClient::UpdateClient(ara::core::String vin, VehiclePackageManagementApp<VehiclePackageManagementProxy>& app)
    : restClient_("OTA_REST_CLIENT_UPDATE_CLIENT")
    , sleepTime_(UPDATE_CLIENT_INTERVAL_CHECK)
    , logger_(CreateLogger("OUCL", "OTA Update Client Logger", ara::log::LogLevel::kVerbose))
    , vin_(vin)
    , serviceApp_(app)
{
    logger_.LogInfo() << "Initialize UpdateClient";
}

apd::rest::Uri UpdateClient::BuildGetSoftwarePackageURL(const ara::core::String& path)
{
    const ara::core::String urlString = "/" + vin_ + "/" + path;
    logger_.LogInfo() << "URI:" << urlString;
    return Uri(StringView(urlString.c_str()));
}

apd::rest::Uri UpdateClient::BuildPostVehiclePackageURL()
{
    const ara::core::String urlString = "/" + vin_;
    logger_.LogInfo() << "URI:" << urlString;
    return Uri(StringView(urlString.c_str()));
}

apd::rest::Uri UpdateClient::BuildGetReqSoftwarePackageURL(const ara::core::String& path)
{
    const ara::core::String urlString = "/" + path;
    logger_.LogInfo() << "URI:" << urlString;
    return Uri(StringView(urlString.c_str()));
}

bool UpdateClient::GetReqSwPackage(SoftwarePackageInformation& softwarePackageInformation,
    ara::vucm::SwNameVersionVectorType& requiredSoftwarePackages)
{
    if (!(serviceApp_.IsCorrectStatus(ara::vucm::TransferStateType::kIdle))) {
        logger_.LogError() << "Vehicle Managemnent Service is busy!";
        return false;
    }

    ara::vucm::SwNameVersionVectorType availableSoftwarePackages;
    // fill data of availabale sw packages
    for (auto x : softwarePackageInformation) {
        ara::ucm::SwNameVersionType info;
        info.name = x.name;
        info.version = x.softwareCluster.version;
        availableSoftwarePackages.push_back(info);
    }

    logger_.LogInfo() << "UCM master idle";

    requiredSoftwarePackages
        = serviceApp_.GetService()->SwPackageInventory(availableSoftwarePackages).get().requiredSoftwarePackages;

    logger_.LogInfo() << "UCM master requests following packages:";
    for (auto reqSoftwarePackage : requiredSoftwarePackages) {
        logger_.LogInfo() << "Name:" << reqSoftwarePackage.name;
        logger_.LogInfo() << "Version:" << reqSoftwarePackage.version;
    }

    return true;
}

int UpdateClient::RestRequest(const apd::rest::RequestMethod& method,
    const apd::rest::Uri& url,
    bool request,
    apd::rest::Pointer<apd::rest::ogm::Object>& restRequest,
    bool response,
    apd::rest::Pointer<apd::rest::ogm::Object>& restResponse)
{
    int httpCode = 0;

    Pointer<Request> serverRequest = nullptr;

    if (request) {
        serverRequest = std::make_unique<Request>(method, url, std::move(restRequest));
    } else {
        serverRequest = std::make_unique<Request>(method, url);
    }

    RequestHeader& header = serverRequest->GetHeader();
    // Setup request headers
    header.SetField("Accept", "application/json;charset=UTF-8");

    if (request) {
        header.SetField("Content-Type", "application/json;charset=UTF-8");
    }

    try {
        // perform request
        auto task = restClient_.Send(*serverRequest);
        Pointer<Reply> rep = task.get();
        auto& responseHeader = rep->GetHeader();

        httpCode = responseHeader.GetStatus();

        logger_.LogInfo() << "Status: " << httpCode;

        if (httpCode == 200) {
            if (response) {
                const auto& rootObj = rep->GetObject().get();
                restResponse = ogm::Copy(&rootObj);
            }
        } else {
            logger_.LogError() << "HTTP request failed: " << httpCode;
        }
    } catch (const std::exception& e) {
        logger_.LogError() << "HTTP request failed by throwing an exception: " << e.what();
    }

    return httpCode;
}

bool UpdateClient::GetPackageFromServer(const apd::rest::RequestMethod& method,
    bool request,
    apd::rest::Pointer<apd::rest::ogm::Object>& restRequest,
    const apd::rest::Uri& downloadUrl,
    const ara::core::String& file,
    int& httpCode)
{
    logger_.LogInfo() << "Get " << downloadUrl.GetPath().GetAsString() << " / " << file;

    Pointer<Request> serverRequest;
    if (request) {
        serverRequest = std::make_unique<Request>(method, downloadUrl, std::move(restRequest));
    } else {
        serverRequest = std::make_unique<Request>(method, downloadUrl);
    }

    // Setup request headers
    RequestHeader& header = serverRequest->GetHeader();
    header.SetField("Accept", "application/octet-stream");

    // perform request
    auto task = restClient_.Send(*serverRequest);
    Pointer<Reply> rep = task.get();
    auto& responseHeader = rep->GetHeader();

    const auto& data = rep->ReleaseBinary().get();

    httpCode = responseHeader.GetStatus();

    if (httpCode == 200) {
        logger_.LogInfo() << "Download complete, binary data of length: " << data->size();

        FILE* fp = fopen(file.c_str(), "wb");
        if (fp == nullptr) {
            logger_.LogError() << "Failed to open file for download: " << file;
            return false;
        }

        constexpr size_t count = 1;
        if (fwrite(data->c_str(), data->size(), count, fp) != count) {
            logger_.LogError() << "Writing to file failed!\n";
            fclose(fp);
            return false;
        }
        fclose(fp);
        return true;
    } else {
        logger_.LogError() << "HTTP request failed, Code: " << httpCode << "\n";
    }
    return false;
}

void UpdateClient::PrintSoftwarePackageInfo(SoftwarePackage& softwarePackage)
{
    logger_.LogInfo() << "fqn" << softwarePackage.fqn;
    logger_.LogInfo() << "name" << softwarePackage.name;
    logger_.LogInfo() << "softwareCluster.fqn" << softwarePackage.softwareCluster.fqn;
    logger_.LogInfo() << "softwareCluster.name" << softwarePackage.softwareCluster.name;
    logger_.LogInfo() << "softwareCluster.version" << softwarePackage.softwareCluster.version;
    logger_.LogInfo() << "downloadUrl" << softwarePackage.downloadUrl;
}

bool UpdateClient::GetVehiclePackageFromUpdateServer(ara::core::String path,
    FileInformation& vehiclePackageInformation,
    ara::vucm::SwNameVersionVectorType& requiredSoftwarePackages)
{
    Pointer<ogm::Object> restRequest = nullptr;
    JsonBuildRequiredSoftwarePackages(restRequest, requiredSoftwarePackages);
    logger_.LogInfo() << serialize::Serializer::OgmToJson(Copy(restRequest));
    int status = 0;
    const ara::core::String fileName = {"VPM.zip"};
    const ara::core::String filePath = ara::core::String{path + "/" + fileName};
    vehiclePackageInformation.filename = fileName;
    if (GetPackageFromServer(RequestMethod::kPost, true, restRequest, BuildPostVehiclePackageURL(), filePath, status)) {
        logger_.LogInfo() << "Got answer from server with reduced veh. package info";
        return true;
    }
    return false;
}

bool UpdateClient::GetSwPackagesFromUpdateServer(ara::core::String path,
    ara::core::Vector<FileInformation>& fileInformations,
    SoftwarePackageInformation& availableSoftwarePackages,
    ara::vucm::SwNameVersionVectorType& requiredSoftwarePackages)
{
    for (auto availableSoftwarePackage : availableSoftwarePackages) {
        for (auto requiredSoftwarePackage : requiredSoftwarePackages) {
            if ((requiredSoftwarePackage.name == availableSoftwarePackage.name)
                && requiredSoftwarePackage.version == availableSoftwarePackage.softwareCluster.version) {
                int status = 0;
                const ara::core::String filePath = path + "/" + availableSoftwarePackage.name + ".zip";
                FileInformation fileInformation;
                fileInformation.filename = filePath;
                fileInformations.push_back(fileInformation);
                Pointer<ogm::Object> restRequest = nullptr;
                if (GetPackageFromServer(RequestMethod::kGet,
                        false,
                        restRequest,
                        BuildGetReqSoftwarePackageURL(availableSoftwarePackage.downloadUrl),
                        filePath,
                        status)) {
                    logger_.LogInfo() << "Got SW package:" << requiredSoftwarePackage.name;
                } else {
                    logger_.LogError() << "Download of SW package" << requiredSoftwarePackage.name
                                       << "from server failed!";
                    logger_.LogError() << "Http code:" << status;
                    return false;
                }
            }
        }
    }
    return true;
}

bool UpdateClient::TransferVehiclePackage(ara::core::String const& dataLocation,
    ara::core::String const& vehiclepackage)
{
    logger_.LogInfo() << "Transferring" << dataLocation + "/" + vehiclepackage;
    return serviceApp_.TransferVehiclePackage(dataLocation + "/" + vehiclepackage);
}

bool UpdateClient::TransferSoftwarePackages(ara::core::String const& dataLocation)
{
    bool result = true;

    const auto packages = serviceApp_.GetRequestedPackages();
    for (auto& swPackage : packages) {
        // maybe this should include version information, but would require changes in backend
        logger_.LogInfo() << "Transferring" << dataLocation + "/" + swPackage.name + ".zip";
        result &= serviceApp_.TransferSoftwarePackage(dataLocation, swPackage.name, swPackage.version);
    }
    return result;
}

bool UpdateClient::GetDataFromServer(NewSoftwareDeploymentData& deploymentData,
    SoftwarePackageInformation& softwarePackageInformation,
    ara::vucm::SwNameVersionVectorType& requiredSoftwarePackages)
{
    // deploymentData filled on the fly
    if (!GetVehiclePackageFromUpdateServer(
            deploymentData.filesLocation, deploymentData.vehiclePackage, requiredSoftwarePackages)) {
        logger_.LogError() << "Download of reduced vehicle package from server failed!";
        return false;
    }
    return GetSwPackagesFromUpdateServer(
        deploymentData.filesLocation, deploymentData.swPackages, softwarePackageInformation, requiredSoftwarePackages);
}

bool UpdateClient::Deploy(NewSoftwareDeploymentData& deploymentData)
{
    logger_.LogInfo() << "Send to UCM master";
    if (!TransferVehiclePackage(deploymentData.filesLocation, deploymentData.vehiclePackage.filename)) {
        return false;
    }
    return TransferSoftwarePackages(deploymentData.filesLocation);
}

bool UpdateClient::CreateDirectoryIfNotExist(ara::core::String pathToDir)
{
    if (!boost::filesystem::exists(pathToDir.c_str())) {
        boost::filesystem::create_directory(pathToDir.c_str());
        logger_.LogDebug() << "Dir:" << pathToDir << "is created";
        return true;
    }
    return false;
}

void UpdateClient::CleanDirectory(ara::core::String pathToDir)
{
    logger_.LogInfo() << "Cleaning directory:" << pathToDir;
    boost::filesystem::path path_to_remove(pathToDir.c_str());
    for (boost::filesystem::directory_iterator end_dir_it, it(path_to_remove); it != end_dir_it; ++it) {
        boost::filesystem::remove_all(it->path());
    }
}

void UpdateClient::ServiceRoutine()
{
    while (continueExecution_) {
        // build get tasks URL
        const ara::core::String stringTask = "software_packages_list.json";

        Uri uri = BuildGetSoftwarePackageURL(stringTask);

        Pointer<ogm::Object> restResponse = nullptr;
        Pointer<ogm::Object> restRequest = nullptr;

        logger_.LogInfo() << "Checking for new vehicle package info...";

        const int status = RestRequest(RequestMethod::kGet, uri, false, restRequest, true, restResponse);
        if (status == 200) {
            if (restResponse != nullptr) {
                // restResponse
                logger_.LogInfo() << "Got answer from server";
                SoftwarePackageInformation softwarePackageInformation = GetSoftwarePackageInformation(restResponse);
                for (auto x : softwarePackageInformation) {
                    PrintSoftwarePackageInfo(x);
                }

                logger_.LogInfo() << "Communication to Vehicle Management Service to get req. Sw Packages";

                if (!CreateDirectoryIfNotExist(tempDirectory_)) {
                    CleanDirectory(tempDirectory_);
                }

                ara::vucm::SwNameVersionVectorType requiredSoftwarePackages;
                if (GetReqSwPackage(softwarePackageInformation, requiredSoftwarePackages)) {
                    logger_.LogInfo() << "Got information about Req. sw packages from UCM master";
                    if (!requiredSoftwarePackages.empty()) {
                        logger_.LogInfo() << "download req. sw packages";

                        NewSoftwareDeploymentData deploymentData;
                        deploymentData.filesLocation = tempDirectory_;

                        if (!GetDataFromServer(deploymentData, softwarePackageInformation, requiredSoftwarePackages)) {
                            logger_.LogError() << "Get update data from update server failed!";
                        } else {
                            logger_.LogInfo() << "Deploy new Software...";
                            if (Deploy(deploymentData)) {
                                logger_.LogInfo() << "All software packages have been transferred to UCM Master";
                            } else {
                                logger_.LogError() << "Software deployment failed!";
                            }
                        }
                    } else {
                        logger_.LogInfo() << "No Sw required for download";
                    }
                }
            }
        }
        std::this_thread::sleep_for(sleepTime_);
    }
    restClient_.Stop(apd::rest::ShutdownPolicy::kGraceful).wait();
    logger_.LogInfo() << "Update Client stopped";
}

void UpdateClient::StartServiceSync()
{
    serviceThread_ = std::move(std::thread(&UpdateClient::ServiceRoutine, this));
}

void UpdateClient::StopServiceSync()
{
    logger_.LogInfo() << "Stopping Update Client";
    continueExecution_ = false;
    serviceThread_.join();
}

}  // namespace updateClient
}  // namespace ota
}  // namespace ucm
}  // namespace apd
