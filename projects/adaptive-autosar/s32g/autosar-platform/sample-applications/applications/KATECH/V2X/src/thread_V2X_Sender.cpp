#include "thread_V2X_Sender.h"
#include "thread_V2X_Main.h"

namespace v2x
{

std::shared_ptr<Thread_V2XSender> Thread_V2XSender::getInstance()
{
    static std::shared_ptr<Thread_V2XSender> static_instance = std::make_shared<Thread_V2XSender>();
    return static_instance;
}

void Thread_V2XSender::thread_internalBridge()
{
    std::shared_ptr<Thread_V2XSender> instance = getInstance();
    while(instance->mThreadAlive) {
        // send data
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if(v2x_event_send_flag){
            instance->mV2X_provider.send(v2xData);
            v2x_event_send_flag = false;
            adcm::Log::Info() << "[V2X] send V2X Data Event ";
        }
    }
}

Thread_V2XSender::Thread_V2XSender()
{
    mV2X_provider.init("V2X/V2X/PPort_v2x_data");
    mThreadAlive = true;
    mThreadHandle = std::thread(thread_internalBridge);
}

Thread_V2XSender::~Thread_V2XSender()
{
    mThreadAlive = false;
    mThreadHandle.join();
}

}
