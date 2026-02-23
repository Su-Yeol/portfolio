#include "etc/jsonParser.h"

#include "logger.h"

namespace adcm
{
namespace etc
{

std::vector<std::string> JsonParser::split(const std::string& s, char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while(getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}


void printJsonNode(const boost::property_tree::ptree& pt, const std::string& parentKey = "")
{
    for(const auto& child : pt) {
        std::string currentKey = parentKey.empty() ? child.first : parentKey + "." + child.first;
        adcm::Log::Info() << "Key: " << currentKey << ", Value: " << child.second.data();

        if(!child.second.empty()) {
            printJsonNode(child.second, currentKey);
        }
    }
}


int JsonParser::getValue(std::string node_path, std::string& value)
{
    int result = 0;

    try {
        value = mPropertyTree.get<std::string>(node_path);
        INFO("Param %s = %s", node_path.c_str(), value.c_str());

    } catch(const boost::property_tree::ptree_bad_data& e) {
        // 데이터 변환 오류 처리
        ERROR("Catch Exeception : %s", e.what());
        result = -1;

    } catch(const std::exception& e) {
        // 기타 예외 처리
        ERROR("Catch Exeception : %s", e.what());
        result = -2;
    }

    return result;
}

int JsonParser::getValue(std::string node_path, double& value)
{
    int result = 0;

    try {
        value = mPropertyTree.get<double>(node_path);
        INFO("Param %s = %.6lf", node_path.c_str(), value);

    } catch(const boost::property_tree::ptree_bad_data& e) {
        // 데이터 변환 오류 처리
        ERROR("Catch Exeception : %s", e.what());
        result = -1;

    } catch(const std::exception& e) {
        // 기타 예외 처리
        ERROR("Catch Exeception : %s", e.what());
        result = -2;
    }

    return result;
}

int JsonParser::getValue(std::string node_path, int& value)
{
    int result = 0;

    try {
        value = mPropertyTree.get<int>(node_path);
        INFO("Param %s = %d", node_path.c_str(), value);

    } catch(const boost::property_tree::ptree_bad_data& e) {
        // 데이터 변환 오류 처리
        ERROR("Catch Exeception : %s", e.what());
        result = -1;

    } catch(const std::exception& e) {
        // 기타 예외 처리
        ERROR("Catch Exeception : %s", e.what());
        result = -2;
    }

    return result;
}

int JsonParser::getValue(std::string node_path, bool& value)
{
    int result = 0;

    try {
        value = mPropertyTree.get<bool>(node_path);
        INFO("Param %s = %s", node_path.c_str(), (value) ? "true" : "false");

    } catch(const boost::property_tree::ptree_bad_data& e) {
        // 데이터 변환 오류 처리
        ERROR("Catch Exeception : %s", e.what());
        result = -1;

    } catch(const std::exception& e) {
        // 기타 예외 처리
        ERROR("Catch Exeception : %s", e.what());
        result = -2;
    }

    return result;
}

int JsonParser::loadJson(std::string json_filePath)
{
    try {
        boost::property_tree::read_json(json_filePath.c_str(), mPropertyTree);
        INFO("Json config file %s Loaded.", json_filePath.c_str());

    } catch(const boost::property_tree::json_parser::json_parser_error& e) {
        // JSON 파싱 중 오류가 발생한 경우
        ERROR("Json parser Error : %s", e.what());
        return -1;
    }

    return 0;
}
std::shared_ptr<JsonParser> JsonParser::getInstance()
{
    static std::shared_ptr<JsonParser> instance = std::make_shared<JsonParser>();
    return instance;
}

}
}