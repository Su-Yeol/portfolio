# #  --------------------------------------------------------------------------
# #  |              _    _ _______     .----.      _____         _____        |
# #  |         /\  | |  | |__   __|  .  ____ .    / ____|  /\   |  __ \       |
# #  |        /  \ | |  | |  | |    .  / __ \ .  | (___   /  \  | |__) |      |
# #  |       / /\ \| |  | |  | |   .  / / / / v   \___ \ / /\ \ |  _  /       |
# #  |      / /__\ \ |__| |  | |   . / /_/ /  .   ____) / /__\ \| | \ \       |
# #  |     /________\____/   |_|   ^ \____/  .   |_____/________\_|  \_\      |
# #  |                              . _ _  .                                  |
# #  --------------------------------------------------------------------------
# #
# #  All Rights Reserved.
# #  Any use of this source code is subject to a license agreement with the
# #  AUTOSAR development cooperation.
# #  More information is available at www.autosar.org.
# #
# #  Disclaimer
# #
# #  This work (specification and/or software implementation) and the material
# #  contained in it, as released by AUTOSAR, is for the purpose of information
# #  only. AUTOSAR and the companies that have contributed to it shall not be
# #  liable for any use of the work.
# #
# #  The material contained in this work is protected by copyright and other
# #  types of intellectual property rights. The commercial exploitation of the
# #  material contained in this work requires a license to such intellectual
# #  property rights.
# #
# #  This work may be utilized or reproduced without any modification, in any
# #  form or by any means, for informational purposes only. For any other
# #  purpose, no part of the work may be utilized or reproduced, in any form
# #  or by any means, without permission in writing from the publisher.
# #
# #  The work has been developed for automotive applications only. It has
# #  neither been developed, nor tested for non-automotive applications.
# #
# #  The word AUTOSAR and the AUTOSAR logo are registered trademarks.
# #  --------------------------------------------------------------------------

# cmake_minimum_required(VERSION 3.4)
# project(apd-interfaces-arxmls VERSION 1.0.0 LANGUAGES NONE)

# include(AUTOSAR/apd)

# include(GNUInstallDirs)
# include(CMakePackageConfigHelpers)

# # List of all interface arxml files available in sample-applications.
# set(APD_INTERFACES_ARXMLS_FILES
#   "actor.arxml"
#   "actor_someip.arxml"
#   "alive_someip.arxml"
#   "alive.arxml"
#   "brake_feedback_someip.arxml"
#   "brake_feedback.arxml"
#   "brake_someip.arxml"
#   "brake.arxml"
#   "CameraDetection_someip.arxml"
#   "CameraFeatures_someip.arxml"
#   "cmservice1_Someip.arxml"
#   "cmservice1.arxml"
#   "cmservice5_Someip.arxml"
#   "cmservice5.arxml"
#   "cmservice6_Someip.arxml"
#   "cmservice6.arxml"
#   "cmts_someip.arxml"
#   "cmts.arxml"
#   "cm_validator_publisher_someip.arxml"
#   "cm_validator_publisher.arxml"
#   "data_identifier_someip.arxml"
#   "data_identifier.arxml"
#   "ddsservice1_Dds.arxml"
#   "ddsservice1.arxml"
#   "ddsservice2_Dds.arxml"
#   "ddsservice2.arxml"
#   "ddsservice3_Dds.arxml"
#   "ddsservice3.arxml"
#   "ddsservice4_Dds.arxml"
#   "ddsservice4.arxml"
#   "ddsservice5_Dds.arxml"
#   "ddsservice5.arxml"
#   "fake_video_someip.arxml"
#   "fake_video.arxml"
#   "healthIndicator.arxml"
#   "healthIndicator_someip.arxml"
#   "healthInfo.arxml"
#   "healthInfo_someip.arxml"
#   "image_pp_someip.arxml"
#   "image_pp.arxml"
#   "image_someip.arxml"
#   "image.arxml"
#   "lane_someip.arxml"
#   "lane.arxml"
#   "lidar.arxml"
#   "LidarDetections_someip.arxml"
#   "line_sensor.arxml"
#   "line_sensor_someip.arxml"
#   "persistency_kvs_demo.arxml"
#   "persistency_redundancy_demo.arxml"
#   "PotentiallyMovingObjects_someip.arxml"
#   "radar_dds.arxml"
#   "radar_someip.arxml"
#   "radar.arxml"
#   "raw_service_validation.arxml"
#   "raw_uds.arxml"
#   "res_timingrefserviceapp_someip.arxml"
#   "res_timingrefserviceapp.arxml"
#   "res_titservice.arxml"
#   "res_titservice_someip.arxml"
#   "RadarDetections_someip.arxml"
#   "RoadObjects_someip.arxml"
#   "routine_someip.arxml"
#   "statemachine_service_someip.arxml"
#   "StaticObjects_someip.arxml"
#   "swclusterdescription_kvsinterface.arxml"
#   "tlv_demo_sender.arxml"
#   "tlv_demo_sender_someip.arxml"
#   "trigger_in_out_someip.arxml"
#   "trigger_in_someip.arxml"
#   "trigger_out_someip.arxml"
#   "TutorialInterface_someip_deployment.arxml"
#   "TutorialInterface.arxml"
#   "ucm_deployment_manifest.arxml"
#   "ucm_master_deployment_manifest.arxml"
#   "ucm_package_manager_kvs.arxml"
#   "ucm_transfer_status_kvsinterface.arxml"
#   "ucm_vehicle_package_manager_kvs.arxml"
#   "ultrasonic_sensor_someip.arxml"
#   "ultrasonic_sensor.arxml"
#   "UltrasonicDetections_someip.arxml"
#   "UltrasonicFeatures_someip.arxml"
#   "update_allowed_service_someip.arxml"
#   "update_request_someip.arxml"
#   "vehicle_someip.arxml"
#   "vehicle.arxml"
#   "video_someip.arxml"
#   "video.arxml"
# )

# set(APD_INTERFACES_ARXMLS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")

# # Create *-config.cmake file
# configure_package_config_file(
#   ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
#   ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
#   INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
#   PATH_VARS APD_INTERFACES_ARXMLS_DIR
# )

# # Install *-config.cmake file
# install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
#   DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
# )

# # Install the arxml files
# install(FILES ${APD_INTERFACES_ARXMLS_FILES}
#   DESTINATION "${APD_INTERFACES_ARXMLS_DIR}"


cmake_minimum_required(VERSION 3.4)
project(katech-interfaces-arxmls VERSION 1.0.0 LANGUAGES NONE)


include(AUTOSAR/apd)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# List of all interface arxml files available in sample-applications.
set(KATECH_INTERFACES_ARXMLS_FILES
  "services/bridge_control.arxml"
  "services/can_data.arxml"
  "services/can_data_etc.arxml"
  "services/collect_fault_control.arxml"
  "services/collect_fault_hdmap.arxml"
  "services/collect_fault_hmi.arxml"
  "services/collect_fault_localization.arxml"
  "services/collect_fault_perception_camera.arxml"
  "services/collect_fault_perception_lidar.arxml"
  "services/collect_fault_planning.arxml"
  "services/collect_fault_routing.arxml"
  "services/collect_fault_v2x.arxml"
  "services/control_bridge.arxml"
  "services/driving_trajectory.arxml"
  "services/emergency_braking.arxml"
  "services/lane_detection.arxml"
  "services/local_map.arxml" 
  "services/matching_info.arxml"
  "services/perception_obstacles.arxml"
  "services/planning_hmi_data.arxml"
  "services/point_map.arxml"
  "services/report_fault.arxml"
  "services/routing_result.arxml"
  "services/routing_search.arxml"
  "services/semantic_map.arxml"
  "services/sensor_radar.arxml"
  "services/smart_cruise_control.arxml"
  "services/traffic_light.arxml"
  "services/v2x_data.arxml"
  "services/vehicle_location.arxml"
  "shared_type/error_type.arxml"
  "shared_type/shared_type.arxml"
  "system/someip/someip_bridge_control.arxml"
  "system/someip/someip_can_data.arxml"
  "system/someip/someip_can_data_etc.arxml"
  "system/someip/someip_collect_fault_control.arxml"
  "system/someip/someip_collect_fault_hdmap.arxml"
  "system/someip/someip_collect_fault_hmi.arxml"
  "system/someip/someip_collect_fault_localization.arxml"
  "system/someip/someip_collect_fault_perception_camera.arxml"
  "system/someip/someip_collect_fault_perception_lidar.arxml"
  "system/someip/someip_collect_fault_planning.arxml"
  "system/someip/someip_collect_fault_routing.arxml"
  "system/someip/someip_collect_fault_v2x.arxml"
  "system/someip/someip_control_bridge.arxml"
  "system/someip/someip_driving_trajectory.arxml"
  "system/someip/someip_emergency_braking.arxml"
  "system/someip/someip_lane_detection.arxml"
  "system/someip/someip_local_map.arxml"
  "system/someip/someip_matching_info.arxml"
  "system/someip/someip_perception_obstacles.arxml"
  "system/someip/someip_planning_hmi_data.arxml"
  "system/someip/someip_point_map.arxml"
  "system/someip/someip_report_fault.arxml"
  "system/someip/someip_routing_result.arxml"
  "system/someip/someip_routing_search.arxml"
  "system/someip/someip_semantic_map.arxml"
  "system/someip/someip_sensor_radar.arxml"
  "system/someip/someip_smart_cruise_control.arxml"
  "system/someip/someip_traffic_light.arxml"
  "system/someip/someip_v2x_data.arxml"
  "system/someip/someip_vehicle_location.arxml"
)

set(KATECH_INTERFACES_ARXMLS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")

# Create *-config.cmake file
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  PATH_VARS KATECH_INTERFACES_ARXMLS_DIR
)

# Install *-config.cmake file
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

# Install the arxml files
install(FILES ${KATECH_INTERFACES_ARXMLS_FILES}
  DESTINATION "${KATECH_INTERFACES_ARXMLS_DIR}"
)
