#  --------------------------------------------------------------------------
#  |              _    _ _______     .----.      _____         _____        |
#  |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
#  |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
#  |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
#  |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
#  |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
#  |                              . _ _  .                                  |
#  --------------------------------------------------------------------------
#
#  All Rights Reserved.
#  Any use of this source code is subject to a license agreement with the
#  AUTOSAR development cooperation.
#  More information is available at www.autosar.org.
#
#  Disclaimer
#
#  This work (specification and/or software implementation) and the material
#  contained in it, as released by AUTOSAR, is for the purpose of information
#  only. AUTOSAR and the companies that have contributed to it shall not be
#  liable for any use of the work.
#
#  The material contained in this work is protected by copyright and other
#  types of intellectual property rights. The commercial exploitation of the
#  material contained in this work requires a license to such intellectual
#  property rights.
#
#  This work may be utilized or reproduced without any modification, in any
#  form or by any means, for informational purposes only. For any other
#  purpose, no part of the work may be utilized or reproduced, in any form
#  or by any means, without permission in writing from the publisher.
#
#  The work has been developed for automotive applications only. It has
#  neither been developed, nor tested for non-automotive applications.
#
#  The word AUTOSAR and the AUTOSAR logo are registered trademarks.
#  --------------------------------------------------------------------------

set(APD_APPLICATION_ARXMLS
  "EmoSampleApplication.arxml"
  "fusion.arxml"
  "PersistencyDemo.arxml"
  "PersistencyRedundancyDemo.arxml"
  "radar.arxml"
  "StateManager.arxml"
)

set(APD_INTERFACES_ARXMLS
  "persistency_kvs_demo.arxml"
  "persistency_redundancy_demo.arxml"
  "radar_dds.arxml"
  "radar.arxml"
  "routine_someip.arxml"
  "statemachine_service_someip.arxml"
  "trigger_in_out_someip.arxml"
  "trigger_in_someip.arxml"
  "trigger_out_someip.arxml"
  "update_allowed_service_someip.arxml"
  "update_request_someip.arxml"
)

set(APD_MACHINE_ARXMLS
  "machine_manifest.arxml"
)

set(APD_DLT_ARXMLS
  "DltContext/default_dltcontext.arxml"
  "dltlogsink_not_machine_based.arxml"
)

set(APD_PROCESS_ARXMLS
  "processes/EmoSampleApplication_execution_manifest.arxml"
  "processes/EmoSampleApplication_process_design.arxml"
  "processes/fusion_execution_manifest.arxml"
  "processes/fusion_process_design.arxml"
  "processes/fusion_service_instance_manifest.arxml"
  "processes/PersistencyDemo_execution_manifest.arxml"
  "processes/PersistencyRedundancyDemo_execution_manifest.arxml"
  "processes/PersistencyRedundancyDemo_process_design.arxml"
  "processes/radar_execution_manifest.arxml"
  "processes/radar_process_design.arxml"
  "processes/radar_service_instance_manifest.arxml"
  "processes/StateManager_execution_manifest.arxml"
  "processes/StateManager_process_design.arxml"
  "processes/StateManager_service_instance_manifest.arxml"
)
