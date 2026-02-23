#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include "logger.h"
#include "NatsConnManager.h"

#include <queue>
#include <map>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Object.h>
#include <Poco/Dynamic/Var.h>
#include <condition_variable>

#include "bridge_control_provider.h"
#include "control_bridge_subscriber.h"

// #define HMI_SERVER_URL "nats://nats.beyless.com:4222"
#define HMI_SERVER_URL "https://nats.beyless.com"
// #define HMI_SERVER_URL "https://nats.beyless.com:8443"

extern std::atomic_bool continueExecution;
extern std::atomic_uint gReceivedEvent_count_control_bridge;
extern std::atomic_uint gReceivedEvent_count_nats;
extern std::atomic_uint gMainthread_Loopcount;

std::shared_ptr<adcm::etc::NatsConnManager> natsManager;
std::mutex gMTX_remote_control;  // 뮤텍스 선언
std::queue<std::string> gJsonQueue_remote_control;
std::condition_variable gCV_remote_control;  // condition variable 선언

void asyncCb(natsConnection* nc, natsSubscription* sub, natsStatus err, void* closure)
{
    std::cout << "Async error: " << err << " - " << natsStatus_GetText(err) << std::endl;
    natsManager->NatsSubscriptionGetDropped(sub, (int64_t*)&natsManager->dropped);
}

void processJsonQueue() 
{
    Poco::JSON::Parser parser;
    adcm::BridgeControl_Provider bridgeControl_provider;
    bridgeControl_provider.init("Katech_Bridge/Katech_Bridge/PPort_bridge_control"); // NATS -> AXON
    std::string jsonStr;
    unsigned long int old_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    unsigned long long timestamp_nats = 0;

    while (continueExecution)
    {
        // wait event
        {
            std::unique_lock<std::mutex> lock(gMTX_remote_control);
            bool rx_event = gCV_remote_control.wait_for(lock, std::chrono::milliseconds(120), [] { return !gJsonQueue_remote_control.empty(); }); // 큐에 데이터가 있을 때까지 대기

            if(!rx_event)
            {
                INFO("[REMOTE] Nats event timeout!!!");
                continue;
            }
            else
            {
                while (!gJsonQueue_remote_control.empty())
                {
                    jsonStr = gJsonQueue_remote_control.front();
                    gJsonQueue_remote_control.pop();
                    // lock은 이후에 unlock()을 통해 해제할 수 있습니다.
                }
            }

            lock.unlock();
        }
        // parse json
        gReceivedEvent_count_nats++;
        Poco::Dynamic::Var result = parser.parse(jsonStr);
        Poco::JSON::Object::Ptr jsonObject = result.extract<Poco::JSON::Object::Ptr>();
        std::map<std::string, Poco::Dynamic::Var> jsonMap;

        for (const auto& key : jsonObject->getNames())
        {
            jsonMap[key] = jsonObject->get(key);
        }

        // for (const auto& pair : jsonMap)
        // {
        //     INFO("%s : %s", pair.first.c_str(), pair.second.toString().c_str());
        // }
        double steering = std::stod(jsonMap["steering"].toString());
        double acc = std::stod(jsonMap["acc"].toString());
        double breaking = std::stod(jsonMap["break"].toString());
        std::string turnsignal = jsonMap["turnsignal"].toString();
        bool remote_enable = jsonMap["remote_enable"].toString() == "true";
        bool enable = jsonMap["enable"].toString() == "true";
        unsigned long long timestamp = std::stoi(jsonMap["timestamp"].toString());
        adcm::bridge_control_Objects bridgeControl;
        bridgeControl.Acc = acc;
        bridgeControl.Break = breaking;
        bridgeControl.Steer = steering;
        bridgeControl.Enable = enable;
        bridgeControl.RemoteEnable = remote_enable;  
        bridgeControl.Turn_signal = turnsignal == "L" ? 1 :
                                    turnsignal == "R" ? 2 :
                                    turnsignal == "E" ? 3 :
                                    0;
        unsigned long int new_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        unsigned long int diff_time = new_time - old_time;
        unsigned long long diff_nats_event = timestamp - timestamp_nats;
        old_time = new_time;
        timestamp_nats = timestamp;
        bridgeControl_provider.send(bridgeControl); // AXON -> S32G
        INFO("bridgeControl Send time : %3ldms , nats event time : %3lldms", diff_time, diff_nats_event);

        // INFO("steering : %lf", steering);
        // INFO("acc : %lf", acc);
        // INFO("break : %lf", breaking);
        // INFO("turnsignal : %s", turnsignal.c_str());
        // INFO("enable : %d", enable);
        // INFO("remote_enable : %d", remote_enable);
        // INFO("timestamp : %ld", timestamp);
        // INFO("--------------------------------");
                
        adcm::Log::Info() <<"[From NATS] ACC" << bridgeControl.Acc;
        adcm::Log::Info() <<"[From NATS] Break" << bridgeControl.Break;
        adcm::Log::Info() <<"[From NATS] Steer" << bridgeControl.Steer;
        adcm::Log::Info() <<"[From NATS] RemoteEnable" << bridgeControl.RemoteEnable;
        adcm::Log::Info() <<"[From NATS] Enable" << bridgeControl.Enable;
        adcm::Log::Info() <<"[From NATS] Turn_signal" << bridgeControl.Turn_signal;
    }
}

void onMsg(natsConnection* nc, natsSubscription* sub, natsMsg* msg, void* closure)
{
    const char* subject = NULL;
    // 뮤텍스를 사용하여 공유 변수 접근 보호
    subject = natsMsg_GetSubject(msg);
    // INFO("Received msg: [ %s[%d] = %s]",  subject, natsMsg_GetDataLength(msg), natsMsg_GetData(msg));
    {
        std::lock_guard<std::mutex> lock(gMTX_remote_control);
        std::string jsonStr = natsMsg_GetData(msg);
        gJsonQueue_remote_control.push(jsonStr);
        gCV_remote_control.notify_one();  // 새 데이터가 추가되었음을 알림
    }
    natsManager->NatsMsgDestroy(msg);
}


int main_thread()
{
    natsStatus s = NATS_OK;
    std::thread processJsonQueueThread(processJsonQueue);
    // const char*  subject = "sensorData.*";
    std::vector<const char*> subject = {"katech.remote.control"};
    natsManager = std::make_shared<adcm::etc::NatsConnManager>(
                      HMI_SERVER_URL, subject, onMsg, asyncCb, adcm::etc::NatsConnManager::Mode::Default);
    s = natsManager->NatsExecute();
    adcm::Log::Info() << "Katech_Bridge ThreadReceiveControlBridge Start!!";
    adcm::ControlBridge_Subscriber controlBridge_subscriber;
    controlBridge_subscriber.init("Katech_Bridge/Katech_Bridge/RPort_control_bridge"); // AXON -> NATS
    unsigned long int old_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    while(continueExecution)
    {
        gMainthread_Loopcount++;
        adcm::Log::Verbose() << "[Katech_Bridge] ThreadReceiveControlBridge loop";
        bool controlBridge_rxEvent = controlBridge_subscriber.waitEvent(120); // wait event

        if(controlBridge_rxEvent)
        {
            unsigned long int new_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            unsigned long int diff_time = new_time - old_time;
            old_time = new_time;
            INFO("controlBridge_rxEvent : %ldms", diff_time);

            while(!controlBridge_subscriber.isEventQueueEmpty())
            {
                auto data = controlBridge_subscriber.getEvent();
                gReceivedEvent_count_control_bridge++;

                if(s == NATS_OK)
                {
                    double Acc = data->Acc;
                    double Break = data->Break;
                    double Steer = data->Steer;
                    bool RemoteEnable = data->RemoteEnable;
                    bool Enable = data->Enable;
                    char Turn_signal = data->Turn_signal;
                    const char* pubSubject = "katech.remote.status";
                    // INFO("NATS Publish : JON");
                    adcm::Log::Info() <<"[To NATS] ACC" << data->Acc;
                    adcm::Log::Info() <<"[To NATS] Break" << data->Break;
                    adcm::Log::Info() <<"[To NATS] Steer" << data->Steer;
                    adcm::Log::Info() <<"[To NATS] RemoteEnable" << data->RemoteEnable;
                    adcm::Log::Info() <<"[To NATS] Enable" << data->Enable;
                    adcm::Log::Info() <<"[To NATS] Turn_signal" << data->Turn_signal;
                    natsManager->ClearJsonData();
                    natsManager->addJsonData("acc", Acc);
                    natsManager->addJsonData("break", Break);
                    natsManager->addJsonData("steer", Steer);
                    natsManager->addJsonData("remote_enable", RemoteEnable);
                    natsManager->addJsonData("enable", Enable);
                    natsManager->addJsonData("turn_signal", Turn_signal);
                    natsManager->addJsonData("timestamp", std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()));
                    // natsManager->PrintSendData();
                    natsManager->NatsPublishJson(pubSubject);
                }
                else
                {
                    INFO("Nats Connection error");

                    try
                    {
                        natsManager = std::make_shared<adcm::etc::NatsConnManager>(
                                          HMI_SERVER_URL, subject, onMsg, asyncCb, adcm::etc::NatsConnManager::Mode::Default);
                        s = natsManager->NatsExecute();
                    }
                    catch (std::exception e)
                    {
                        INFO("Nats reConnection error");
                    }
                }
            }
        }
        else
        {
            INFO("controlBridge_rxEvent : time out!!!");
        }
    }

    processJsonQueueThread.join();
}