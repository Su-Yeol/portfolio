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

#ifndef ARA_UCM_PKGMGR_FLASH_DATA_PARSER_H_
#define ARA_UCM_PKGMGR_FLASH_DATA_PARSER_H_

#include "ara/core/vector.h"
#include "ara/core/string.h"
#include <iostream>

#include "ara/log/logger.h"

/// @brief Datatype representing the type of record
enum class RecordType : uint8_t
{
    kData = 0,
    kEndOfFile,
    kExtendedSegmentAddress,
    kStartSegmentAddress,
    kExtendedLinearAddress,
    kStartLinearAddress,
    kUnknown = 255u
};
/// @brief Datatype representing a data record of the intel hex format
struct DataRecord
{
    uint32_t byteCount;
    uint32_t address;
    ara::core::Vector<uint8_t> data;
};
typedef ara::core::Vector<DataRecord> DataRecords;

/// @brief Parser for firmware file in IntelHex format
class FlashData
{
public:
    FlashData() = default;
    ///@brief Input Stream for Intel HEX File Decoding (friend function)
    /// @param dataIn size of the result data in byte
    /// @param fdpLocal reference to
    friend std::istream& operator>>(std::istream& dataIn, FlashData& fdpLocal);

    /// @brief check for errors during parsing
    /// @return true if error occured, otherwise false
    const DataRecords& getDataRecords() const;

    /// @brief Returns segment base address as vector
    /// @return segment base address as vector
    const ara::core::Vector<uint8_t> getSegmentBaseAddressAsVector() const;

    /// @brief Returns absolute payload size as vector
    /// @return absolute payload size as vector
    const ara::core::Vector<uint8_t> getAbsoluteDataSizeAsVector() const;

    /// @brief Returns the absolute payload size of flash data
    /// @return the absolute payload size of flash data
    uint32_t getAbsoluteDataSize() const;

    /// @brief check for errors during parsing
    /// @return true if error occured, otherwise false
    bool hasError();

private:
    bool checkChecksum(ara::core::String::iterator& ihLineIterator, uint32_t lineCounter, ara::core::String ihLine);
    uint8_t stringToHex(ara::core::String value);
    void decodeDataRecord(uint8_t recordLength, uint32_t loadOffset, ara::core::String::const_iterator data);

    /// @brief List of the data records read out of hex file
    DataRecords dataRecords_;

    /// @brief Segement base address read out of the hex file
    uint32_t segmentBaseAddress_ = 0;

    /// @brief Total length of all payloads in the hex file
    uint32_t payloadTotalLength_ = 0;

    /// @brief Segement base address read out of the hex file as vector
    ara::core::Vector<uint8_t> segmentBaseAddressAsVector_;

    /// @brief Indicating if EOL marker was found in hex file
    bool foundEof_ = false;

    /// @brief Indicating if an error occured during parsing
    bool errorInFile_ = false;

    ara::log::Logger& logger_{
        ara::log::CreateLogger("FLDA", "Flash Data for Intel HEX format", ara::log::LogLevel::kVerbose)};
};

#endif
