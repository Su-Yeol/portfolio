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

# Common CMake header
cmake_minimum_required(VERSION 3.12)
project(diag_seedkey VERSION 0.0.1 LANGUAGES CXX)

include(AUTOSAR/apd)

# ara-gen / json-gen specific definitions (will be used below)
set(APP_NAME seedkey)
set(SWC_NAME SeedKeyProvider)
set(IFACE_FULLNAME /ara/diag/SecurityAccess)

set(PROCESS_NAME SeedKey_instance)
set(PROCESS_FQN /apd/diag/${PROCESS_NAME})

set(LOG_LEVEL debug)


# Dependencies
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(ara-arxmls REQUIRED)
find_package(ara-com REQUIRED)
find_package(ara-gen REQUIRED)
find_package(ara-log REQUIRED)
find_package(ara-exec-execution-client REQUIRED)

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

option(NO_ARAEXEC "Don't use ara::exec application state reporting" OFF)

if(NOT DEFINED DIAGSEEDKEY_DIR)
    set(DIAGSEEDKEY_DIR ${CMAKE_INSTALL_PREFIX})
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
set(INSTALL_EMCONFDIR "${DIAGSEEDKEY_DIR}/etc" CACHE PATH "configuration files location for ExecutionManager")
set(INSTALL_EMSTATEDIR "${DIAGSEEDKEY_DIR}/var" CACHE PATH "data files location for ExecutionManager")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -pedantic -Werror -Wall -Wextra -Wsign-promo -Wnon-virtual-dtor -Wctor-dtor-privacy -Woverloaded-virtual -Wold-style-cast -Wpointer-arith -Wshadow -Wunused -Wuninitialized -Winit-self -Wdeprecated -Wfloat-equal")

add_compile_definitions(USE_ARA)
if (NO_ARAEXEC)
	add_compile_definitions(NO_ARAEXEC)
endif()

# List of .arxml files
set(ARXMLS
    	files/seedkey_app.arxml
    	files/system_manifest.arxml
    	${ARA_MODEL_DIR}/metainfo.arxml
    	${ARA_MODEL_DIR}/security_access.arxml
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

# Split iface fullname into namespace (prefix) and short name (suffix) parts
# remove leading slash, if present
string(REGEX REPLACE "^/+" "" IFACE_FULLNAME ${IFACE_FULLNAME})
# split interface name into namespace and name
string(FIND ${IFACE_FULLNAME} "/" IFACE_POS REVERSE)
string(SUBSTRING ${IFACE_FULLNAME} 0 ${IFACE_POS} IFACE_NAMESPACE)
math(EXPR IFACE_POS "${IFACE_POS} + 1")
string(SUBSTRING ${IFACE_FULLNAME} ${IFACE_POS} -1 IFACE_NAME)

set(GEN_DEST ${PROJECT_BINARY_DIR}/gen/seed_key)

##########################
## ara::gen to auto-generate proxy/skeleton
##########################

# Some files in the output section below may be named same as APP_NAME coincidentally.
# But APP_NAME should NOT be used here, because it's just a coincidence.
add_aragen(
  TARGET ${APP_NAME}Generated
  DESTINATION ${GEN_DEST}
  PROCESS_FQN ${PROCESS_FQN}
  ARXMLS ${ARXMLS}
)

##########################
## jsongen to auto-generate vsomeip config
##########################

# vsomeip config
add_jsongen(TARGET ${APP_NAME}JSONGenerated
            DESTINATION ${GEN_DEST}
            MACHINE "/ara/diag/applications/dmMachine"
            LOG_LEVEL "${LOG_LEVEL}"
            LOG_CONSOLE
            ARXMLS ${ARXMLS}
           )
add_dependencies(${APP_NAME}JSONGenerated ${APP_NAME}Generated)

# Include search paths
include_directories(${GEN_DEST}/includes/)

# Application
set(SEEDKEY_src
    src/main.cpp
   )

add_executable(
    ${APP_NAME}
    ${SEEDKEY_src}
    ${${APP_NAME}Generated_SOURCES}
)

add_dependencies(
    ${APP_NAME}
    ${APP_NAME}Generated
    ${APP_NAME}JSONGenerated
)

target_link_libraries(
    ${APP_NAME}
    ara::exec_execution_client
    ara_com
    ${COMMUNICATION_LIBRARY}
    ara::log
    Threads::Threads
)

##########################
## Install stuff
##########################

apd_install_app_executable(${APP_NAME} ${APP_NAME})
apd_install_app_execution_manifest(${APP_NAME} files/MANIFEST.json)

# FIXME: Calculator installs this file under /opt/calculator/etc. This puts in under /etc.
install(FILES
       	${${APP_NAME}JSONGenerated_OUTPUTS}
        PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
        DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}
        RENAME vsomeip_${APP_NAME}.json
       )
