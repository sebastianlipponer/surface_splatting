set(SOURCES
    src/LoadOGL.cpp
    src/LoadOGLCore.cpp
    src/TwBar.cpp
    src/TwColors.cpp
    src/TwEventSDL.c
    src/TwEventSDL12.c
    src/TwEventSDL13.c
    src/TwEventSDL20.c
    src/TwFonts.cpp
    src/TwMgr.cpp
    src/TwOpenGL.cpp
    src/TwOpenGLCore.cpp
    src/TwPrecomp.cpp
)

set(HEADERS
    include/AntTweakBar.h
)
    
add_definitions(-DTW_EXPORTS -DTW_STATIC)

if (WIN32)
    list(APPEND SOURCES src/TwEventWin.c)
endif()

if (UNIX)
    add_definitions(-D_UNIX -D__PLACEMENT_NEW_INLINE)
endif()

include_directories(include)

add_library(anttweakbar STATIC ${HEADERS} ${SOURCES})

set_target_properties(anttweakbar PROPERTIES FOLDER "External Dependencies")

set(anttweakbar_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL "anttweakbar include directory")
set(anttweakbar_LIBRARIES anttweakbar
    CACHE INTERNAL "anttweakbar library")
