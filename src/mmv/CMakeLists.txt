find_package(OpenMP REQUIRED)

add_executable(mmv)

target_sources(mmv PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/mmv.cpp"
    )

target_link_libraries(mmv mediacopier)
target_link_libraries(mmv OpenMP::OpenMP_CXX)

install(TARGETS mmv
    RUNTIME
    DESTINATION bin
    COMPONENT App
    )
