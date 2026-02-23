#ifndef _THREAD_REPORT_H_
#define _THREAD_REPORT_H_

#include "main_hmi.h"
#include "httprequest.h"
#include "NatsConnManager.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <iostream>
#include <vector>
#include <utility>
#include <sstream>
#include <iomanip> // for std::setprecision
#include <fstream>
#include <string>

#define NATS

namespace v2x
{

class Thread_Report
{

public:
    static std::shared_ptr<Thread_Report> getInstance();
    Thread_Report();
    #ifdef NATS
    Thread_Report(natsMsgHandler onMsgFunc, natsErrHandler asyncFunc);
    #endif
    ~Thread_Report();

protected:

private:

    bool mThreadAlive;
    std::thread mThreadHandle;
    
    ara::core::String serverURL;
    ara::core::String serverIP;
    ara::core::String serverPort;
    ara::core::String serverProtocol;

    natsMsgData msgData;
    
    #ifdef NATS
    natsMsgHandler mOnMsgFunc = NULL;
    natsErrHandler mAsyncFunc = NULL;

    ara::core::String getNatsServerInfo(const ara::core::String& filePath);

    static void innerthread(natsMsgHandler onMsgFunc, natsErrHandler asyncFunc);
    #else
    static void innerthread();
    #endif

};

}

#endif
