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
project(vehicle_packages VERSION 1.0.0 LANGUAGES)

# Set these variables (like in the yocto recipe) to build the application as a specific process on a specific machine
if(NOT TARGET_MACHINE)
    set(TARGET_MACHINE "RadarFusionMachine")
endif()

include(GNUInstallDirs)
include(AUTOSAR/apd)

# Import ARA
message(STATUS "Importing Dependencies")

# Generic stuff (ara generator and arxmls)
find_package(ara-gen REQUIRED)

find_package(ara-ucm REQUIRED)

add_compile_options(-Werror)

message(STATUS "Sysroot dir is ${SYSROOT_DIR}, prefix ${CMAKE_INSTALL_PREFIX}, data dir ${CMAKE_INSTALL_DATADIR}")

set(DATA_DIR "${CMAKE_INSTALL_DATADIR}/apdtest/ucm/")
generate_vehicle_package(
    MAJOR_VERSION 1
    MINOR_VERSION 0
    CERTIFICATE_FILES ${OEMSUBCA_CRT} ${INTEGRATOR_CRT}
    CAMPAIGN_NAME ucm_vpm_picar
    ARXML_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/templates/ucm_vpm_picar.arxml
    ARXML_SWCL ${SYSROOT_DIR}/${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/apdtest/ucm/ControllerAppSwpkgUpdate_2.0.0.arxml ${SYSROOT_DIR}/${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/apdtest/ucm/UltrasonicSensorAppSwpkgInstall_1.0.0.arxml ${SYSROOT_DIR}/${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/apd-cluster-arxmls/cluster_documentations.arxml
    PRIVATE_KEY ${INTEGRATOR_KEY}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ucm_vpm_picar.arvp PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION ${DATA_DIR})

foreach(TESTMODE dualBank singleBank)
    foreach(TESTCASE depFail goodCase_V1 goodCase_V2 VersionFail)
        set(CSWPP_NAME "${TESTMODE}_${TESTCASE}")
        set(CSWCL_NAME "${TESTMODE}")
        if(${TESTCASE} MATCHES ".*V2$")
            set(CSWCL_VERSION "2.0.0")
        else()
            set(CSWCL_VERSION "1.0.0")
        endif()
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/templates/ucm_vpm_classic.arxml ${CMAKE_CURRENT_BINARY_DIR}/ucm_vpm_classic_${CSWPP_NAME}_generated.arxml)
        generate_vehicle_package(
            MAJOR_VERSION 1
            MINOR_VERSION 0
            CERTIFICATE_FILES ${OEMSUBCA_CRT} ${INTEGRATOR_CRT}
            CAMPAIGN_NAME ucm_vpm_classic_${CSWPP_NAME}
            ARXML_INPUT ${CMAKE_CURRENT_BINARY_DIR}/ucm_vpm_classic_${CSWPP_NAME}_generated.arxml
            ARXML_SWCL ${SYSROOT_DIR}/${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/apdtest/ucm/${CSWPP_NAME}SwpkgInstall_${CSWCL_VERSION}.arxml ${SYSROOT_DIR}/${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/apd-cluster-arxmls/cluster_documentations.arxml
            PRIVATE_KEY ${INTEGRATOR_KEY}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ucm_vpm_classic_${CSWPP_NAME}.arvp" PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION ${DATA_DIR})
    endforeach()
endforeach()
