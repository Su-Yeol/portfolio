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

#include "smtool_client.h"
#include "logger.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <sys/fcntl.h>
#include <sys/poll.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

namespace apd
{
namespace smtool
{

namespace
{

auto& GetLogger()
{
    static Logger logger("SmtoolClient", LogLevel::kTrace);
    return logger;
}

constexpr const char* kInputPipePath = kSmtoolServerToClientPipePath;
constexpr const char* kOutputPipePath = kSmtoolClientToServerPipePath;

}  // namespace

SmtoolClient::~SmtoolClient()
{
    if ((inputPipeFd_ != kInvalidFileDescriptor) && (::close(inputPipeFd_) != 0)) {
        GetLogger().LogError() << "Failed to close input pipe\n";
    }

    if ((outputPipeFd_ != kInvalidFileDescriptor) && (::close(outputPipeFd_) != 0)) {
        GetLogger().LogError() << "Failed to close output pipe\n";
    }
}

ara::core::String SmtoolClient::ReceiveResponse()
{
    GetLogger().LogTrace() << __FUNCTION__ << " called\n";

    OpenInputPipe();

    if (inputPipeFd_ == kInvalidFileDescriptor) {
        GetLogger().LogError() << "Failed to receive message, input pipe is closed\n";
        return "";
    }

    constexpr int kPollTimeoutMs = 35000;
    constexpr nfds_t kFileDescriptorsCount = 1u;
    pollfd pollFileDescriptor{};
    pollFileDescriptor.fd = inputPipeFd_;
    pollFileDescriptor.events = POLLIN;

    GetLogger().LogInfo() << "Waiting on response from the State Management....\n";
    int pollResult = ::poll(&pollFileDescriptor, kFileDescriptorsCount, kPollTimeoutMs);

    if (pollResult <= 0) {
        GetLogger().LogError() << "Failed to receive message, poll() timed-out or an error occurred\n";
        return "";
    }

    return ReadMessage();
}

void SmtoolClient::OpenInputPipe()
{
    if (inputPipeFd_ != kInvalidFileDescriptor) {
        GetLogger().LogDebug() << "Input pipe is already open\n";
        return;
    }

    constexpr unsigned int kMaxAttempts = 10u;
    unsigned int attemptsMade = 0u;

    while (attemptsMade < kMaxAttempts) {
        int inputPipeFlags = (O_RDONLY | O_NONBLOCK);
        inputPipeFd_ = ::open(kInputPipePath, inputPipeFlags);

        if (inputPipeFd_ != kInvalidFileDescriptor) {
            // remove O_NONBLOCK flag
            inputPipeFlags &= ~O_NONBLOCK;
            if (::fcntl(inputPipeFd_, F_SETFL, inputPipeFlags) == -1) {
                GetLogger().LogError() << "Failed to change input pipe flags: " << ::strerror(errno) << "\n";
            }
            break;
        } else {
            if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
                GetLogger().LogDebug() << "EWOULDBLOCK/EAGAIN in input pipe\n";
            } else {
                GetLogger().LogError() << "Failed to open input pipe: " << ::strerror(errno) << "\n";
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        ++attemptsMade;
    }
}

ara::core::String SmtoolClient::ReadMessage() const
{
    constexpr size_t kBufferSize = 100u;
    char buffer[kBufferSize] = {};
    ara::core::String message;

    GetLogger().LogDebug() << "Reading response from the State Management....\n";
    while (true) {
        const ssize_t bytesRead = ::read(inputPipeFd_, buffer, kBufferSize);

        if (bytesRead == -1) {
            GetLogger().LogError() << "Failed to receive message, reading from pipe failed\n";
            return "";
        }

        if ((bytesRead == 0) && message.empty()) {
            // pipe is closed at the other side or there is no new message
            GetLogger().LogTrace() << "There is no any new message in the pipe\n";
            return "";
        }

        ara::core::StringView messageChunk(buffer, bytesRead);
        message += messageChunk;

        if (static_cast<size_t>(bytesRead) < kBufferSize) {
            // end of message
            break;
        }
    }

    return message;
}

bool SmtoolClient::SendRequest(const ara::core::String& message)
{
    GetLogger().LogTrace() << __FUNCTION__ << " called\n";

    if (outputPipeFd_ == kInvalidFileDescriptor) {
        GetLogger().LogInfo() << "Opening pipe to State Management (" << kOutputPipePath
                              << "), this will block until SM is reachable\n";
        outputPipeFd_ = ::open(kOutputPipePath, O_WRONLY);

        if (outputPipeFd_ == kInvalidFileDescriptor) {
            GetLogger().LogError() << "Failed to send message, cannot open output pipe: " << kOutputPipePath
                                   << " because of: " << ::strerror(errno);
            return false;
        }
    }

    const ssize_t bytesWritten = ::write(outputPipeFd_, message.data(), message.size());

    if (static_cast<size_t>(bytesWritten) != message.size()) {
        GetLogger().LogError() << "Failed to send message, writing to pipe failed\n";
        return false;
    }
    return true;
}

}  // namespace smtool
}  // namespace apd
