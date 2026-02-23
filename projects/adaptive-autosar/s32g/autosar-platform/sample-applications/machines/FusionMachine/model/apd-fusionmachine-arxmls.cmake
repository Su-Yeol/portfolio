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
  "controller.arxml"
  "EmoSampleApplication.arxml"
  "fusion.arxml"
  "PersistencyDemo.arxml"
  "PersistencyRedundancyDemo.arxml"
  "shmMaster.arxml"
  "StateManager.arxml"
  "TimeSyncConsumer.arxml"
  "TlvDemoReceiver.arxml"
  "UCM_flashing_adapter.arxml"
  "UCM_ota_client.arxml"
  "UCM_package_manager.arxml"
  "UCM_transfer_performance_check.arxml"
  "UpdatableApp.arxml"
  "vehicle_state_manager_application.arxml"
)

set(APD_INTERFACES_ARXMLS
  "actor_someip.arxml"
  "actor.arxml"
  "healthIndicator_someip.arxml"
  "healthIndicator.arxml"
  "healthInfo_someip.arxml"
  "healthInfo.arxml"
  "line_sensor_someip.arxml"
  "line_sensor.arxml"
  "persistency_kvs_demo.arxml"
  "persistency_redundancy_demo.arxml"
  "radar_someip.arxml"
  "radar.arxml"
  "routine_someip.arxml"
  "statemachine_service_someip.arxml"
  "swclusterdescription_kvsinterface.arxml"
  "tlv_demo_sender_someip.arxml"
  "tlv_demo_sender.arxml"
  "trigger_in_out_someip.arxml"
  "trigger_in_someip.arxml"
  "trigger_out_someip.arxml"
  "ucm_deployment_manifest.arxml"
  "ucm_master_deployment_manifest.arxml"
  "ucm_package_manager_kvs.arxml"
  "ucm_transfer_status_kvsinterface.arxml"
  "ultrasonic_sensor_someip.arxml"
  "ultrasonic_sensor.arxml"
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
  "vehicle_package_management.arxml"
)

set(APD_PROCESS_ARXMLS
  "processes/ara_package_manager_execution_manifest.arxml"
  "processes/ara_package_manager_instance_manifest.arxml"
  "processes/ara_tsync_daemon_slave_execution_manifest.arxml"
  "processes/controller_execution_manifest.arxml"
  "processes/controller_instance_manifest.arxml"
  "processes/EmoSampleApplication_execution_manifest.arxml"
  "processes/EmoSampleApplication_process_design.arxml"
  "processes/flashing_adapter_execution_manifest.arxml"
  "processes/flashing_adapter_service_instance_manifest.arxml"
  "processes/fusion_execution_manifest.arxml"
  "processes/fusion_process_design.arxml"
  "processes/fusion_service_instance_manifest.arxml"
  "processes/ota_client_execution_manifest.arxml"
  "processes/ota_client_instance_manifest.arxml"
  "processes/PersistencyDemo_execution_manifest.arxml"
  "processes/PersistencyRedundancyDemo_execution_manifest.arxml"
  "processes/PersistencyRedundancyDemo_process_design.arxml"
  "processes/shmMaster_execution_manifest.arxml"
  "processes/shmMaster_service_instance_manifest.arxml"
  "processes/shmMaster_process_design.arxml"
  "processes/StateManager_execution_manifest.arxml"
  "processes/StateManager_process_design.arxml"
  "processes/StateManager_service_instance_manifest.arxml"
  "processes/TimeSyncConsumer_execution_manifest.arxml"
  "processes/TimeSyncConsumer_process_design.arxml"
  "processes/TlvDemoReceiver_process_design.arxml"
  "processes/TlvDemoReceiver_execution_manifest.arxml"
  "processes/TlvDemoReceiver_service_instance_manifest.arxml"
  "processes/transfer_performance_check_execution_manifest.arxml"
  "processes/transfer_performance_check_instance_manifest.arxml"
  "processes/UpdatableApp_execution_manifest.arxml"
  "processes/UpdatableApp.arxml"
  "processes/vsm_execution_manifest.arxml"
  "processes/vsm_service_instance_manifest.arxml"
)
