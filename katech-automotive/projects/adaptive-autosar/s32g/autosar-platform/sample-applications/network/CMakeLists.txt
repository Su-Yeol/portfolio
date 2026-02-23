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

# cmake_minimum_required(VERSION 3.15)
# project(apd-network-arxmls VERSION "1.0.0" LANGUAGES NONE)

# include(AUTOSAR/apd)

# include(GNUInstallDirs)
# include(CMakePackageConfigHelpers)

# # List of all network arxml files available in apd.
# set(APD_NETWORK_ARXML_FILES
#   "network.arxml"
# )

# set(APD_NETWORK_ARXMLS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")

# # Create *-config.cmake file
# configure_package_config_file(
#   ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
#   ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
#   INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
#   PATH_VARS APD_NETWORK_ARXMLS_DIR
# )

# # Install *-config.cmake file
# install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
#   DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
# )

# # Install the arxml files
# install(FILES ${APD_NETWORK_ARXML_FILES}
#   DESTINATION "${APD_NETWORK_ARXMLS_DIR}"
# )


cmake_minimum_required(VERSION 3.4)
project(adcm-network-arxmls VERSION "1.0.0" LANGUAGES NONE)

include(AUTOSAR/apd)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# List of all network arxml files available in apd.
set(ADCM_NETWORK_ARXML_FILES
  "network.arxml"
)

set(ADCM_NETWORK_ARXMLS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}")

# Create *-config.cmake file
configure_package_config_file(
  ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
  PATH_VARS ADCM_NETWORK_ARXMLS_DIR
)

# Install *-config.cmake file
install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}"
)

# Install the arxml files
install(FILES ${ADCM_NETWORK_ARXML_FILES}
  DESTINATION "${ADCM_NETWORK_ARXMLS_DIR}"
)
