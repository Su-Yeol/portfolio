#ifndef __LOGGER_IMG_NATS_H__
#define __LOGGER_IMG_NATS_H__

#include <opencv2/opencv.hpp>
#include <thread>

#include "NatsConnManager.h"

namespace katech
{
class LoggerImage
{


public:
    static std::shared_ptr<LoggerImage> getInstance();
    void connectServer(std::string ip, std::string topic);
    void sendImage(const cv::Mat& image);


private:
    std::shared_ptr<katech::etc::NatsConnManager> natsManager;
    std::string mTopic;
    std::string mURL;
    std::mutex frameQueueMutex; // Mutex for perception_res
    std::deque<std::vector<unsigned char>> perception_res;
    std::shared_ptr<std::thread> mSender;
    const size_t maxFrames = 100;


    static void onMsg(natsConnection * nc, natsSubscription * sub, natsMsg * msg, void * closure);
    static void asyncCb(natsConnection * nc, natsSubscription * sub, natsStatus err, void * closure);
    static void Thread_Sender();

    static std::string vectorToStdString(const std::vector<unsigned char> & data);
};

}
#endif
