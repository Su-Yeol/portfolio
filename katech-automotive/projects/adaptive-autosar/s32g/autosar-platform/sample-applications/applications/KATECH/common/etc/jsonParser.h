#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace adcm
{
namespace etc
{
class JsonParser
{
public:

    static std::shared_ptr<JsonParser> getInstance();
    int loadJson(std::string json_filePath);
    int getValue(std::string node_path, std::string& value);
    int getValue(std::string node_path, double& value);
    int getValue(std::string node_path, int& value);
    int getValue(std::string node_path, bool& value);


private:
    boost::property_tree::ptree mPropertyTree;
    std::vector<std::string> split(const std::string& s, char delim);
    int findNode(std::string node_path, boost::property_tree::ptree& node);
};
}
}