set(COVERAGE_OUTPUT_DIR "${CMAKE_BINARY_DIR}/coverage")
set(BASELINE "${COVERAGE_OUTPUT_DIR}/baseline.info")
set(TRACEFILE "${COVERAGE_OUTPUT_DIR}/trace.info")
set(REPORT_DIR "${COVERAGE_OUTPUT_DIR}/report")

add_custom_command(
    OUTPUT "${TRACEFILE}"
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${COVERAGE_OUTPUT_DIR}"
    COMMAND lcov -o "${BASELINE}" -d "${CMAKE_BINARY_DIR}" -c -i
    COMMAND ctest
    COMMAND lcov -o "${TRACEFILE}" -d "${CMAKE_BINARY_DIR}" -c
    COMMAND lcov -o "${TRACEFILE}" -a "${BASELINE}" -a "${TRACEFILE}"
    COMMAND lcov -o ${TRACEFILE} -e ${TRACEFILE}
                "${CMAKE_SOURCE_DIR}/*"
    COMMAND lcov -o ${TRACEFILE} -r ${TRACEFILE}
                "${CMAKE_SOURCE_DIR}/build/*"
                "${CMAKE_SOURCE_DIR}/extern/*"
                "${CMAKE_SOURCE_DIR}/app/main.cpp"
                "${CMAKE_SOURCE_DIR}/app/test/*"
                "${CMAKE_SOURCE_DIR}/lib/test/*"
    COMMAND genhtml ${TRACEFILE} --output-directory ${REPORT_DIR}
    VERBATIM)

add_custom_target(coverage
    DEPENDS ${TRACEFILE})
