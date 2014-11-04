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

#ifndef PROGRAM_MESH3_HPP
#define PROGRAM_MESH3_HPP

#include "shader.hpp"
#include "buffer.hpp"

namespace GLviz
{

class Camera;

class UniformBufferCamera : public glUniformBuffer
{

public:
    UniformBufferCamera();

    void set_buffer_data(Camera const& camera);
};

class UniformBufferMaterial : public glUniformBuffer
{

public:
    void set_buffer_data(const float* mbuf);
};

class UniformBufferWireframe : public glUniformBuffer
{

public:
    UniformBufferWireframe();

    void set_buffer_data(float const* color, int const* viewport);
};

class UniformBufferSphere : public glUniformBuffer
{

public:
    UniformBufferSphere();

    void set_buffer_data(float radius, float projection);
};

class ProgramMesh3 : public glProgram
{

public:
    ProgramMesh3();

    void set_wireframe(bool enable);
    void set_smooth(bool enable);

private:
    void initialize_shader_obj();
    void initialize_program_obj();

private:
    glVertexShader   m_mesh3_vs_obj;
    glGeometryShader m_mesh3_gs_obj;
    glFragmentShader m_mesh3_fs_obj;

    bool m_wireframe, m_smooth;
};

class ProgramSphere : public glProgram
{

public:
    ProgramSphere();

private:
    void initialize_shader_obj();
    void initialize_program_obj();

private:
    glVertexShader    m_sphere_vs_obj;
    glFragmentShader  m_sphere_fs_obj;
};

}

#endif // PROGRAM_MESH3_HPP
