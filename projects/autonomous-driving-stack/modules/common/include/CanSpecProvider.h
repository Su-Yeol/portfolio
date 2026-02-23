#ifndef AUTODRIVE_CAN_SPEC_PROVIDER_H
#define AUTODRIVE_CAN_SPEC_PROVIDER_H

#include <cstdint>
#include <string>
#include <vector>

class ICanSpecProvider
{
public:
    virtual ~ICanSpecProvider() = default;
    virtual std::uint32_t GetU32(const std::string &key, std::uint32_t fallback) const = 0;
    virtual std::int32_t GetI32(const std::string &key, std::int32_t fallback) const = 0;
    virtual double GetF64(const std::string &key, double fallback) const = 0;
    virtual std::uint8_t GetU8(const std::string &key, std::uint8_t fallback) const = 0;
    virtual std::vector<std::uint32_t> GetU32List(
        const std::string &key,
        const std::vector<std::uint32_t> &fallback) const = 0;
    virtual std::string ActiveConfigPath() const = 0;
};

const ICanSpecProvider &GetCanSpecProvider();

#endif
