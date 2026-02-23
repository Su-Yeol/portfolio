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

#include <iostream>

#include "flash_data.h"

const DataRecords& FlashData::getDataRecords() const
{
    return dataRecords_;
}

uint32_t FlashData::getAbsoluteDataSize() const
{
    return payloadTotalLength_;
}

const ara::core::Vector<uint8_t> FlashData::getSegmentBaseAddressAsVector() const
{
    return segmentBaseAddressAsVector_;
}

const ara::core::Vector<uint8_t> FlashData::getAbsoluteDataSizeAsVector() const
{
    ara::core::Vector<uint8_t> arrayOfByte(4);
    for (int i = 0; i < 4; i++)
        arrayOfByte[3 - i] = (payloadTotalLength_ >> (i * 8));
    return arrayOfByte;
}

uint8_t FlashData::stringToHex(ara::core::String value)
{
    uint8_t returnValue = 0;
    ara::core::String::iterator valueIterator;

    if (value.length() == 2) {
        valueIterator = value.begin();

        for (uint8_t x = 0; x < 2; x++) {
            // Shift result variable 4 bits to the left
            returnValue <<= 4;

            if (*valueIterator >= '0' && *valueIterator <= '9') {
                returnValue += static_cast<uint8_t>(*valueIterator - '0');
            } else if (*valueIterator >= 'A' && *valueIterator <= 'F') {
                returnValue += static_cast<uint8_t>(*valueIterator - 'A' + 10);
            } else if (*valueIterator >= 'a' && *valueIterator <= 'f') {
                returnValue += static_cast<uint8_t>(*valueIterator - 'a' + 10);
            } else {
                returnValue = 0;
            }

            // Iterate to next char in the string
            ++valueIterator;
        }
    } else {
        // Error occured - more or less than two nibbles in the string
    }

    return returnValue;
}

void FlashData::decodeDataRecord(uint8_t recordLength, uint32_t loadOffset, ara::core::String::const_iterator data)
{
    // Variable to store a byte of the record as a two char string
    ara::core::String sByteRead;

    // Variable to store the byte of the record as an u.char

    ara::core::Vector<uint8_t> payload;

    // Calculate new segment base address by clearing the low four bytes and then
    // adding the current loadOffset for this line of Intel HEX data
    segmentBaseAddress_ &= ~(0xFFFFUL);
    segmentBaseAddress_ += loadOffset;

    for (uint8_t x = 0; x < recordLength; x++) {
        uint8_t byteRead;
        sByteRead.erase();

        sByteRead = *data;
        ++data;
        sByteRead += *data;
        ++data;

        byteRead = stringToHex(sByteRead);

        payload.push_back(byteRead);
        ++segmentBaseAddress_;
    }
    dataRecords_.push_back({recordLength, loadOffset, payload});
}
namespace
{
ara::core::String getNextByte(ara::core::String::iterator& ihLineIterator)
{
    ara::core::String ihByte;
    ihByte = *ihLineIterator;
    ++ihLineIterator;
    ihByte += *ihLineIterator;
    ++ihLineIterator;
    return ihByte;
}
}  // namespace

bool FlashData::checkChecksum(ara::core::String::iterator& ihLineIterator,
    uint32_t lineCounter,
    ara::core::String ihLine)
{
    uint8_t intelHexChecksum = 0;
    ara::core::String ihByte;
    // Run through the whole line to check the checksum
    for (ihLineIterator = ihLine.begin(); ihLineIterator != ihLine.end();) {
        // Convert the line in pair of chars (making a single byte)
        // into single bytes, and then add to the checksum variable.
        // By adding all the bytes in a line together *including* the
        // checksum byte, we should get a result of '0' at the end.
        // If not, there is a checksum error

        ihByte = *ihLineIterator;
        ++ihLineIterator;
        // Just in case there are an odd number of chars in the
        // just check we didn't reach the end of the ara::core::String early
        if (ihLineIterator != ihLine.end()) {
            // Variable to hold a single byte (two chars) of data
            uint8_t byteRead;
            ihByte += *ihLineIterator;
            ++ihLineIterator;

            byteRead = stringToHex(ihByte);

            intelHexChecksum += byteRead;
        } else {
            logger_.LogInfo() << "Odd number of characters in line " + lineCounter;
        }
    }
    return intelHexChecksum;
}

bool FlashData::hasError()
{
    return errorInFile_;
}

std::istream& operator>>(std::istream& dataIn, FlashData& fdpLocal)
{
    // string to store lines of Intel Hex info
    ara::core::String ihLine;
    // string to store a single byte of Intel HEX info
    ara::core::String ihByte;
    // iterator for this variable
    ara::core::String::iterator ihLineIterator;
    // Create a line counter
    uint32_t lineCounter = 0;

    uint8_t recordLength;
    uint32_t loadOffset;
    RecordType recordType;

    do {
        // Clear string before this next round
        ihLine.erase();

        // Get a line of data
        dataIn >> ihLine;

        // If the line contained some data, process it
        if (ihLine.length() > 0) {
            // Increment line counter
            lineCounter++;

            // Set ara::core::String iterator to start of ara::core::String
            ihLineIterator = ihLine.begin();

            // Check that we have a ':' record mark at the beginning
            if (*ihLineIterator != ':') {
                // Add some warning code here
                ara::core::String message;

                fdpLocal.logger_.LogWarn() << "Line without record mark ':' found @ line " + lineCounter;

                // If this is the first line, let's simply give up. Chances
                // are this is not an Intel HEX file at all
                if (lineCounter == 1) {
                    fdpLocal.logger_.LogError() << "Intel HEX File decode aborted; ':' missing in "
                                                   "first line.";

                    // Erase ihLine content and break out of do...while loop
                    ihLine.erase();
                    fdpLocal.errorInFile_ = true;
                    break;
                }
            } else {
                // Remove the record mark from the ara::core::String as we don't need it
                // anymore
                ihLine.erase(ihLineIterator);
            }

            // Make sure the checksum was ok
            if (fdpLocal.checkChecksum(ihLineIterator, lineCounter, ihLine) == 0) {
                // Reset iterator back to beginning of the line so we can now
                // decode it
                ihLineIterator = ihLine.begin();

                // Get the record length
                recordLength = fdpLocal.stringToHex(getNextByte(ihLineIterator));

                // Get the load offset (2 bytes)
                loadOffset = static_cast<uint32_t>(fdpLocal.stringToHex(getNextByte(ihLineIterator)));
                loadOffset <<= 8;

                loadOffset += static_cast<uint32_t>(fdpLocal.stringToHex(getNextByte(ihLineIterator)));

                // Get the record type
                recordType = static_cast<RecordType>(fdpLocal.stringToHex(getNextByte(ihLineIterator)));

                // Decode the INFO or DATA portion of the record
                switch (recordType) {
                case RecordType::kData:
                    fdpLocal.payloadTotalLength_ += recordLength;
                    fdpLocal.decodeDataRecord(recordLength, loadOffset, ihLineIterator);

                    break;

                case RecordType::kEndOfFile:
                    // Check that the EOF record wasn't already found. If
                    // it was, generate appropriate error
                    if (fdpLocal.foundEof_ == false) {
                        fdpLocal.foundEof_ = true;
                    } else {
                        fdpLocal.logger_.LogWarn()
                            << "Additional End Of File record @ line " + lineCounter << " found.";
                        fdpLocal.errorInFile_ = true;
                    }
                    break;

                case RecordType::kExtendedLinearAddress:
                    // Make sure we have 2 bytes of data
                    if (recordLength == 2) {
                        // Extract the two bytes of the ELA
                        uint32_t extLinAddress = 0;

                        extLinAddress = static_cast<uint32_t>(fdpLocal.stringToHex(getNextByte(ihLineIterator)));
                        fdpLocal.segmentBaseAddressAsVector_.push_back(extLinAddress);
                        extLinAddress <<= 8;

                        extLinAddress += static_cast<uint32_t>(fdpLocal.stringToHex(getNextByte(ihLineIterator)));
                        fdpLocal.segmentBaseAddressAsVector_.push_back(extLinAddress);
                        // ELA is bits 16-31 of the segment base address
                        // (SBA), so shift left 16 bits
                        extLinAddress <<= 16;
                        fdpLocal.segmentBaseAddressAsVector_.push_back(0x00);
                        fdpLocal.segmentBaseAddressAsVector_.push_back(0x00);
                        // Update the SBA
                        fdpLocal.segmentBaseAddress_ = extLinAddress;
                    } else {
                        // Note the error
                        fdpLocal.logger_.LogError()
                            << "Extended Linear Address @ line " << lineCounter << " not 2 bytes as required.";
                        fdpLocal.errorInFile_ = true;
                    }
                    fdpLocal.logger_.LogInfo() << "Ext. Lin. Address 0x" << std::hex << fdpLocal.segmentBaseAddress_;
                    break;
                default:
                    // Handle the error here
                    fdpLocal.logger_.LogError() << "Record unknown";
                    fdpLocal.errorInFile_ = true;
                    break;
                }
            } else {
                fdpLocal.logger_.LogError() << "checksum error";
                fdpLocal.errorInFile_ = true;
            }
        }
    } while (ihLine.length() > 0);

    fdpLocal.logger_.LogInfo() << "Decoded " << lineCounter << " lines from file.";

    return (dataIn);
}
