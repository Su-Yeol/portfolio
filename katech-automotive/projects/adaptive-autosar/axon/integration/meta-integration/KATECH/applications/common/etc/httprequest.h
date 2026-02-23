#ifndef __HTTP_REQUEST__
#define __HTTP_REQUEST__

#include "Poco/URI.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/JSON/Object.h"
#include "Poco/Timespan.h"

#include "logger.h"


namespace katech
{
namespace etc
{

class HTTPRequest
{
public:
    void init(const std::string uri);

    template<typename K, typename V>
    void addJsonData(K key, V value)
    {
        mObject.set(key, value);
    }
    void clearJsonData();
    void sendHTTPRequest();

    bool getFlag();
    void printSendData();

    ~HTTPRequest();

private:
    std::shared_ptr<Poco::Net::HTTPClientSession> mSession;
    std::shared_ptr<Poco::Net::HTTPRequest> mRequest;
    Poco::Net::HTTPResponse mResponse;
    Poco::JSON::Object mObject;


    std::uint32_t mStatus;
    std::string mReason;
    bool errorFlag = false;
 

};

}
}
#endif
