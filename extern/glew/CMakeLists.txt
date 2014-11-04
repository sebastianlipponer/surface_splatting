set(SOURCES
    src/glew.c
)

set(HEADERS
    include/GL/glew.h
    include/GL/glxew.h
    include/GL/wglew.h
)


add_definitions(-DGLEW_NO_GLU -DGLEW_STATIC)
if(UNIX)
    add_definitions(-fPIC)
endif()

include_directories(include)

add_library(glew STATIC ${HEADERS} ${SOURCES})

set_target_properties(glew PROPERTIES FOLDER "External Dependencies")

set(glew_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
    CACHE INTERNAL "glew include directory")
set(glew_LIBRARIES glew
    CACHE INTERNAL "glew library")
