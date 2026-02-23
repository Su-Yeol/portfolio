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

set(TEST_NAME TestOTAClient)

set(TEST_GEN_DEST ${PROJECT_BINARY_DIR}/gen/${TEST_NAME})
set(TEST_INTERFACES_MAKE_TARGET ${TEST_NAME}InterfacesGenerated)
set(TEST_PROCESS_ARTIFACTS_MAKE_TARGET ${TEST_NAME}ProcessGenerated)

add_process_artifacts(
    DESTINATION ${TEST_GEN_DEST}
    TARGET ${TEST_PROCESS_ARTIFACTS_MAKE_TARGET}
    PROCESS_FQN ${PROCESS_FQN}
    ARXMLS ${ARXML_SET}
)

add_middleware_interfaces(
    DESTINATION ${TEST_GEN_DEST}
    TARGET ${TEST_INTERFACES_MAKE_TARGET}
    SWC_FQN ${SWC_FQN}
    ARXMLS ${ARXML_SET}
)

set(TEST_SOURCES
    vpackage_management_app_test.cpp
)

set(TEST_TARGET adcm-ucm-testota-client)

add_executable(${TEST_TARGET} ${TEST_SOURCES} ${PROJECT_SOURCE_DIR}/src/find_service.cpp ${PROJECT_SOURCE_DIR}/src/data_transfer.cpp ${PROJECT_SOURCE_DIR}/src/vpackage_management_app.cpp ${UCM_ACCESSOR_SOURCES} ${${TEST_PROCESS_ARTIFACTS_MAKE_TARGET}_SOURCES})

target_link_libraries(${TEST_TARGET} GTest::Main ${GMOCK})
target_link_libraries(${TEST_TARGET} ${CMAKE_THREAD_LIBS_INIT} ${ARA_LOGGING_LIBRARIES} ara::exec_execution_client ara_com ara_vsomeipbinding ara::log ara::ara_rest ara::iam Boost::filesystem ${Boost_LIBRARIES})

add_dependencies(${TEST_TARGET} ${TEST_INTERFACES_MAKE_TARGET} ${TEST_PROCESS_ARTIFACTS_MAKE_TARGET})

target_include_directories(${TEST_TARGET}
    PRIVATE ${PROJECT_SOURCE_DIR}/src
    PRIVATE ${PROJECT_SOURCE_DIR}/src/ucm_accessor
    PRIVATE ${TEST_GEN_DEST}/includes/
    PRIVATE ${GEN_DEST}/net-bindings/vsomeip/
	  PRIVATE ${VSOMEIP_INCLUDE_DIRS}
)

if (ARA_RUN_TESTS)
    docker_exec_test(${TEST_TARGET} /build/src/unit_test)
endif()

install(TARGETS ${TEST_TARGET} RUNTIME DESTINATION ${APD_CMAKE_INSTALL_BINTESTDIR})
