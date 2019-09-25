# Fix error LNK2019: unresolved external symbol memset.
file(READ build/cmake/CMakeLists.txt GLEW_CMAKELISTS_TXT)
string(REPLACE "target_link_libraries (glew LINK_PRIVATE -nodefaultlib -noentry)"
    "#target_link_libraries (glew LINK_PRIVATE -nodefaultlib -noentry)"
    GLEW_CMAKELISTS_TXT "${GLEW_CMAKELISTS_TXT}")
file(WRITE build/cmake/CMakeLists.txt "${GLEW_CMAKELISTS_TXT}")
