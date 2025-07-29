set(KF_LIBS)

if(${USE_QT5})
    find_package(KF5I18n REQUIRED)
    find_package(KF5JobWidgets REQUIRED)
    list(APPEND KF_LIBS KF5::I18n KF5::JobWidgets)
    set(KF_VERSION ${KF5I18n_VERSION})
else()
    find_package(KF6I18n REQUIRED)
    find_package(KF6JobWidgets REQUIRED)
    list(APPEND KF_LIBS KF6::I18n KF6::JobWidgets)
    set(KF_VERSION ${KF6I18n_VERSION})
endif()

set(TARGET_NAME mediacopier-plasma)

add_executable(${TARGET_NAME} ${CLI_TARGET_FLAGS})

target_sources(${TARGET_NAME} PRIVATE "source/main.cpp")

target_link_libraries(${TARGET_NAME} PUBLIC
    ${KF_LIBS} ${MEDIACOPIER_UI_LIB})

install(TARGETS ${TARGET_NAME})

set(DESKTOP_FILE_NAME "${TARGET_NAME}.desktop")

configure_file("${DESKTOP_FILE_NAME}.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications")

set(SERVICE_MENU_FILE_NAME "servicemenu/${TARGET_NAME}.desktop")

configure_file("${SERVICE_MENU_FILE_NAME}.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${SERVICE_MENU_FILE_NAME}")

# install location has changed with version 5.85 according to [1], but [2] seems to contradict (?)
# [1] https://develop.kde.org/docs/apps/dolphin/service-menus/#where-the-servicemenus-are-located
# [2] https://invent.kde.org/documentation/develop-kde-org/-/issues/140
if(${KF_VERSION} VERSION_GREATER_EQUAL "5.85")
    set(INSTALL_SERVICE_MENUS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/kio/servicemenus")
else()
    set(INSTALL_SERVICE_MENUS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/kservices5/ServiceMenus")
endif()

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${SERVICE_MENU_FILE_NAME}"
    DESTINATION "${INSTALL_SERVICE_MENUS_DIR}")
