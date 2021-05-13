find_package(log4cplus)

set(TARGET_NAME mediacopier-cli)

add_executable(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME mediacopier)

target_sources(${TARGET_NAME} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp")

target_link_libraries(${TARGET_NAME} PRIVATE
    "${LOG4CPLUS_LIBRARY}" ${MEDIACOPIER_LIB_TARGET_NAME})
