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
# get_filename_component(PROJECTNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
# project(${PROJECTNAME} VERSION 1.0.0 LANGUAGES CXX)

# # Set these variables (like in the yocto recipe) to build the application as a specific process on a specific machine
# if(NOT TARGET_MACHINE)
#     set(TARGET_MACHINE "RadarFusionMachine")
# endif()
# if(NOT APP_NAME)
#     set(APP_NAME "TimeSyncProvider")
# endif()
# if(NOT PROC_NAME)
#     set(PROC_NAME "TimeSyncProvider")
# endif()

# include(GNUInstallDirs)
# include(AUTOSAR/apd)

# # Generic stuff (ara generator and arxmls)
# find_package(ara-gen REQUIRED)

# find_package(ara-arxmls REQUIRED)
# find_package(ara-adi-sensoritf-arxmls REQUIRED)
# find_package(apd-applications-arxmls REQUIRED)
# find_package(apd-common-machine-arxmls REQUIRED)
# find_package(apd-interfaces-arxmls REQUIRED)
# find_package(apd-network-arxmls REQUIRED)
# find_package(apd-dlt-arxmls REQUIRED)
 
# string(TOLOWER ${TARGET_MACHINE} TARGET_MACHINE_LOWER)
# find_package(apd-${TARGET_MACHINE_LOWER}-arxmls REQUIRED)

# set(THREADS_PREFER_PTHREAD_FLAG ON CACHE BOOL "Prefer -pthread flag for building with pthreads")
# find_package(ara-tsync REQUIRED)
# find_package(ara-core REQUIRED)
# find_package(ara-exec-execution-client REQUIRED)
# find_package(ara-log REQUIRED)
# find_package(Threads REQUIRED)

# # List all applications that will run on the machine
# list(TRANSFORM APD_APPLICATION_ARXMLS PREPEND ${APD_APPLICATIONS_ARXMLS_DIR}/)

# # List all interfaces that will be used by applications on the machine
# list(TRANSFORM APD_INTERFACES_ARXMLS PREPEND ${APD_INTERFACES_ARXMLS_DIR}/)

# # List all adi sensor interfaces that will be used by applications on the machine
# list(TRANSFORM ARA_ADI_SENSORITF_ARXMLS PREPEND ${ARA_ADI_SENSORITF_ARXMLS_DIR}/)

# # List all processes that will run on the machine
# list(TRANSFORM APD_PROCESS_ARXMLS PREPEND ${APD_MACHINE_ARXMLS_DIR}/)

# # List all other files of the machine (like machine manifest)
# list(TRANSFORM APD_MACHINE_ARXMLS PREPEND ${APD_MACHINE_ARXMLS_DIR}/)

# # List all dlt related files that will be used by machine or applications on the machine
# list(TRANSFORM APD_DLT_ARXMLS PREPEND ${APD_DLT_ARXMLS_DIR}/)

# # List all files required from ara-api
# set(ARA_COMMON_ARXMLS
#  ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_GeneralDefinition_DltLogMessages.arxml
#  ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_PhysicalDimensions_Blueprint.arxml
#  ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_StandardTypes.arxml
#  ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_Units_Blueprint.arxml
#  ${ARA_ARXMLS_DIR}/package_management.arxml
#   ${ARA_ARXMLS_DIR}/routine.arxml
#   ${ARA_ARXMLS_DIR}/shared_types.arxml
#   ${ARA_ARXMLS_DIR}/sm_types.arxml
#   ${ARA_ARXMLS_DIR}/statemachine_service.arxml
#   ${ARA_ARXMLS_DIR}/system_components.arxml
#   ${ARA_ARXMLS_DIR}/trigger_in_out.arxml
#   ${ARA_ARXMLS_DIR}/trigger_in.arxml
#   ${ARA_ARXMLS_DIR}/trigger_out.arxml
#   ${ARA_ARXMLS_DIR}/ucmtypes.arxml
#   ${ARA_ARXMLS_DIR}/update_allowed_service.arxml
#   ${ARA_ARXMLS_DIR}/update_request.arxml
#  ${ARA_ARXMLS_DIR}/vehicle_package_management.arxml
# )

# # List all files of the network
# set(APD_NETWORK_ARXMLS
#   ${APD_NETWORK_ARXMLS_DIR}/network.arxml
# )

# # Make use of common machine elements
# set(APD_COMMON_MACHINE_ARXMLS
#   ${APD_COMMON_MACHINE_ARXMLS_DIR}/execution_manifest.arxml
#   ${APD_COMMON_MACHINE_ARXMLS_DIR}/machine_manifest.arxml
#   ${APD_COMMON_MACHINE_ARXMLS_DIR}/service_instance_manifest.arxml
# )

# set(ARXML_SET
#   ${ARA_COMMON_ARXMLS}
#   ${APD_APPLICATION_ARXMLS}
#   ${APD_INTERFACES_ARXMLS}
#   ${APD_COMMON_MACHINE_ARXMLS}
#   ${APD_MACHINE_ARXMLS}
#   ${APD_PROCESS_ARXMLS}
#   ${APD_NETWORK_ARXMLS}
#   ${APD_DLT_ARXMLS}
#   ${ARA_ADI_SENSORITF_ARXMLS}
# )

# set(GEN_DEST ${PROJECT_BINARY_DIR}/gen)

# set(SWC_FQN /apd/AdaptiveApplicationSwComponentTypes/${APP_NAME})
# set(PROCESS_FQN /apd/Machines/${TARGET_MACHINE}/Processes/${PROC_NAME})

# set(PROCESS_ARTIFACTS_MAKE_TARGET ${PROC_NAME}ProcessGenerated)

# add_process_artifacts(
#   DESTINATION ${GEN_DEST}
#   TARGET ${PROCESS_ARTIFACTS_MAKE_TARGET}
#   PROCESS_FQN ${PROCESS_FQN}
#   ARXMLS ${ARXML_SET}
# )

# # Define source files
# set(EXECUTABLE_SOURCE
#     src/tsync_provider.cpp
# )

# add_executable(${PROJECT_NAME} ${EXECUTABLE_SOURCE})

# target_link_libraries(${PROJECT_NAME}
#     ara::exec_execution_client
#     ara::log
#     ara::core
#     Threads::Threads
#     ara::tsync
#     rt
# )

# add_dependencies(${PROJECT_NAME} ${PROCESS_ARTIFACTS_MAKE_TARGET})

# # Install Binary
# apd_install_app_executable(${PROJECT_NAME} ${PROJECT_NAME})

# # Install Execution Manifest
# set(EXECUTION_MANIFEST_PATH ${GEN_DEST}/processes/${PROC_NAME}_MANIFEST.json)
# apd_install_app_execution_manifest(${PROJECT_NAME} ${EXECUTION_MANIFEST_PATH})
# if(${TARGET_MACHINE} STREQUAL "RadarMachine")
# # Add the interface file containing the interface name, type and PTP configuration
# apd_install_app_file(ara_tsync_daemon etc TimeSyncProviderInterface.json)
# endif()

cmake_minimum_required(VERSION 3.15)
get_filename_component(PROJECTNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECTNAME} VERSION 1.0.0 LANGUAGES CXX)

# Set these variables (like in the yocto recipe) to build the application as a specific process on a specific machine
if(NOT TARGET_MACHINE)
    set(TARGET_MACHINE "AdaptiveMachine")
endif()
if(NOT APP_NAME)
    set(APP_NAME "TimeSyncProvider")
endif()
if(NOT PROC_NAME)
    set(PROC_NAME "TimeSyncProvider")
endif()

add_compile_definitions(DEFAULT_LOG_TAG="${PROC_NAME}")

include(GNUInstallDirs)
include(AUTOSAR/apd)

# Generic stuff (ara generator and arxmls)
find_package(ara-gen REQUIRED)

find_package(ara-arxmls REQUIRED)
find_package(ara-adi-sensoritf-arxmls REQUIRED)
find_package(katech-applications-arxmls REQUIRED)
find_package(katech-common-machine-arxmls REQUIRED)
find_package(katech-interfaces-arxmls REQUIRED)
find_package(katech-network-arxmls REQUIRED)
 
string(TOLOWER ${TARGET_MACHINE} TARGET_MACHINE_LOWER)
find_package(katech-${TARGET_MACHINE_LOWER}-arxmls REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON CACHE BOOL "Prefer -pthread flag for building with pthreads")
find_package(ara-tsync REQUIRED)
find_package(ara-core REQUIRED)
find_package(ara-exec-execution-client REQUIRED)
find_package(ara-log REQUIRED)
find_package(Threads REQUIRED)

# List all applications that will run on the machine
list(TRANSFORM KATECH_APPLICATION_ARXMLS PREPEND ${KATECH_APPLICATIONS_ARXMLS_DIR}/)

# List all interfaces that will be used by applications on the machine
list(TRANSFORM KATECH_INTERFACES_ARXMLS PREPEND ${KATECH_INTERFACES_ARXMLS_DIR}/)

# List all adi sensor interfaces that will be used by applications on the machine
list(TRANSFORM ARA_ADI_SENSORITF_ARXMLS PREPEND ${ARA_ADI_SENSORITF_ARXMLS_DIR}/)

# List all processes that will run on the machine
list(TRANSFORM KATECH_PROCESS_ARXMLS PREPEND ${KATECH_MACHINE_ARXMLS_DIR}/)

# List all other files of the machine (like machine manifest)
list(TRANSFORM KATECH_MACHINE_ARXMLS PREPEND ${KATECH_MACHINE_ARXMLS_DIR}/)

# List all files required from ara-api
set(ARA_COMMON_ARXMLS
  ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_GeneralDefinition_DltLogMessages.arxml
  ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_PhysicalDimensions_Blueprint.arxml
  ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_StandardTypes.arxml
  ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_Units_Blueprint.arxml
  ${ARA_ARXMLS_DIR}/package_management.arxml
  ${ARA_ARXMLS_DIR}/routine.arxml
  ${ARA_ARXMLS_DIR}/shared_types.arxml
  ${ARA_ARXMLS_DIR}/sm_types.arxml
  ${ARA_ARXMLS_DIR}/statemachine_service.arxml
  # ${ARA_ARXMLS_DIR}/system_components.arxml
  ${KATECH_APPLICATIONS_ARXMLS_DIR}/system_components.arxml
  ${ARA_ARXMLS_DIR}/trigger_in_out.arxml
  ${ARA_ARXMLS_DIR}/trigger_in.arxml
  ${ARA_ARXMLS_DIR}/trigger_out.arxml
  ${ARA_ARXMLS_DIR}/ucmtypes.arxml
  ${ARA_ARXMLS_DIR}/update_allowed_service.arxml
  ${ARA_ARXMLS_DIR}/update_request.arxml
  ${ARA_ARXMLS_DIR}/vehicle_package_management.arxml
)

# List all files of the network
set(KATECH_NETWORK_ARXMLS
  ${KATECH_NETWORK_ARXMLS_DIR}/network.arxml
)

# Make use of common machine elements
set(KATECH_COMMON_MACHINE_ARXMLS
  ${KATECH_COMMON_MACHINE_ARXMLS_DIR}/execution_manifest.arxml
  ${KATECH_COMMON_MACHINE_ARXMLS_DIR}/machine_manifest.arxml
  ${KATECH_COMMON_MACHINE_ARXMLS_DIR}/service_instance_manifest.arxml
)

set(ARXML_SET
  ${ARA_COMMON_ARXMLS}
  ${KATECH_APPLICATION_ARXMLS}
  ${KATECH_INTERFACES_ARXMLS}
  ${KATECH_COMMON_MACHINE_ARXMLS}
  ${KATECH_MACHINE_ARXMLS}
  ${KATECH_PROCESS_ARXMLS}
  ${KATECH_NETWORK_ARXMLS}
  ${ARA_ADI_SENSORITF_ARXMLS}
)

set(GEN_DEST ${PROJECT_BINARY_DIR}/gen)

set(SWC_FQN /katech/AdaptiveApplicationSwComponentTypes/${APP_NAME})
set(PROCESS_FQN /katech/Machines/${TARGET_MACHINE}/Processes/${PROC_NAME})

set(PROCESS_ARTIFACTS_MAKE_TARGET ${PROC_NAME}ProcessGenerated)

# # FIXME: DDS requires its own generator output directory to avoid conflict with process related generated parts
# set(OPENDDS_GEN_DEST ${PROJECT_BINARY_DIR}/dds_gen)

# # FIXME: The MAKE_TARGET for add_opendds is fixed (the TARGET parameter of this function is not evaluated)
# set(OPENDDS_MAKE_TARGET ${APP_NAME}Generated)

# Define source files
# set(EXECUTABLE_SOURCE
set(APPLICATION_src
    src/tsync_provider.cpp
)

add_process_artifacts(
  DESTINATION ${GEN_DEST}
  TARGET ${PROCESS_ARTIFACTS_MAKE_TARGET}
  PROCESS_FQN ${PROCESS_FQN}
  ARXMLS ${ARXML_SET}
)

# add_executable(${PROJECT_NAME} ${EXECUTABLE_SOURCE})
add_executable(${CMAKE_PROJECT_NAME} ${APPLICATION_src})


# target_link_libraries(${PROJECT_NAME}
target_link_libraries(
    ${CMAKE_PROJECT_NAME} 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${ARA_LIBRARIES} 
    ${COMMUNICATION_LIBRARY} 
    ara::exec_execution_client
    ara::log
    ara::core
    Threads::Threads
    ara::tsync
    rt
)

add_dependencies(${CMAKE_PROJECT_NAME} ${PROCESS_ARTIFACTS_MAKE_TARGET})

# Install Binary
# apd_install_app_executable(${PROJECT_NAME} ${PROJECT_NAME})
apd_install_app_executable(${CMAKE_PROJECT_NAME} ${CMAKE_PROJECT_NAME})

# Install Execution Manifest
# set(EXECUTION_MANIFEST_PATH ${GEN_DEST}/processes/${PROC_NAME}_MANIFEST.json)
# apd_install_app_execution_manifest(${PROJECT_NAME} ${EXECUTION_MANIFEST_PATH})
set(EM_CONFIG_FILENAME ${GEN_DEST}/processes/${PROC_NAME}_MANIFEST.json)
apd_install_app_execution_manifest(${CMAKE_PROJECT_NAME} ${EM_CONFIG_FILENAME})

# if(${TARGET_MACHINE} STREQUAL "AdaptiveMachine")
# # Add the interface file containing the interface name, type and PTP configuration
# apd_install_app_file(ara_tsync_daemon etc TimeSyncProviderInterface.json)
# endif()
