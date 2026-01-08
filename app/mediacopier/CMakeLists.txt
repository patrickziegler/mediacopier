find_package(spdlog REQUIRED)

set(TARGET_NAME mediacopier)

add_executable(${TARGET_NAME} ${CLI_TARGET_FLAGS})

set(EXECUTABLE_NAME ${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE "source/main.cpp")

target_link_libraries(${TARGET_NAME} PUBLIC
    ${MEDIACOPIER_UI_LIB})

install(TARGETS ${TARGET_NAME})

set(DESKTOP_FILE_NAME "${TARGET_NAME}.desktop")

configure_file("${DESKTOP_FILE_NAME}.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")

set(ICON_FILE_NAME "${TARGET_NAME}.svg")

install(FILES "${ICON_FILE_NAME}"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps")

register_code_format_target(${TARGET_NAME})
