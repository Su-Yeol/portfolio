#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include <logger.h>
#include <string.h>



#include "udpClient.h"


namespace katech
{
namespace etc
{

udpClient::udpClient(udpClientCallback callback, unsigned short port)
{
    INFO("init start");
    struct sockaddr_in serv_adr;
    // set callback
    cb = callback;
    // udp port open
    mSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(mSocket < 0) {
        ERROR("UDP socket creation error, errono = %d(%s)", errno, strerror(errno));
        return;
    }

    // receive udp
    mPort = port;
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(port);
    INFO("listen port = %d", port);

    if(bind(mSocket, reinterpret_cast<struct sockaddr*>(&serv_adr), sizeof(serv_adr)) < 0) {
        ERROR("bind() error, errono = %d(%s)", errno, strerror(errno));
        return;
    }

    // thread start
    alive = true;
    mThreadUdpClient = std::make_shared<std::thread>(thread_udpClient, this);
    INFO("init complete");
}

udpClient::~udpClient()
{
    ::close(mSocket);
    mThreadUdpClient->join();
}

void udpClient::thread_udpClient(udpClient* instance)
{
    const int buffer_size = 40 * 1024 + 3;
    unsigned char low_buffer[buffer_size];
    struct sockaddr server_addr;
    socklen_t server_addr_size;
    int received_length;
    std::shared_ptr<udpBuffer> merge_buffer = std::make_shared<udpBuffer>();
    unsigned short lastSequence = 0;
    int port = instance->mPort;

    while(instance->alive) {
        received_length = recvfrom(instance->mSocket, reinterpret_cast<char*>(low_buffer), buffer_size, 0, &server_addr, &server_addr_size);
        //INFO("udp low buffer rx[%d/%d] -- %d/%d/%d", merge_buffer->size(), received_length, low_buffer[0], low_buffer[1], low_buffer[2]);

        if(received_length > 0) {
            merge_buffer->insert(merge_buffer->end(), low_buffer, low_buffer + received_length);
            instance->cb(port, merge_buffer);
            merge_buffer = std::make_shared<udpBuffer>();

        } else {
            ERROR("UDP Sending Error = %d,  errono = %d(%s)", received_length, errno, strerror(errno));
        }
    }
}

std::shared_ptr<std::vector<std::string>> udpClient::splitPacket(std::shared_ptr<udpBuffer> buffer, char divider)
{
    std::string received_data = std::string(reinterpret_cast<char*>(&(*buffer)[0]));
    received_data = received_data.substr(0, buffer->size()); // memory reuer error fix
    std::shared_ptr<std::vector<std::string>> result;
    result = std::make_shared<std::vector<std::string>>();
    //INFO("input<%d>[%p](%s) = [%p] %s", received_data.size(), &(*buffer)[0], &(*buffer)[0], received_data.c_str(), received_data.c_str());
    int find_index = 0;
    int start_index = 0;

    while(find_index >= 0) {
        //INFO("index = %d / %d",start_index ,find_index);
        find_index = received_data.find(divider, start_index);

        if(find_index < 0) {
            // can't find
            std::string substr = received_data.substr(start_index);
            result->push_back(substr);
            //INFO("find last index = %d (%s)",start_index ,substr.c_str());

        } else {
            std::string substr = received_data.substr(start_index, find_index - start_index);
            result->push_back(substr);
            //INFO("find[%d/%d] (%s)", start_index, find_index ,substr.c_str());
            start_index = find_index + 1;
        }
    }

    INFO("find string count = %d", result->size());

    for(int i = 0 ; i < result->size(); i++) {
        INFO("Find String[%d] = %s", i, (*result)[i].c_str());
    }

    return result;
}


}
}
