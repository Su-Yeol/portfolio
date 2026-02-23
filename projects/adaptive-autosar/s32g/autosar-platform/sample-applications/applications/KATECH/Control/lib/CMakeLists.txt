cmake_minimum_required(VERSION 3.15)

project(KatechLibrary LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(_GNU_SOURCE)

if(DEFINED ENV{SDKTARGETSYSROOT})
    set(SYSROOT $ENV{SDKTARGETSYSROOT})
else()
    message(WARNING "SDKTARGETSYSROOT not set. Host includes will be used!")
    set(SYSROOT "/usr")
endif()

set(ADCM_INCLUDE_DIRS
    ${SYSROOT}/usr/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../common/service/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../build/gen/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/../../common/logger/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../common/sensor/gps
    ${CMAKE_CURRENT_SOURCE_DIR}/../../common/
    ${CMAKE_CURRENT_SOURCE_DIR}/common/include
)

include_directories(${ADCM_INCLUDE_DIRS})

message(STATUS "Using SYSROOT: ${SYSROOT}")
message(STATUS "Include path: ${CMAKE_CURRENT_SOURCE_DIR}/../build/gen/includes")

set(ADCM_PRIVATE_IMPL_SRC ${CMAKE_CURRENT_SOURCE_DIR}/../private-adcm/src/ADCM_KATECH_impl.cpp)
set(ADCM_PREBUILT_LIB ${CMAKE_CURRENT_SOURCE_DIR}/adcm_private/lib/libadcm_katech.a)
option(ADCM_REQUIRE_PRIVATE_BINARY "Fail configuration if private implementation is not available" OFF)

if(EXISTS ${ADCM_PRIVATE_IMPL_SRC})
    message(STATUS "Using private source implementation: ${ADCM_PRIVATE_IMPL_SRC}")
    add_library(LIB_KATECH STATIC
        ${ADCM_PRIVATE_IMPL_SRC}
        common/src/CanSpecConfigLoader.cpp
    )
elseif(EXISTS ${ADCM_PREBUILT_LIB})
    message(STATUS "Using prebuilt private library: ${ADCM_PREBUILT_LIB}")
    add_library(LIB_KATECH STATIC IMPORTED GLOBAL)
    set_target_properties(LIB_KATECH PROPERTIES
        IMPORTED_LOCATION ${ADCM_PREBUILT_LIB}
        INTERFACE_INCLUDE_DIRECTORIES "${ADCM_INCLUDE_DIRS}"
    )
else()
    if(ADCM_REQUIRE_PRIVATE_BINARY)
        message(FATAL_ERROR "No private ADCM implementation found. Expected one of: ${ADCM_PRIVATE_IMPL_SRC} or ${ADCM_PREBUILT_LIB}")
    endif()

    message(WARNING "Private ADCM implementation not found. Building PUBLIC STUB from ADCM_KATECH.cpp")
    add_library(LIB_KATECH STATIC
        ADCM_KATECH.cpp
        common/src/CanSpecConfigLoader.cpp
    )
    target_compile_definitions(LIB_KATECH PRIVATE ADCM_PUBLIC_STUB=1)
endif()

get_target_property(LIB_KATECH_IS_IMPORTED LIB_KATECH IMPORTED)
if(NOT LIB_KATECH_IS_IMPORTED)
    set_target_properties(LIB_KATECH PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    )
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
