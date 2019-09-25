function(glviz_shader_wrap_cpp _out)
    foreach(_shader IN LISTS ARGN)
        get_filename_component(_filename ${_shader} NAME_WE)

        get_filename_component(_ext ${_shader} EXT)
        string(REGEX REPLACE "\\." "_" _ext "${_ext}")

        set(_input "${_shader}")
        if (NOT IS_ABSOLUTE "${_input}")
            set(_input "${CMAKE_CURRENT_SOURCE_DIR}/${_shader}")
        endif()
        set(_output "${CMAKE_CURRENT_BINARY_DIR}/${_filename}${_ext}.cpp")

        add_custom_command(
            OUTPUT "${_output}"
            COMMAND $<TARGET_FILE:GLviz::embed_resource> "${_filename}${_ext}" "${_input}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            DEPENDS "${_input}"
        )
        
        list(APPEND _shader_cpp "${_output}")
    endforeach()
        
    set(${_out} ${_shader_cpp} PARENT_SCOPE)
endfunction()
