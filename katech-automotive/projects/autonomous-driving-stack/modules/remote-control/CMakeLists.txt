# Remote Control Module (ASIL B)
# Reference: SW-RMT-REQ-001, SW-RMT-ARCH-001
# Two targets: remotePC (x86 sender) and remoteS32G (aarch64 receiver)

# --- PC variant (UDP command sender) ---
add_executable(run_remote_pc src/remotePC.cpp)
target_include_directories(run_remote_pc PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
target_link_libraries(run_remote_pc PRIVATE common_lib pthread)
set_safety_compiler_warnings(run_remote_pc)
enable_coverage(run_remote_pc)

# --- S32G variant (UDP receiver + CAN forwarder) ---
add_executable(run_remote_s32g src/remoteS32G.cpp)
target_include_directories(run_remote_s32g PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)
target_link_libraries(run_remote_s32g PRIVATE common_lib pthread)
set_safety_compiler_warnings(run_remote_s32g)
enable_coverage(run_remote_s32g)
