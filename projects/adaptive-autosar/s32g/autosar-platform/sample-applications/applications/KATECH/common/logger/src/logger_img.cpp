#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "logger.h"
#include "logger_img.h"
#include "NatsConnManager.h"
#include "base64.h"

namespace adcm
{



void LoggerImage::onMsg(natsConnection* nc, natsSubscription* sub, natsMsg* msg, void* closure)
{
    adcm::Log::Info() << "Received msg: [" << natsMsg_GetSubject(msg) << " : " << natsMsg_GetDataLength(msg) << "]" << natsMsg_GetData(msg);
    // We should be using a mutex to protect those variables since
    // they are used from the subscription's delivery and the main
    // threads. For demo purposes, this is fine.
    getInstance()->natsManager->NatsMsgDestroy(msg);
}



void LoggerImage::asyncCb(natsConnection* nc, natsSubscription* sub, natsStatus err, void* closure)
{
    adcm::Log::Info() << "Async error: " << err << " - " << natsStatus_GetText(err);
    getInstance()->natsManager->NatsSubscriptionGetDropped(sub, const_cast<int64_t*>(&(getInstance()->natsManager->dropped)));
}


std::shared_ptr<LoggerImage> LoggerImage::getInstance()
{
    static std::shared_ptr<LoggerImage> instance = std::make_shared<LoggerImage>();
    return instance;
}



void LoggerImage::connectServer(std::string ip, std::string topic)
{
    natsStatus s = NATS_OK;
    mURL = ip + ":4222";
    mTopic = topic;
    natsManager = std::make_shared<adcm::etc::NatsConnManager>(mURL.c_str(), mTopic.c_str(), onMsg, asyncCb, adcm::etc::NatsConnManager::Mode::Default);
    s = natsManager->NatsExecute();
	if(s == NATS_OK){
    	//natsManager->NatsSleep(1000);
    	mSender = std::make_shared<std::thread>(Thread_Sender);
	}
}

std::string LoggerImage::vectorToStdString(const std::vector<unsigned char>& data)
{
    return std::string(data.begin(), data.end());
}


void LoggerImage::sendImage(const cv::Mat& image)
{
	INFO("sendimg");
    // Convert cv::Mat image to PNG byte array
    std::vector<unsigned char> byteArray;
    cv::imencode(".png", image, byteArray);
    {
        // Lock the perception_res to ensure thread safety
        std::lock_guard<std::mutex> lock(frameQueueMutex);
        // Add the new perception_res to the back of the deque
        perception_res.push_back(byteArray);

        // If the deque size exceeds the maximum frames, remove the oldest frame from the front
        if(perception_res.size() > maxFrames) {
            perception_res.pop_front();
        }
    }
}

void LoggerImage::Thread_Sender()
{
    std::string encodedImage;
    std::string temp;
    std::vector<unsigned char> vectorData;
    bool perception_flag = false;
    natsStatus s = NATS_OK;

    while(true) {
        //INFO("loop...");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        {
            // Lock the perception_res before accessing it to copy the data
            std::lock_guard<std::mutex> lock(getInstance()->frameQueueMutex);

            if(!getInstance()->perception_res.empty()) {
                INFO("current perception res length : %d", getInstance()->perception_res.size());
                // Loop to pop elements from the front of the deque until only the latest frame is left
                vectorData = getInstance()->perception_res.back();
                perception_flag = true;
            }
        }

        if(perception_flag) {
            INFO("Send Image to Nats Server!!!");
            temp = vectorToStdString(vectorData);
            encodedImage = base64_encode(reinterpret_cast<const unsigned char*>(temp.c_str()), temp.length());

            if(s == NATS_OK) {
                INFO("NATS Publish");
                getInstance()->natsManager->ClearJsonData();

                if(encodedImage.empty()) {
                    getInstance()->natsManager->addJsonData("image", "no image");

                } else {
                    getInstance()->natsManager->addJsonData("image", encodedImage);
                }

                getInstance()->natsManager->PrintSendData();
                getInstance()->natsManager->NatsPublishJson(getInstance()->mTopic.c_str());
                // natsManager->NatsSleep(1000);

            } else {
                ERROR("Nats Connection error");
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                try {
                    getInstance()->natsManager = std::make_shared<adcm::etc::NatsConnManager>(
                                      getInstance()->mURL.c_str(), getInstance()->mTopic.c_str(), onMsg, asyncCb, adcm::etc::NatsConnManager::Mode::Default);
                    s = getInstance()->natsManager->NatsExecute();

                } catch(std::exception e) {
                    ERROR("Nats reConnection error");
                }
            }

            // Lock the perception_res again to remove the processed data
            std::lock_guard<std::mutex> lock(getInstance()->frameQueueMutex);
            // Remove the first (latest) perception_res from the deque
            getInstance()->perception_res.clear();
            perception_flag = false;
        }
    }
}


}
