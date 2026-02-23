#include "httprequest.h"
#include "logger.h"

namespace adcm
{
namespace etc
{

void HTTPRequest::init(const std::string uri)
{
    Poco::URI serverUri(uri);
    adcm::Log::Info() << "create a HTTPRequest -> " << uri;
    //create a HTTPRequest
    mSession = std::make_shared<Poco::Net::HTTPClientSession>(serverUri.getHost(), serverUri.getPort());
    mRequest = std::make_shared<Poco::Net::HTTPRequest> (Poco::Net::HTTPRequest::HTTP_POST, serverUri.getPathAndQuery(), Poco::Net::HTTPMessage::HTTP_1_1);

}

void HTTPRequest::sendHTTPRequest()
{
    //adcm::Log::Info() << "sendJsonData function";

    std::stringstream ss;
    mObject.stringify(ss);
    mRequest->setKeepAlive(true);
    mRequest->setContentType("application/json");
    mRequest->setContentLength(ss.str().size());
    //Send HTTPRequest
    //adcm::Log::Info() << "Send HTTPRequest";

    try {
        mSession->setTimeout(Poco::Timespan(0, 300000));
        //adcm::Log::Info() << "set Timeout";
        std::ostream& os = mSession->sendRequest(*mRequest);
        mObject.stringify(os);
        mStatus = mResponse.getStatus();
        mReason = mResponse.getReason();
        adcm::Log::Info() << "Send HTTPRequest Status / Reason = " << mStatus << " / "<< mReason;

    } catch(const Poco::Exception& exception) {
        errorFlag = true;
        adcm::Log::Error() << "Send HTTPRequest failed, error : " << exception.displayText();
    }
}

void HTTPRequest::printSendData()
{
    //Poco::Dynamic::Var jsonObj = mObject;
    //std::string jsonStr = jsonObj.toString();
    //adcm::Log::Info() << jsonStr;

    std::stringstream sendData;

    mObject.stringify(sendData);
    adcm::Log::Info() << " Send data : " << sendData.str() <<  ", length : " << sendData.str().size();
}

void HTTPRequest::clearJsonData()
{
    mObject.clear();
}

bool HTTPRequest::getFlag()
{
    return errorFlag;
}


HTTPRequest::~HTTPRequest()
{

}

}
}