#ifndef _THREAD_V2X_MAIN_H_
#define _THREAD_V2X_MAIN_H_

#include "main_v2x.h"
#include "dataQueue.h"

#include "thread_V2X_Sender.h"
#include "thread_V2X_Receiver.h"

namespace v2x
{
// communication sta tus
extern std::atomic_bool continueExecution;
extern katech::v2x_data_Objects v2xData;
extern std::atomic_uint gJ2735Received_Count_spat;
extern std::atomic_bool v2x_event_send_flag;
extern void threadV2XMain();
}

#endif