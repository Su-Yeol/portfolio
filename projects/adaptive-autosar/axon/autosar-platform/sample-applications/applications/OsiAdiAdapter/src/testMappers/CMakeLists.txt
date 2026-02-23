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


set (TEST_TARGET OsiAdiAdapter_test_Mappers)

#list (APPEND CMAKE_PREFIX_PATH "/home/bhakti/ara-project/native-docker-build/build/RadarFusionMachine")
add_executable (${TEST_TARGET}
                ${PROJECT_SOURCE_DIR}/src/createOsiDummyData.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/GeneralElementsMapper.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/RoadObjectsMapper.cpp 
                ${PROJECT_SOURCE_DIR}/src/adiMappers/LidarDetectionsMapper.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/RadarDetectionsMapper.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/TrafficLightsMapper.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/TrafficSignsMapper.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/StaticObjectsMapper.cpp             
                ${PROJECT_SOURCE_DIR}/src/adiMappers/ObjectStatusMapper.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/CameraFeaturesMapper.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/CameraDetectionsMapper.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/UltrasonicDetectionsMapper.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/UltrasonicFeaturesMapper.cpp
                ${PROJECT_SOURCE_DIR}/src/adiMappers/PMObjectsMapper.cpp
                testMapperScripts.cpp)   
                
target_include_directories (${TEST_TARGET}
                            PUBLIC
                            ${GEN_DEST}/includes
                            ${ARA_INCLUDE_DIRS}
                            ${PROJECT_SOURCE_DIR}/include)
           
target_link_libraries (${TEST_TARGET}
                       ${GMOCK}
                       ${GTEST_BOTH_LIBRARIES}
                       ${ARA_LIBRARIES} 
                        
                       ara_com e2exf e2e ara::log ara::exec_execution_client ara::core open_simulation_interface)

if (ARA_RUN_TESTS)
    docker_exec_test(${TEST_TARGET} ${CMAKE_CURRENT_BINARY_DIR})
endif ()
