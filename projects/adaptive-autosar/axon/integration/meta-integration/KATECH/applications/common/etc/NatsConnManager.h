
#ifndef SUBSCRIBER_H_
#define SUBSCRIBER_H_

#include "nats/nats.h"
#include "Poco/JSON/Object.h"

namespace katech
{
namespace etc
{


class NatsConnManager
{


#define SERVER_ID   "CHANGE_ME_USER"
#define SERVER_PW   "CHANGE_ME_PASSWORD"
#define MAX_SERVERS 2

#define STATS_IN        0x1
#define STATS_OUT       0x2
#define STATS_COUNT     0x4

public:
    enum class Mode {
        Default,
        Publish_Only,
    };
    NatsConnManager(const char * serverUrl, const std::vector<const char*> subjects, Mode mode = Mode::Default);  //For only Publish_Only;
    NatsConnManager(const char * serverUrl, const std::vector<const char*> subjects, natsMsgHandler onMsgFunc, natsErrHandler asyncFunc, Mode mode = Mode::Default);
    ~NatsConnManager();

    natsConnection*      conn = NULL;
    natsSubscription*    sub   = NULL;
    natsStatistics*      stats = NULL;

    volatile int64_t dropped = 0;
    Mode subscribeFlag = Mode::Default;

    template<typename K, typename V>
    void addJsonData(K key, V value)
    {
        jsonObj.set(key, value);
    }

    template<typename K, typename V>
    void addJsonArrayData(K key, std::vector<V> values)
    {
        Poco::JSON::Array jsonArray;
        for (const V& value : values)
        {
            jsonArray.add(value);
        }
        jsonObj.set(key, jsonArray);
    }

    natsStatus NatsExecute(); 

    natsStatus printStats(int mode, natsConnection* conn, natsSubscription* sub, natsStatistics* stats);

    natsStatus NatsPublish(const char* subject, const char*  payload);
    natsStatus NatsPublishJson(const char* subject);
     
    natsStatus NatsReply(natsMsg* msg, const char*  payload);
    natsStatus NatsReplyJson(natsMsg* msg);
   

    void NatsSleep(int64_t sleepTime);
    void NatsMsgDestroy(natsMsg* msg);
    void NatsSubscriptionGetDropped(natsSubscription* sub, int64_t* msgs);

    void ClearJsonData();
    void PrintSendData();
    bool NatsIsConnected();

private:
    Poco::JSON::Object jsonObj;

    const char* mServerUrl = NULL;
    std::vector<const char*>  subj;

    natsOptions*         opts  = NULL;

    const char*       certFile = NULL;
    const char*       keyFile  = NULL;

    natsMsgHandler onMsgCb = NULL;
    natsErrHandler asyncCb = NULL;

    natsStatus NatsCreateNetsOptions();
    natsStatus NatsConnection();
    void NatsPrintLastErrorStack();
    
};

}
}


#endif /* SUBSCRIBER_H_ */
