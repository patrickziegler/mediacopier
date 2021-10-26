find_package(KF5JobWidgets REQUIRED)
find_package(spdlog REQUIRED)

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)

set(TARGET_NAME mediacopier-kde)

add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "job.hpp" "job.cpp" "main.cpp" "worker.cpp" "worker.hpp")

target_link_libraries(${TARGET_NAME} PUBLIC
    KF5::JobWidgets spdlog::spdlog
    ${MEDIACOPIER_LIB_TARGET_NAME})
