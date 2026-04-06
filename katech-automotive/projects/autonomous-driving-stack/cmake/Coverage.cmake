# Code coverage configuration for ISO 26262 compliance
# Reference: SW-ADS-UT-001 (Table 10 coverage targets)
#
# Coverage targets by ASIL:
#   ASIL B: Statement >= 95%, Branch >= 90%
#   ASIL C: Statement >= 100%, Branch >= 95%, MC/DC recommended
#   ASIL D: Statement 100%, Branch 100%, MC/DC >= 95%

option(ENABLE_COVERAGE "Enable code coverage measurement (gcov/lcov)" OFF)

function(enable_coverage target_name)
    if(ENABLE_COVERAGE)
        target_compile_options(${target_name} PRIVATE --coverage -fprofile-arcs -ftest-coverage)
        target_link_options(${target_name} PRIVATE --coverage)
    endif()
endfunction()

if(ENABLE_COVERAGE)
    find_program(LCOV_PATH lcov)
    find_program(GENHTML_PATH genhtml)

    if(LCOV_PATH AND GENHTML_PATH)
        add_custom_target(coverage
            COMMAND ${LCOV_PATH} --capture --directory ${CMAKE_BINARY_DIR}
                    --output-file ${CMAKE_BINARY_DIR}/coverage.info
                    --rc lcov_branch_coverage=1
            COMMAND ${LCOV_PATH} --remove ${CMAKE_BINARY_DIR}/coverage.info
                    '/usr/*' '*/third_party/*' '*/tests/*' '*/googletest/*'
                    --output-file ${CMAKE_BINARY_DIR}/coverage.info
                    --rc lcov_branch_coverage=1
            COMMAND ${GENHTML_PATH} ${CMAKE_BINARY_DIR}/coverage.info
                    --output-directory ${CMAKE_BINARY_DIR}/coverage_report
                    --branch-coverage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating code coverage report (ISO 26262 Table 10)"
        )
    else()
        message(WARNING "lcov/genhtml not found -- coverage target disabled")
    endif()
endif()
