# Fix SDL header includes.
file(READ examples/imgui_impl_sdl.cpp IMGUI_IMPL_SDL)
string(REPLACE "#include <SDL" "#include <SDL2/SDL"
    IMGUI_IMPL_SDL "${IMGUI_IMPL_SDL}")
file(WRITE examples/imgui_impl_sdl_patched.cpp "${IMGUI_IMPL_SDL}")
