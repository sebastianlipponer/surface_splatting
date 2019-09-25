cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

project(surface_splatting
    LANGUAGES CXX
)

set(extern_install_dir "${CMAKE_SOURCE_DIR}/.extern/install")
list(APPEND CMAKE_MODULE_PATH "${extern_install_dir}/cmake/Modules")
list(APPEND CMAKE_PREFIX_PATH "${extern_install_dir}")

# Visual studio solution directories.
set_property(GLOBAL PROPERTY USE_FOLDERS on)

# Enable simultaneous compilation of source files for MSVC.
add_compile_options(
    $<$<CXX_COMPILER_ID:MSVC>:/MP>
)

# Put all executables and libraries into a common directory.
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")
set(LIBRARY_OUTPUT_PATH    "${PROJECT_BINARY_DIR}/bin")

add_subdirectory(src)
