#ifndef __KATECH_ETC_UDP_CLIENT_H__
#define __KATECH_ETC_UDP_CLIENT_H__

#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <vector>

namespace katech
{
namespace etc
{
class udpClient
{
public:

    typedef std::vector<unsigned char> udpBuffer;

    typedef void(* udpClientCallback)(int port, std::shared_ptr<udpBuffer> buffer);

    udpClient(udpClientCallback callback, unsigned short port);
    ~udpClient();
    std::shared_ptr<std::vector<std::string>> splitPacket(std::shared_ptr<udpBuffer> buffer, char divider);
private:

    int mSocket;
    int mPort;
    udpClientCallback cb;
    bool alive;
    std::shared_ptr<std::thread> mThreadUdpClient;
    static void thread_udpClient(udpClient* instance);

};
}
}

#endif
