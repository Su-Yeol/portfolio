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

cmake_minimum_required(VERSION 3.4)
project(machine_manifest VERSION 1.0.0 LANGUAGES NONE)

# This variables can be overriden if needed.
set(AUTOSAR_MACHINE /ara/apd/machines/DevelopmentMachine CACHE STRING "The path to the machine.")

find_package(ara-gen REQUIRED)
find_package(ara-arxmls REQUIRED)
find_package(apd-network-arxmls REQUIRED)

file(GLOB ARA_ARXMLS ${ARA_ARXMLS_DIR}/*.arxml)
file(GLOB NETWORK_ARXMLS ${APD_NETWORK_ARXMLS_DIR}/*.arxml)
file(GLOB MACHINE_ARXMLS ${APD_MACHINE_DIR}/*.arxml)

set(GEN_DEST ${PROJECT_BINARY_DIR}/gen)
get_filename_component(MACHINE ${AUTOSAR_MACHINE} NAME CACHE)
set(MACHINE_CONFIG_MAKE_TARGET ${MACHINE}_MACHINE_config)

set(MACHINE_CONFIG_FILENAME ${GEN_DEST}/machines/${MACHINE}_machine_manifest.json)
set(MACHINE_INTERFACE_FILENAME ${GEN_DEST}/machines/${MACHINE}_etc_network_interfaces)

add_em_gen(
    DESTINATION ${GEN_DEST}
    TARGET ${MACHINE_CONFIG_MAKE_TARGET}
    MACHINE ${AUTOSAR_MACHINE}
    ARXMLS ${ARA_ARXMLS} ${NETWORK_ARXMLS} ${MACHINE_ARXMLS}
)

add_custom_target(machine_manifest ALL DEPENDS ${MACHINE_CONFIG_MAKE_TARGET})

install(FILES ${MACHINE_CONFIG_FILENAME} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/system/ RENAME machine_manifest.json)
install(FILES ${MACHINE_INTERFACE_FILENAME} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/system/ RENAME interfaces)
