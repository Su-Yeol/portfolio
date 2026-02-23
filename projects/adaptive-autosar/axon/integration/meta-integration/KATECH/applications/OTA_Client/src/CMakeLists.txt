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

set(APP_NAME OTA_Client)
set(APP_EXECUTABLE ${APP_NAME})

set(PROCESS_NAME ota_client)
set(PROCESS_FQN /ara/apd/ota_client/processes/${PROCESS_NAME})

set(GEN_DEST ${PROJECT_BINARY_DIR}/gen/${APP_NAME})
set(SWC_FQN /ara/apd/ota_client/software_components/OTA)
set(MACHINE_NAME DevelopmentMachine)
set(MACHINE /ara/apd/machines/${MACHINE_NAME})
set(VSOMEIP_CONFIG_FILENAME ${GEN_DEST}/machines/vsomeip/${MACHINE_NAME}_vsomeip.json)
set(EM_CONFIG_FILENAME ${GEN_DEST}/processes/${PROCESS_NAME}_MANIFEST.json)
set(SI_MANIFEST_FILENAME ${GEN_DEST}/processes/${PROCESS_NAME}_SI_MANIFEST.json)
set(REST_CONFIG_FILENAME ../files/configuration.json)
set(HAWKBIT_CONFIG_FILENAME ./hawkbit_provider/config.ini)

set(INTERFACES_MAKE_TARGET ${APP_NAME}InterfacesGenerated)
set(PROCESS_ARTIFACTS_MAKE_TARGET ${APP_NAME}ProcessGenerated)
set(MACHINE_ARTIFACTS_MAKE_TARGET ${APP_NAME}MachineGenerated)
set(UCM_ARXMLS_SEARCH_PATHS /usr/${CMAKE_INSTALL_DATAROOTDIR})

find_file(UCM_MASTER_DEPLOYMENT_MANIFEST
    ucm_master_deployment_manifest.arxml
    PATHS
    ${UCM_ARXMLS_SEARCH_PATHS}
    PATH_SUFFIXES
    vehicle_package_manager
    ONLY_CMAKE_FIND_ROOT_PATH
)

# process manifest is needed to generate the startup dependencies
find_file(UCM_MASTER_PROCESS_MANIFEST
    ucm_master.arxml
    PATHS
    ${UCM_ARXMLS_SEARCH_PATHS}
    PATH_SUFFIXES
    vehicle_package_manager
    ONLY_CMAKE_FIND_ROOT_PATH
)

# process manifest references elements of the kvs manifest
find_file(UCM_MASTER_KVS_MANIFEST
  ucm_master_kvs_interfaces.arxml
    PATHS
    ${UCM_ARXMLS_SEARCH_PATHS}
  PATH_SUFFIXES
    vehicle_package_manager
    ONLY_CMAKE_FIND_ROOT_PATH
)

# process manifest is needed to generate the startup dependencies
find_file(UCM_PROCESS_MANIFEST
  ucm.arxml
    PATHS
    ${UCM_ARXMLS_SEARCH_PATHS}
  PATH_SUFFIXES
    package_manager
  ONLY_CMAKE_FIND_ROOT_PATH
)
find_file(SM_PROCESS_MANIFEST
  sm_proc.arxml
    PATHS
    ${UCM_ARXMLS_SEARCH_PATHS}
  PATH_SUFFIXES
    sm-arxmls
    ONLY_CMAKE_FIND_ROOT_PATH
)

# process manifest references elements of the kvs manifest
find_file(UCM_KVS_MANIFEST
  ucm_kvs_interfaces.arxml
    PATHS
    ${UCM_ARXMLS_SEARCH_PATHS}
  PATH_SUFFIXES
    package_manager
  ONLY_CMAKE_FIND_ROOT_PATH
)

# find the ucm_client.arxml in order to set up the execution dependency
# since the ucm_client.arxml is not part of the SDK or bitbake toolchain
# we directly look at the sibling directory
find_file(UCM_CLIENT_MANIFEST
  ucm_client.arxml
    PATHS
    ${PROJECT_SOURCE_DIR}/../../../sample-applications/ucm_examples/pkgmgr_sample
  PATH_SUFFIXES
    files
    REQUIRED
  NO_CMAKE_FIND_ROOT_PATH
)

set(OTA_Client_ARXML_MANIFESTS
    ${PROJECT_SOURCE_DIR}/files/OTA.arxml
    ${PROJECT_SOURCE_DIR}/files/OTA_instance_manifest.arxml
    ${PROJECT_SOURCE_DIR}/files/OTA_mapping_manifest.arxml
    ${PROJECT_SOURCE_DIR}/files/OTA_timing_manifest.arxml
)

set(ARXML_SET
    ${ARA_ARXMLS_DIR}/AUTOSAR_MOD_StandardTypes.arxml
    ${ARA_ARXMLS_DIR}/shared_types.arxml
    ${ARA_ARXMLS_DIR}/ucmtypes.arxml
    ${ARA_ARXMLS_DIR}/machine_manifest.arxml
    ${ARA_ARXMLS_DIR}/network_manifest.arxml
    ${ARA_ARXMLS_DIR}/vehicle_package_management.arxml
    ${ARA_ARXMLS_DIR}/system_components.arxml
    ${UCM_MASTER_DEPLOYMENT_MANIFEST}
    ${UCM_MASTER_PROCESS_MANIFEST}
    ${UCM_MASTER_KVS_MANIFEST}
    ${OTA_Client_ARXML_MANIFESTS}
    ${UCM_PROCESS_MANIFEST}
    ${SM_PROCESS_MANIFEST}
    ${UCM_KVS_MANIFEST}
    ${UCM_CLIENT_MANIFEST}
)

add_process_artifacts(
    DESTINATION ${GEN_DEST}
    TARGET ${PROCESS_ARTIFACTS_MAKE_TARGET}
    PROCESS_FQN ${PROCESS_FQN}
    ARXMLS ${ARXML_SET}
)

add_middleware_interfaces(
    DESTINATION ${GEN_DEST}
    TARGET ${INTERFACES_MAKE_TARGET}
    SWC_FQN ${SWC_FQN}
    ARXMLS ${ARXML_SET}
)

set(MAIN_src
    ${PROJECT_SOURCE_DIR}/src/main.cpp)

set(PROJECT_SOURCES
	${PROJECT_SOURCE_DIR}/src/find_service.cpp
	${PROJECT_SOURCE_DIR}/src/data_transfer.cpp
	${PROJECT_SOURCE_DIR}/src/vpackage_management_app.cpp
	${${PROCESS_ARTIFACTS_MAKE_TARGET}_SOURCES}
)

# add more .cpps here later
set(HAWKBIT_SOURCES
	${PROJECT_SOURCE_DIR}/src/hawkbit_provider/hawkbit_config_file.cpp
	${PROJECT_SOURCE_DIR}/src/hawkbit_provider/hawkbit_client.cpp
	${PROJECT_SOURCE_DIR}/src/hawkbit_provider/json_helper.cpp
)

set(UPDATE_CLIENT_SOURCES
	${PROJECT_SOURCE_DIR}/src/pull_provider/json_helper.cpp
	${PROJECT_SOURCE_DIR}/src/pull_provider/update_client.cpp
)

set(UCM_ACCESSOR_SOURCES
	${PROJECT_SOURCE_DIR}/src/ucm_accessor/ucm_accessor_skeleton.cpp
	${PROJECT_SOURCE_DIR}/src/ucm_accessor/ucm_accessor.cpp
  ${PROJECT_SOURCE_DIR}/src/ucm_accessor/logger.cpp
)

# vsomeip config
add_machine_artifacts(
    DESTINATION ${GEN_DEST}
    TARGET ${MACHINE_ARTIFACTS_MAKE_TARGET}
    MACHINE_FQN ${MACHINE}
    ARXMLS ${ARXML_SET}
)

add_executable(${APP_EXECUTABLE}
	${PROJECT_SOURCES}
	${HAWKBIT_SOURCES}
	${UPDATE_CLIENT_SOURCES}
	${UCM_ACCESSOR_SOURCES}
	${MAIN_src}
)

target_include_directories(${APP_NAME}
	PRIVATE ${PROJECT_SOURCE_DIR}/src/
	PRIVATE ${PROJECT_SOURCE_DIR}/src/provider/
	PRIVATE ${PROJECT_SOURCE_DIR}/src/hawkbit_provider
	PRIVATE ${PROJECT_SOURCE_DIR}/src/ucm_accessor
	PRIVATE ${PROJECT_SOURCE_DIR}/src/pull_provider
	PRIVATE ${GEN_DEST}/includes/
	PRIVATE ${GEN_DEST}/net-bindings/vsomeip/
	PRIVATE ${VSOMEIP_INCLUDE_DIRS}
)

target_link_libraries (${APP_EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT} ${ARA_LOGGING_LIBRARIES} ara::exec_execution_client ara_com ara_vsomeipbinding ara::log ara::ara_rest ara::iam Boost::filesystem ${Boost_LIBRARIES})

add_dependencies(${APP_EXECUTABLE} ${INTERFACES_MAKE_TARGET} ${PROCESS_ARTIFACTS_MAKE_TARGET} ${MACHINE_ARTIFACTS_MAKE_TARGET})

# Install access_control_list.json
install(FILES ${PROJECT_SOURCE_DIR}/files/access_control_list.json
	DESTINATION ${APD_APP_INSTALL_PREFIX}/${PROJECT_NAME}/etc
	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ
	COMPONENT development
)

apd_install_app_executable(${APP_NAME} ${APP_EXECUTABLE})
apd_install_app_manifest(${APP_NAME} ${EM_CONFIG_FILENAME})
apd_install_com_manifest(${APP_NAME} ${VSOMEIP_CONFIG_FILENAME})
apd_install_app_file_rename(${PROJECT_NAME} etc ${SI_MANIFEST_FILENAME} service_instance_manifest.json)
apd_install_app_file(${APP_NAME} etc ${REST_CONFIG_FILENAME})
apd_install_app_file(${APP_NAME} etc ${HAWKBIT_CONFIG_FILENAME})

add_subdirectory(unit_test)