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

#include <sstream>
#include <fstream>
#include "ara/core/string.h"  // ara::core::String, std::stoull
#include "UpdateServer.h"

using namespace apd::rest;
using namespace std::placeholders;

const ara::core::String UpdateListUrl = "/updatelist";
const ara::core::String ActivateUpdateUrl = "/activate_updates";

const ara::core::String FeedbackActivateUrl = "/feedback/activate";
const ara::core::String FeedbackUpdateUrl = "/feedback/download";
const ara::core::String DownloadUrl = "/download/";

const ara::core::String FeedbackDownloadWildcardUrl = DownloadUrl + "*";

UpdateServer::UpdateServer()
    : restServer_("UpdateServer",
        Router{Route{RequestMethod::kGet,
                   Pattern(UpdateListUrl.c_str()),
                   ([this](const Route& route, const ServerRequest& request, ServerReply& reply) {
                       return UpdateListGetHandler(route, request, reply);
                   })},
            Route{RequestMethod::kGet,
                Pattern(ActivateUpdateUrl.c_str()),
                ([this](const Route& route, const ServerRequest& request, ServerReply& reply) {
                    return ActivateUpdateGetHandler(route, request, reply);
                })},
            Route{RequestMethod::kGet,
                Pattern(FeedbackDownloadWildcardUrl.c_str()),
                ([this](const Route& route, const ServerRequest& request, ServerReply& reply) {
                    return DownloadGetHandler(route, request, reply);
                })},
            Route{RequestMethod::kPost,
                Pattern(FeedbackActivateUrl.c_str()),
                ([this](const Route& route, const ServerRequest& request, ServerReply& reply) {
                    return FeedbackActivatePostHandler(route, request, reply);
                })},
            Route{RequestMethod::kPost,
                Pattern(FeedbackUpdateUrl.c_str()),
                ([this](const Route& route, const ServerRequest& request, ServerReply& reply) {
                    return FeedbackUpdatePostHandler(route, request, reply);
                })}})
{
    auto srvTask = restServer_.Start(apd::rest::StartupPolicy::kDetached);
    deploymentInformation_.swProcessAvailable = false;
    deploymentInformation_.swActivateAvailable = false;
    logger_.LogInfo() << "Rest Server Start";
}

Route::Upshot UpdateServer::UpdateListGetHandler(const Route& route, const ServerRequest& request, ServerReply& reply)
{
    (void)route;
    (void)request;
    logger_.LogInfo() << "UpdateListGetHandler";
    std::lock_guard<std::mutex> guard(accessDeploymentInformation_);
    if (deploymentInformation_.swProcessAvailable) {
        Pointer<apd::rest::ogm::Object> rootObj = apd::rest::ogm::Object::Make();
        Pointer<ogm::Array> updatesArray = ogm::Array::Make();

        for (auto x : deploymentInformation_.fileInformationMap) {
            Pointer<apd::rest::ogm::Object> downloadObj = apd::rest::ogm::Object::Make();
            downloadObj->Insert(ogm::Field::Make("download_link", ogm::String::Make((x.second.downloadUrl).c_str())));
            downloadObj->Insert(ogm::Field::Make("id", ogm::String::Make(std::to_string(x.first))));
            updatesArray->Append(std::move(downloadObj));
        }
        rootObj->Insert(ogm::Field::Make("updates", std::move(updatesArray)));
        rootObj->Insert(ogm::Field::Make("feedback_url", ogm::String::Make(FeedbackUpdateUrl.c_str())));
        reply.Send(std::move(rootObj));
        return Route::Upshot::Accept;
    }
    SendDefaultReply(reply, 204);
    return Route::Upshot::Accept;
}

Route::Upshot UpdateServer::ActivateUpdateGetHandler(const Route& route,
    const ServerRequest& request,
    ServerReply& reply)
{
    (void)route;
    (void)request;
    logger_.LogInfo() << "ActivateUpdateGetHandler";
    std::lock_guard<std::mutex> guard(accessDeploymentInformation_);
    if (deploymentInformation_.swActivateAvailable) {
        Pointer<ogm::Object> rootObj = ogm::Object::Make();
        rootObj->Insert(ogm::Field::Make("feedback_url", ogm::String::Make(FeedbackActivateUrl.c_str())));

        reply.Send(std::move(rootObj));
    } else {
        SendDefaultReply(reply, 204);  // HTTP status: No content
    }

    return Route::Upshot::Accept;
}

void UpdateServer::SendDefaultReply(ServerReply& reply, int httpCode)
{
    logger_.LogInfo() << "SendDefaultReply with HTTP status:" << httpCode;
    Pointer<ogm::Object> rootObj = ogm::Object::Make();
    ReplyHeader& header = reply.GetHeader();
    header.SetStatus(httpCode);
    reply.Send(std::move(rootObj));
}

Route::Upshot UpdateServer::DownloadGetHandler(const Route& route, const ServerRequest& request, ServerReply& reply)
{
    (void)route;
    logger_.LogInfo() << "DownloadGetHandler";
    std::lock_guard<std::mutex> guard(accessDeploymentInformation_);
    if (deploymentInformation_.swProcessAvailable) {

        const std::size_t numSegments = request.GetHeader().GetUri().GetPath().NumSegments();

        logger_.LogInfo() << "NumSegments: " << numSegments;
        if (numSegments == 2) {
            logger_.LogInfo() << "Segemt 2:" << request.GetHeader().GetUri().GetPath().GetSegmentsVector()[1].Get();
        }
        ara::ucm::TransferIdType requestedTransferIdDownload
            = StringToTransferId(request.GetHeader().GetUri().GetPath().GetSegmentsVector()[1].Get());

        if (deploymentInformation_.fileInformationMap.count(requestedTransferIdDownload)) {
            logger_.LogInfo() << "Found requested Id in list";

            const ara::core::String pathToFile
                = deploymentInformation_.fileInformationMap[requestedTransferIdDownload].pathToFile;
            logger_.LogInfo() << "Opening file for transfer:" << pathToFile;
            std::stringstream buffer;
            std::ifstream ifs(pathToFile.c_str(), std::ifstream::in | std::ifstream::binary);
            if (ifs.is_open()) {
                logger_.LogInfo() << "File opened, sending file data";
                buffer << ifs.rdbuf();
                ifs.close();
                reply.Send(buffer.str());
            } else {
                logger_.LogInfo() << "File opened failed";
                SendDefaultReply(reply, 500);  // internal server error
            }
        }
    } else {
        SendDefaultReply(reply, 204);  // HTTP status: No content
    }

    return Route::Upshot::Accept;
}

Route::Upshot UpdateServer::FeedbackUpdatePostHandler(const Route& route,
    const ServerRequest& request,
    ServerReply& reply)
{
    (void)route;
    logger_.LogInfo() << "FeedbackUpdatePostHandler";

    std::lock_guard<std::mutex> guard(accessDeploymentInformation_);
    const auto& requestObj = request.GetObject().get();
    Pointer<apd::rest::ogm::Object> rootObj = ogm::Copy(&requestObj);
    deploymentInformation_.updateStatus = GetStatus(rootObj, "status");
    logger_.LogInfo() << "Got client Status" << deploymentInformation_.updateStatus;

    deploymentInformation_.swProcessAvailable = false;

    SendDefaultReply(reply, 200);  // HTTP status: No content
    return Route::Upshot::Accept;
}

Route::Upshot UpdateServer::FeedbackActivatePostHandler(const Route& route,
    const ServerRequest& request,
    ServerReply& reply)
{
    (void)route;
    logger_.LogInfo() << "FeedbackActivatePostHandler";

    std::lock_guard<std::mutex> guard(accessDeploymentInformation_);
    const auto& requestObj = request.GetObject().get();
    Pointer<apd::rest::ogm::Object> rootObj = ogm::Copy(&requestObj);
    deploymentInformation_.activateStatus = GetStatus(rootObj, "status");
    logger_.LogInfo() << "Got client Status" << deploymentInformation_.activateStatus;

    deploymentInformation_.swActivateAvailable = false;

    SendDefaultReply(reply, 200);  // HTTP status: No content
    return Route::Upshot::Accept;
}

ara::core::String UpdateServer::TransferIdToString(ara::ucm::TransferIdType id)
{
    std::ostringstream os;
    os << id;
    return os.str();
}

ara::ucm::TransferIdType UpdateServer::StringToTransferId(ara::core::String idAsString)
{
    const ara::ucm::TransferIdType value = std::stoull(idAsString.c_str());
    return value;
}

void UpdateServer::AddSoftwarePackage(ara::ucm::TransferIdType id,
    const ara::core::String& fileName,
    const ara::core::String& pathToFile)
{
    logger_.LogInfo() << "AddSoftwarePackage";
    std::lock_guard<std::mutex> guard(accessDeploymentInformation_);

    FileInformation fileInformation;
    fileInformation.filename = fileName;
    fileInformation.pathToFile = pathToFile;

    fileInformation.downloadUrl = DownloadUrl + TransferIdToString(id);

    deploymentInformation_.fileInformationMap.insert(std::make_pair(id, fileInformation));

    logger_.LogInfo() << "Added software with id:" << TransferIdToString(id) << "Filename:" << fileName;
    logger_.LogInfo() << "path to file:" << pathToFile << "download url:" << fileInformation.downloadUrl;
}

apd::rest::Pointer<apd::rest::ogm::Field> UpdateServer::JsonExtractValue(
    apd::rest::Pointer<apd::rest::ogm::Object>& root,
    const ara::core::String& path)
{
    ara::core::String copyPath(path);
    const ara::core::String delimiter = ".";
    std::size_t pos = 0;
    Pointer<ogm::Object> value = Copy(root);
    Pointer<ogm::Field> field = NULL;

    auto newValue = value->Find((copyPath.substr(0, pos)).c_str());

    while ((pos = copyPath.find(delimiter)) != ara::core::String::npos) {
        newValue = value->Find((copyPath.substr(0, pos)).c_str());

        if (newValue == value->GetFields().end()) {
            return nullptr;
        }
        copyPath.erase(0, pos + delimiter.length());

        ogm::Visit(&((*newValue).GetValue()),
            [&value](const ogm::Object* object_with_in_field) { value = Copy(object_with_in_field); });
    }

    newValue = value->Find(copyPath.c_str());

    if (newValue == value->GetFields().end()) {
        return nullptr;
    }

    return apd::rest::ogm::Copy(&(*newValue));
}

ara::core::String UpdateServer::GetStatus(apd::rest::Pointer<apd::rest::ogm::Object>& root,
    const ara::core::String& path)
{
    Pointer<ogm::Field> field = JsonExtractValue(root, path);
    Pointer<ogm::String> content;

    if (field == nullptr) {
        return nullptr;
    }

    ogm::Visit(&field->GetValue(), [&content](ogm::String* str) { content = Copy(str); });
    if (content == nullptr) {
        return ara::core::String("");
    }
    return ara::core::String(content->GetValue());
}

void UpdateServer::ProcessSoftwarePackages()
{
    std::lock_guard<std::mutex> guard(accessDeploymentInformation_);
    deploymentInformation_.swProcessAvailable = true;
}

void UpdateServer::ActivateSoftwarePackages()
{
    std::lock_guard<std::mutex> guard(accessDeploymentInformation_);
    deploymentInformation_.swActivateAvailable = true;
}

bool UpdateServer::CheckForFinishedSwProcessing()
{
    std::lock_guard<std::mutex> guard(accessDeploymentInformation_);
    // Todo error handling
    if (deploymentInformation_.updateStatus == "ok" || deploymentInformation_.updateStatus == "failed") {
        logger_.LogInfo() << "CheckForFinishedSwProcessing:" << deploymentInformation_.updateStatus;
        deploymentInformation_.swProcessAvailable = false;
        deploymentInformation_.updateStatus = "";
        return true;
    }
    return false;
}

bool UpdateServer::CheckForFinishedSwActivation()
{
    std::lock_guard<std::mutex> guard(accessDeploymentInformation_);
    // Todo error handling
    if (deploymentInformation_.activateStatus == "complete" || deploymentInformation_.activateStatus == "failed") {
        logger_.LogInfo() << "CheckForFinishedSwActivation:" << deploymentInformation_.activateStatus;
        deploymentInformation_.activateStatus = "";
        return true;
    }
    return false;
}

void UpdateServer::RemoveSoftwarePackages()
{
    logger_.LogInfo() << "RemoveSoftwarePackages";
    {
        std::lock_guard<std::mutex> guard(accessDeploymentInformation_);
        deploymentInformation_.fileInformationMap.clear();
    }
    logger_.LogDebug() << "RemoveSoftwarePackages is over";
}
