# Static analysis integration for ISO 26262 ASIL compliance
# Reference: PRC-VER-001 Section 3, SW-ADS-UD-001

option(ENABLE_CLANG_TIDY "Run clang-tidy during compilation" OFF)
option(ENABLE_CPPCHECK  "Run cppcheck during compilation"   OFF)

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY
            "${CLANG_TIDY_EXE}"
            "--config-file=${CMAKE_SOURCE_DIR}/.clang-tidy"
        )
        message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXE}")
    else()
        message(WARNING "clang-tidy not found -- static analysis disabled")
    endif()
endif()

if(ENABLE_CPPCHECK)
    find_program(CPPCHECK_EXE NAMES "cppcheck")
    if(CPPCHECK_EXE)
        set(CMAKE_CXX_CPPCHECK
            "${CPPCHECK_EXE}"
            "--enable=all"
            "--std=c++17"
            "--suppress=missingIncludeSystem"
            "--suppress=unmatchedSuppression"
            "--inline-suppr"
            "--quiet"
        )
        message(STATUS "cppcheck enabled: ${CPPCHECK_EXE}")
    else()
        message(WARNING "cppcheck not found -- static analysis disabled")
    endif()
endif()
