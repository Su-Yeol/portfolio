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

/**
 * \brief   DDS Tcp server
 * \note    Project: Adaptive Platform Test Applications
 *
 * \ingroup DDSApp01
 */

#include "ddsApp01_TcpServer.hpp"
#include "ddsApp01_utils.hpp"

#include <pthread.h>
#include <netinet/in.h>
#include <stdint.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <netinet/tcp.h>
#include <sys/select.h>
#include <stdio.h>

#include <vector>
#include <exception>

#include <iostream>

namespace
{

static ara::log::Logger& GetLogger()
{
    static ara::log::Logger& logger = CreateLogger("DDS", "TCPServer Log", ara::log::LogLevel::kVerbose);
    return logger;
}

}  // namespace

/* Public function definitions --------------------------------------------------------------------*/

ddsApp01TcpServer::ddsApp01TcpServer(const std::int16_t port)
{
    // Open socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        GetLogger().LogError() << "ddsApp01 Socket error";
        return;
    }

    // Bind socket
    bool rv_bind = Bind(port);
    if (rv_bind != true) {
        GetLogger().LogError() << "ddsApp01 Binding error";
        return;
    }

    // Listen by socket
    bool rv_listen = Listen();
    if (rv_listen != true) {
        GetLogger().LogError() << "ddsApp01 Listening error";
        return;
    }

    GetLogger().LogInfo() << " ddsApp01 TCP Socket init complete";
}

ddsApp01TcpServer::~ddsApp01TcpServer(void)
{ }

/* Private function definitions ----------------------------------------------------------------*/

bool ddsApp01TcpServer::Bind(const std::int16_t port)
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    std::int32_t rv_bind;
    rv_bind = bind(sockfd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr));
    if (rv_bind < 0) {
        return false;
    }
    return true;
}

bool ddsApp01TcpServer::Listen(void)
{
    const std::int32_t backlog{5};
    std::int32_t rv_listen = listen(sockfd, backlog);
    if (rv_listen < 0) {
        return false;
    }
    return true;
}

bool ddsApp01TcpServer::Accept(void)
{
    struct timeval tv;
    fd_set readfds;
    int ret_select;

    /* timeout counter */
    tv.tv_sec = 0;
    tv.tv_usec = 400;

    /* clear read FD */
    FD_ZERO(&readfds);

    /* add sockfd */
    FD_SET(sockfd, &readfds);

    ret_select = select(sockfd + 1, &readfds, NULL, NULL, &tv);
    if (ret_select == -1) {
        /* error */
        GetLogger().LogInfo() << " ddsApp01TcpServer:Accept() select error ";
        return false;
    } else if (ret_select == 0) {
        return false;
    } else {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        accepted = accept(sockfd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
        if (accepted < 0) {
            GetLogger().LogInfo() << " ddsApp01TcpServer:Accept()  Accepting error";
            return false;
        }
    }
    return true;
}

void ddsApp01TcpServer::Disconnect(void)
{
    // Deny both reading and writing
    const std::int32_t howto{SHUT_RDWR};

    shutdown(accepted, howto);
    close(accepted);
}

/* Public function definition ----------------------------------------------------------------------- */

bool ddsApp01TcpServer::Run(std::string& request)
{
    // Accept new connection
    bool ret_accept = Accept();
    if (ret_accept != true) {
        return false;
    }

    // Read request line
    bool ret_read = ReadLine(request);
    if (ret_read != true) {
        return false;
    }
    GetLogger().LogInfo() << "readline request = " << request;

    return true;
}

bool ddsApp01TcpServer::ReadLine(std::string& line)
{
    const std::int32_t nbytes = 1024 * 4;
    line.resize(nbytes);
    std::int32_t index = 0;

    std::int32_t nread;
    nread = read(accepted, reinterpret_cast<char*>(&line[index]), nbytes);
    if (nread < 0) {
        GetLogger().LogError() << "ddsApp01TcpServer:ReadLine() Read error";
        return false;
    } else if (nread == 0) {
        GetLogger().LogInfo() << "ddsApp01TcpServer:ReadLine() No Message";
    }

    index += nread;

    line.resize(index);
    GetLogger().LogInfo() << "request number :" << line;
    return true;
}
