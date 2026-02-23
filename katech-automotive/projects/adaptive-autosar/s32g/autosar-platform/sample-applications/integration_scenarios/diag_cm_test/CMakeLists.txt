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
project(diag_integration_test VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 14)

include(AUTOSAR/apd)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(GNUInstallDirs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Wall")

if(NOT DEFINED HELLODIAGWORLD_DIR)
    set(HELLODIAGWORLD_DIR ${CMAKE_INSTALL_PREFIX})
endif()
if(NOT DEFINED CROSSFUNCREQS_DIR)
    set(CROSSFUNCREQS_DIR ${CMAKE_INSTALL_PREFIX})
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

if(HAS_VSOMEIP_BINDING AND HAS_WRSOMEIP_BINDING)
    message(FATAL_ERROR "Both VSOMEIP and WRSOMEIP Bindings are enabled")
endif()

# use vsomeip binding by default
if(NOT HAS_VSOMEIP_BINDING AND NOT HAS_WRSOMEIP_BINDING)
    set(HAS_VSOMEIP_BINDING ON CACHE BOOL "Use communication over some/ip via vsomeip" FORCE)
endif()

if(HAS_VSOMEIP_BINDING)
    set(COMMUNICATION_LIBRARY ara_vsomeipbinding)
    set(SOMEIP_BINDING_LIBRARY vsomeip)
    add_compile_definitions(HAS_VSOMEIP_BINDING)
endif()

if(HAS_WRSOMEIP_BINDING)
    set(COMMUNICATION_LIBRARY ara_wrsomeipbinding)
    set(SOMEIP_BINDING_LIBRARY wrsomeip)
    add_compile_definitions(HAS_WRSOMEIP_BINDING)
endif()

add_subdirectory(hellodiagworld)
add_subdirectory(crossfuncreqs)
