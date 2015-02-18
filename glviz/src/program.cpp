// This file is part of GLviz.
//
// Copyright(c) 2014, 2015 Sebastian Lipponer
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

#include "program.hpp"

#include "camera.hpp"

#include <Eigen/Core>
#include <iostream>
#include <cstdlib>

using namespace Eigen;

extern unsigned char const mesh3_vs_glsl[];
extern unsigned char const mesh3_gs_glsl[];
extern unsigned char const mesh3_fs_glsl[];

extern unsigned char const sphere_vs_glsl[];
extern unsigned char const sphere_fs_glsl[];

namespace GLviz
{

ProgramSphere::ProgramSphere()
{
    initialize_shader_obj();
    initialize_program_obj();
}

void
ProgramSphere::initialize_shader_obj()
{
    m_sphere_vs_obj.load_from_cstr(
        reinterpret_cast<char const*>(sphere_vs_glsl));
    m_sphere_fs_obj.load_from_cstr(
        reinterpret_cast<char const*>(sphere_fs_glsl));

    attach_shader(m_sphere_vs_obj);
    attach_shader(m_sphere_fs_obj);
}

void
ProgramSphere::initialize_program_obj()
{
    try
    {
        m_sphere_vs_obj.compile();
        m_sphere_fs_obj.compile();
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
        set_uniform_block_binding("Material", 1);
        set_uniform_block_binding("Sphere", 3);
    }
    catch (uniform_not_found_error const& e)
    {
        std::cerr << "Warning: Failed to set a uniform variable." << std::endl
            << e.what() << std::endl;
    }
}

void
UniformBufferWireframe::set_buffer_data(float const* color,
    int const* viewport)
{
    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 4 * sizeof(GLfloat), color);
    glBufferSubData(GL_UNIFORM_BUFFER, 4 * sizeof(GLfloat),
        2 * sizeof(GLint), viewport);
    unbind();
}

UniformBufferCamera::UniformBufferCamera()
    : glUniformBuffer(32 * sizeof(GLfloat))
{
}

void
UniformBufferCamera::set_buffer_data(Camera const& camera)
{
    Matrix4f const& modelview_matrix = camera.get_modelview_matrix();
    Matrix4f const& projection_matrix = camera.get_projection_matrix();

    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f),
        modelview_matrix.data());
    glBufferSubData(GL_UNIFORM_BUFFER, 16 * sizeof(GLfloat),
        sizeof(Matrix4f), projection_matrix.data());
    unbind();
}

void
UniformBufferMaterial::set_buffer_data(float const* mbuf)
{
    bind();
    glBufferData(GL_UNIFORM_BUFFER, 4 * sizeof(GLfloat), mbuf,
        GL_DYNAMIC_DRAW);
    unbind();
}

UniformBufferWireframe::UniformBufferWireframe()
    : glUniformBuffer(4 * sizeof(GLfloat) + 2 * sizeof(GLint))
{
}

UniformBufferSphere::UniformBufferSphere()
    : glUniformBuffer(2 * sizeof(GLfloat))
{
}

void
UniformBufferSphere::set_buffer_data(float radius, float projection)
{
    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLfloat), &radius);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat),
        sizeof(GLfloat), &projection);
    unbind();
}

ProgramMesh3::ProgramMesh3()
    : glProgram(), m_wireframe(false), m_smooth(false)
{
    initialize_shader_obj();
    initialize_program_obj();
}

void
ProgramMesh3::set_wireframe(bool enable)
{
    if (m_wireframe != enable)
    {
        m_wireframe = enable;
        initialize_program_obj();
    }
}

void
ProgramMesh3::set_smooth(bool enable)
{
    if (m_smooth != enable)
    {
        m_smooth = enable;
        initialize_program_obj();
    }
}

void
ProgramMesh3::initialize_shader_obj()
{
    m_mesh3_vs_obj.load_from_cstr(
        reinterpret_cast<char const*>(mesh3_vs_glsl));
    m_mesh3_gs_obj.load_from_cstr(
        reinterpret_cast<char const*>(mesh3_gs_glsl));
    m_mesh3_fs_obj.load_from_cstr(
        reinterpret_cast<char const*>(mesh3_fs_glsl));

    attach_shader(m_mesh3_vs_obj);
    attach_shader(m_mesh3_gs_obj);
    attach_shader(m_mesh3_fs_obj);
}

void
ProgramMesh3::initialize_program_obj()
{
    try
    {
        std::map<std::string, int> defines;

        defines.insert(std::make_pair("WIREFRAME", m_wireframe ? 1 : 0));
        defines.insert(std::make_pair("SMOOTH", m_smooth ? 1 : 0));

        m_mesh3_vs_obj.compile(defines);
        m_mesh3_gs_obj.compile(defines);
        m_mesh3_fs_obj.compile(defines);
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
        set_uniform_block_binding("Material", 1);
        
        if (m_wireframe)
        {
            set_uniform_block_binding("Wireframe", 2);
        }
    }
    catch (uniform_not_found_error const& e)
    {
        std::cerr << "Warning: Failed to set a uniform variable." << std::endl
            << e.what() << std::endl;
    }
}

}
