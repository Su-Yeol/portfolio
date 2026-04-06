# AEB Control Module (ASIL D)
# Reference: SW-AEB-REQ-001, SW-AEB-ARCH-001

set(AEB_SOURCES
    src/AEBControl_S32G_v2.cpp
    src/Communicator.cpp
)

if(USE_PRIVATE_IMPL)
    list(APPEND AEB_SOURCES
        ${CMAKE_SOURCE_DIR}/modules/common/src/private/aeb-control/AEBControl_S32G_v2_impl.cpp
        ${CMAKE_SOURCE_DIR}/modules/common/src/private/aeb-control/Communicator_impl.cpp
    )
endif()

add_executable(run_aeb ${AEB_SOURCES})

target_include_directories(run_aeb PRIVATE
    ${CMAKE_SOURCE_DIR}/modules/common/include
)

target_link_libraries(run_aeb PRIVATE common_lib pthread)
set_safety_compiler_warnings(run_aeb)
enable_coverage(run_aeb)
