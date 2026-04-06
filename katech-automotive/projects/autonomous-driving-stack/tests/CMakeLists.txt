# Test Suite for ISO 26262 Verification
# Reference: SW-ADS-UT-001 (Unit Testing), SW-ADS-IT-001 (Integration Testing)

# --- Unit Tests (ISO 26262 Part 6, Clause 9) ---

# UT: CRC Provider (ASIL D - SW-CMN-REQ-CRC-001~004)
add_executable(test_crc_provider common/test_crc_provider.cpp)
target_link_libraries(test_crc_provider PRIVATE common_lib GTest::gtest_main)
target_include_directories(test_crc_provider PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
enable_coverage(test_crc_provider)
add_test(NAME UT_CrcProvider COMMAND test_crc_provider)

# UT: Signal Codec (ASIL D - SW-CMN-REQ-SIG-001~005)
add_executable(test_signal_codec common/test_signal_codec.cpp)
target_link_libraries(test_signal_codec PRIVATE GTest::gtest_main)
target_include_directories(test_signal_codec PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
enable_coverage(test_signal_codec)
add_test(NAME UT_SignalCodec COMMAND test_signal_codec)

# UT: Config Parser (ASIL D - SW-CMN-REQ-CFG-001~002)
add_executable(test_config_parser common/test_config_parser.cpp)
target_link_libraries(test_config_parser PRIVATE common_lib GTest::gtest_main)
target_include_directories(test_config_parser PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
enable_coverage(test_config_parser)
add_test(NAME UT_ConfigParser COMMAND test_config_parser
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# UT: CAN Spec Config Loader (ASIL D - SW-CMN-REQ-CFG-001~004)
add_executable(test_can_spec_config_loader common/test_can_spec_config_loader.cpp)
target_link_libraries(test_can_spec_config_loader PRIVATE common_lib GTest::gtest_main)
target_include_directories(test_can_spec_config_loader PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
enable_coverage(test_can_spec_config_loader)
add_test(NAME UT_CanSpecConfigLoader COMMAND test_can_spec_config_loader
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# --- Integration Tests (ISO 26262 Part 6, Clause 10) ---

# IT: CRC + SignalCodec + CanSpec pipeline
add_executable(test_crc_codec_pipeline integration/test_crc_codec_pipeline.cpp)
target_link_libraries(test_crc_codec_pipeline PRIVATE common_lib GTest::gtest_main)
target_include_directories(test_crc_codec_pipeline PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
enable_coverage(test_crc_codec_pipeline)
add_test(NAME IT_CrcCodecPipeline COMMAND test_crc_codec_pipeline
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# IT: Config pipeline (ConfigParser + CanSpecConfigLoader)
add_executable(test_config_pipeline integration/test_config_pipeline.cpp)
target_link_libraries(test_config_pipeline PRIVATE common_lib GTest::gtest_main)
target_include_directories(test_config_pipeline PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
enable_coverage(test_config_pipeline)
add_test(NAME IT_ConfigPipeline COMMAND test_config_pipeline
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# IT: Fault injection tests
add_executable(test_fault_injection integration/test_fault_injection.cpp)
target_link_libraries(test_fault_injection PRIVATE common_lib GTest::gtest_main)
target_include_directories(test_fault_injection PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
enable_coverage(test_fault_injection)
add_test(NAME IT_FaultInjection COMMAND test_fault_injection
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
