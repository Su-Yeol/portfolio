#include "CrcProvider.h"

namespace
{

    class PublicCrcProvider final : public ICrcProvider
    {
    public:
        std::uint16_t ComputeCrc16CcittWithDataId(
            const std::uint8_t *payload,
            std::size_t payload_len,
            std::uint16_t seed,
            std::uint8_t data_id_l,
            std::uint8_t data_id_h) const override
        {
            auto crc = seed;
            for (std::size_t i = 0; i < payload_len; ++i)
            {
                crc = Update(crc, payload[i]);
            }
            crc = Update(crc, data_id_l);
            crc = Update(crc, data_id_h);
            return crc;
        }

    private:
/**
 * @brief Short description of `Update`.
 *
 * Detailed explanation of what `Update` does,
 * including major steps and responsibilities.
 *
 * @param crc Input parameter.
 * @param data Input parameter.
 * @return Output value from the function.
 */
        static std::uint16_t Update(std::uint16_t crc, std::uint8_t data)
        {
            crc ^= static_cast<std::uint16_t>(data) << 8;
            for (int bit = 0; bit < 8; ++bit)
            {
                if (crc & 0x8000)
                {
                    crc = static_cast<std::uint16_t>((crc << 1) ^ 0x1021);
                }
                else
                {
                    crc = static_cast<std::uint16_t>(crc << 1);
                }
            }
            return crc;
        }
    };

}

/**
 * @brief Short description of `GetCrcProvider`.
 *
 * Detailed explanation of what `GetCrcProvider` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return Output value from the function.
 */
const ICrcProvider &GetCrcProvider()
{
#ifdef PRIVATE_SPEC
    return GetPrivateCrcProvider();
#else
    static PublicCrcProvider provider;
    return provider;
#endif
}
