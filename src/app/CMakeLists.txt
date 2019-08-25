add_executable(mcp)
target_sources(mcp PRIVATE "mcp.cpp")
target_link_libraries(mcp PRIVATE mediacopier)

add_executable(mmv)
target_sources(mmv PRIVATE "mmv.cpp")
target_link_libraries(mmv PRIVATE mediacopier)

install(TARGETS mcp mmv
    RUNTIME
    DESTINATION bin
    COMPONENT App
    )
