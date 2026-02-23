#ifndef KATECH_CONTROL_CAN_SPEC_PROVIDER_H
#define KATECH_CONTROL_CAN_SPEC_PROVIDER_H

#include <cstdint>
#include <string>

class ICanSpecProvider {
public:
    virtual ~ICanSpecProvider() = default;

    virtual std::uint32_t GetU32(const std::string& key) const = 0;
    virtual std::int32_t GetI32(const std::string& key) const = 0;
    virtual double GetF64(const std::string& key) const = 0;
    virtual std::string GetString(const std::string& key) const = 0;

    virtual std::uint32_t GetU32(const std::string& key, std::uint32_t fallback) const {
        try {
            return GetU32(key);
        } catch (...) {
            return fallback;
        }
    }

    virtual std::int32_t GetI32(const std::string& key, std::int32_t fallback) const {
        try {
            return GetI32(key);
        } catch (...) {
            return fallback;
        }
    }

    virtual double GetF64(const std::string& key, double fallback) const {
        try {
            return GetF64(key);
        } catch (...) {
            return fallback;
        }
    }

    virtual std::string GetString(const std::string& key, const std::string& fallback) const {
        try {
            return GetString(key);
        } catch (...) {
            return fallback;
        }
    }

    virtual std::string ActiveConfigPath() const = 0;
};

const ICanSpecProvider& GetCanSpecProvider();

#endif
