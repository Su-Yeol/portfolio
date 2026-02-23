#include "CrcProvider.h"

#include <stdexcept>

namespace
{

    class PrivateCrcProviderStub final : public ICrcProvider
    {
    public:
        std::uint16_t ComputeCrc16CcittWithDataId(
            const std::uint8_t *,
            std::size_t,
            std::uint16_t,
            std::uint8_t,
            std::uint8_t) const override
        {
            throw std::runtime_error(
                "PRIVATE_SPEC=ON requires local implementation at modules/common/src/private/CrcProviderPrivate.cpp");
        }
    };

}

/**
 * @brief Short description of `GetPrivateCrcProvider`.
 *
 * Detailed explanation of what `GetPrivateCrcProvider` does,
 * including major steps and responsibilities.
 *
 * @param None.
 * @return Output value from the function.
 */
const ICrcProvider &GetPrivateCrcProvider()
{
    static PrivateCrcProviderStub provider;
    return provider;
}
