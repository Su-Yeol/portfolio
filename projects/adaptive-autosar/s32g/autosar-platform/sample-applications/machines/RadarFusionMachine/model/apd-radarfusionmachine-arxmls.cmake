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
  "actor.arxml"
  "AdiOsiAdapter.arxml" 
  "CMValidatorPublisher.arxml"
  "CMValidatorSubscriber.arxml"
  "controller.arxml"
  "DemonstratorTutorial.arxml"
  "EmoIntegrationTest.arxml"
  "EmoSampleApplication.arxml"
  "ExtendedTutorial.arxml"
  "FaultyCalibrationApplication.arxml"
  "FaultyProbingApplication.arxml"
  "FaultyReportingApplication.arxml"
  "fc_firewall.arxml"
  "fusion.arxml"
  "linesensor.arxml"
  "OsiAdiAdapter.arxml"
  "GenericAdasApplication.arxml"
  "OEMApp.arxml"
  "PersistencyDemo.arxml"
  "PersistencyRedundancyDemo.arxml"
  "PhmDemo.arxml"
  "radar.arxml"
  "RES_TimingRefApp.arxml"
  "RES_TimingRefServiceApp.arxml"
  "RES_TraceIntegrationTest.arxml"
  "RES_TraceIntegrationTest2.arxml"
  "secdemo_access_manager.arxml"
  "secdemofc.arxml"
  "seciamtestapp.arxml"
  "SensorDemoApplication.arxml"
  "SmartCameraSensor.arxml"
  "smtool.arxml"
  "StateManager.arxml"
  "TimeSyncConsumer.arxml"
  "TimeSyncProvider.arxml"
  "TlvDemoReceiver.arxml"
  "TlvDemoSender.arxml"
  "UCM_flashing_adapter.arxml"
  "UCM_ota_client.arxml"
  "UCM_package_manager.arxml"
  "UCM_pkgmgr_sample.arxml"
  "UCM_transfer_performance_check.arxml"
  "UCM_update_adapter_agl.arxml"
  "UCM_update_adapter_android.arxml"
  "UCM_vehicle_package_manager.arxml"
  "ultrasonicsensor.arxml"
  "UpdatableApp.arxml"
  "vehicle_driver_application.arxml"
  "vehicle_state_manager_application.arxml"
)

set(APD_INTERFACES_ARXMLS
  "actor_someip.arxml"
  "actor.arxml"
  "CameraFeatures_someip.arxml"
  "CameraDetection_someip.arxml"
  "cm_validator_publisher_someip.arxml"
  "cm_validator_publisher.arxml"
  "LidarDetections_someip.arxml"
  "line_sensor_someip.arxml"
  "line_sensor.arxml"
  "persistency_kvs_demo.arxml"
  "persistency_redundancy_demo.arxml"
  "PotentiallyMovingObjects_someip.arxml"
  "radar_someip.arxml"
  "radar.arxml"
  "RadarDetections_someip.arxml"
  "res_timingrefserviceapp_someip.arxml"
  "res_timingrefserviceapp.arxml"
  "res_titservice.arxml"
  "res_titservice_someip.arxml"
  "RoadObjects_someip.arxml"
  "routine_someip.arxml"
  "statemachine_service_someip.arxml"
  "StaticObjects_someip.arxml"
  "swclusterdescription_kvsinterface.arxml"
  "tlv_demo_sender_someip.arxml"
  "tlv_demo_sender.arxml"
  "trigger_in_out_someip.arxml"
  "trigger_in_someip.arxml"
  "trigger_out_someip.arxml"
  "TutorialInterface_someip_deployment.arxml"
  "TutorialInterface.arxml"
  "ucm_deployment_manifest.arxml"
  "ucm_master_deployment_manifest.arxml"
  "ucm_package_manager_kvs.arxml"
  "ucm_transfer_status_kvsinterface.arxml"
  "ucm_vehicle_package_manager_kvs.arxml"
  "ultrasonic_sensor_someip.arxml"
  "ultrasonic_sensor.arxml"
  "UltrasonicDetections_someip.arxml"
  "UltrasonicFeatures_someip.arxml"
  "update_allowed_service_someip.arxml"
  "update_request_someip.arxml"
)

set(APD_MACHINE_ARXMLS
  "machine_manifest.arxml"
)

set(APD_DLT_ARXMLS
  "DltContext/default_dltcontext.arxml"
  "DltContext/RES_TraceIntegrationTest_dltcontext.arxml"
  "DltContext/RES_TraceIntegrationTest2_dltcontext.arxml"
  "DltMessage/RES_TraceIntegrationTest_modeledmsg.arxml"
  "dltlogsink_not_machine_based.arxml"
)

set(ARA_COMMON_ARXMLS
  "package_management.arxml"
  "routine.arxml"
  "shared_types.arxml"
  "sm_types.arxml"
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
  "processes/actor_execution_manifest.arxml"
  "processes/actor_instance_manifest.arxml"
  "processes/AdiOsiAdapter_execution_manifest.arxml"
  "processes/AdiOsiAdapter_process_design.arxml"
  "processes/AdiOsiAdapter_service_instance_manifest.arxml"
  "processes/ara_access_manager_execution_manifest.arxml"
  "processes/ara_package_manager_execution_manifest.arxml"
  "processes/ara_package_manager_instance_manifest.arxml"
  "processes/ara_vehicle_package_manager_execution_manifest.arxml"
  "processes/ara_vehicle_package_manager_instance_manifest.arxml"
  "processes/CMValidatorPublisher_execution_manifest.arxml"
  "processes/CMValidatorPublisher_process_design.arxml"
  "processes/CMValidatorPublisher_service_instance_manifest.arxml"
  "processes/CMValidatorSubscriber_execution_manifest.arxml"
  "processes/CMValidatorSubscriber_process_design.arxml"
  "processes/CMValidatorSubscriber_service_instance_manifest.arxml"
  "processes/controller_execution_manifest.arxml"
  "processes/controller_instance_manifest.arxml"
  "processes/DemonstratorTutorial_execution_manifest.arxml"
  "processes/DemonstratorTutorial_process_design.arxml"
  "processes/EmoIntegrationTest_execution_manifest.arxml"
  "processes/EmoSampleApplication_execution_manifest.arxml"
  "processes/EmoSampleApplication_process_design.arxml"
  "processes/ExtendedTutorial_execution_manifest.arxml"
  "processes/ExtendedTutorial_process_design.arxml"
  "processes/FaultyCalibrationApplication_execution_manifest.arxml"
  "processes/FaultyCalibrationApplication_process_design.arxml"
  "processes/FaultyProbingApplication_execution_manifest.arxml"
  "processes/FaultyProbingApplication_process_design.arxml"
  "processes/FaultyReportingApplication_execution_manifest.arxml"
  "processes/flashing_adapter_execution_manifest.arxml"
  "processes/flashing_adapter_service_instance_manifest.arxml"
  "processes/fc_firewall_execution_manifest.arxml"
  "processes/fc_firewall_process_design.arxml"
  "processes/fusion_execution_manifest.arxml"
  "processes/fusion_process_design.arxml"
  "processes/GenericAdasApplication_execution_manifest.arxml"
  "processes/GenericAdasApplication_process_design.arxml"
  "processes/GenericAdasApplication_service_instance_manifest.arxml"
  "processes/fusion_service_instance_manifest.arxml"
  "processes/linesensor_execution_manifest.arxml"
  "processes/linesensor_instance_manifest.arxml"
  "processes/OsiAdiAdapter_execution_manifest.arxml"
  "processes/OsiAdiAdapter_process_design.arxml"
  "processes/OsiAdiAdapter_service_instance_manifest.arxml"
  "processes/OEMApp_execution_manifest.arxml"
  "processes/OEMApp.arxml"
  "processes/ota_client_execution_manifest.arxml"
  "processes/ota_client_instance_manifest.arxml"
  "processes/PersistencyDemo_execution_manifest.arxml"
  "processes/PersistencyRedundancyDemo_execution_manifest.arxml"
  "processes/PersistencyRedundancyDemo_process_design.arxml"
  "processes/PhmDemo_execution_manifest.arxml"
  "processes/PhmDemo_process_design.arxml"
  "processes/pkgmgr_sample_execution_manifest.arxml"
  "processes/pkgmgr_sample_instance_manifest.arxml"
  "processes/radar_execution_manifest.arxml"
  "processes/radar_process_design.arxml"
  "processes/radar_service_instance_manifest.arxml"
  "processes/RES_TimingRefApp_execution_manifest.arxml"
  "processes/RES_TimingRefApp_process_design.arxml"
  "processes/RES_TimingRefApp_service_instance_manifest.arxml"
  "processes/RES_TimingRefServiceApp_execution_manifest.arxml"
  "processes/RES_TimingRefServiceApp_process_design.arxml"
  "processes/RES_TimingRefServiceApp_service_instance_manifest.arxml"
  "processes/RES_TraceIntegrationTest_execution_manifest.arxml"
  "processes/RES_TraceIntegrationTest_process_design.arxml"
  "processes/RES_TraceIntegrationTest_service_instance_manifest.arxml"
  "processes/RES_TraceIntegrationTest2_execution_manifest.arxml"
  "processes/RES_TraceIntegrationTest2_process_design.arxml"
  "processes/RES_TraceIntegrationTest2_service_instance_manifest.arxml"
  "processes/secdemofc_execution_manifest.arxml"
  "processes/secdemofc_process_design.arxml"
  "processes/seciamtestapp_execution_manifest.arxml"
  "processes/seciamtestapp_process_design.arxml"
  "processes/SensorDemoApplication_execution_manifest.arxml"
  "processes/SensorDemoApplication_process_design.arxml"
  "processes/SmartCameraSensor_execution_manifest.arxml"
  "processes/SmartCameraSensor_process_design.arxml"
  "processes/SmartCameraSensor_service_instance_manifest.arxml"
  "processes/smtool_execution_manifest.arxml"
  "processes/smtool_process_design.arxml"
  "processes/smtool_service_instance_manifest.arxml"
  "processes/StateManager_execution_manifest.arxml"
  "processes/StateManager_process_design.arxml"
  "processes/StateManager_service_instance_manifest.arxml"
  "processes/TimeSyncConsumer_execution_manifest.arxml"
  "processes/TimeSyncConsumer_process_design.arxml"
  "processes/TimeSyncProvider_execution_manifest.arxml"
  "processes/TimeSyncProvider_process_design.arxml"
  "processes/TlvDemoReceiver_execution_manifest.arxml"
  "processes/TlvDemoReceiver_process_design.arxml"
  "processes/TlvDemoReceiver_service_instance_manifest.arxml"
  "processes/TlvDemoSender_execution_manifest.arxml"
  "processes/TlvDemoSender_process_design.arxml"
  "processes/TlvDemoSender_service_instance_manifest.arxml"
  "processes/transfer_performance_check_execution_manifest.arxml"
  "processes/TutorialInterface_instance_manifest.arxml"
  "processes/ultrasonicsensor_execution_manifest.arxml"
  "processes/ultrasonicsensor_instance_manifest.arxml"
  "processes/UpdatableApp_execution_manifest.arxml"
  "processes/UpdatableApp.arxml"
  "processes/update_adapter_agl_execution_manifest.arxml"
  "processes/update_adapter_agl_service_instance_manifest.arxml"
  "processes/update_adapter_android_execution_manifest.arxml"
  "processes/update_adapter_android_service_instance_manifest.arxml"
  "processes/vdi_execution_manifest.arxml"
  "processes/vdi_service_instance_manifest.arxml"
  "processes/vsm_execution_manifest.arxml"
  "processes/vsm_service_instance_manifest.arxml"
)
