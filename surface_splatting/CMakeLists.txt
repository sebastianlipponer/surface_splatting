# Use static anttweakbar.
add_definitions(-DTW_NO_LIB_PRAGMA -DTW_STATIC)

# Use static GLEW.
add_definitions(-DGLEW_STATIC)

# Use static freeglut.
add_definitions(-DFREEGLUT_LIB_PRAGMAS=0 -DFREEGLUT_STATIC)

file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/resources/" GLVIZ_RESOURCES_DIR)
configure_file(config.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/config.hpp")

file(GLOB OPENGL_SHADERS shader/*)
shader_wrap_cpp(SOURCES_OPENGL_SHADERS "${OPENGL_SHADERS}")

add_executable(surface_splatting
    main.cpp
    framebuffer.hpp
    framebuffer.cpp
    program_finalization.hpp
    program_finalization.cpp
    program_attribute.hpp
    program_attribute.cpp
    splat_renderer.cpp
    splat_renderer.hpp
    ${OPENGL_SHADERS}
    ${SOURCES_OPENGL_SHADERS}
)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${glviz_INCLUDE_DIRS}
)

add_dependencies(surface_splatting
    glviz
)

target_link_libraries(surface_splatting
    ${glviz_LIBRARIES}
)
