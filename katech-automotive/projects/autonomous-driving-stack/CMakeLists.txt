cmake_minimum_required(VERSION 3.15)
project(autonomous-driving-stack
    VERSION 0.1.0
    LANGUAGES CXX
    DESCRIPTION "KATECH Autonomous Driving Stack (ISO 26262 ASIL B-D)"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- Safety build options ---
option(USE_PRIVATE_IMPL "Include private safety-critical implementations" OFF)
option(BUILD_TESTING    "Build unit and integration tests"                ON)

# --- ISO 26262 build infrastructure ---
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(StaticAnalysis)
include(Coverage)

# --- Modules ---
add_subdirectory(modules/common)
add_subdirectory(modules/aeb-control)
add_subdirectory(modules/control-module)
add_subdirectory(modules/decision-module)
add_subdirectory(modules/remote-control)

# --- Tests (ISO 26262 Part 6, Clause 9-10) ---
if(BUILD_TESTING)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    add_subdirectory(tests)
endif()
