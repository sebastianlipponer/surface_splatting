find_package(GLviz REQUIRED CONFIG)

file(TO_NATIVE_PATH "${PROJECT_SOURCE_DIR}/resources/" GLVIZ_RESOURCES_DIR)
configure_file(config.hpp.in "${CMAKE_CURRENT_BINARY_DIR}/config.hpp")

# Surface splatting shader.
set(SHADER_GLSL
    shader/attribute_fs.glsl
    shader/attribute_vs.glsl
    shader/finalization_fs.glsl
    shader/finalization_vs.glsl
    shader/lighting.glsl
)

include(GLvizShaderWrapCpp)
glviz_shader_wrap_cpp(SHADER_CPP ${SHADER_GLSL})

add_library(shader STATIC)

target_sources(shader PRIVATE
    ${SHADER_GLSL}
    ${SHADER_CPP}
)

source_group("Shader Files" FILES ${SHADER_GLSL})

# Surface splatting executable.
add_executable(surface_splatting)

target_sources(surface_splatting PRIVATE
    main.cpp
    framebuffer.hpp
    framebuffer.cpp
    program_finalization.hpp
    program_finalization.cpp
    program_attribute.hpp
    program_attribute.cpp
    splat_renderer.cpp
    splat_renderer.hpp
)

target_include_directories(surface_splatting
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_link_libraries(surface_splatting
    PRIVATE shader
            GLviz::glviz
)
