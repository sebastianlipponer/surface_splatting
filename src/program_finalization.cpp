// This file is part of Surface Splatting.
//
// Copyright (C) 2010, 2015 by Sebastian Lipponer.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

#include "program_finalization.hpp"

#include <iostream>
#include <cstdlib>

extern unsigned char const finalization_vs_glsl[];
extern unsigned char const finalization_fs_glsl[];
extern unsigned char const lighting_glsl[];

ProgramFinalization::ProgramFinalization()
    : m_smooth(false), m_multisampling(false)
{
    initialize_shader_obj();
    initialize_program_obj();
}

void
ProgramFinalization::set_multisampling(bool enable)
{
    if (m_multisampling != enable)
    {
        m_multisampling = enable;
        initialize_program_obj();
    }
}

void
ProgramFinalization::set_smooth(bool enable)
{
    if (m_smooth != enable)
    {
        m_smooth = enable;
        initialize_program_obj();
    }
}

void
ProgramFinalization::initialize_shader_obj()
{
    m_finalization_vs_obj.load_from_cstr(
        reinterpret_cast<char const*>(finalization_vs_glsl));
    m_finalization_fs_obj.load_from_cstr(
        reinterpret_cast<char const*>(finalization_fs_glsl));
    m_lighting_fs_obj.load_from_cstr(
        reinterpret_cast<char const*>(lighting_glsl));

    attach_shader(m_finalization_vs_obj);
    attach_shader(m_finalization_fs_obj);
    attach_shader(m_lighting_fs_obj);
}

void
ProgramFinalization::initialize_program_obj()
{
    try
    {
        std::map<std::string, int> defines;
        defines.insert(std::make_pair("SMOOTH", m_smooth ? 1 : 0));
        defines.insert(std::make_pair("MULTISAMPLING",
            m_multisampling ? 1 : 0));

        m_finalization_vs_obj.compile(defines);
        m_finalization_fs_obj.compile(defines);
        m_lighting_fs_obj.compile(defines);
    }
    catch (shader_compilation_error const& e)
    {
        std::cerr << "Error: A shader failed to compile." << std::endl
            << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    try
    {
        link();
    }
    catch (shader_link_error const& e)
    {
        std::cerr << "Error: A program failed to link." << std::endl
            << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    try
    {
        set_uniform_block_binding("Camera", 0);
        set_uniform_block_binding("Raycast", 1);
        set_uniform_block_binding("Parameter", 3);
    }
    catch (uniform_not_found_error const& e)
    {
        std::cerr << "Warning: Failed to set a uniform variable." << std::endl
            << e.what() << std::endl;
    }
}
