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

project(Linesensor VERSION 1.0.0 LANGUAGES CXX)

include(ExternalProject)

set(CLUSTER_NAME ${PROJECT_NAME})

include(AUTOSAR/apd)
include("GNUInstallDirs")
find_package(ara-core REQUIRED)  # generator
find_package(ara-gen REQUIRED)  # generator
find_package(ara-ucm REQUIRED)

find_package(apd-cluster-arxmls REQUIRED)

# List all other files of the machine (like machine manifest)
list(TRANSFORM APD_CLUSTER_ARXMLS PREPEND ${APD_CLUSTER_ARXMLS_DIR}/)

set(SWCL_SHORTNAME "LinesensorAppSwcl")
set(SWPKG_SHORTNAME "LinesensorAppSwpkgInstall")

# define what should be included in the cluster
set(CLUSTER_FILES_RELATIVE
    linesensor/bin/linesensor
    linesensor/etc/MANIFEST.json
    linesensor/etc/service_instance_manifest.json
)

if(PICAR)
    set(ARGS_PICAR "-DPICAR=1")
endif()

swcl_app_external_project(
    PROJECT_NAME linesensor_external_project
    SRC_DIR ${PROJECT_SOURCE_DIR}/../../../applications/PiCar_Linesensor
    EXTRA_CMAKE ${ARGS_PICAR}
)

install_software_cluster(
    SWCL_ARXML_TEMPLATE "linesensor_swcl.arxml"
    SWCL_FQN "/apd/SoftwareClusters/${CLUSTER_NAME}/${SWCL_SHORTNAME}"
    CLUSTER_FILES_RELATIVE ${CLUSTER_FILES_RELATIVE}
    ADDITIONAL_ARXML_FILES ${APD_CLUSTER_ARXMLS}
    APPLICATION_INSTALL_TARGETS linesensor_external_project
)
