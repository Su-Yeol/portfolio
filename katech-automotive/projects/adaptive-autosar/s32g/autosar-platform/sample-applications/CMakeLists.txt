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

project(apd C CXX)

# ---- User-configurable options; override these with -D... on the command-line.

# Use the currently recommended way to set a default value for CMAKE_INSTALL_PREFIX; this
# does have limitations (see CMake issue 21242), but they don't matter much for us here.
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "/tmp/apd" CACHE STRING "Install location" FORCE)
endif()

# Allow to build the unit tests as well.
set(ARA_ENABLE_TESTS Off CACHE BOOL "Build the unit tests as well")

# ---- Internal definitions, do not touch.

# This is for the apd.cmake, which is needed by all our modules.
# We install it to this location, so let's make it accessible to all.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_INSTALL_PREFIX}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules)

# These are for the PkgConfig module
list(APPEND CMAKE_PREFIX_PATH /usr/local/jansson)
list(APPEND CMAKE_PREFIX_PATH /usr/local/dlt)
list(APPEND CMAKE_PREFIX_PATH /usr/local/rapidjson)

# These are for finding GTest and GMock
set(GTEST_ROOT /usr/local/gtest)
list(APPEND CMAKE_LIBRARY_PATH /usr/local/gtest/lib)

# CMake's usual "list" separation with ";" does not work when passing on to ExternalProject
string(REPLACE ";" "%" CMAKE_MODULE_PATH_FIXED "${CMAKE_MODULE_PATH}")
string(REPLACE ";" "%" CMAKE_PREFIX_PATH_FIXED "${CMAKE_PREFIX_PATH}")
string(REPLACE ";" "%" CMAKE_LIBRARY_PATH_FIXED "${CMAKE_LIBRARY_PATH}")


set(APD_APP_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}/opt")


include(ExternalProject)

message("Start configuring top level project : ${PROJECT_NAME}")
message("Generating using CMake version: " ${CMAKE_MAJOR_VERSION} "." ${CMAKE_MINOR_VERSION} "." ${CMAKE_PATCH_VERSION})

if(NOT TARGET_MACHINE)
    set(TARGET_MACHINE "RadarFusionMachine")
endif()
function(apd_add_external_project APD_NAME APD_PATH)
	ExternalProject_Add(${APD_NAME}
		SOURCE_DIR ${PROJECT_SOURCE_DIR}/${APD_PATH}
		LIST_SEPARATOR %
		BUILD_ALWAYS 1
		CMAKE_ARGS
			--no-warn-unused-cli
			-DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
			-DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH_FIXED}
			-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH_FIXED}
			-DCMAKE_LIBRARY_PATH=${CMAKE_LIBRARY_PATH_FIXED}
			-DGTEST_ROOT=${GTEST_ROOT}
			-DARA_ENABLE_TESTS=${ARA_ENABLE_TESTS}
			-DAPD_APP_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}/opt
            -DTARGET_MACHINE=${TARGET_MACHINE}
	)
	if (${ARGC} GREATER 2)
		message("Adding dependencies: ${APD_NAME} -> ${ARGN}")
		ExternalProject_Add_StepDependencies(${APD_NAME} build ${ARGN})
	endif()
endfunction()

# Define "wrapper targets" for all our modules, including their dependencies.
add_subdirectory(../ara-api ara-api-build)

apd_add_external_project(apd_machine_app_arxmls                      applications)
apd_add_external_project(apd_machine_interfaces_arxmls               interfaces                         apd_cmake_modules)
apd_add_external_project(apd_machine_network_arxmls                  network                            apd_cmake_modules)
apd_add_external_project(apd_machine_artifacts          machines/${TARGET_MACHINE}                      apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_cmake_modules
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls)
apd_add_external_project(apd_machine_manifest           machine_manifest                                apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_network_arxmls)
apd_add_external_project(apd_machine_arxmls             machines/${TARGET_MACHINE}/model                apd_ara_gen
                                                                                                        apd_arxmls)
apd_add_external_project(apd_updatable_app              applications/UpdatableApp                       apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log
                                                                                                        ara_ucm_native)
apd_add_external_project(apd_cm_validator_publisher     applications/CMValidatorPublisher               apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_log
                                                                                                        ara_exec)
apd_add_external_project(apd_cm_validator_subscriber    applications/CMValidatorSubscriber              apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_log
                                                                                                        ara_exec)
apd_add_external_project(apd_demonstrator_tutorial      applications/DemonstratorTutorial               apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_emo_integration_test       applications/EmoIntegrationTest                 apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_log
                                                                                                        ara_exec)
apd_add_external_project(apd_emo_sample_application     applications/EmoSampleApplication               apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_log
                                                                                                        ara_exec)
apd_add_external_project(apd_extended_tutorial          applications/ExtendedTutorial                   apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log
                                                                                                        ara_per)
apd_add_external_project(apd_faulty_calibration_application applications/FaultyCalibrationApplication   apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_log)
apd_add_external_project(apd_faulty_proping_application applications/FaultyProbingApplication           apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls)
apd_add_external_project(apd_faulty_reporting_application applications/FaultyReportingApplication       apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_persistency_demo           applications/PersistencyDemo                    apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_per
                                                                                                        ara_log
                                                                                                        ara_exec)
apd_add_external_project(apd_phm_demo                   applications/PhmDemo                            apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_phm
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_res_timing_ref_app         applications/RES_TimingRefApp                   apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_res_timing_ref_service_app applications/RES_TimingRefServiceApp            apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_res_trace_integration_test_app      applications/RES_TraceIntegrationTest                   apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_res_trace_integration2_test_app      applications/RES_TraceIntegrationTest2                   apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_sec_demo_fc                applications/SecDemoFc                          apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log
                                                                                                        ara_iam)
apd_add_external_project(apd_sec_iam_test_app           applications/SecIamTestApp                      apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log
                                                                                                        ara_iam)
apd_add_external_project(apd_sensor_demo_application    applications/SensorDemoApplication              apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_time_sync_consumer         applications/TimeSyncConsumer                   apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log
                                                                                                        ara_tsync)
apd_add_external_project(apd_time_sync_provider         applications/TimeSyncProvider                   apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log
                                                                                                        ara_tsync)
apd_add_external_project(apd_tlv_demo_receiver          applications/TlvDemoReceiver                    apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_tlv_demo_sender            applications/TlvDemoSender                      apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_calculator                 diag_samples/calculator                         apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        ara_com
                                                                                                        ara_log
                                                                                                        ara_exec)
apd_add_external_project(apd_diag_cm_test               integration_scenarios/diag_cm_test              apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        ara_com
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_persistencyredundancydemo  per_examples/persistencyredundancydemo          apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        ara_com
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_seed_key_provider          diag_samples/seed_key_provider                  apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        ara_com
                                                                                                        ara_log
                                                                                                        ara_exec)
if(TARGET_MACHINE STREQUAL "FusionMachine" OR TARGET_MACHINE STREQUAL "RadarFusionMachine")
apd_add_external_project(apd_fusion                     applications/fusion                             apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_log
                                                                                                        ara_exec)
apd_add_external_project(apd_ucm_flashing_adapter       applications/UCM_flashing_adapter               apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log
                                                                                                        ara_per
                                                                                                        ara_ucm_ucmlibs
                                                                                                        ara_ucm_dpduapi)
apd_add_external_project(apd_ucm_ota_client             applications/UCM_ota_client                     apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_rest
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_ucm_pkgmgr_sample          applications/UCM_pkgmgr_sample                  apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_ucm_transfer_perf_check    applications/UCM_transfer_performance_check     apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_ucm_update_adapter         applications/UCM_update_adapter                 apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_rest
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log
                                                                                                        ara_per
                                                                                                        ara_ucm_ucmlibs)
apd_add_external_project(apd_ucm_veh_driver_app         applications/UCM_vehicle_driver_application     apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_rest
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
endif()
if(TARGET_MACHINE STREQUAL "RadarMachine" OR TARGET_MACHINE STREQUAL "RadarFusionMachine")
apd_add_external_project(apd_radar                      applications/radar                              apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
apd_add_external_project(apd_smtool                     applications/smtool                             apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_types
                                                                                                        ara_log)
apd_add_external_project(apd_ucm_veh_state_mgr_app      applications/UCM_vehicle_state_manager_application      apd_ara_gen
                                                                                                        apd_arxmls
                                                                                                        apd_common_machine_arxmls
                                                                                                        apd_rest
                                                                                                        apd_machine_app_arxmls
                                                                                                        apd_machine_interfaces_arxmls
                                                                                                        apd_machine_network_arxmls
                                                                                                        ara_com
                                                                                                        ara_core_core
                                                                                                        ara_exec
                                                                                                        ara_log)
endif()
