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

project(apd-testsuiteecu2machine-arxmls VERSION 1.0.0 LANGUAGES NONE)

include("${PROJECT_NAME}.cmake")

# Do not edit any of the following lines!
# They are generic and do not depend on a specific machine when following the new structure.
# ------------------------------------------------------

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(APD_MACHINE_ARXMLS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")
set(APD_MACHINE_PROCESS_ARXMLS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/processes")

# Create *-config.cmake file
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/${PROJECT_NAME}-config.cmake.in
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  PATH_VARS APD_MACHINE_ARXMLS_DIR APD_MACHINE_PROCESS_ARXMLS_DIR
)

# Install *-config.cmake file
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

# Install *.cmake file
install(FILES "${PROJECT_NAME}.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

# Install the arxml files
install(FILES ${APD_MACHINE_ARXMLS}
  DESTINATION "${APD_MACHINE_ARXMLS_DIR}"
)

install(FILES ${APD_PROCESS_ARXMLS}
  DESTINATION "${APD_MACHINE_PROCESS_ARXMLS_DIR}"
)
