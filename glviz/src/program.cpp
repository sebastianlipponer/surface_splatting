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
    try
    {
        m_sphere_vs_obj.load_from_cstr(
            reinterpret_cast<char const*>(sphere_vs_glsl));
        m_sphere_fs_obj.load_from_cstr(
            reinterpret_cast<char const*>(sphere_fs_glsl));
    }
    catch (file_open_error const& e)
    {
        std::cerr << "Error: Failed to open shader file." << std::endl
            << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

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
    try
    {
        m_mesh3_vs_obj.load_from_cstr(
            reinterpret_cast<char const*>(mesh3_vs_glsl));
        m_mesh3_gs_obj.load_from_cstr(
            reinterpret_cast<char const*>(mesh3_gs_glsl));
        m_mesh3_fs_obj.load_from_cstr(
            reinterpret_cast<char const*>(mesh3_fs_glsl));
    }
    catch (file_open_error const& e)
    {
        std::cerr << "Error: Failed to open shader file." << std::endl
            << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    attach_shader(m_mesh3_vs_obj);
    attach_shader(m_mesh3_gs_obj);
    attach_shader(m_mesh3_fs_obj);
}

void
ProgramMesh3::initialize_program_obj()
{
    try
    {
        std::set<std::string> defines;

        if (m_wireframe)
        {
            defines.insert("WIREFRAME");
        }

        if (m_smooth)
        {
            defines.insert("SMOOTH");
        }

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
