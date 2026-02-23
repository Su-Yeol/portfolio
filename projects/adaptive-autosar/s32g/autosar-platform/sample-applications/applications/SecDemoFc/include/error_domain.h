// --------------------------------------------------------------------------
// |              _    _ _______     .----.      _____         _____        |
// |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
// |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
// |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
// |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
// |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
// |                              . _ _  .                                  |
// --------------------------------------------------------------------------
//
// All Rights Reserved.
// Any use of this source code is subject to a license agreement with the
// AUTOSAR development cooperation.
// More information is available at www.autosar.org.
//
// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/// @file
/// @brief Error domain for DemoFC

#ifndef DEMO_FC_ERROR_DOMAIN_H_
#define DEMO_FC_ERROR_DOMAIN_H_

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

namespace apd
{
namespace demofc
{

/// @brief An enumeration with errors that can occur within DemoFC
enum class DemoFCErrc : ara::core::ErrorDomain::CodeType
{
    kReaderError = 1,  ///< Deserializer Reader error
    kInstanceSpecifierValidationError = 2,  ///< Deserializer InstanceSpecifier validiation error
    kError  ///< Any other error
};

/// @brief Specialization of ara::core::Exception
class DemoFCException : public ara::core::Exception
{
public:
    /// @brief Construct a new DemoFCException from an ErrorCode.
    ///
    /// @param err  the ErrorCode
    explicit DemoFCException(ara::core::ErrorCode&& err)
        : ara::core::Exception(std::move(err))
    { }
};

/// @brief An error domain for DemoFC errors
class DemoFCErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief ID for the DemoFC error domain; Is returned by Id()
    /// @uptrace{SWS_CORE_10952, 3ebc0e9e7c39613eb7ac8a79918fa3520acbe867}
    /// @uptrace{SWS_CORE_00010, 398e83564b49bcde885d9b6a62ed70f412561522}
    /// @uptrace{SWS_CORE_00016, bc482abec797292b4e1fbc7d56e57072700778e4}
    /// spec summary: bits 63-32 are 0 (because this is a vendor-defined error domain), bits 31-16 are the vendor id
    /// (0x0AAA for the APD), bits 15-0 are the error domain identifier
    static constexpr ara::core::ErrorDomain::IdType kId = 0x000000000AAA05feULL;

public:
    /// @brief Alias for the error code value enumeration
    using Errc = DemoFCErrc;

    /// @brief Alias for the exception base class
    using Exception = DemoFCException;

    /// @brief Default constructor
    ///
    constexpr DemoFCErrorDomain() noexcept
        : ara::core::ErrorDomain(kId)
    { }

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    ///
    /// @returns "DemoFC"
    char const* Name() const noexcept override
    {
        return "DemoFC";
    }

    /// @brief Translate an error code value into a text message.
    ///
    /// @param errorCode  the error code value
    /// @returns the text message, never nullptr
    char const* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept override
    {
        DemoFCErrc const code = static_cast<DemoFCErrc>(errorCode);
        switch (code) {
        case DemoFCErrc::kReaderError:
            return "ReaderError";
        case DemoFCErrc::kInstanceSpecifierValidationError:
            return "InstanceSpecifierValidationError";
        case DemoFCErrc::kError:
            return "Error";
        default:
            return "Unknown error";
        }
    }

    /// @brief Throw the exception type corresponding to the given ErrorCode.
    ///
    /// @param errorCode  the ErrorCode instance
    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) override
    {
        ara::core::ifc::ThrowOrTerminate<DemoFCException>(errorCode);
    }
};

namespace internal
{
constexpr DemoFCErrorDomain g_DemoFCErrorDomain;
}

/// @brief Return a reference to the global DemoFCErrorDomain
///
/// @returns the DemoFCErrorDomain
inline constexpr ara::core::ErrorDomain const& GetDemoFCErrorDomain()
{
    return internal::g_DemoFCErrorDomain;
}

/// @brief Create a new ErrorCode within DemoFCErrorDomain.
///
/// @param code  the DemoFCErrorDomain-specific error code value
/// @param data  optional vendor-specific error data
/// @returns a new ErrorCode instance
inline constexpr ara::core::ErrorCode MakeErrorCode(DemoFCErrc code,
    ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode(static_cast<ara::core::ErrorDomain::CodeType>(code), GetDemoFCErrorDomain(), data);
}

}  // namespace demofc
}  // namespace apd

#endif  // DEMO_FC_ERROR_DOMAIN_H_
