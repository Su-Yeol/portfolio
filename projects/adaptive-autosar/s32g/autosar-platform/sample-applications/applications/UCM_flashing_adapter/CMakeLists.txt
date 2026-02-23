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

cmake_minimum_required(VERSION 3.12)
get_filename_component(PROJECTNAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project(${PROJECTNAME} VERSION 1.0.0 LANGUAGES CXX)

# Set these variables (like in the yocto recipe) to build the application as a specific process on a specific machine
if(NOT TARGET_MACHINE)
    set(TARGET_MACHINE "RadarFusionMachine")
endif()
if(NOT APP_NAME)
    set(APP_NAME "flashing_adapter")
endif()
if(NOT PROC_NAME)
    set(PROC_NAME "flashing_adapter")
endif()

include(GNUInstallDirs)
include(AUTOSAR/apd)

# Import ARA
message(STATUS "Importing Dependencies")

# Generic stuff (ara generator and arxmls)
find_package(ara-gen REQUIRED)

find_package(ara-arxmls REQUIRED)
find_package(ara-adi-sensoritf-arxmls REQUIRED)
find_package(apd-applications-arxmls REQUIRED)
find_package(apd-common-machine-arxmls REQUIRED)
find_package(apd-interfaces-arxmls REQUIRED)
find_package(apd-network-arxmls REQUIRED)
find_package(apd-dlt-arxmls REQUIRED)

string(TOLOWER ${TARGET_MACHINE} TARGET_MACHINE_LOWER)
find_package(apd-${TARGET_MACHINE_LOWER}-arxmls REQUIRED)

find_package(ara-com REQUIRED)
find_package(Threads REQUIRED)
find_package(ara-core REQUIRED)
find_package(ara-exec-execution-client REQUIRED)
find_package(ara-log REQUIRED)
find_package(ara-per REQUIRED)

find_package(ara-ucm-libs REQUIRED)
find_package(ara-ucm-dpduapi REQUIRED)

find_package(Boost 1.60.0 REQUIRED COMPONENTS system filesystem thread)
find_package(Poco COMPONENTS Foundation Zip REQUIRED)

add_compile_options(-Werror)

# List all applications that will run on the machine
list(TRANSFORM APD_APPLICATION_ARXMLS PREPEND ${APD_APPLICATIONS_ARXMLS_DIR}/)

# List all interfaces that will be used by applications on the machine
list(TRANSFORM APD_INTERFACES_ARXMLS PREPEND ${APD_INTERFACES_ARXMLS_DIR}/)

# List all adi sensor interfaces that will be used by applications on the machine
list(TRANSFORM ARA_ADI_SENSORITF_ARXMLS PREPEND ${ARA_ADI_SENSORITF_ARXMLS_DIR}/)

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
    ${ARA_ARXMLS_DIR}/adi_types.arxml
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
    ${ARA_ADI_SENSORITF_ARXMLS}
)

set(APP_EXECUTABLE ${APP_NAME})

set(PROCESS_FQN /apd/Machines/${TARGET_MACHINE}/Processes/${PROC_NAME})

set(GEN_DEST ${PROJECT_BINARY_DIR}/gen/${APP_NAME})
set(SWC_FQN /apd/AdaptiveApplicationSwComponentTypes/${APP_NAME})
set(MACHINE /apd/Machines/${TARGET_MACHINE}/${TARGET_MACHINE})
set(EM_CONFIG_FILENAME ${GEN_DEST}/processes/${PROC_NAME}_MANIFEST.json)
set(SI_MANIFEST_FILENAME ${GEN_DEST}/processes/${PROC_NAME}_SI_MANIFEST.json)

set(INTERFACES_MAKE_TARGET ${APP_NAME}InterfacesGenerated)
set(PROCESS_ARTIFACTS_MAKE_TARGET ${APP_NAME}ProcessGenerated)
set(MACHINE_ARTIFACTS_MAKE_TARGET ${APP_NAME}MachineGenerated)

add_process_artifacts(
    DESTINATION ${GEN_DEST}
    TARGET ${PROCESS_ARTIFACTS_MAKE_TARGET}
    PROCESS_FQN ${PROCESS_FQN}
    ARXMLS ${ARXML_SET}
)

add_middleware_interfaces(
    DESTINATION ${GEN_DEST}
    TARGET ${INTERFACES_MAKE_TARGET}
    SWC_FQN ${SWC_FQN}
    ARXMLS ${ARXML_SET}
)

# block size for TransferStart method
add_definitions(-DUCM_DATA_BLOCK_SIZE=1048576)

# add more .cpps here later
set(PROJECT_SOURCES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/flashing_adapter_app.cpp
    ${PROJECT_SOURCE_DIR}/src/ecu_reprogramming/ecu_reprogramming_sequence.cpp
    ${PROJECT_SOURCE_DIR}/src/ecu_reprogramming/single_bank_ecu_reprogramming.cpp
    ${PROJECT_SOURCE_DIR}/src/ecu_reprogramming/dual_bank_ecu_reprogramming.cpp
    ${PROJECT_SOURCE_DIR}/src/parser/prog_conf.cpp
    ${PROJECT_SOURCE_DIR}/src/parser/prog_conf_parser.cpp
    ${PROJECT_SOURCE_DIR}/src/flash_data/flash_data.cpp
    ${PROJECT_SOURCE_DIR}/src/uds_interface/uds_interface.cpp
    ${PROJECT_SOURCE_DIR}/src/uds_interface/pdu_api_handle.cpp
    ${${PROCESS_ARTIFACTS_MAKE_TARGET}_SOURCES}
)

if(HAS_VSOMEIP_BINDING)
    set(VSOMEIP_CONFIG_FILENAME ${GEN_DEST}/machines/vsomeip/${TARGET_MACHINE}_vsomeip.json)
endif()

if(HAS_WRSOMEIP_BINDING)
    set(WRSOMEIP_CONFIG_FILENAME ${GEN_DEST}/machines/wrsomeip/${TARGET_MACHINE}_wrsomeip.json)
endif()


# vsomeip config
add_machine_artifacts(
    DESTINATION ${GEN_DEST}
    TARGET ${MACHINE_ARTIFACTS_MAKE_TARGET}
    MACHINE_FQN ${MACHINE}
    ARXMLS ${ARXML_SET}
)

add_executable(${APP_EXECUTABLE}
    ${PROJECT_SOURCES}
)

if(HAS_VSOMEIP_BINDING AND HAS_WRSOMEIP_BINDING)
    message(FATAL_ERROR "Both VSOMEIP and WRSOMEIP Bindings are enabled")
endif()

# use vsomeip binding by default
if(NOT HAS_VSOMEIP_BINDING AND NOT HAS_WRSOMEIP_BINDING)
    set(HAS_VSOMEIP_BINDING ON CACHE BOOL "Use communication over some/ip via vsomeip" FORCE)
endif(NOT HAS_VSOMEIP_BINDING AND NOT HAS_WRSOMEIP_BINDING)

if(HAS_VSOMEIP_BINDING)
    set(COMMUNICATION_LIBRARY ara_vsomeipbinding)
    set(SOMEIP_CONFIG_FILENAME ${VSOMEIP_CONFIG_FILENAME})
    set(SOMEIP_BINDING_LIBRARY vsomeip)
    add_compile_definitions(HAS_VSOMEIP_BINDING)
    target_include_directories(${APP_NAME}
        PRIVATE ${GEN_DEST}/includes/
        PRIVATE ${GEN_DEST}/net-bindings/vsomeip/
        PRIVATE ${VSOMEIP_INCLUDE_DIRS}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ecu_reprogramming
            ${CMAKE_CURRENT_SOURCE_DIR}/src/parser
            ${CMAKE_CURRENT_SOURCE_DIR}/src/uds_interface
            ${CMAKE_CURRENT_SOURCE_DIR}/src/flash_data
    )
endif(HAS_VSOMEIP_BINDING)

if(HAS_WRSOMEIP_BINDING)
    set(COMMUNICATION_LIBRARY ara_wrsomeipbinding)
    set(SOMEIP_CONFIG_FILENAME ${WRSOMEIP_CONFIG_FILENAME})
    set(SOMEIP_BINDING_LIBRARY wrsomeip)
    add_compile_definitions(HAS_WRSOMEIP_BINDING)
    target_include_directories(${APP_NAME}
        PRIVATE ${GEN_DEST}/includes/
        PRIVATE ${GEN_DEST}/net-bindings/wrsomeip/
        PRIVATE ${WRSOMEIP_INCLUDE_DIRS}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/src/ecu_reprogramming
            ${CMAKE_CURRENT_SOURCE_DIR}/src/parser
            ${CMAKE_CURRENT_SOURCE_DIR}/src/uds_interface
            ${CMAKE_CURRENT_SOURCE_DIR}/src/flash_data
    )
endif(HAS_WRSOMEIP_BINDING)

target_link_libraries(${APP_EXECUTABLE}
        ${CMAKE_THREAD_LIBS_INIT}
        ${ARA_LOGGING_LIBRARIES}
        ara::exec_execution_client
        ara::core
        ara_com
        ara::log
        ${COMMUNICATION_LIBRARY}
)

target_link_libraries(${APP_EXECUTABLE} ara::ara_ucm_libtransfer)
target_link_libraries(${APP_EXECUTABLE} ara::ara_ucm_libextraction)
target_link_libraries(${APP_EXECUTABLE} Poco::Foundation)
target_link_libraries(${APP_EXECUTABLE} Poco::Zip)
target_link_libraries(${APP_EXECUTABLE} Boost::filesystem)
target_link_libraries(${APP_EXECUTABLE} ucm::dpduapi)
target_link_libraries(${APP_EXECUTABLE} Boost::thread)

add_dependencies(${APP_EXECUTABLE} ${INTERFACES_MAKE_TARGET} ${PROCESS_ARTIFACTS_MAKE_TARGET} ${MACHINE_ARTIFACTS_MAKE_TARGET})

file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/flashing_adapter_transfer_status_database.json)
file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/flashing_adapter_sw_cluster_desc_database.json)

apd_install_app_executable(${APP_NAME} ${APP_EXECUTABLE})
apd_install_app_execution_manifest(${APP_NAME} ${EM_CONFIG_FILENAME})
apd_install_app_service_instance_manifest(${APP_NAME} ${SI_MANIFEST_FILENAME})
apd_install_app_file(${APP_NAME} var ${CMAKE_CURRENT_BINARY_DIR}/flashing_adapter_transfer_status_database.json)
apd_install_app_file(${APP_NAME} var ${CMAKE_CURRENT_BINARY_DIR}/flashing_adapter_sw_cluster_desc_database.json)
