// This file is part of GLviz.
//
// Copyright (C) 2014 by Sebastian Lipponer.
// 
// GLviz is free software: you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// GLviz is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with GLviz. If not, see <http://www.gnu.org/licenses/>.

#include "glviz.hpp"

#include "camera.hpp"

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <AntTweakBar.h>

#include <iostream>
#include <cstdlib>
#include <functional>

namespace GLviz
{

namespace
{

int m_opengl_major_version(3), m_opengl_minor_version(3);
int m_screen_width(1280), m_screen_height(720);
unsigned int m_timer_msec = 16;

SDL_Window* m_sdl_window;
SDL_GLContext m_gl_context;

std::function<void ()>                      m_display_callback;
std::function<void (unsigned int)>          m_timer_callback;
std::function<void(int width, int height)>  m_reshape_callback;
std::function<void ()>                      m_close_callback;

TwBar*   m_bar;
Camera*  m_camera;

void
reshapeFunc(int width, int height)
{
    m_screen_width  = width;
    m_screen_height = height;

    if (m_reshape_callback)
    {
        m_reshape_callback(width, height);
    }

    TwWindowSize(m_screen_width, m_screen_height);
}

void
mouseFunc(int button, int state, int x, int y)
{
    const float xf = static_cast<float>(x)
        / static_cast<float>(m_screen_width);
    const float yf = static_cast<float>(y)
        / static_cast<float>(m_screen_height);

    switch (button)
    {
        case SDL_BUTTON_LEFT:
            m_camera->trackball_begin_motion(xf, yf);
            break;

        case SDL_BUTTON_RIGHT:
            m_camera->trackball_begin_motion(xf, yf);
            break;

        case SDL_BUTTON_MIDDLE:
            m_camera->trackball_begin_motion(xf, yf);
            break;
    }
}

void
motionFunc(int state, int x, int y)
{
    const float xf = static_cast<float>(x)
        / static_cast<float>(m_screen_width);
    const float yf = static_cast<float>(y)
        / static_cast<float>(m_screen_height);

    if (state & SDL_BUTTON_LMASK)
    {
        m_camera->trackball_end_motion_rotate(xf, yf);
    }
    else if (state & SDL_BUTTON_RMASK)
    {
        m_camera->trackball_end_motion_zoom(xf, yf);
    }
    else if (state & SDL_BUTTON_MMASK)
    {
        m_camera->trackball_end_motion_translate(xf, yf);
    }
}

bool
process_events()
{
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION))
            continue;

        switch (event.type)
        {
            case SDL_KEYDOWN:
                break;
            case SDL_KEYUP:
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                    reshapeFunc(event.window.data1, event.window.data2);
                break;
            case SDL_MOUSEBUTTONUP:
                SDL_CaptureMouse(SDL_FALSE);
                break;
            case SDL_MOUSEBUTTONDOWN:
                SDL_CaptureMouse(SDL_TRUE);
                mouseFunc(event.button.button, event.button.state,
                    event.button.x, event.button.y);
                break;
            case SDL_MOUSEMOTION:
                motionFunc(event.motion.state, event.motion.x,
                    event.motion.y);
                break;
            case SDL_QUIT:
                quit = true;
                break;
        }
    }

    return quit;
}

}

int
screen_width()
{
    return m_screen_width;
}

int
screen_height()
{
    return m_screen_height;
}

Camera*
camera()
{
    return m_camera;
}

void
set_camera(Camera& camera)
{
    m_camera = &camera;
}

TwBar*
twbar()
{
    return m_bar;
}

void
display_callback(std::function<void ()> display_callback)
{
    m_display_callback = display_callback;
}

void
timer_callback(std::function<void (unsigned int)> timer_callback,
    unsigned int timer_msec)
{
    m_timer_callback = timer_callback;
    m_timer_msec = timer_msec;
}

void
reshape_callback(std::function<void(int width, int height)> reshape_callback)
{
    m_reshape_callback = reshape_callback;
}

void
close_callback(std::function<void ()> close_callback)
{
    m_close_callback = close_callback;
}

void
cout_opengl_version()
{
    GLint context_major_version, context_minor_version, context_profile;

    glGetIntegerv(GL_MAJOR_VERSION, &context_major_version);
    glGetIntegerv(GL_MINOR_VERSION, &context_minor_version);
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &context_profile);

    std::cout << "  OpenGL version " << context_major_version << "."
        << context_minor_version << " ";

    switch (context_profile)
    {
        case GL_CONTEXT_CORE_PROFILE_BIT:
            std::cout << "core";
            break;

        case GL_CONTEXT_COMPATIBILITY_PROFILE_BIT:
            std::cout << "compatibility";
            break;
    }

    std::cout << " profile context." << std::endl;
}

void
cout_glew_version()
{
    std::cout << "  GLEW version " << glewGetString(GLEW_VERSION)
        << "." << std::endl;
}

void
init(int argc, char* argv[])
{
    // Initialize SDL.
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "Failed to initialize SDL Video:" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        std::exit(EXIT_FAILURE);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE);

    //SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
    //SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    //SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
    //SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    //SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    m_sdl_window = SDL_CreateWindow("GLviz",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_screen_width, m_screen_height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!m_sdl_window)
    {
        std::cerr << "Failed to create SDL window:" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        std::exit(EXIT_FAILURE);
    }

    m_gl_context = SDL_GL_CreateContext(m_sdl_window);
    if (!m_gl_context)
    {
        std::cerr << "Failed to initialize OpenGL:" << std::endl;
        std::cerr << "Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        std::exit(EXIT_FAILURE);
    }

    // Print OpenGL version.
    cout_opengl_version();

    // Initialize GLEW.
    {
        glewExperimental = GL_TRUE;
        GLenum error = glewInit();

        if (GLEW_OK != error)
        {
            std::cerr << "Failed to initialize GLEW:" << std::endl;
            std::cerr << "Error: " << glewGetErrorString(error) << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    // Print GLEW version.
    cout_glew_version();
    std::cout << std::endl;

    // Initialize AntTweakBar.
    {
        int tw_init_ok = TwInit(TW_OPENGL_CORE, NULL);

        if (!tw_init_ok)
        {
            std::cerr << TwGetLastError() << std::endl;
            exit(EXIT_FAILURE);
        }

        m_bar = TwNewBar("TweakBar");
        TwWindowSize(m_screen_width, m_screen_height);

        TwDefine(" GLOBAL help='some useful text' ");
        TwDefine(" TweakBar size='275 400' color='100 100 100' \
            refresh=0.01 ");
    }
}

int
exec(Camera& camera)
{
    m_camera = &camera;
    Uint32 last_time = 0;

    reshapeFunc(m_screen_width, m_screen_height);

    while (!process_events())
    {
        if (m_timer_callback)
        {
            const Uint32 time = SDL_GetTicks();
            const Uint32 delta_t_msec = time - last_time;

            if (delta_t_msec >= m_timer_msec)
            {
                last_time = time;
                m_timer_callback(delta_t_msec);
            }
        }

        if (m_display_callback)
        {
            m_display_callback();
        }

        TwDraw();
        SDL_GL_SwapWindow(m_sdl_window);
    }

    TwTerminate();
    if (m_close_callback)
    {
        m_close_callback();
    }

    SDL_GL_DeleteContext(m_gl_context);
    SDL_DestroyWindow(m_sdl_window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

}
