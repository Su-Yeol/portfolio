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
#include <memory>
#include "diagApp01_data_identifier.hpp"

/* Public function definitions -------------------------------------------*/

DIAGApp01DataIdentifier::DIAGApp01DataIdentifier(ara::com::InstanceIdentifier instanceId)
    : Skeleton{instanceId}
    , numDIAGApp01DataIdentifier{0}
{ }

DIAGApp01DataIdentifier::~DIAGApp01DataIdentifier()
{ }

auto DIAGApp01DataIdentifier::Write(const ByteArray& dataRecord) -> decltype(Skeleton::Write(dataRecord))
{
    decltype(Skeleton::Write(dataRecord))::PromiseType promise;

    // Set data that is returned in the answer of the WriteDid request.
    // "numDIAGApp01DataIdentifier" does not affect the test result,if needed, it can be removed.
    numDIAGApp01DataIdentifier = dataRecord.at(0u);

    m_logger_dst3.LogDebug()
        << "DataServiceBrakeCounter-service: write sentData to xxx(ByteArray data is can not view?)";

    promise.set_value();

    return promise.get_future();
}

auto DIAGApp01DataIdentifier::Read() -> decltype(Skeleton::Read())
{

    ara::diag::DataIdentifier::ReadOutput data;
    decltype(Skeleton::Read())::PromiseType promise;

    // Set data that is returned in the answer of the ReadDid request.
    m_logger_dst3.LogDebug() << "DIAGApp01DataIdentifier-service: start!";

    data.dataRecord.resize(1);

    data.dataRecord[0] = readData;

    promise.set_value(std::move(data));

    readData = readData + readAdd;

    return promise.get_future();
}
