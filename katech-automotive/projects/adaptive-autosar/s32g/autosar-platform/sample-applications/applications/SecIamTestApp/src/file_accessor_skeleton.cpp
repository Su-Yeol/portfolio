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

#include "file_accessor_skeleton.h"
#include "ara/core/string.h"
#include "Poco/BinaryWriter.h"

#include <sys/time.h>
#include <sys/types.h>

namespace apd
{
namespace iamtest
{

FileAccessorSkeleton::FileAccessorSkeleton()
{
    mLogger_.LogDebug() << "Application Skeleton created!";
    if (!Connect()) {
        mLogger_.LogError() << "Couldn't establish IPC connect!";
    }
}

FileAccessorSkeleton::~FileAccessorSkeleton()
{
    mLogger_.LogDebug() << "Application Skeleton destroyed. Close IPC connectection.";
    mIPCClientAdapterImpl.Disconnect();
}

bool FileAccessorSkeleton::Connect()
{
    const ara::core::String ipcServerPath = "/usr/run/secdemofc/request_access.sock";
    auto global_timeouts = timeval{2, 0};
    auto type = SOCK_SEQPACKET;

    auto res = mIPCClientAdapterImpl.Connect(ipcServerPath.c_str(), global_timeouts, global_timeouts, type);
    if (!res.HasValue()) {
        mLogger_.LogError() << "Couldn't connect to IPC server:" << res.Error().Message();
        return false;
    }

    return true;
}

bool FileAccessorSkeleton::Send(const ara::core::InstanceSpecifier& instanceSpecifier)
{
    constexpr std::size_t buf_size = 2048;
    char buf[buf_size];

    auto serializedBytes = SerializeMetaModelIdentifier(instanceSpecifier, buf, buf_size);
    if (!serializedBytes.has_value()) {
        mLogger_.LogWarn() << "Couldn't serialize requested object!";
        return false;
    }
    mLogger_.LogDebug() << "Serialized bytes:" << serializedBytes.value_or(0);

    auto res = mIPCClientAdapterImpl.Send(&buf, sizeof(buf));
    if (!res.HasValue()) {
        mLogger_.LogWarn() << "Couldn't send message:" << res.Error().Message();
        return false;
    }

    return true;
}

ara::core::Optional<std::size_t> FileAccessorSkeleton::SerializeMetaModelIdentifier(
    const ara::core::InstanceSpecifier& instanceSpecifier,
    char* buf,
    const std::size_t buf_size) const
{
    Poco::Buffer<char> buffer(buf, buf_size);
    Poco::BasicMemoryBinaryWriter<char> writer(buffer);

    writer << instanceSpecifier.ToString().data();
    if (writer.good()) {
        mLogger_.LogDebug() << "Serialization - Chars written:" << writer.stream().charsWritten();
        return writer.stream().charsWritten();
    }
    return ara::core::nullopt;
}

}  // namespace iamtest
}  // namespace apd
