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

find_package(Threads REQUIRED)

find_package(ara-arxmls REQUIRED)
find_package(ara-com)
find_package(ara-gen)
find_package(ara-log REQUIRED)

find_package(ara-exec-execution-client REQUIRED)

set(PROCESS_NAME crossfuncreqs)
set(PROCESS_FQN /ara/diag/${PROCESS_NAME})
set(GEN_DEST ${PROJECT_BINARY_DIR}/gen/${PROCESS_NAME})
set(MACHINE /ara/diag/applications/dmMachine)

file(GLOB ARXMLS ${ARA_MODEL_DIR}/*.arxml)
list(APPEND ARXMLS ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_GeneralDefinition_DltLogMessages.arxml)
list(APPEND ARXMLS ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_PhysicalDimensions_Blueprint.arxml)
list(APPEND ARXMLS ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_StandardTypes.arxml)
list(APPEND ARXMLS ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_Units_Blueprint.arxml)
list(APPEND ARXMLS ${ARA_ARXMLS_DIR}/shared_types.arxml)
list(APPEND ARXMLS files/cfr_app.arxml)

add_aragen(
        DESTINATION ${GEN_DEST}
        TARGET ${PROCESS_NAME}Generated
        PROCESS_FQN ${PROCESS_FQN}
        ARXMLS ${ARXMLS}
)

add_jsongen(
	DESTINATION ${GEN_DEST}
	TARGET ${PROCESS_NAME}JSONGenerated
	MACHINE ${MACHINE}
	LOG_LEVEL "debug"
	LOG_DLT
	LOG_CONSOLE
        ARXMLS ${ARXMLS}
)

set(CROSSFUNCREQS_src
        src/main.cpp
        ${crossfuncreqsGenerated_SOURCES}
)

if(HAS_VSOMEIP_BINDING)
    include_directories(${VSOMEIP_INCLUDE_DIRS})
endif()

if(HAS_WRSOMEIP_BINDING)
    include_directories(${WRSOMEIP_INCLUDE_DIRS})
endif()

add_executable(${PROCESS_NAME} ${CROSSFUNCREQS_src})
add_dependencies(${PROCESS_NAME}JSONGenerated ${PROCESS_NAME}Generated)
add_dependencies(${PROCESS_NAME} ${PROCESS_NAME}Generated ${PROCESS_NAME}JSONGenerated)

target_include_directories(${PROCESS_NAME}
    PRIVATE ${GEN_DEST}/includes/
    PRIVATE ${ARA_INCLUDE_DIRS}
)

target_link_libraries(${PROCESS_NAME}
    ara::exec_execution_client
    ara_com
    ${COMMUNICATION_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
    ara::log
)

apd_install_app_executable(${PROCESS_NAME} ${PROCESS_NAME})
apd_install_app_execution_manifest(${PROCESS_NAME} files/MANIFEST.json)
apd_install_app_file_rename(${PROCESS_NAME} etc ${${PROCESS_NAME}JSONGenerated_OUTPUTS} ${SOMEIP_BINDING_LIBRARY}.json)
