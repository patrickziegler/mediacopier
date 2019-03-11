find_package(OpenMP REQUIRED)

add_executable(mmv)

target_include_directories(mmv PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_sources(mmv PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/mmv.cpp"
    )

target_link_libraries(mmv mediacopier)
target_link_libraries(mmv OpenMP::OpenMP_CXX)

target_link_options(mmv PRIVATE "-lexiv2")

install(TARGETS mmv
    RUNTIME
    DESTINATION bin
    COMPONENT App
    )
