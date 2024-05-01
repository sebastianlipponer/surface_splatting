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

#include "program_attribute.hpp"

#include <iostream>
#include <cstddef>

extern unsigned char const attribute_vs_glsl[];
extern unsigned char const attribute_fs_glsl[];
extern unsigned char const lighting_glsl[];

ProgramAttribute::ProgramAttribute()
    : m_ewa_filter(false), m_backface_culling(false),
      m_visibility_pass(true), m_smooth(false), m_color_material(false),
      m_pointsize_method(0)
{
    initialize_shader_obj();
    initialize_program_obj();
}

void
ProgramAttribute::set_ewa_filter(bool enable)
{
    if (m_ewa_filter != enable)
    {
        m_ewa_filter = enable;
        initialize_program_obj();
    }
}

void
ProgramAttribute::set_pointsize_method(unsigned int pointsize_method)
{
    if (m_pointsize_method != pointsize_method)
    {
        m_pointsize_method = pointsize_method;
        initialize_program_obj();
    }
}

void
ProgramAttribute::set_backface_culling(bool enable)
{
    if (m_backface_culling != enable)
    {
        m_backface_culling = enable;
        initialize_program_obj();
    }
}

void
ProgramAttribute::set_visibility_pass(bool enable)
{
    if (m_visibility_pass != enable)
    {
        m_visibility_pass = enable;
        initialize_program_obj();
    }
}

void
ProgramAttribute::set_smooth(bool enable)
{
    if (m_smooth != enable)
    {
        m_smooth = enable;
        initialize_program_obj();
    }
}

void
ProgramAttribute::set_color_material(bool enable)
{
    if (m_color_material != enable)
    {
        m_color_material = enable;
        initialize_program_obj();
    }
}

void
ProgramAttribute::initialize_shader_obj()
{
    m_attribute_vs_obj.load_from_cstr(
        reinterpret_cast<char const*>(attribute_vs_glsl));
    m_lighting_vs_obj.load_from_cstr(
        reinterpret_cast<char const*>(lighting_glsl));

    m_attribute_fs_obj.load_from_cstr(
        reinterpret_cast<char const*>(attribute_fs_glsl));
}

void
ProgramAttribute::initialize_program_obj()
{
    try
    {
        detach_all();

        attach_shader(m_attribute_vs_obj);
        attach_shader(m_attribute_fs_obj);
        attach_shader(m_lighting_vs_obj);

        std::map<std::string, int> defines;
        
        defines.insert(std::make_pair("EWA_FILTER",
            m_ewa_filter ? 1 : 0));
        defines.insert(std::make_pair("POINTSIZE_METHOD",
            static_cast<int>(m_pointsize_method)));
        defines.insert(std::make_pair("BACKFACE_CULLING",
            m_backface_culling ? 1 : 0));
        defines.insert(std::make_pair("VISIBILITY_PASS",
            m_visibility_pass ? 1 : 0));
        defines.insert(std::make_pair("SMOOTH",
            m_smooth ? 1 : 0));
        defines.insert(std::make_pair("COLOR_MATERIAL",
            m_color_material ? 1 : 0));

        m_attribute_vs_obj.compile(defines);
        m_attribute_fs_obj.compile(defines);
        m_lighting_vs_obj.compile(defines);
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
        set_uniform_block_binding("Frustum", 2);
        set_uniform_block_binding("Parameter", 3);
    }
    catch (uniform_not_found_error const& e)
    {
        std::cerr << "Warning: Failed to set a uniform variable." << std::endl
            << e.what() << std::endl;
    }
}
