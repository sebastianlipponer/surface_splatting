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

#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <GL/glew.h>

namespace GLviz
{

class glUniformBuffer
{

public:
    glUniformBuffer();
    glUniformBuffer(GLsizeiptr size);

    ~glUniformBuffer();

    void bind_buffer_base(GLuint index);

protected:
    void bind();
    void unbind();

private:
    GLuint m_uniform_buffer_obj;
};

class glVertexArray
{

public:
    glVertexArray();
    ~glVertexArray();

    void bind();
    void unbind();

private:
    GLuint m_vertex_array_obj;
};

class glArrayBuffer
{

public:
    glArrayBuffer();
    ~glArrayBuffer();

    void bind();
    void unbind();

    void set_buffer_data(GLsizeiptr size, GLvoid const* ptr);

private:
    GLuint m_array_buffer_obj;
};

class glElementArrayBuffer
{

public:
    glElementArrayBuffer();
    ~glElementArrayBuffer();

    void bind();
    void unbind();

    void set_buffer_data(GLsizeiptr size, GLvoid const* ptr);

private:
    GLuint m_element_array_buffer_obj;
};

}

#endif // BUFFER_HPP
