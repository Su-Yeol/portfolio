#include "thread_V2X_Main.h"

namespace v2x
{

std::shared_ptr<Thread_V2XReceiver> Thread_V2XReceiver::getInstance()
{
    static std::shared_ptr<Thread_V2XReceiver> static_instance = std::make_shared<Thread_V2XReceiver>();
    return static_instance;
}


void Thread_V2XReceiver::getSpatMessage(const std::string& message)
{
    boost::property_tree::ptree propertyTree;
    std::istringstream spat_message(message);
    boost::property_tree::read_xml(spat_message, propertyTree);

    boost::property_tree::ptree IntersectionState;
    boost::property_tree::ptree states;
    
    katech::movementState movementState;
    std::vector<boost::property_tree::ptree> movementStateVector;

    IntersectionState = propertyTree.get_child("MessageFrame.value.SPAT.intersections.IntersectionState");
    
    for(auto &it : IntersectionState)
    {
        if(it.first == "id"){
#ifdef DEB_V2X
            katech::Log::Info() << it.first << " : " << it.second.get<std::int64_t>("id");
#endif
            v2xData.intersections.id = it.second.get<std::int64_t>("id");
        }else if(it.first == "revision"){
#ifdef DEB_V2X
            katech::Log::Info() << it.first << " : " << it.second.get_value<std::uint8_t>();
#endif
            v2xData.intersections.revision = it.second.get_value<std::uint8_t>();
        }else if(it.first == "status"){
#ifdef DEB_V2X
            katech::Log::Info() << it.first << " : " << it.second.get_value<std::uint64_t>();
#endif
            v2xData.intersections.status = it.second.get_value<std::uint64_t>();
        }else if(it.first == "moy"){
#ifdef DEB_V2X
            katech::Log::Info() << it.first << " : " << it.second.get_value<std::uint64_t>();
#endif
            v2xData.intersections.moy = it.second.get_value<std::uint64_t>();
        }else if(it.first == "timeStamp"){
#ifdef DEB_V2X
            katech::Log::Info() << it.first << " : " << it.second.get_value<std::uint64_t>();
#endif
            v2xData.intersections.timeStamp = it.second.get_value<std::uint64_t>();
        }else{
            v2xData.intersections.states.MovementState.clear();
            states = it.second;
        }
    }

    for(auto &state : states)
    {
        if(state.first == "MovementState")
            movementStateVector.push_back(state.second);
    }

    if(!movementStateVector.empty()) {
#ifdef DEB_V2X
        katech::Log::Info() << "MovementState: ";
#endif
        for(typename std::vector<boost::property_tree::ptree>::iterator itr = movementStateVector.begin(); itr != movementStateVector.end(); ++itr) {
            for(auto &state : *itr){
                if(state.first == "movementName")
                {
                    movementState.movementName = state.second.get_value<std::string>();
#ifdef DEB_V2X
                    katech::Log::Info() <<  state.first << " : " << state.second.get_value<std::string>();
#endif
                }
                else if(state.first == "signalGroup")
                {
                    movementState.signalGroup = state.second.get_value<std::uint8_t>();
#ifdef DEB_V2X
                    katech::Log::Info() <<  state.first << " : " << state.second.get_value<std::uint8_t>();
#endif
                }
                else if(state.first == "state-time-speed")
                {
                    auto movementEvent = state.second.get_child("MovementEvent");
                    auto eventState = state.second.get_child("MovementEvent.eventState");
                    auto timings = state.second.get_child("MovementEvent.timing");


                    for(auto &event : eventState)
                    {
                        movementState.eventState = event.first;
#ifdef DEB_V2X
                        katech::Log::Info() << event.first;
#endif
                    }

                    for(auto &timing : timings){
                        if(timing.first == "minEndTime"){
                            movementState.timing_minEndTime = timing.second.get_value<std::uint16_t>();
#ifdef DEB_V2X
                            katech::Log::Info() << "minEndTime : " << timing.second.get_value<std::uint16_t>();
#endif
                        }else if(timing.first == "maxEndTime"){
                            movementState.timing_maxEndTime = timing.second.get_value<std::uint16_t>();
#ifdef DEB_V2X
                            katech::Log::Info() << "maxEndTime : " <<timing.second.get_value<std::uint16_t>();
#endif
                        }
                    }
                }
            }
            v2xData.intersections.states.MovementState.push_back(movementState);
        }
    } else {
        katech::Log::Info() << "movementStateVector empty!!! ";
    }

    v2x_event_send_flag = true;

}

void Thread_V2XReceiver::openSocket()
{
    katech::Log::Info() << "open Socket...";
    struct sockaddr_in serv_adr;
    mSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if(mSocket < 0) {
        ERROR("UDP socket creation error, errono = %d(%s)", errno, strerror(errno));
        return;
    }

    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(mPort);
    katech::Log::Info() << "listening port : " << mPort;

    if(bind(mSocket, reinterpret_cast<struct sockaddr*>(&serv_adr), sizeof(serv_adr)) < 0) {
        ERROR("bind() error, errono = %d(%s)", errno, strerror(errno));
        return;
    }

    katech::Log::Info() << "open Socket(" << mPort << ")";
}

void Thread_V2XReceiver::getSocketInfo(const ara::core::String& filePath)
{
    boost::property_tree::ptree propertyTree;
    boost::property_tree::read_json(filePath.c_str(), propertyTree);

    auto it = propertyTree.get_child("SOCKET");

    for (auto iter = it.begin(); iter != it.end(); ++iter)
    {
        if(iter->first == "port")
            mPort = std::stoi(iter->second.data());
        else
            ERROR("invalid item");
    }
}

void Thread_V2XReceiver::thread_internalBridgeSpat()
{
    katech::Log::Verbose() << "Thread_V2XReceiver::thread_internalBridgeSpat";
    std::shared_ptr<Thread_V2XReceiver> instance = getInstance();
    
    instance->getSocketInfo("./etc/serverInfo.json");
    instance->openSocket();

    r_asn_info_t * p_asninfo = NULL;
    u8 * xer_buff = NULL;
    s32 xer_buff_len = 0;
    u32 buffer_max_size = 1024*100;
    u8 buffer[buffer_max_size] = {0};
    u32 received_length = 0;
    socklen_t server_addr_size;
    struct sockaddr server_addr;
    std::string message_type;
    std::string spat_message;

    while(instance->mThreadAlive) {
        std::memset(buffer, 0, sizeof(buffer));
        received_length = recvfrom(instance->mSocket, buffer, sizeof(buffer), 0, &server_addr, &server_addr_size);
#ifdef DEB_V2X
        katech::Log::Info() << "received_length : " << received_length;
#endif
        if(received_length > 0) {
            if(asn_decode_j2735(buffer, received_length, &p_asninfo) == R_SUCCESS){
                xer_buff = r_calloc_u8(buffer_max_size);
                xer_buff_len = r_asn_to_xer(p_asninfo, xer_buff, buffer_max_size);

                if(xer_buff_len > 0){
                    xer_buff[xer_buff_len] = '\0';
                    char msg_id[3] = {xer_buff[30],xer_buff[31],'\0'};
                    message_type = msg_id;
#ifdef DEB_V2X
                    katech::Log::Info() << "Message Type Number : " << message_type;
                    INFO("receive length : %d\n%s", received_length, xer_buff);
#endif
                    if(message_type == "19"){
                        gJ2735Received_Count_spat++;
                        spat_message = static_cast<std::string>(reinterpret_cast<const char *>(xer_buff));
                        instance->getSpatMessage(spat_message);
                    }else{
                        katech::Log::Info() << "It's not SPAT Message";
                    }
                }else{
                    katech::Log::Info() << "xer_buff_len less than 0";
                }
            }
        } else {
            ERROR("There is some error to receive UDP data (received_length : %d, Error : %d(%s))", received_length, errno, strerror(errno));
        }
    }
}

Thread_V2XReceiver::Thread_V2XReceiver()
{   
    mThreadAlive = true;
    mThreadHandleSpat = std::thread(thread_internalBridgeSpat);
}

Thread_V2XReceiver::~Thread_V2XReceiver()
{
    mThreadAlive = false;
    mThreadHandleMap.join();
    mThreadHandleSpat.join();
    close(mSocket);
}

}