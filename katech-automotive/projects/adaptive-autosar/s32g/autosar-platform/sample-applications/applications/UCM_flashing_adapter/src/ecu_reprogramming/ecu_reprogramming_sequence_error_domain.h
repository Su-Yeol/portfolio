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

#ifndef ARA_UCM_ECU_REPROGRAMMING_SEQUENCE_ERROR_DOMAIN_H_
#define ARA_UCM_ECU_REPROGRAMMING_SEQUENCE_ERROR_DOMAIN_H_

#include "ara/core/error_code.h"
#include "ara/core/error_domain.h"
#include "ara/core/exception.h"

enum class RepSequenceErrorDomainErrc : ara::core::ErrorDomain::CodeType
{
    kConfigurationInvalid,
    kInitializationFailed,
    kReprogrammingSessionSwitchFailed,
    kEraseRoutineFailed,
    kRequestDownloadFailed,
    kTransferDataFailed,
    kRequestTransferExitFailed,
    kDependencyCheckFailed,
    kEcuResetFailed,
    kReadVersionFailed,
    kVersionIncorrect,
    kActivationFailed,
    kRollbackFailed,
    kServiceBusy  ///< Another processing is already ongoing and therefore the current processing request has to be
                  ///< rejected.
};

class RepSequenceErrorDomainException : public ara::core::Exception
{
public:
    explicit RepSequenceErrorDomainException(ara::core::ErrorCode err) noexcept
        : ara::core::Exception(err)
    { }
};

class RepSequenceErrorDomainErrorDomain final : public ara::core::ErrorDomain
{
    /// @brief ID for the ecu reprogramming error domain; Is returned by Id()
    /// @uptrace{SWS_CORE_10952, 3ebc0e9e7c39613eb7ac8a79918fa3520acbe867}
    /// @uptrace{SWS_CORE_00010, 398e83564b49bcde885d9b6a62ed70f412561522}
    /// @uptrace{SWS_CORE_00016, bc482abec797292b4e1fbc7d56e57072700778e4}
    /// spec summary: bits 63-32 are 0 (because this is a vendor-defined error domain), bits 31-16 are the vendor id
    /// (0x0AAA for the APD), bits 15-0 are the error domain identifier
    static constexpr ara::core::ErrorDomain::IdType kId = 0x000000000AAA0042ULL;

public:
    using Errc = RepSequenceErrorDomainErrc;

    using Exception = RepSequenceErrorDomainException;

    /// @brief Default constructor
    constexpr RepSequenceErrorDomainErrorDomain() noexcept
        : ara::core::ErrorDomain(kId)
    { }

    /// @brief Return the "shortname" ApApplicationErrorDomain.SN of this error domain.
    char const* Name() const noexcept override
    {
        return "UCMErrorDomain";
    }

    char const* Message(ara::core::ErrorDomain::CodeType errorCode) const noexcept override
    {
        Errc const code = static_cast<Errc>(errorCode);
        switch (code) {
        case Errc::kConfigurationInvalid:
            return "Programming configuration is invalid.";
        case Errc::kReprogrammingSessionSwitchFailed:
            return "Could not enter programming mode of ECU.";
        case Errc::kEraseRoutineFailed:
            return "Erase routine failed.";
        case Errc::kRequestDownloadFailed:
            return "Request for download failed.";
        case Errc::kTransferDataFailed:
            return "Transfer of data failed.";
        case Errc::kDependencyCheckFailed:
            return "The dependency check on the ECU failed.";
        case Errc::kEcuResetFailed:
            return "Could not reset ECU.";
        case Errc::kReadVersionFailed:
            return "The read of the version of the ECU software failed.";
        case Errc::kVersionIncorrect:
            return "Received version does not match expected version.";
        case Errc::kActivationFailed:
            return "Activation of the ECU failed.";
        case Errc::kRollbackFailed:
            return "The rollback command failed.";
        case Errc::kServiceBusy:
            return "The service is currently busy.";
        default:
            return "Unknown error";
        }
    }

    void ThrowAsException(ara::core::ErrorCode const& errorCode) const noexcept(false) override
    {
        ara::core::ifc::ThrowOrTerminate<Exception>(errorCode);
    }
};

namespace internal
{
constexpr RepSequenceErrorDomainErrorDomain g_RepSequenceErrorDomainErrorDomain;
}

inline constexpr ara::core::ErrorDomain const& GetRepSequenceErrorDomainErrorDomain() noexcept
{
    return internal::g_RepSequenceErrorDomainErrorDomain;
}

inline constexpr ara::core::ErrorCode MakeErrorCode(RepSequenceErrorDomainErrc code,
    ara::core::ErrorDomain::SupportDataType data) noexcept
{
    return ara::core::ErrorCode(
        static_cast<ara::core::ErrorDomain::CodeType>(code), GetRepSequenceErrorDomainErrorDomain(), data);
}

#endif  // ARA_UCM_ECU_REPROGRAMMING_SEQUENCE_ERROR_DOMAIN_H_
