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
  "ST_CMApp01.arxml"
  "ST_CMApp03.arxml"
  "ST_CMApp10.arxml"
  "ST_CMApp12.arxml"
  "ST_CMApp13.arxml"
  "ST_DDSApp01.arxml"
  "ST_DDSApp02.arxml"
  "ST_DDSApp03.arxml"
  "ST_DIAGApp01.arxml"
  "STBG_EMOAppSysFG.arxml"
  "STBG_EMOAppSysFG1.arxml"
  "STBG_EMOAppSysFG2.arxml"
  "STC1_EMOApp02.arxml"
  "STC1_EMOApp03.arxml"
  "STC1_EMOApp04.arxml"
  "STC1_EMOApp05.arxml"
  "STC2_EMOApp02.arxml"
  "STC2_EMOApp03.arxml"
  "STC2_EMOApp04.arxml"
  "STC2_EMOApp05.arxml"
  "STC2_EMOApp06.arxml"
  "StateManager.arxml"
  "UCM_package_manager.arxml"
)

set(APD_INTERFACES_ARXMLS
  "cmservice1_Someip.arxml"
  "cmservice1.arxml"
  "cmservice5_Someip.arxml"
  "cmservice5.arxml"
  "cmservice6_Someip.arxml"
  "cmservice6.arxml"
  "data_identifier_someip.arxml"
  "data_identifier.arxml"
  "ddsservice1_Dds.arxml"
  "ddsservice1.arxml"
  "ddsservice2_Dds.arxml"
  "ddsservice2.arxml"
  "ddsservice3_Dds.arxml"
  "ddsservice3.arxml"
  "ddsservice4_Dds.arxml"
  "ddsservice4.arxml"
  "ddsservice5_Dds.arxml"
  "ddsservice5.arxml"
  "routine_someip.arxml"
  "statemachine_service_someip.arxml"
  "trigger_in_out_someip.arxml"
  "trigger_in_someip.arxml"
  "trigger_out_someip.arxml"
  "ucm_deployment_manifest.arxml"
  "ucm_package_manager_kvs.arxml"
  "ucm_transfer_status_kvsinterface.arxml"
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

set(ARA_COMMON_ARXMLS
  "routine.arxml"
  "package_management.arxml"
  "shared_types.arxml"
  "statemachine_service.arxml"
  "trigger_in_out.arxml"
  "trigger_in.arxml"
  "trigger_out.arxml"
  "ucmtypes.arxml"
  "update_allowed_service.arxml"
  "update_request.arxml"
)

set(APD_PROCESS_ARXMLS
  "processes/ara_package_manager_execution_manifest.arxml"
  "processes/ara_package_manager_instance_manifest.arxml"
  "processes/ST_CMApp01Instance1_execution_manifest.arxml"
  "processes/ST_CMApp01Instance1_process_design.arxml"
  "processes/ST_CMApp01Instance1_service_instance_manifest.arxml"
  "processes/ST_CMApp03Instance1_execution_manifest.arxml"
  "processes/ST_CMApp03Instance1_process_design.arxml"
  "processes/ST_CMApp03Instance1_service_instance_manifest.arxml"
  "processes/ST_CMApp10Instance1_execution_manifest.arxml"
  "processes/ST_CMApp10Instance1_process_design.arxml"
  "processes/ST_CMApp10Instance1_service_instance_manifest.arxml"
  "processes/ST_CMApp12Instance1_execution_manifest.arxml"
  "processes/ST_CMApp12Instance1_process_design.arxml"
  "processes/ST_CMApp12Instance1_service_instance_manifest.arxml"
  "processes/ST_CMApp13Instance1_execution_manifest.arxml"
  "processes/ST_CMApp13Instance1_process_design.arxml"
  "processes/ST_CMApp13Instance1_service_instance_manifest.arxml"
  "processes/ST_DDSApp01Instance1_execution_manifest.arxml"
  "processes/ST_DDSApp01Instance1_process_design.arxml"
  "processes/ST_DDSApp01Instance1_service_instance_manifest.arxml"
  "processes/ST_DDSApp02Instance1_execution_manifest.arxml"
  "processes/ST_DDSApp02Instance1_process_design.arxml"
  "processes/ST_DDSApp02Instance1_service_instance_manifest.arxml"
  "processes/ST_DIAGApp01Instance1_execution_manifest.arxml"
  "processes/ST_DIAGApp01Instance1_process_design.arxml"
  "processes/ST_DIAGApp01Instance1_service_instance_manifest.arxml"
  "processes/STBG_EMOApp_process_design.arxml"
  "processes/STBG_EMOApp_execution_manifest.arxml"
  "processes/STC_EMO_00001_process_design.arxml"
  "processes/STC_EMO_00001_execution_manifest.arxml"
  "processes/STC_EMO_00002_process_design.arxml"
  "processes/STC_EMO_00002_execution_manifest.arxml"
  "processes/StateManager_execution_manifest.arxml"
  "processes/StateManager_service_instance_manifest.arxml"
  "processes/StateManager.arxml"
)
