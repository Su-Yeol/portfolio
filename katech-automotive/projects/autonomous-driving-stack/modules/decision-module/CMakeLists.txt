# Decision Module (ASIL B~C)
# Reference: SW-DEC-REQ-001, SW-DEC-ARCH-001
# Two variants: K-City (run_kcity) and CAN Gateway (run_gateway)

# --- K-City variant ---
set(KCITY_SOURCES
    source_kcity/MainModule.cpp
    source_kcity/Communicator.cpp
    source_kcity/PathManager.cpp
)

if(USE_PRIVATE_IMPL)
    file(GLOB KCITY_PRIVATE_SRCS
        ${CMAKE_SOURCE_DIR}/modules/common/src/private/decision-module/kcity/*_impl.cpp
    )
    list(APPEND KCITY_SOURCES ${KCITY_PRIVATE_SRCS})
endif()

add_executable(run_kcity ${KCITY_SOURCES})
target_include_directories(run_kcity PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
target_link_libraries(run_kcity PRIVATE common_lib pthread)
set_safety_compiler_warnings(run_kcity)
enable_coverage(run_kcity)

# --- CAN Gateway variant ---
# Note: CANGateway.cpp and CRC.cpp each contain their own main().
# CANGateway.cpp is the gateway executable; CRC.cpp is a standalone CRC test tool.
set(GATEWAY_SOURCES
    source_gateway/CANGateway.cpp
)

if(USE_PRIVATE_IMPL)
    file(GLOB GATEWAY_PRIVATE_SRCS
        ${CMAKE_SOURCE_DIR}/modules/common/src/private/decision-module/gateway/*_impl.cpp
    )
    list(APPEND GATEWAY_SOURCES ${GATEWAY_PRIVATE_SRCS})
endif()

add_executable(run_gateway ${GATEWAY_SOURCES})
target_include_directories(run_gateway PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
target_link_libraries(run_gateway PRIVATE common_lib pthread)
set_safety_compiler_warnings(run_gateway)
enable_coverage(run_gateway)
