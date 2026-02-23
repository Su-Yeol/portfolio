#include "CanSpecProvider.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace {
std::string Trim(std::string s) {
    const auto not_space = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}

class FileCanSpecProvider : public ICanSpecProvider {
public:
    FileCanSpecProvider() {
        const char* env = std::getenv("AUTODRIVE_CAN_SPEC");
        if (env != nullptr && *env != '\0' && LoadFromPath(env)) {
            active_path_ = env;
            return;
        }

        env = std::getenv("KATECH_CONTROL_CAN_SPEC");
        if (env != nullptr && *env != '\0' && LoadFromPath(env)) {
            active_path_ = env;
            return;
        }

        const std::vector<std::string> candidates = {
            "./lib/common/config/can_spec.ini",
            "../lib/common/config/can_spec.ini",
            "./Control/lib/common/config/can_spec.ini",
            "./lib/common/control_can_spec.ini",
            "../lib/common/control_can_spec.ini"
        };
        for (const auto& path : candidates) {
            if (LoadFromPath(path)) {
                active_path_ = path;
                return;
            }
        }

        active_path_ = "<none>";
    }

    std::uint32_t GetU32(const std::string& key) const override {
        return static_cast<std::uint32_t>(std::stoull(RequireEntry(key), nullptr, 0));
    }

    std::int32_t GetI32(const std::string& key) const override {
        return static_cast<std::int32_t>(std::stoll(RequireEntry(key), nullptr, 0));
    }

    double GetF64(const std::string& key) const override {
        return std::stod(RequireEntry(key));
    }

    std::string GetString(const std::string& key) const override {
        return RequireEntry(key);
    }

    std::string ActiveConfigPath() const override {
        return active_path_;
    }

private:
    const std::string& RequireEntry(const std::string& key) const {
        const auto it = entries_.find(key);
        if (it == entries_.end()) {
            throw std::runtime_error("Missing can_spec key: " + key);
        }
        return it->second;
    }

    bool LoadFromPath(const std::string& path) {
        std::ifstream in(path);
        if (!in.is_open()) {
            return false;
        }

        std::unordered_map<std::string, std::string> loaded;
        std::string line;
        while (std::getline(in, line)) {
            const auto sharp = line.find('#');
            if (sharp != std::string::npos) {
                line = line.substr(0, sharp);
            }
            line = Trim(line);
            if (line.empty()) {
                continue;
            }

            const auto equal = line.find('=');
            if (equal == std::string::npos) {
                continue;
            }

            std::string key = Trim(line.substr(0, equal));
            std::string value = Trim(line.substr(equal + 1));
            if (!key.empty()) {
                loaded[std::move(key)] = std::move(value);
            }
        }

        entries_ = std::move(loaded);
        return true;
    }

    std::unordered_map<std::string, std::string> entries_;
    std::string active_path_;
};
}  // namespace

const ICanSpecProvider& GetCanSpecProvider() {
    static FileCanSpecProvider provider;
    return provider;
}
