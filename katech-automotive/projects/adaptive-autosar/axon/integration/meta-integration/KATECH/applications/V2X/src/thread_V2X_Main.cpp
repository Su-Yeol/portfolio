#include "thread_V2X_Main.h"
#include "dataQueue.h"

namespace v2x
{

std::atomic_bool continueExecution{true};
std::atomic_bool v2x_event_send_flag{false};
katech::v2x_data_Objects v2xData;
std::atomic_uint gJ2735Received_Count_spat{0};

void report_process()
{
    if(gJ2735Received_Count_spat != 0) {
        katech::Log::Info() << "spat Message Received count = " << gJ2735Received_Count_spat;
        gJ2735Received_Count_spat = 0;

    } else {
        katech::Log::Info() << "V2X doesn't receive spat Message!!!";
    }
}

void threadV2XMain()
{
    int mainthread_Loopcount = 0;
    katech::Log::Info() << "V2X Thread Start!!";

    v2x::Thread_V2XReceiver::getInstance();
    v2x::Thread_V2XSender::getInstance();

    katech::Log::Info() << "V2X Thread Initialized!!";

    while(v2x::continueExecution) {
        mainthread_Loopcount++;
        katech::Log::Verbose() << "[V2X] Application loop";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        if(mainthread_Loopcount >= 10) {
            report_process();
            mainthread_Loopcount = 0;
        }
    }
}

}
