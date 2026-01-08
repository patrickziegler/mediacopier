function(register_code_format_target target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "register_code_format_target: '${target}' is not a target")
    endif()

    set_property(GLOBAL APPEND PROPERTY CODE_FORMAT_TARGETS ${target})
    message(STATUS "register_code_format_target: added '${target}'")

endfunction()

function(add_code_format_targets)
    find_program(CLANG_FORMAT_EXE clang-format)

    if(NOT CLANG_FORMAT_EXE)
        message(WARNING "add_code_format_targets: 'clang-format' not found, no targets added")
        return()
    endif()

    message(STATUS "clang-format found: ${CLANG_FORMAT_EXE}")

    get_property(TARGETS_LIST GLOBAL PROPERTY CODE_FORMAT_TARGETS)
    set(ALL_SOURCES)

    foreach(target IN LISTS TARGETS_LIST)
        get_target_property(target_sources ${target} SOURCES)
        get_target_property(target_source_dir ${target} SOURCE_DIR)

        foreach(source ${target_sources})
            if(NOT IS_ABSOLUTE "${source}")
                set(source "${target_source_dir}/${source}")
            endif()

            get_filename_component(ext "${source}" LAST_EXT)
            if(ext STREQUAL ".cpp" OR ext STREQUAL ".hpp")
                list(APPEND ALL_SOURCES "${source}")
            endif()

        endforeach()

    endforeach()

    add_custom_target(codeformat
        COMMENT "Formatting files with clang-format .."
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCES}
    )
    message(STATUS "add_code_format_targets: added target 'codeformat'")

    add_custom_target(formatcheck
        COMMENT "Checking files with clang-format .."
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_SOURCES}
    )
    message(STATUS "add_code_format_targets: added target 'formatcheck")

endfunction()
