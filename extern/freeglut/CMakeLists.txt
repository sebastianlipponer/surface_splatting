if (UNIX)
    include(CheckIncludeFiles)

    check_include_files(dlfcn.h HAVE_DLFCN_H)
    check_include_files(errno.h HAVE_ERRNO_H)
    check_include_files(fcntl.h HAVE_FCNTL_H)
    check_include_files(GL/glu.h HAVE_GL_GLU_H)
    check_include_files(GL/glx.h HAVE_GL_GLX_H)
    check_include_files(GL/gl.h HAVE_GL_GL_H)
    check_include_files(inttypes.h HAVE_INTTYPES_H)
    check_include_files(libusbhid.h HAVE_LIBUSBHID_H)
    check_include_files(limits.h HAVE_LIMITS_H)
    check_include_files(memory.h HAVE_MEMORY_H)
    check_include_files(stdint.h HAVE_STDINT_H)
    check_include_files(stdlib.h HAVE_STDLIB_H)
    check_include_files(strings.h HAVE_STRINGS_H)
    check_include_files(string.h HAVE_STRING_H)
    check_include_files(sys/ioctl.h HAVE_SYS_IOCTL_H)
    check_include_files(sys/param.h HAVE_SYS_PARAM_H)
    check_include_files(sys/stat.h HAVE_SYS_STAT_H)
    check_include_files(sys/time.h HAVE_SYS_TIME_H)
    check_include_files(sys/types.h HAVE_SYS_TYPES_H)
    check_include_files(unistd.h HAVE_UNISTD_H)
    check_include_files(usbhid.h HAVE_USBHID_H)
    check_include_files(X11/extensions/xf86vmode.h
        HAVE_X11_EXTENSIONS_XF86VMODE_H)
    check_include_files(X11/extensions/XInput2.h
        HAVE_X11_EXTENSIONS_XINPUT2_H)
    check_include_files(X11/extensions/XInput.h
        HAVE_X11_EXTENSIONS_XINPUT_H)
    check_include_files(X11/extensions/XI.h
        HAVE_X11_EXTENSIONS_XI_H)
    check_include_files(X11/extensions/Xrandr.h
        HAVE_X11_EXTENSIONS_XRANDR_H)

    add_definitions(-DHAVE_CONFIG_H)
    configure_file(src/config.h.in
        "${CMAKE_CURRENT_BINARY_DIR}/src/config.h")
endif()

set(SOURCES
    src/freeglut_callbacks.c
    src/freeglut_cursor.c
    src/freeglut_display.c
    src/freeglut_ext.c
    src/freeglut_font.c
    src/freeglut_font_data.c
    src/freeglut_gamemode.c
    src/freeglut_geometry.c
    src/freeglut_glutfont_definitions.c
    src/freeglut_init.c
    src/freeglut_input_devices.c
    src/freeglut_internal.h
    src/freeglut_joystick.c
    src/freeglut_main.c
    src/freeglut_menu.c
    src/freeglut_misc.c
    src/freeglut_overlay.c
    src/freeglut_spaceball.c
    src/freeglut_state.c
    src/freeglut_stroke_mono_roman.c
    src/freeglut_stroke_roman.c
    src/freeglut_structure.c
    src/freeglut_teapot.c
    src/freeglut_teapot_data.h
    src/freeglut_videoresize.c
    src/freeglut_window.c
    src/freeglut_xinput.c
)

set(HEADERS
    include/GL/freeglut.h
    include/GL/freeglut_ext.h
    include/GL/freeglut_std.h
    include/GL/glut.h
)

add_definitions(-DFREEGLUT_EXPORTS)

# Disable stupid MSVC warnings.
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS) 
endif()

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}/src
    include
)

add_library(freeglut STATIC ${HEADERS} ${SOURCES})

set_target_properties(freeglut PROPERTIES FOLDER "External Dependencies")

set(LIBRARIES freeglut)
if (UNIX)
    list(APPEND LIBRARIES Xxf86vm Xrandr Xi pthread)
endif()

set(freeglut_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL "freeglut include directory")
set(freeglut_LIBRARIES ${LIBRARIES}
    CACHE INTERNAL "freeglut library")
