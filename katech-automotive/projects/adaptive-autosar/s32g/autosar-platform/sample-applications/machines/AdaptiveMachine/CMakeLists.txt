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

cmake_minimum_required(VERSION 3.15)

include(GNUInstallDirs)

project(adcm-adaptivemachine-artifacts VERSION 1.0.0 LANGUAGES NONE)

set(MACHINE_NAME "AdaptiveMachine")

find_package(adcm-adaptivemachine-arxmls REQUIRED)

# Do not edit any of the following lines!
# They are generic and do not depend on a specific machine when following the new structure.
# ------------------------------------------------------

find_package(ara-gen REQUIRED)

find_package(ara-arxmls REQUIRED)
find_package(adcm-applications-arxmls REQUIRED)
find_package(adcm-common-machine-arxmls REQUIRED)
find_package(adcm-interfaces-arxmls REQUIRED)
find_package(adcm-network-arxmls REQUIRED)

# List all applications that will run on the machine
list(TRANSFORM ADCM_APPLICATION_ARXMLS PREPEND ${ADCM_APPLICATIONS_ARXMLS_DIR}/)

# List all interfaces that will be used by applications on the machine
list(TRANSFORM ADCM_INTERFACES_ARXMLS PREPEND ${ADCM_INTERFACES_ARXMLS_DIR}/)

# List all processes that will run on the machine
list(TRANSFORM ADCM_PROCESS_ARXMLS PREPEND ${ADCM_MACHINE_ARXMLS_DIR}/)

# List all other files of the machine (like machine manifest)
list(TRANSFORM ADCM_MACHINE_ARXMLS PREPEND ${ADCM_MACHINE_ARXMLS_DIR}/)

# List all files of the network
set(ADCM_NETWORK_ARXMLS
  ${ADCM_NETWORK_ARXMLS_DIR}/network.arxml
)

# List all files required from ara-api
set(ARA_COMMON_ARXMLS
  ${ARA_ARXMLS_DIR}/adi_types.arxml
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
set(ADCM_COMMON_MACHINE_ARXMLS
  ${ADCM_COMMON_MACHINE_ARXMLS_DIR}/execution_manifest.arxml
  ${ADCM_COMMON_MACHINE_ARXMLS_DIR}/machine_manifest.arxml
  ${ADCM_COMMON_MACHINE_ARXMLS_DIR}/service_instance_manifest.arxml
)

set(ARXML_SET
  ${ARA_COMMON_ARXMLS}
  ${ADCM_APPLICATION_ARXMLS}
  ${ADCM_INTERFACES_ARXMLS}
  ${ADCM_COMMON_MACHINE_ARXMLS}
  ${ADCM_MACHINE_ARXMLS}
  ${ADCM_PROCESS_ARXMLS}
  ${ADCM_NETWORK_ARXMLS}
)

set(MACHINE_FQN /adcm/Machines/${MACHINE_NAME}/${MACHINE_NAME})
set(MACHINE_ARTIFACTS_MAKE_TARGET ${MACHINE_NAME}_MACHINE_config)
set(MACHINE_ARTIFACTS_GEN_OUTPUT_DIR ${PROJECT_BINARY_DIR}/gen)

add_machine_artifacts(
  DESTINATION ${MACHINE_ARTIFACTS_GEN_OUTPUT_DIR}
  TARGET ${MACHINE_ARTIFACTS_MAKE_TARGET}
  MACHINE_FQN ${MACHINE_FQN}
  ARXMLS ${ARXML_SET}
)

add_custom_target(machine_artifacts ALL DEPENDS ${MACHINE_ARTIFACTS_MAKE_TARGET})

# Machine Manifest
set(MACHINE_MANIFEST_FILENAME ${MACHINE_ARTIFACTS_GEN_OUTPUT_DIR}/machines/${MACHINE_NAME}_machine_manifest.json)
install(FILES ${MACHINE_MANIFEST_FILENAME} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/system/ RENAME machine_manifest.json)

# Network Interfaces
set(MACHINE_INTERFACE_FILENAME ${MACHINE_ARTIFACTS_GEN_OUTPUT_DIR}/machines/${MACHINE_NAME}_etc_network_interfaces)
install(FILES ${MACHINE_INTERFACE_FILENAME} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/network/ RENAME interfaces)

# DLT
set(MACHINE_DLT_CONFIG_FILENAME ${MACHINE_ARTIFACTS_GEN_OUTPUT_DIR}/machines/${MACHINE_NAME}_dlt.conf)
install(FILES ${MACHINE_DLT_CONFIG_FILENAME} PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/ RENAME dlt.conf)

# SOME/IP Config (might not be generated when no application makes use of SOME/IP)
set(MACHINE_SOMEIP_CONFIG_FILENAME ${MACHINE_ARTIFACTS_GEN_OUTPUT_DIR}/machines/vsomeip/${MACHINE_NAME}_vsomeip.json)
# the existence of the config file has to be checked at install time, therefore it is wrapped into a code script
# the file(INSTALL) command does not use CMAKE_INSTALL_PREFIX. Yocto install step uses DESTDIR, but native build
# needs a workaround
install(CODE "if(EXISTS ${MACHINE_SOMEIP_CONFIG_FILENAME})
                file(INSTALL ${MACHINE_SOMEIP_CONFIG_FILENAME} DESTINATION ${APD_ROOTDIR}${CMAKE_INSTALL_SYSCONFDIR}/machine_artifacts PERMISSIONS OWNER_READ GROUP_READ WORLD_READ RENAME vsomeip.json)
              endif()")

