# Common Module (ASIL D by inheritance)
# Reference: SW-CMN-REQ-001, SW-CMN-ARCH-001

set(COMMON_SOURCES
    src/ConfigParser.cpp
    src/CanSpecConfigLoader.cpp
    src/CrcProviderPublic.cpp
    src/CrcProviderPrivate.cpp
)

add_library(common_lib STATIC ${COMMON_SOURCES})

target_include_directories(common_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(common_lib PUBLIC cxx_std_17)
set_safety_compiler_warnings(common_lib)
enable_coverage(common_lib)

if(USE_PRIVATE_IMPL)
    target_compile_definitions(common_lib PUBLIC USE_PRIVATE_IMPL)
endif()
