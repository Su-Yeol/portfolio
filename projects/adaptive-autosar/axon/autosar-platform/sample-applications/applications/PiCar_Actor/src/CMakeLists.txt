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

set(PROCESS_FQN /apd/Machines/${TARGET_MACHINE}/Processes/${PROC_NAME})
set(SWC_FQN /apd/AdaptiveApplicationSwComponentTypes/${APP_NAME})
set(MACHINE /apd/Machines/${TARGET_MACHINE}/${TARGET_MACHINE})

set(VSOMEIP_CONFIG_FILENAME ${GEN_DEST}/machines/vsomeip/${TARGET_MACHINE}_vsomeip.json)
set(EM_CONFIG_FILENAME ${GEN_DEST}/processes/${PROC_NAME}_MANIFEST.json)
set(SI_MANIFEST_FILENAME ${GEN_DEST}/processes/${PROC_NAME}_SI_MANIFEST.json)

set(INTERFACES_MAKE_TARGET ${APP_NAME}InterfacesGenerated)
set(PROCESS_ARTIFACTS_MAKE_TARGET ${APP_NAME}ProcessGenerated)
set(MACHINE_ARTIFACTS_MAKE_TARGET ${APP_NAME}MachineGenerated)

set(ARXML_SET
  ${APD_APPLICATION_ARXMLS}
  ${APD_COMMON_MACHINE_ARXMLS}
  ${APD_INTERFACES_ARXMLS}
  ${APD_MACHINE_ARXMLS}
  ${APD_PROCESS_ARXMLS}
  ${APD_NETWORK_ARXMLS}
  ${ARA_COMMON_ARXMLS}
  ${APD_DLT_ARXMLS}
  ${ARA_ADI_SENSORITF_ARXMLS}
)

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

set(MAIN_src
    ${PROJECT_SOURCE_DIR}/src/main.cpp)

# add more .cpps here later
set(PROJECT_SOURCES
    ${${PROCESS_ARTIFACTS_MAKE_TARGET}_SOURCES}
    ${PROJECT_SOURCE_DIR}/src/actor_app.cpp
    ${PROJECT_SOURCE_DIR}/src/actor_service.cpp
)

if(PICAR)
    add_compile_definitions(PICAR=1)
    set(PROJECT_SOURCES
        ${PROJECT_SOURCES}
        ${PROJECT_SOURCE_DIR}/src/gpio.cpp
        ${PROJECT_SOURCE_DIR}/src/picar.cpp
        ${PROJECT_SOURCE_DIR}/src/picar_impl.cpp
        ${PROJECT_SOURCE_DIR}/src/pwm_i2c.cpp
        ${PROJECT_SOURCE_DIR}/src/pwm_pca9685.cpp)
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
    ${MAIN_src}
)

target_include_directories(${APP_NAME}
    PRIVATE ${PROJECT_SOURCE_DIR}/include/
    PRIVATE ${GEN_DEST}/includes/
    PRIVATE ${GEN_DEST}/net-bindings/vsomeip/
    PRIVATE ${VSOMEIP_INCLUDE_DIRS}
)

target_link_libraries (${APP_EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT} ${ARA_LOGGING_LIBRARIES} 
    ara::exec_execution_client ara_com ara_vsomeipbinding ara::core ara::log ${Boost_LIBRARIES})

add_dependencies(${APP_EXECUTABLE} ${INTERFACES_MAKE_TARGET} ${PROCESS_ARTIFACTS_MAKE_TARGET} 
    ${MACHINE_ARTIFACTS_MAKE_TARGET})

apd_install_app_executable(${APP_NAME} ${APP_EXECUTABLE})
apd_install_app_execution_manifest(${APP_NAME} ${EM_CONFIG_FILENAME})
apd_install_app_service_instance_manifest(${APP_NAME} ${SI_MANIFEST_FILENAME})
apd_install_app_file_rename(${PROJECT_NAME} etc ${VSOMEIP_CONFIG_FILENAME} vsomeip.json)
