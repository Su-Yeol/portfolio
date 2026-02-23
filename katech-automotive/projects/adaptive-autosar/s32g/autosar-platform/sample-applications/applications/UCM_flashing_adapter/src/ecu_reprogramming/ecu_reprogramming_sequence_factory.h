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

#ifndef ARA_UCM_PKGMGR_ECU_REPROGRAMMING_SEQUENCE_FACTORY_H_
#define ARA_UCM_PKGMGR_ECU_REPROGRAMMING_SEQUENCE_FACTORY_H_

#include "single_bank_ecu_reprogramming.h"
#include "dual_bank_ecu_reprogramming.h"
#include "prog_conf_parser.h"
#include "uds_interface.h"
#include "pdu_api_handle.h"

class EcuReprogrammingSequenceFactory
{
public:
    EcuReprogrammingSequenceFactory(std::shared_ptr<PduApiHandle>& pduApiHandle,
        const ara::core::String& path,
        ara::core::String& progConfFile)
        : path_{path}
        , progConfFile_{progConfFile}
        , pduApiHandle_{pduApiHandle}
    { }
    virtual ~EcuReprogrammingSequenceFactory() = default;

    /**
     * @brief Create ecu reprogramming sequence instance according to type in prog conf
     * @return Pointer to generated EcuReprogrammingSequence
     */
    virtual std::unique_ptr<EcuReprogrammingSequence> Create()
    {
        ProgConfParser progConfParser;
        std::unique_ptr<EcuReprogrammingSequence> reprogrammingSequence;
        logger_.LogInfo() << "Parsing prog conf file:" << progConfFile_;

        auto progConf = progConfParser.ParseFromFile(progConfFile_);

        if (progConf) {
            switch (progConf->GetReprogrammingSequenceType()) {
            case ReprogrammingSequenceType::SingleBankEcu:
                logger_.LogInfo() << "SingleBankEcu reprogramming sequence is created";
                reprogrammingSequence
                    = std::make_unique<SingleBankEcuReprogramming>(pduApiHandle_, std::move(progConf), path_);
                break;
            case ReprogrammingSequenceType::DualBankEcu:
                logger_.LogInfo() << "DualBankEcu reprogramming sequence is created";
                reprogrammingSequence
                    = std::make_unique<DualBankEcuReprogramming>(pduApiHandle_, std::move(progConf), path_);
                break;
            default:
                logger_.LogError() << "Unknown reprogramming sequence type!";
                reprogrammingSequence = nullptr;
                break;
            }
        }

        if (reprogrammingSequence) {
            const auto result = reprogrammingSequence->Init().GetResult();
            if (!result.HasValue()) {
                reprogrammingSequence = nullptr;
            }
        }

        return reprogrammingSequence;
    }

private:
    /**
     * @brief path to files used during reprogramming
     */
    const ara::core::String path_;

    /**
     * @brief path to files used during reprogramming
     */
    ara::core::String progConfFile_;

    std::shared_ptr<PduApiHandle> pduApiHandle_;
    ara::log::Logger& logger_{
        ara::log::CreateLogger("ERSF", "EcuReprogrammingSequenceFactory", ara::log::LogLevel::kVerbose)};
};

#endif  // ARA_UCM_PKGMGR_ECU_REPROGRAMMING_SEQUENCE_FACTORY_H_
