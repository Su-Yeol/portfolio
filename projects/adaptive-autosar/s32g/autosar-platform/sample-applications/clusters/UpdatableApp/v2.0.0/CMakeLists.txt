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

project(UpdatableApp VERSION 2.0.0 LANGUAGES CXX)

include(ExternalProject)

set(CLUSTER_NAME ${PROJECT_NAME})

include(AUTOSAR/apd)
include("GNUInstallDirs")
find_package(ara-core REQUIRED)  # generator
find_package(ara-gen REQUIRED)  # generator
find_package(ara-ucm REQUIRED)
find_package(apd-applications-arxmls REQUIRED)
find_package(apd-common-machine-arxmls REQUIRED)
find_package(ara-adi-sensoritf-arxmls REQUIRED)
find_package(apd-interfaces-arxmls REQUIRED)
find_package(apd-network-arxmls REQUIRED)
find_package(apd-dlt-arxmls REQUIRED)
find_package(ara-arxmls REQUIRED)

find_package(apd-cluster-arxmls REQUIRED)

# List all other files of the machine (like machine manifest)
list(TRANSFORM APD_CLUSTER_ARXMLS PREPEND ${APD_CLUSTER_ARXMLS_DIR}/)


string(TOLOWER ${TARGET_MACHINE} TARGET_MACHINE_LOWER)
find_package(apd-${TARGET_MACHINE_LOWER}-arxmls REQUIRED)

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
    ${APD_CLUSTER_ARXMLS}
)

set(SWCL_SHORTNAME "UpdatableAppSwcl")
set(SWPKG_SHORTNAME "UpdatableAppSwpkgUpdate")

# define what should be included in the cluster
set(CLUSTER_FILES_RELATIVE
    UpdatableApp/bin/UpdatableApp
    UpdatableApp/etc/MANIFEST.json
    UpdatableApp/etc/service_instance_manifest.json
)

swcl_app_external_project(
    PROJECT_NAME UpdatableApp
    SRC_DIR ${PROJECT_SOURCE_DIR}/../../../applications/UpdatableApp
)

build_software_package(
    SWCL_ARXML_TEMPLATE "UpdatableApp_swcl.arxml"
    SWPKG_ARXML_TEMPLATE "UpdatableApp_swp_update.arxml"
    SWCL_FQN "/apd/SoftwareClusters/${CLUSTER_NAME}/${SWCL_SHORTNAME}"
    SWPKG_FQN "/apd/SoftwarePackages/${CLUSTER_NAME}/${SWPKG_SHORTNAME}"
    SWPKG_ARCHIVE "${SWPKG_SHORTNAME}_${PROJECT_VERSION}"
    CLUSTER_FILES_RELATIVE ${CLUSTER_FILES_RELATIVE}
    ADDITIONAL_ARXML_FILES ${ARXML_SET}
    APPLICATION_INSTALL_TARGETS UpdatableApp
)
