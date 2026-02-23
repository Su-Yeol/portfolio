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

#Add this to the yocto EXTRA_OECMAKE
#EXTRA_OECMAKE += " \
#    -DTARGET_MACHINE=testSuiteEcu1Machine \
#    -DAPP_NAME=ST_CMApp01 \
#    -DPROC_NAME=ST_CMApp01Instance1 \
#"

cmake_minimum_required(VERSION 3.15)
get_filename_component(PROJECTNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECTNAME} VERSION 1.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(AUTOSAR/apd)

# Generic stuff (ara generator and arxmls)
find_package(ara-gen REQUIRED)

find_package(ara-arxmls REQUIRED)
find_package(apd-applications-arxmls REQUIRED)
find_package(apd-common-machine-arxmls REQUIRED)
find_package(apd-interfaces-arxmls REQUIRED)
find_package(apd-network-arxmls REQUIRED)
find_package(apd-dlt-arxmls REQUIRED)

string(TOLOWER ${TARGET_MACHINE} TARGET_MACHINE_LOWER)
find_package(apd-${TARGET_MACHINE_LOWER}-arxmls REQUIRED)

# Functional Cluster (APIs)
set(THREADS_PREFER_PTHREAD_FLAG ON CACHE BOOL "Prefer -pthread flag for building with pthreads")
find_package(Threads REQUIRED)
find_package(ara-core REQUIRED)
find_package(ara-log REQUIRED)
find_package(ara-exec-execution-client REQUIRED)

#-----------------------------------------------------------------------------------------------------------------------------------------------

# List all applications that will run on the machine
list(TRANSFORM APD_APPLICATION_ARXMLS PREPEND ${APD_APPLICATIONS_ARXMLS_DIR}/)

# List all interfaces that will be used by applications on the machine
list(TRANSFORM APD_INTERFACES_ARXMLS PREPEND ${APD_INTERFACES_ARXMLS_DIR}/)

# List all processes that will run on the machine
list(TRANSFORM APD_PROCESS_ARXMLS PREPEND ${APD_MACHINE_ARXMLS_DIR}/)

# List all other files of the machine (like machine manifest)
list(TRANSFORM APD_MACHINE_ARXMLS PREPEND ${APD_MACHINE_ARXMLS_DIR}/)

# List all dlt related files that will be used by machine or applications on the machine
list(TRANSFORM APD_DLT_ARXMLS PREPEND ${APD_DLT_ARXMLS_DIR}/)

# List all files of the network
set(APD_NETWORK_ARXMLS
  ${APD_NETWORK_ARXMLS_DIR}/network.arxml
)

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
  ${ARA_ARXMLS_DIR}/system_components.arxml
  ${ARA_ARXMLS_DIR}/trigger_in_out.arxml
  ${ARA_ARXMLS_DIR}/trigger_in.arxml
  ${ARA_ARXMLS_DIR}/trigger_out.arxml
  ${ARA_ARXMLS_DIR}/ucmtypes.arxml
  ${ARA_ARXMLS_DIR}/update_allowed_service.arxml
  ${ARA_ARXMLS_DIR}/update_request.arxml
  ${ARA_ARXMLS_DIR}/vehicle_package_management.arxml
)

# Make use of common machine elements
set(APD_COMMON_MACHINE_ARXMLS
  ${APD_COMMON_MACHINE_ARXMLS_DIR}/execution_manifest.arxml
  ${APD_COMMON_MACHINE_ARXMLS_DIR}/machine_manifest.arxml
  ${APD_COMMON_MACHINE_ARXMLS_DIR}/service_instance_manifest.arxml
)

set(ARXML_SET
  ${ARA_COMMON_ARXMLS}
  ${APD_APPLICATION_ARXMLS}
  ${APD_INTERFACES_ARXMLS}
  ${APD_COMMON_MACHINE_ARXMLS}
  ${APD_MACHINE_ARXMLS}
  ${APD_PROCESS_ARXMLS}
  ${APD_NETWORK_ARXMLS}
  ${APD_DLT_ARXMLS}
)

set(GEN_DEST ${PROJECT_BINARY_DIR}/gen)

# process configration
#-----------------------------------------------------------------------------------------------------------------------------------------------

set(APP_PREFIX "STBG")
set(APP_SUFFIX "EMOAppSysFG2")
set(EXECUTABLE_NAME ${APP_PREFIX}_${APP_SUFFIX})
set(PROC_NAME ${EXECUTABLE_NAME}Instance)


set(PROCESS_FQN /apd/Machines/${TARGET_MACHINE}/Processes/${PROC_NAME})
set(PROCESS_ARTIFACTS_MAKE_TARGET ${PROC_NAME}ProcessGenerated)

add_process_artifacts(
  DESTINATION ${GEN_DEST}
  TARGET ${PROCESS_ARTIFACTS_MAKE_TARGET}
  PROCESS_FQN ${PROCESS_FQN}
  ARXMLS ${ARXML_SET}
)

# executable configration
#-----------------------------------------------------------------------------------------------------------------------------------------------
set(MAIN_SRC main_${APP_SUFFIX}.cpp)
set(EXECUTABLE_SOURCE
  ${PROJECT_SOURCE_DIR}/../../common/src/${MAIN_SRC}
  ${PROJECT_SOURCE_DIR}/../../common/src/EMOApp_common.cpp
  ${${PROCESS_ARTIFACTS_MAKE_TARGET}_SOURCES}
)

add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_SOURCE})

target_include_directories(
  ${EXECUTABLE_NAME}
	PRIVATE ${PROJECT_SOURCE_DIR}/../../common/include
)

target_link_libraries(
  ${EXECUTABLE_NAME}
  ${CMAKE_THREAD_LIBS_INIT}
  ${ARA_LIBRARIES}
  ${COMMUNICATION_LIBRARY}
  ara::exec_execution_client
  ara::core
  ara::log
  Threads::Threads
)

add_dependencies(${EXECUTABLE_NAME} ${PROCESS_ARTIFACTS_MAKE_TARGET})

set(EM_CONFIG_FILENAME ${GEN_DEST}/processes/${PROC_NAME}_MANIFEST.json)
apd_install_app_execution_manifest(${EXECUTABLE_NAME} ${EM_CONFIG_FILENAME})

apd_install_app_executable(${EXECUTABLE_NAME} ${EXECUTABLE_NAME})
