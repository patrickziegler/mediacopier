find_package(OpenMP REQUIRED)

add_executable(mcp)

target_sources(mcp PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/mcp.cpp"
    )

target_link_libraries(mcp mediacopier)
target_link_libraries(mcp OpenMP::OpenMP_CXX)

install(TARGETS mcp
    RUNTIME
    DESTINATION bin
    COMPONENT App
    )
