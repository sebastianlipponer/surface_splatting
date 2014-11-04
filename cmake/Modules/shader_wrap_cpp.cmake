function(shader_wrap_cpp _OUT SHADERS)
    foreach(SHADER ${SHADERS})
        get_filename_component(FILE ${SHADER} NAME_WE)
        get_filename_component(EXT ${SHADER} EXT)

        string(REGEX REPLACE "\\." "_" EXT "${EXT}")

        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${FILE}${EXT}.cpp"
            COMMAND embed_resource ARGS "${FILE}${EXT}" "${SHADER}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            DEPENDS ${SHADER}
        )
        
        list(APPEND SOURCE "${CMAKE_CURRENT_BINARY_DIR}/${FILE}${EXT}.cpp")
    endforeach(SHADER)

    set(${_OUT} ${SOURCE} PARENT_SCOPE)
endfunction()
