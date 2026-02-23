#ifndef __THREAD_V2X_RECEIVER_SENDER_H__
#define __THREAD_V2X_RECEIVER_SENDER_H__

#include <thread>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "main_v2x.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>  // for getopt
#include <iostream>
#include <fstream>

#include "r_type.h"
#include "r_debug.h"
#include "r_mem.h"
#include "r_file.h"
#include "r_asn.h"

#include "asn_codec.h"
#include "spat_decode.h"

// #define DEB_V2X

namespace v2x
{

extern "C" {
    r_ret_t asn_decode_j2735(const u8 *j2735_frame, u32 j2735_frame_len, r_asn_info_t **out_info_frame);
    u8* r_calloc_u8(u32 mem_size);
    s32 r_asn_to_xer(r_asn_info_t *info_frame, u8 *out_xer, u32 out_xer_len);
}

class Thread_V2XReceiver
{

public:
    static std::shared_ptr<Thread_V2XReceiver> getInstance();
    Thread_V2XReceiver();
    ~Thread_V2XReceiver();

protected:

private:
    static void thread_internalBridgeMap();
    static void thread_internalBridgeSpat();

    int init();
    void getSpatXer(const ara::core::String& filePath);
    ara::core::String getMapXer(const ara::core::String& filePath);
    void getSpatMessage(const std::string& message);
    void getSocketInfo(const ara::core::String& filePath);
    void openSocket();

    std::thread mThreadHandleMap;
    std::thread mThreadHandleSpat;

    bool mThreadAlive;
    int mSocket;
    int mPort;
};

}

#endif