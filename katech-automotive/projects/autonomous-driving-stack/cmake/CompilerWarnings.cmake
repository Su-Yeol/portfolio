# ISO 26262 ASIL D compliant compiler warning flags
# Reference: SW-ADS-UD-001, MISRA C++:2008 / AUTOSAR C++14

function(set_safety_compiler_warnings target_name)
    target_compile_options(${target_name} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wshadow
        -Wcast-align
        -Wunused
        -Wnull-dereference
        -Wdouble-promotion
        -Wformat=2
        -Wimplicit-fallthrough
        -Woverloaded-virtual
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op
        -Wuseless-cast
        -Wno-error=unused-parameter  # Stubs intentionally have unused params
    )

    # ASIL D: treat warnings as errors in release builds
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target_name} PRIVATE -Werror)
    endif()
endfunction()
