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

set(DATA_DIR "${CMAKE_INSTALL_DATADIR}/apdtest/ucm/")

foreach(TESTMODE dualBank singleBank)
    foreach(TESTCASE depFail goodCase_V1 goodCase_V2 VersionFail)
        set(CSWPP_NAME "${TESTMODE}_${TESTCASE}")
        set(CSWCL_NAME "${TESTMODE}")
        if(${TESTCASE} MATCHES ".*V2$")
            set(CSWCL_VERSION "2.0.0")
        else()
            set(CSWCL_VERSION "1.0.0")
        endif()
        create_classic_swp(
            INPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/files/${CSWPP_NAME}"
            SWPKG_NAME ${CSWPP_NAME}
            SWPKG_VERSION ${CSWCL_VERSION}
            SWPKG_ARXML_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/classic_swp.arxml"
            SWCL_ARXML_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/classic_swcl.arxml"
            )
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${CSWPP_NAME}SwpkgInstall_${CSWCL_VERSION}.zip" PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION ${DATA_DIR})
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${CSWPP_NAME}SwpkgInstall_${CSWCL_VERSION}.arxml" PERMISSIONS OWNER_READ GROUP_READ WORLD_READ DESTINATION ${DATA_DIR})
    endforeach()
endforeach()
