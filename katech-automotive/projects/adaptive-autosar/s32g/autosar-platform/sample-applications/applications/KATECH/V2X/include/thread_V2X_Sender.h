#ifndef __THREAD_V2X_SENDER_SENDER_H__
#define __THREAD_V2X_SENDER_SENDER_H__

#include <thread>

#include "v2x_data_provider.h"
#include "main_v2x.h"

namespace v2x
{

class Thread_V2XSender
{

public:
    static std::shared_ptr<Thread_V2XSender> getInstance();
    Thread_V2XSender();
    ~Thread_V2XSender();

protected:

private:
    static void thread_internalBridge();

    adcm::V2XData_Provider mV2X_provider;
    std::thread mThreadHandle;
    bool mThreadAlive;

};

}

#endif
