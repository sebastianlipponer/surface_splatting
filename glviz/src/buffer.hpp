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
