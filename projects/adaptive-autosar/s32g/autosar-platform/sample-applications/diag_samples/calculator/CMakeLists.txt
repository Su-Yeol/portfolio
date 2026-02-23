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
project(diag_calculator VERSION 0.0.1 LANGUAGES CXX)

include(AUTOSAR/apd)

message(STATUS "---------------------------------------------------")
message(STATUS "./calculator/ - Importing dependencies ------------")

set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)
find_package(ara-arxmls REQUIRED)
find_package(ara-com REQUIRED)
find_package(ara-gen REQUIRED)
find_package(ara-log REQUIRED)
find_package(ara-exec-execution-client REQUIRED)

set(APP_NAME Calculator)
set(APP_EXECUTABLE ${APP_NAME})
set(PROCESS_NAME calculator)

if(HAS_VSOMEIP_BINDING AND HAS_WRSOMEIP_BINDING)
    message(FATAL_ERROR "Both VSOMEIP and WRSOMEIP Bindings are enabled")
endif()

# use vsomeip binding by default
if(NOT HAS_VSOMEIP_BINDING AND NOT HAS_WRSOMEIP_BINDING)
    set(HAS_VSOMEIP_BINDING ON CACHE BOOL "Use communication over some/ip via vsomeip" FORCE)
endif()

if(HAS_VSOMEIP_BINDING)
    set(COMMUNICATION_LIBRARY ara_vsomeipbinding)
    add_compile_definitions(HAS_VSOMEIP_BINDING)
endif()

if(HAS_WRSOMEIP_BINDING)
    set(COMMUNICATION_LIBRARY ara_wrsomeipbinding)
    add_compile_definitions(HAS_WRSOMEIP_BINDING)
endif()

include(GNUInstallDirs)

if(NOT DEFINED DIAGSAMPLE_DIR)
    set(DIAGSAMPLE_DIR ${CMAKE_INSTALL_PREFIX})
endif()

find_file(DIAG_ARXML name system_manifest.arxml
         PATHS "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}"
         PATH_SUFFIXES diag
         )
if("${DIAG_ARXML}" STREQUAL "DIAG_ARXML-NOTFOUND")
    message(FATAL_ERROR "Failed to find DIAG ARXML Files")
else()
    get_filename_component(ARA_MODEL_DIR "${DIAG_ARXML}" DIRECTORY CACHE)
endif()

# Execution Manager expects some configs in the location which doesn't conform
# GNUInstallDirs special cases ((ara-api/exec/execution_manager/README.md).
# Workaround is to use EMCONFDIR instead of SYSCONFDIR in this case.
set(INSTALL_EMCONFDIR "${DIAGSAMPLE_DIR}/etc" CACHE PATH "configuration files location for ExecutionManager")
set(INSTALL_LOCALSTATEDIR "${DIAGSAMPLE_DIR}/var" CACHE PATH "data files location for ExecutionManager")

add_compile_definitions(USE_ARA)

set(GEN_DEST ${PROJECT_BINARY_DIR}/gen/calculator)
set(PROCESS_NAME Calculator_instance1)
set(PROCESS_FQN /ara/diag/example/${PROCESS_NAME})

add_aragen(
  TARGET calculatorGenerated
  DESTINATION ${GEN_DEST}
  PROCESS_FQN ${PROCESS_FQN}
  ARXMLS
    ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_GeneralDefinition_DltLogMessages.arxml
    ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_PhysicalDimensions_Blueprint.arxml
    ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_StandardTypes.arxml
    ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_Units_Blueprint.arxml
    ${ARA_ARXMLS_DIR}/shared_types.arxml
    ${ARA_MODEL_DIR}/calculator.arxml
    ${ARA_MODEL_DIR}/dcm_calculator_plugin_typed.arxml
    ${ARA_MODEL_DIR}/dcm_plugin_raw_diagnostics.arxml
    ${ARA_MODEL_DIR}/diagnostic_errors.arxml
    ${ARA_MODEL_DIR}/diagnostics.arxml
    ${ARA_MODEL_DIR}/security_access.arxml
    ${ARA_MODEL_DIR}/service_ClearDTC.arxml
    ${ARA_MODEL_DIR}/service_DTCInformation.arxml
    ${ARA_MODEL_DIR}/service_DataIdentifier.arxml
    ${ARA_MODEL_DIR}/service_DiagnosticCondition.arxml
    ${ARA_MODEL_DIR}/service_DiagnosticEvent.arxml
    ${ARA_MODEL_DIR}/service_DiagnosticMonitor.arxml
    ${ARA_MODEL_DIR}/service_DiagnosticStatus.arxml
    ${ARA_MODEL_DIR}/service_DoIPGroupIdentification.arxml
    ${ARA_MODEL_DIR}/service_IndicatorStatus.arxml
    ${ARA_MODEL_DIR}/service_OperationCycle.arxml
    ${ARA_MODEL_DIR}/service_RoutineService.arxml
    ${ARA_MODEL_DIR}/system_manifest.arxml
    ${ARA_MODEL_DIR}/system_manifest_eth.arxml
    ${ARA_MODEL_DIR}/system_manifest_security_access.arxml
)

# Include search paths
include_directories(${GEN_DEST}/includes)

set(Calculator_src
    src/add_interface_impl.cpp
    src/calculator.cpp
    src/diag_add_arg_interface_impl.cpp
    src/main.cpp
    src/runtime_calculator.cpp
    src/common/runtime.cpp
)

add_executable(
    Calculator
    ${Calculator_src}
    ${calculatorGenerated_SOURCES}
)

add_dependencies(Calculator calculatorGenerated)

target_link_libraries(
    Calculator
    ara::exec_execution_client
    ara_com
    ${COMMUNICATION_LIBRARY}
    ara::log
    Threads::Threads
)

apd_install_app_executable(calculator Calculator)
apd_install_app_execution_manifest(calculator files/MANIFEST.json)
apd_install_app_file(calculator etc files/calculator_cfg.json)
apd_install_app_file(calculator etc files/vsomeip_calculator.json)
