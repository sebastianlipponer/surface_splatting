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
