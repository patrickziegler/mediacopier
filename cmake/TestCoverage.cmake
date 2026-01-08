set(COVERAGE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/coverage")
set(REPORT_DIR "${COVERAGE_OUTPUT_DIR}/report")

set(TRACEFILE "${COVERAGE_OUTPUT_DIR}/trace.info")
set(TRACEFILE_FILTERED "${COVERAGE_OUTPUT_DIR}/trace_filtered.info")

set(LCOV_GCOV_OPTION "")
if(DEFINED ENV{LCOV_GCOV_TOOL} AND NOT "$ENV{LCOV_GCOV_TOOL}" STREQUAL "")
    set(LCOV_GCOV_OPTION --gcov-tool "$ENV{LCOV_GCOV_TOOL}")
endif()

add_custom_command(
    OUTPUT "${TRACEFILE}" always

    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"

    COMMAND ${CMAKE_COMMAND} -E make_directory "${COVERAGE_OUTPUT_DIR}"

    COMMAND lcov
                --capture
                --directory "${CMAKE_BINARY_DIR}"
                --output-file "${TRACEFILE}"
                --rc geninfo_auto_base=1
                --ignore-errors inconsistent
                ${LCOV_GCOV_OPTION}

    COMMAND lcov
                --remove ${TRACEFILE}
                    "/usr/*" "*.hpp"
                    "${CMAKE_SOURCE_DIR}/build/*"
                    "${CMAKE_SOURCE_DIR}/build-coverage/*"
                    "${CMAKE_SOURCE_DIR}/lib/core/test/*"
                --output-file ${TRACEFILE_FILTERED}
                --ignore-errors unused
                ${LCOV_GCOV_OPTION}

    COMMAND genhtml ${TRACEFILE_FILTERED}
                --output-directory ${REPORT_DIR}

    VERBATIM # for correct handling of wildcards in command line parameters
)

add_custom_target(coverage
    DEPENDS ${TRACEFILE} always
)
