cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

project(ImGui
    VERSION 1.90.5
    LANGUAGES CXX
)

set(CMAKE_DEBUG_POSTFIX "d")

add_library(imgui STATIC)

target_sources(imgui PRIVATE
    imgui.h
    imgui_internal.h
    imconfig.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h

    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_tables.cpp
    imgui_widgets.cpp
)

target_link_libraries(imgui PUBLIC
    SDL2::SDL2main
    SDL2::SDL2-static
)

find_package(SDL2 REQUIRED CONFIG)
find_package(glew REQUIRED CONFIG)

add_library(imgui_sdl STATIC)

target_sources(imgui_sdl PRIVATE
    backends/imgui_impl_sdl2.h
    backends/imgui_impl_opengl3.h

    backends/imgui_impl_sdl2.cpp
    backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_sdl
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(imgui_sdl PUBLIC
    SDL2::SDL2main
    SDL2::SDL2-static
    GLEW::glew_s
)

set(CMAKE_INSTALL_LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include")
set(CMAKE_INSTALL_BINDIR "${CMAKE_INSTALL_PREFIX}/bin")


set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "Config.cmake.in"
    ${project_config}
    INSTALL_DESTINATION "${config_install_dir}"
)

install(TARGETS imgui imgui_sdl
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    FILES imgui.h imconfig.h backends/imgui_impl_sdl2.h
          backends/imgui_impl_opengl3.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)

install(
    EXPORT ImGuiTargets
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
