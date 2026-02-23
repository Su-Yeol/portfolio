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

set(ADCM_APPLICATION_ARXMLS
  "Control.arxml"
  "Guardian.arxml"
  "Hdmap.arxml"
  "Hmi.arxml"
  "Katech_Bridge.arxml"
  "Localization.arxml"
  "Perception_Camera.arxml"
  "Perception_Lidar.arxml"
  "Planning.arxml"
  "Routing.arxml"
  "V2X.arxml"
  "TimeSyncProvider.arxml"
  "TimeSyncConsumer.arxml"
)

set(ADCM_INTERFACES_ARXMLS
  "bridge_control.arxml"
  "can_data.arxml"
  "can_data_etc.arxml"
  "collect_fault_control.arxml"
  "collect_fault_hdmap.arxml"
  "collect_fault_hmi.arxml"
  "collect_fault_localization.arxml"
  "collect_fault_perception_camera.arxml"
  "collect_fault_perception_lidar.arxml"
  "collect_fault_planning.arxml"
  "collect_fault_routing.arxml"
  "collect_fault_v2x.arxml"
  "control_bridge.arxml"
  "driving_trajectory.arxml"
  "emergency_braking.arxml"
  "lane_detection.arxml"
  "local_map.arxml"
  "matching_info.arxml"
  "perception_obstacles.arxml"
  "planning_hmi_data.arxml"
  "point_map.arxml"
  "report_fault.arxml"
  "routing_result.arxml"
  "routing_search.arxml"
  "semantic_map.arxml"
  "sensor_radar.arxml"
  "smart_cruise_control.arxml"
  "traffic_light.arxml"
  "v2x_data.arxml"
  "vehicle_location.arxml"
  "error_type.arxml"
  "shared_type.arxml"
  "someip_bridge_control.arxml"
  "someip_can_data.arxml"
  "someip_can_data_etc.arxml"
  "someip_collect_fault_control.arxml"
  "someip_collect_fault_hdmap.arxml"
  "someip_collect_fault_hmi.arxml"
  "someip_collect_fault_localization.arxml"
  "someip_collect_fault_perception_camera.arxml"
  "someip_collect_fault_perception_lidar.arxml"
  "someip_collect_fault_planning.arxml"
  "someip_collect_fault_routing.arxml"
  "someip_collect_fault_v2x.arxml"
  "someip_control_bridge.arxml"
  "someip_driving_trajectory.arxml"
  "someip_emergency_braking.arxml"
  "someip_lane_detection.arxml"
  "someip_local_map.arxml"
  "someip_matching_info.arxml"
  "someip_perception_obstacles.arxml"
  "someip_planning_hmi_data.arxml"
  "someip_point_map.arxml"
  "someip_report_fault.arxml"
  "someip_routing_result.arxml"
  "someip_routing_search.arxml"
  "someip_semantic_map.arxml"
  "someip_sensor_radar.arxml"
  "someip_smart_cruise_control.arxml"
  "someip_traffic_light.arxml"
  "someip_v2x_data.arxml"
  "someip_vehicle_location.arxml"
)

set(ADCM_MACHINE_ARXMLS
  "machine_manifest.arxml"
)

set(ARA_COMMON_ARXMLS
  "package_management.arxml"
  "ucmtypes.arxml"
  "vehicle_package_management.arxml"
)

set(ADCM_PROCESS_ARXMLS
  "processes/bridge_control_service_instance_manifest.arxml"
  "processes/can_data_service_instance_manifest.arxml"
  "processes/can_data_etc_service_instance_manifest.arxml"
  "processes/collect_fault_control_service_instance_manifest.arxml"
  "processes/collect_fault_hdmap_service_instance_manifest.arxml"
  "processes/collect_fault_hmi_service_instance_manifest.arxml"
  "processes/collect_fault_localization_service_instance_manifest.arxml"
  "processes/collect_fault_perception_camera_service_instance_manifest.arxml"
  "processes/collect_fault_perception_lidar_service_instance_manifest.arxml"
  "processes/collect_fault_planning_service_instance_manifest.arxml"
  "processes/collect_fault_routing_service_instance_manifest.arxml"
  "processes/collect_fault_v2x_service_instance_manifest.arxml"
  "processes/control_bridge_service_instance_manifest.arxml"
  "processes/Control_execution_manifest.arxml"
  "processes/driving_trajectory_service_instance_manifest.arxml"
  "processes/emergency_braking_service_instance_manifest.arxml"
  "processes/Guardian_execution_manifest.arxml"
  "processes/Hdmap_execution_manifest.arxml"
  "processes/Hmi_execution_manifest.arxml"
  "processes/Katech_Bridge_execution_manifest.arxml"
  "processes/lane_detection_service_instance_manifest.arxml"
  "processes/local_map_service_instance_manifest.arxml"
  "processes/Localization_execution_manifest.arxml"
  "processes/matching_info_service_instance_manifest.arxml"
  "processes/Perception_Camera_execution_manifest.arxml"
  "processes/Perception_Lidar_execution_manifest.arxml"
  "processes/perception_obstacles_service_instance_manifest.arxml"
  "processes/planning_hmi_data_service_instance_manifest.arxml"
  "processes/point_map_service_instance_manifest.arxml"
  "processes/Planning_execution_manifest.arxml"
  "processes/report_fault_service_instance_manifest.arxml"
  "processes/Routing_execution_manifest.arxml"
  "processes/routing_result_service_instance_manifest.arxml"
  "processes/routing_search_service_instance_manifest.arxml"
  "processes/semantic_map_service_instance_manifest.arxml"
  "processes/sensor_radar_service_instance_manifest.arxml"
  "processes/smart_cruise_control_service_instance_manifest.arxml"
  "processes/traffic_light_service_instance_manifest.arxml"
  "processes/v2x_data_service_instance_manifest.arxml"
  "processes/V2X_execution_manifest.arxml"
  "processes/vehicle_location_service_instance_manifest.arxml"
  "processes/TimeSyncProvider_execution_manifest.arxml"
  "processes/TimeSyncProvider_process_design.arxml"
  "processes/TimeSyncConsumer_execution_manifest.arxml"
  "processes/TimeSyncConsumer_process_design.arxml"
)