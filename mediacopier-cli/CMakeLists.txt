add_subdirectory(common)
add_subdirectory(mediacopier-qt)

if(${ENABLE_KDE})
    add_subdirectory(mediacopier-kde)
endif()
