#include "CanSpecProvider.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace
{

/**
 * @brief Short description of `Trim`.
 *
 * Detailed explanation of what `Trim` does,
 * including major steps and responsibilities.
 *
 * @param s Input parameter.
 * @return Output value from the function.
 */
    std::string Trim(const std::string &s)
    {
        const auto first = s.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
        {
            return {};
        }
        const auto last = s.find_last_not_of(" \t\r\n");
        return s.substr(first, last - first + 1);
    }

/**
 * @brief Short description of `ParseU32`.
 *
 * Detailed explanation of what `ParseU32` does,
 * including major steps and responsibilities.
 *
 * @param raw Input parameter.
 * @param fallback Input parameter.
 * @return Output value from the function.
 */
    std::uint32_t ParseU32(const std::string &raw, std::uint32_t fallback)
    {
        std::string token = Trim(raw);
        if (token.empty())
        {
            return fallback;
        }
        int base = 10;
        if (token.size() > 2 && token[0] == '0' && (token[1] == 'x' || token[1] == 'X'))
        {
            base = 16;
        }
        try
        {
            return static_cast<std::uint32_t>(std::stoul(token, nullptr, base));
        }
        catch (...)
        {
            return fallback;
        }
    }

/**
 * @brief Short description of `ParseI32`.
 *
 * Detailed explanation of what `ParseI32` does,
 * including major steps and responsibilities.
 *
 * @param raw Input parameter.
 * @param fallback Input parameter.
 * @return Output value from the function.
 */
    std::int32_t ParseI32(const std::string &raw, std::int32_t fallback)
    {
        std::string token = Trim(raw);
        if (token.empty())
        {
            return fallback;
        }
        int base = 10;
        if (token.size() > 2 && token[0] == '0' && (token[1] == 'x' || token[1] == 'X'))
        {
            base = 16;
        }
        try
        {
            return static_cast<std::int32_t>(std::stol(token, nullptr, base));
        }
        catch (...)
        {
            return fallback;
        }
    }

/**
 * @brief Short description of `ParseF64`.
 *
 * Detailed explanation of what `ParseF64` does,
 * including major steps and responsibilities.
 *
 * @param raw Input parameter.
 * @param fallback Input parameter.
 * @return Output value from the function.
 */
    double ParseF64(const std::string &raw, double fallback)
    {
        std::string token = Trim(raw);
        if (token.empty())
        {
            return fallback;
        }
        try
        {
            return std::stod(token);
        }
        catch (...)
        {
            return fallback;
        }
    }

/**
 * @brief Short description of `Split`.
 *
 * Detailed explanation of what `Split` does,
 * including major steps and responsibilities.
 *
 * @param s Input parameter.
 * @param delim Input parameter.
 * @return Output value from the function.
 */
    std::vector<std::string> Split(const std::string &s, char delim)
    {
        std::vector<std::string> out;
        std::stringstream ss(s);
        std::string item;
        while (std::getline(ss, item, delim))
        {
            out.push_back(Trim(item));
        }
        return out;
    }

    class FileCanSpecProvider : public ICanSpecProvider
    {
    public:
/**
 * @brief Short description of `FileCanSpecProvider`.
 *
 * Detailed explanation of what `FileCanSpecProvider` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return void
 */
        FileCanSpecProvider()
        {
            const char *env = std::getenv("AUTODRIVE_CAN_SPEC");
            if (env != nullptr && *env != '\0')
            {
                active_path_ = env;
            }
            else
            {
                active_path_ = "./common/config/can_spec.ini";
            }
            Load(active_path_);
        }

        std::uint32_t GetU32(const std::string &key, std::uint32_t fallback) const override
        {
            const auto it = kv_.find(key);
            if (it == kv_.end())
            {
                return fallback;
            }
            return ParseU32(it->second, fallback);
        }

        std::uint8_t GetU8(const std::string &key, std::uint8_t fallback) const override
        {
            return static_cast<std::uint8_t>(GetU32(key, fallback));
        }

        std::int32_t GetI32(const std::string &key, std::int32_t fallback) const override
        {
            const auto it = kv_.find(key);
            if (it == kv_.end())
            {
                return fallback;
            }
            return ParseI32(it->second, fallback);
        }

        double GetF64(const std::string &key, double fallback) const override
        {
            const auto it = kv_.find(key);
            if (it == kv_.end())
            {
                return fallback;
            }
            return ParseF64(it->second, fallback);
        }

        std::vector<std::uint32_t> GetU32List(
            const std::string &key,
            const std::vector<std::uint32_t> &fallback) const override
        {
            const auto it = kv_.find(key);
            if (it == kv_.end())
            {
                return fallback;
            }

            std::vector<std::uint32_t> out;
            const auto tokens = Split(it->second, ',');
            out.reserve(tokens.size());
            for (const auto &token : tokens)
            {
                if (token.empty())
                {
                    continue;
                }
                out.push_back(ParseU32(token, 0));
            }
            return out.empty() ? fallback : out;
        }

        std::string ActiveConfigPath() const override
        {
            return active_path_;
        }

    private:
/**
 * @brief Short description of `Load`.
 *
 * Detailed explanation of what `Load` does,
 * including major steps and responsibilities.
 *
 * @param path Input parameter.
 * @return void
 */
        void Load(const std::string &path)
        {
            std::ifstream in(path);
            if (!in.is_open())
            {
                return;
            }

            std::string line;
            while (std::getline(in, line))
            {
                line = Trim(line);
                if (line.empty())
                {
                    continue;
                }
                if (line[0] == '#')
                {
                    continue;
                }
                const auto pos = line.find('=');
                if (pos == std::string::npos)
                {
                    continue;
                }
                const auto key = Trim(line.substr(0, pos));
                const auto value = Trim(line.substr(pos + 1));
                if (!key.empty())
                {
                    kv_[key] = value;
                }
            }
        }

        std::unordered_map<std::string, std::string> kv_;
        std::string active_path_;
    };

}

/**
 * @brief Short description of `GetCanSpecProvider`.
 *
 * Detailed explanation of what `GetCanSpecProvider` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return Output value from the function.
 */
const ICanSpecProvider &GetCanSpecProvider()
{
    static FileCanSpecProvider provider;
    return provider;
}
