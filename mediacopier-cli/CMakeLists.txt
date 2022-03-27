if(${ENABLE_CLI} OR ${ENABLE_KDE})
    add_subdirectory(common)
endif()

if(${ENABLE_CLI})
    add_subdirectory(mediacopier-qt)
endif()

if(${ENABLE_KDE})
    add_subdirectory(mediacopier-kde)
endif()
