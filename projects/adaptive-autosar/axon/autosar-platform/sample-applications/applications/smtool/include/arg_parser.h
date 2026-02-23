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

#ifndef APD_SMTOOL_ARG_PARSER_H_
#define APD_SMTOOL_ARG_PARSER_H_

#include "command.h"

#include "ara/core/optional.h"
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"

namespace apd
{
namespace smtool
{
/// @brief Class used for parsing of command line arguments passed to the application
class ArgParser
{
public:
    /// @brief Constructor that takes as parameters the arguments passed to the application
    ///
    /// @param argc Arguments count
    /// @param argv Array of arguments
    ///
    ArgParser(int argc, char* argv[]);

    ~ArgParser() = default;
    ArgParser(const ArgParser&) = delete;
    ArgParser(ArgParser&&) = delete;
    ArgParser& operator=(const ArgParser&) = delete;
    ArgParser& operator=(ArgParser&&) = delete;

    /// @brief Getter for parsing result
    ///
    /// @returns Command object that stores result of processing of received application arguments
    ///
    const ara::core::Optional<Command>& GetCommand() const noexcept;

    /// @brief A method that processes application arguments and fills internal field with the result
    ///
    void ParseArgs();

private:
    /// @brief Method to be called when an error occurs while processing received application arguments. It sets
    /// appropriate internal state and logs information about the error
    ///
    /// @param errorDescription - information about the error
    ///
    void HandleParsingError(const ara::core::String& errorDescription);

    /// @brief Checks if the passed argument is a short option
    ///
    /// @param argument - a string
    ///
    bool IsShortOption(const ara::core::StringView argument) const;

    /// @brief Processes short option and fills internal field with the result
    ///
    /// @param argument - a string containing short option
    ///
    void ParseShortOption(const ara::core::StringView argument);

    /// @brief Checks if there are unprocessed arguments left
    ///
    /// @returns true if there is at least one unprocessed argument, false otherwise
    ///
    bool HasNextArgument() const noexcept;

    /// @brief Getter for next unprocessed argument
    ///
    /// @returns next unprocessed argument
    ///
    ara::core::StringView GetNextArgument();

    /// @brief Stores results of processing of application arguments
    ara::core::Optional<Command> command_;

    /// @brief Stores application arguments received in constructor
    ara::core::Vector<ara::core::StringView> arguments_;

    /// @brief Flag with information, if there was an error while processing arguments
    bool parsingFailed_ = false;

    /// @brief Position of next unprocessed argument in arguments_ vector
    size_t currentPosition_ = 0u;
};

}  // namespace smtool
}  // namespace apd

#endif  // APD_SMTOOL_ARG_PARSER_H_
