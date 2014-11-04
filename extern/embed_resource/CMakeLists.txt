# Disable stupid MSVC warnings.
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(embed_resource
    embed_resource.cpp
)

set_target_properties(embed_resource PROPERTIES FOLDER "External Dependencies")
