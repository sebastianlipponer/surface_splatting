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

#include "buffer.hpp"

#include <cassert>

namespace GLviz
{

glUniformBuffer::glUniformBuffer()
{
    glGenBuffers(1, &m_uniform_buffer_obj);
}

glUniformBuffer::glUniformBuffer(GLsizeiptr size)
    : glUniformBuffer()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_buffer_obj);
    glBufferData(GL_UNIFORM_BUFFER, size,
        reinterpret_cast<GLfloat*>(0), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glUniformBuffer::~glUniformBuffer()
{
    glDeleteBuffers(1, &m_uniform_buffer_obj);
}

void
glUniformBuffer::bind_buffer_base(GLuint index)
{
    glBindBufferBase(GL_UNIFORM_BUFFER, index, m_uniform_buffer_obj);
}

void
glUniformBuffer::bind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_uniform_buffer_obj);
}

void
glUniformBuffer::unbind()
{
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glVertexArray::glVertexArray()
{
    glGenVertexArrays(1, &m_vertex_array_obj);
}

glVertexArray::~glVertexArray()
{
    glDeleteBuffers(1, &m_vertex_array_obj);
}

void
glVertexArray::bind()
{
    glBindVertexArray(m_vertex_array_obj);
}

void
glVertexArray::unbind()
{
    glBindVertexArray(0);
}

glArrayBuffer::glArrayBuffer()
{
    glGenBuffers(1, &m_array_buffer_obj);
}

glArrayBuffer::~glArrayBuffer()
{
    glDeleteBuffers(1, &m_array_buffer_obj);
}

void
glArrayBuffer::bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_array_buffer_obj);
}

void
glArrayBuffer::unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
glArrayBuffer::set_buffer_data(GLsizeiptr size, GLvoid const* ptr)
{
    bind();

    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, ptr);

    unbind();
}

glElementArrayBuffer::glElementArrayBuffer()
{
    glGenBuffers(1, &m_element_array_buffer_obj);
}

glElementArrayBuffer::~glElementArrayBuffer()
{
    glDeleteBuffers(1, &m_element_array_buffer_obj);
}

void
glElementArrayBuffer::bind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_element_array_buffer_obj);
}

void
glElementArrayBuffer::unbind()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
glElementArrayBuffer::set_buffer_data(GLsizeiptr size, GLvoid const* ptr)
{
    bind();

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, ptr);

    unbind();
}

}
