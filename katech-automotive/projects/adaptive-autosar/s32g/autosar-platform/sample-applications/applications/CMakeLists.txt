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

# cmake_minimum_required(VERSION 3.15)

# project(apd-applications-arxmls VERSION 1.0.0 LANGUAGES NONE)

# include(GNUInstallDirs)
# include(CMakePackageConfigHelpers)

# # List of all application arxml files available in sample-applications
# set(APD_APPLICATIONS_ARXMLS
#   "AdiOsiAdapter/AdiOsiAdapter.arxml"
#   "CMValidatorPublisher/CMValidatorPublisher.arxml"
#   "CMValidatorSubscriber/CMValidatorSubscriber.arxml"
#   "DemonstratorTutorial/DemonstratorTutorial.arxml"
#   "EmoIntegrationTest/EmoIntegrationTest.arxml"
#   "EmoSampleApplication/EmoSampleApplication.arxml"
#   "ExtendedTutorial/ExtendedTutorial.arxml"
#   "FaultyCalibrationApplication/FaultyCalibrationApplication.arxml"
#   "FaultyProbingApplication/FaultyProbingApplication.arxml"
#   "FaultyReportingApplication/FaultyReportingApplication.arxml"
#   "fc_firewall/fc_firewall.arxml"
#   "fusion/fusion.arxml"
#   "GenericAdasApplication/GenericAdasApplication.arxml"
#   "OsiAdiAdapter/OsiAdiAdapter.arxml"
#   "OEM_App/OEMApp.arxml"
#   "PersistencyDemo/PersistencyDemo.arxml"
#   "PersistencyRedundancyDemo/PersistencyRedundancyDemo.arxml"
#   "PhmDemo/PhmDemo.arxml"
#   "PiCar_Actor/files/actor.arxml"
#   "PiCar_Controller/files/controller.arxml"
#   "PiCar_Linesensor/files/linesensor.arxml"
#   "PiCar_Ultrasonicsensor/files/ultrasonicsensor.arxml"
#   "radar/radar.arxml"
#   "RES_TimingRefApp/RES_TimingRefApp.arxml"
#   "RES_TimingRefServiceApp/RES_TimingRefServiceApp.arxml"
#   "RES_TraceIntegrationTest/RES_TraceIntegrationTest.arxml"
#   "RES_TraceIntegrationTest2/RES_TraceIntegrationTest2.arxml"
#   "SecDemoFc/secdemofc.arxml"
#   "SecDemoFc/secdemo_access_manager.arxml"
#   "SecIamTestApp/seciamtestapp.arxml"
#   "SensorDemoApplication/SensorDemoApplication.arxml"
#   "SmartCameraSensor/SmartCameraSensor.arxml"
#   "smtool/smtool.arxml"
#   "ST_CMApp01/ST_CMApp01.arxml"
#   "ST_CMApp02/ST_CMApp02.arxml"
#   "ST_CMApp03/ST_CMApp03.arxml"
#   "ST_CMApp09/ST_CMApp09.arxml"
#   "ST_CMApp10/ST_CMApp10.arxml"
#   "ST_CMApp11/ST_CMApp11.arxml"
#   "ST_CMApp12/ST_CMApp12.arxml"
#   "ST_CMApp13/ST_CMApp13.arxml"
#   "ST_CMApp14/ST_CMApp14.arxml"
#   "ST_DDSApp01/ST_DDSApp01.arxml"
#   "ST_DDSApp02/ST_DDSApp02.arxml"
#   "ST_DDSApp03/ST_DDSApp03.arxml" 
#   "ST_DIAGApp01/ST_DIAGApp01.arxml"
#   "ST_EMOApps/STBG/SystemFG/STBG_EMOAppSysFG.arxml"
#   "ST_EMOApps/STBG/SystemFG1/STBG_EMOAppSysFG1.arxml"
#   "ST_EMOApps/STBG/SystemFG2/STBG_EMOAppSysFG2.arxml"  
#   "ST_EMOApps/STC1/EMOApp02/STC1_EMOApp02.arxml"
#   "ST_EMOApps/STC1/EMOApp03/STC1_EMOApp03.arxml"
#   "ST_EMOApps/STC1/EMOApp04/STC1_EMOApp04.arxml"
#   "ST_EMOApps/STC1/EMOApp05/STC1_EMOApp05.arxml"
#   "ST_EMOApps/STC2/EMOApp02/STC2_EMOApp02.arxml"
#   "ST_EMOApps/STC2/EMOApp03/STC2_EMOApp03.arxml"
#   "ST_EMOApps/STC2/EMOApp04/STC2_EMOApp04.arxml"
#   "ST_EMOApps/STC2/EMOApp05/STC2_EMOApp05.arxml"
#   "ST_EMOApps/STC2/EMOApp06/STC2_EMOApp06.arxml"
#   "StateManager/StateManager.arxml"
#   "TimeSyncConsumer/TimeSyncConsumer.arxml"
#   "TimeSyncProvider/TimeSyncProvider.arxml"
#   "TlvDemoReceiver/TlvDemoReceiver.arxml"
#   "TlvDemoSender/TlvDemoSender.arxml"
#   "UCM_ara_api_applications/UCM_package_manager.arxml"
#   "UCM_ara_api_applications/UCM_vehicle_package_manager.arxml"
#   "UCM_flashing_adapter/UCM_flashing_adapter.arxml"
#   "UCM_ota_client/files/UCM_ota_client.arxml"
#   "UCM_pkgmgr_sample/files/UCM_pkgmgr_sample.arxml"
#   "UCM_transfer_performance_check/files/UCM_transfer_performance_check.arxml"
#   "UCM_update_adapter/files/UCM_update_adapter_agl.arxml"
#   "UCM_update_adapter/files/UCM_update_adapter_android.arxml"
#   "UCM_vehicle_driver_application/files/vehicle_driver_application.arxml"
#   "UCM_vehicle_state_manager_application/files/vehicle_state_manager_application.arxml"
#   "UpdatableApp/UpdatableApp.arxml"
#   "shmDemo/shmClient1/shmClient1.arxml"
#   "shmDemo/shmClient2/shmClient2.arxml"
#   "shmDemo/shmMaster/shmMaster.arxml"
# )

# set(APD_APPLICATIONS_ARXMLS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")

# # Create *-config.cmake file
# configure_package_config_file(
#   ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
#   ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
#   INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
#   PATH_VARS APD_APPLICATIONS_ARXMLS_DIR
# )

# # Install *-config.cmake file
# install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
#   DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
# )

# # Install the arxml files
# install(FILES ${APD_APPLICATIONS_ARXMLS}
#   DESTINATION "${APD_APPLICATIONS_ARXMLS_DIR}"
# )


cmake_minimum_required(VERSION 3.15)

project(adcm-applications-arxmls VERSION 1.0.0 LANGUAGES NONE)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# List of all application arxml files available in sample-applications
set(ADCM_APPLICATIONS_ARXMLS
  "KATECH/Control/Control.arxml"
  "KATECH/Guardian/Guardian.arxml"
  "KATECH/Hdmap/Hdmap.arxml"
  "KATECH/Hmi/Hmi.arxml"
  "KATECH/Katech_Bridge/Katech_Bridge.arxml"
  "KATECH/Localization/Localization.arxml"
  "KATECH/Perception_Camera/Perception_Camera.arxml"
  "KATECH/Perception_Lidar/Perception_Lidar.arxml"
  "KATECH/Planning/Planning.arxml"
  "KATECH/Routing/Routing.arxml"
  "KATECH/V2X/V2X.arxml"
  "KATECH/TimeSyncProvider/TimeSyncProvider.arxml"
  "KATECH/TimeSyncConsumer/TimeSyncConsumer.arxml"
  "KATECH/system_components.arxml"
)

set(ADCM_APPLICATIONS_ARXMLS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")

# Create *-config.cmake file
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/KATECH/cmake/${PROJECT_NAME}-config.cmake.in
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  PATH_VARS ADCM_APPLICATIONS_ARXMLS_DIR
)

# Install *-config.cmake file
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

# Install the arxml files
install(FILES ${ADCM_APPLICATIONS_ARXMLS}
  DESTINATION "${ADCM_APPLICATIONS_ARXMLS_DIR}"
)


