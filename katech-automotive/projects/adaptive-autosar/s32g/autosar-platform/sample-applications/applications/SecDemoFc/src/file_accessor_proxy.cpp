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

#include "file_accessor_proxy.h"
#include "error_domain.h"
#include "aa_ipc_parameter.h"
#include "Poco/BinaryReader.h"
#include "ara/core/error_code.h"

#include <thread>
#include <chrono>
#include <string>

namespace apd
{
namespace demofc
{

FileAccessorProxy::FileAccessorProxy(std::unique_ptr<ara::iam::ifc::ipc::IPCServerAdapterImpl> serverAdapter)
    : mIPCServerAdapterImpl(std::move(serverAdapter))
{
    mClientSocketDescriptor = mIPCServerAdapterImpl->EstablishNewClientConnection(kServerTimeout);

    if (mClientSocketDescriptor <= 0) {
        mLogger_.LogError() << "Error during establishing client connection:" << mClientSocketDescriptor;
    }
}

FileAccessorProxy::~FileAccessorProxy()
{
    mLogger_.LogInfo() << "Drop connection to AA IPC server.";

    mIPCServerAdapterImpl->CloseClientConnection(mClientSocketDescriptor);
    mIPCServerAdapterImpl->CloseServerConnection();
}

bool FileAccessorProxy::InitServerAdapter(ara::iam::ifc::ipc::IPCServerAdapterInterface& serverAdapter)
{
    ara::log::Logger& logger
        = ara::log::CreateLogger("FACC", "FileAccessorProxy InitServerAdapter", ara::log::LogLevel::kInfo);
    logger.LogInfo() << "DemoFC trying to open IPC server" << kIPCServerPath.c_str();

    return serverAdapter.ConnectServerSocket(
        kIPCServerPath.c_str(), kServerTimeout, kServerTimeout, kSocketType, kBlock);
}

void FileAccessorProxy::HandleAccessFileRequest()
{
    auto rpid = mIPCServerAdapterImpl->GetPeerPID(mClientSocketDescriptor);
    if (!rpid.HasValue()) {
        mLogger_.LogError() << "Error receiving Client PID" << rpid.Error().Message();
        return;
    }
    mLogger_.LogInfo() << "Client PID:" << rpid.Value();

    constexpr std::size_t buf_size = 2048;
    char recvBuf[buf_size];

    auto rec_data = mIPCServerAdapterImpl->Receive(recvBuf, buf_size, mClientSocketDescriptor);
    if (!rec_data.HasValue()) {
        mLogger_.LogError() << "No data received:" << rec_data.Error().Message();
        return;
    }
    if (rec_data.Value() < 0) {
        mLogger_.LogError() << "Invalid data received:" << rec_data.Error().Message();
        return;
    }

    auto fileInstanceSpecifier = DeserializeIPCMessage(recvBuf, static_cast<std::size_t>(rec_data.Value()));
    if (!fileInstanceSpecifier.HasValue()) {
        mLogger_.LogError() << "Deserialize Error";
        return;
    }

    auto res_identification = pep.EnforceAccessControl(rpid.Value(), fileInstanceSpecifier.Value());
    PlotAccessEnforcement(res_identification);
}

ara::core::Result<ara::core::InstanceSpecifier> FileAccessorProxy::DeserializeIPCMessage(const char* buf,
    const std::size_t buf_length)
{
    Poco::Buffer<char> buffer(buf, buf_length);
    Poco::BasicMemoryBinaryReader<char> reader(buffer);

    std::string fileMetModelIdentifier;
    reader >> fileMetModelIdentifier;
    if (!reader.good()) {
        mLogger_.LogError() << "Reader Error!";
        return ara::core::Result<ara::core::InstanceSpecifier>::FromError(DemoFCErrc::kReaderError);
    }
    mLogger_.LogDebug() << "Received requested resource:" << fileMetModelIdentifier;

    const auto fileInstanceSpecifier = ara::core::InstanceSpecifier::Create(fileMetModelIdentifier.c_str());
    if (!fileInstanceSpecifier.HasValue()) {
        mLogger_.LogError() << "Error creating FileInstanceSpecifier:" << fileInstanceSpecifier.Error().Message();
        return ara::core::Result<ara::core::InstanceSpecifier>::FromError(
            DemoFCErrc::kInstanceSpecifierValidationError);
    }

    auto bytes_read = (buf_length - static_cast<std::size_t>(reader.available()));
    mLogger_.LogDebug() << "Bytes read:" << bytes_read;

    return fileInstanceSpecifier.Value();
}

void FileAccessorProxy::PlotAccessEnforcement(bool accessGranted)
{
    accessGranted ? mLogger_.LogInfo() << "Access granted" : mLogger_.LogInfo() << "Access denied";
}

}  // namespace demofc
}  // namespace apd
