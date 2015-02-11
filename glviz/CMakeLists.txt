include(shader_wrap_cpp)

# GLviz shader.
file(GLOB OPENGL_SHADERS src/shader/*)
shader_wrap_cpp(SOURCES_OPENGL_SHADERS "${OPENGL_SHADERS}")

source_group("Shader Files" FILES ${OPENGL_SHADERS})

add_library(shader
    ${SOURCES_OPENGL_SHADERS}
    ${OPENGL_SHADERS}
)

add_dependencies(shader
    embed_resource
)

# GLviz.
set(SOURCES
    src/glviz.cpp
    src/camera.cpp
    src/shader.cpp
    src/trackball.cpp
    src/program.cpp
    src/buffer.cpp
    src/utility.cpp
)

set(HEADERS
    src/glviz.hpp
    src/camera.hpp
    src/shader.hpp
    src/trackball.hpp
    src/program.hpp
    src/buffer.hpp
    src/utility.hpp
)

# Use static anttweakbar.
add_definitions(-DTW_NO_LIB_PRAGMA -DTW_STATIC)

# Use static GLEW.
add_definitions(-DGLEW_STATIC)

# Disable stupid MSVC warnings.
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
endif()

add_library(glviz
    ${SOURCES}
    ${HEADERS}
)

include_directories(
    include
    ${eigen_INCLUDE_DIRS}
    ${anttweakbar_INCLUDE_DIRS}
    ${sdl_INCLUDE_DIRS}
    ${glew_INCLUDE_DIRS}
)

add_dependencies(glviz
    shader
    anttweakbar
    SDL2main
    SDL2-static
    glew
)

find_package(OpenGL REQUIRED)

set(glviz_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${eigen_INCLUDE_DIRS}
    ${anttweakbar_INCLUDE_DIRS}
    ${sdl_INCLUDE_DIRS}
    ${glew_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    CACHE INTERNAL "glviz include directory")

set(glviz_LIBRARIES
    glviz
    shader
    ${anttweakbar_LIBRARIES}
    ${sdl_LIBRARIES}
    ${glew_LIBRARIES}
    ${OPENGL_LIBRARIES}
    CACHE INTERNAL "glviz library")
