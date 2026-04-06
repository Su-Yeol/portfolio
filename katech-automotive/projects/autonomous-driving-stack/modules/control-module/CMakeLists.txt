# Control Module (ASIL C)
# Reference: SW-CTL-REQ-001, SW-CTL-ARCH-001

set(CTL_SOURCES
    src/ControlModule.cpp
    src/Communicator.cpp
    src/Controller.cpp
    src/PathManager.cpp
)

if(USE_PRIVATE_IMPL)
    list(APPEND CTL_SOURCES
        ${CMAKE_SOURCE_DIR}/modules/common/src/private/control-module/ControlModule_impl.cpp
        ${CMAKE_SOURCE_DIR}/modules/common/src/private/control-module/Communicator_impl.cpp
        ${CMAKE_SOURCE_DIR}/modules/common/src/private/control-module/Controller_impl.cpp
        ${CMAKE_SOURCE_DIR}/modules/common/src/private/control-module/PathManager_impl.cpp
    )
endif()

add_executable(run_control ${CTL_SOURCES})

target_include_directories(run_control PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)

target_link_libraries(run_control PRIVATE common_lib pthread)
set_safety_compiler_warnings(run_control)
enable_coverage(run_control)
