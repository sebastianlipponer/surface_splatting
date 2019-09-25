cmake_minimum_required(VERSION 3.8.2 FATAL_ERROR)

project(build-extern
    LANGUAGES NONE
)

include(ExternalProject)

function(DownloadProject _name)
    cmake_parse_arguments(_arg "" "URL;URL_MD5" "" ${ARGN})

    if (NOT DEFINED _arg_URL)
        message(FATAL_ERROR "Error: No 'URL' argument given.")
    endif()
    if (NOT DEFINED _arg_URL_MD5)
        message(FATAL_ERROR "Error: No 'URL_MD5' argument given.")
    endif()

    ExternalProject_Add(${_name}
        URL          "${_arg_URL}"
        URL_MD5      "${_arg_URL_MD5}"
        LOG_DOWNLOAD TRUE

        STEP_TARGETS patch

        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""

        ${_arg_UNPARSED_ARGUMENTS}
    )

    string(TOUPPER "${_name}" _upper_name)
    ExternalProject_Get_Property(${_name} SOURCE_DIR)
    set(${_upper_name}_SOURCE_DIR "${SOURCE_DIR}" PARENT_SCOPE)
endfunction()

function(BuildProject _name)
    cmake_parse_arguments(_arg "" "BUILD_TYPE" "" ${ARGN})
    if (NOT DEFINED _arg_BUILD_TYPE)
        message(FATAL_ERROR "Error: No 'BUILD_TYPE' argument given.")
    endif()

    string(TOLOWER "${_arg_BUILD_TYPE}" _lower_build_type)
    string(TOUPPER "${_name}" _upper_name)

    ExternalProject_Add(${_name}-${_lower_build_type}
        DOWNLOAD_COMMAND ""
        SOURCE_DIR ${${_upper_name}_SOURCE_DIR}

        INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
        CMAKE_CACHE_ARGS "-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>"
            "-DCMAKE_BUILD_TYPE:STRING=${_arg_BUILD_TYPE}"

        STEP_TARGETS install

        LOG_CONFIGURE TRUE
        LOG_BUILD TRUE
        LOG_INSTALL TRUE

        ${_arg_UNPARSED_ARGUMENTS}
    )

    ExternalProject_Add_StepDependencies(${_name}-${_lower_build_type}
         configure ${_name}-patch
    )
endfunction()

# Download.
DownloadProject(libsdl
    URL     "https://www.libsdl.org/release/SDL2-2.0.10.zip"
    URL_MD5 "6b2e9a4a2faba4ff277062cf669724f4"
)

DownloadProject(glew
    URL     "https://downloads.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0.zip"
    URL_MD5 "dff2939fd404d054c1036cc0409d19f1"
    PATCH_COMMAND ${CMAKE_COMMAND} -P
        "${CMAKE_CURRENT_SOURCE_DIR}/glew/patch.cmake"
)

DownloadProject(imgui
    URL     "https://github.com/ocornut/imgui/archive/v1.72b.zip"
    URL_MD5 "de566304c3abc2aef99b81deb1dfb37e"

    PATCH_COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui.cmake" <SOURCE_DIR>/CMakeLists.txt
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/Config.cmake.in" <SOURCE_DIR>
    COMMAND ${CMAKE_COMMAND} -P
        "${CMAKE_CURRENT_SOURCE_DIR}/imgui/patch.cmake"
)

DownloadProject(eigen
    URL     "http://bitbucket.org/eigen/eigen/get/3.3.7.zip"
    URL_MD5 "0d9c8496922d5c07609b9f3585f00e49"
)

DownloadProject(glviz
    URL     "https://github.com/sebastianlipponer/glviz/archive/1.0.zip"
    URL_MD5 "d66d00661861405d3ae1adb3d104393c"
)

# Build.
BuildProject(eigen BUILD_TYPE Release)  # Eigen is a header only library.

foreach(_build_type IN ITEMS Debug Release)
    string(TOLOWER "${_build_type}" _lower_build_type)

    BuildProject(libsdl
        BUILD_TYPE ${_build_type}
        CMAKE_CACHE_ARGS "-DSDL_STATIC:BOOL=ON" "-DSDL_SHARED:BOOL=OFF"
                         "-DSDL_AUDIO:BOOL=OFF" "-DDIRECTX:BOOL=OFF"
                         "-DRENDER_D3D:BOOL=OFF")

    BuildProject(glew
        BUILD_TYPE ${_build_type}
        SOURCE_SUBDIR "build/cmake")

    BuildProject(imgui
        BUILD_TYPE ${_build_type}
    )
    ExternalProject_Add_StepDependencies(imgui-${_lower_build_type}
        configure libsdl-${_lower_build_type}-install
                  glew-${_lower_build_type}-install
    )

    BuildProject(glviz
        BUILD_TYPE ${_build_type}
        CMAKE_CACHE_ARGS "-DBUILD_EXAMPLE:BOOL=OFF"
    )
    ExternalProject_Add_StepDependencies(glviz-${_lower_build_type}
        configure libsdl-${_lower_build_type}-install
                  glew-${_lower_build_type}-install
                  imgui-${_lower_build_type}-install
                  eigen-release-install
    )

    unset(_lower_build_type)
endforeach()

# The install step of 'debug' and 'release' projects cannot be executed
# in parallel since they write common files.
foreach(_name IN ITEMS libsdl glew imgui glviz)
    ExternalProject_Add_StepDependencies(${_name}-release
         install ${_name}-debug-install
    )
endforeach()
