add_executable(mmv)

target_sources(mmv PRIVATE
    "mmv.cpp"
    )

target_link_libraries(mmv mediacopier)

install(TARGETS mmv
    RUNTIME
    DESTINATION bin
    COMPONENT App
    )
