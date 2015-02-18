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

#include "shader.hpp"

#include <GL/glew.h>

#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <memory>
#include <iostream>

#include <Eigen/Dense>

using namespace Eigen;

glShader::glShader()
{
}

glShader::~glShader()
{
    glDeleteShader(m_shader_obj);
}

void
glShader::load_from_file(std::string const& filename)
{
    std::ifstream input(filename.c_str());

    if (input.fail())
        throw file_open_error("Could not open file " + filename);

    std::ostringstream output;
    output << input.rdbuf();

    input.close();

    m_source = output.str();
}

void
glShader::load_from_cstr(char const* source_cstr)
{
    m_source = std::string(source_cstr);
}

void
glShader::compile(std::map<std::string, int> const& define_list)
{
    // Configure source.
    std::string source = m_source;

    for (std::map<std::string, int>::const_iterator it = define_list.begin();
        it != define_list.end(); ++it)
    {
        std::ostringstream define;
        define << "#define " << it->first;

        std::size_t pos = source.find(define.str(), 0);
        
        if (pos != std::string::npos)
        {
            std::size_t len = source.find("\n", pos) - pos + 1;

            define << " " << it->second << "\n";
            source.replace(pos, len, define.str());
        }
    }

    // Compile configured source.
    const char* source_cstr = source.c_str();

    glShaderSource(m_shader_obj, 1, &source_cstr, NULL);
    glCompileShader(m_shader_obj);

    if (!is_compiled())
    {
        throw shader_compilation_error(infolog());
    }
}

bool
glShader::is_compiled() const
{
    GLint status;
    glGetShaderiv(m_shader_obj, GL_COMPILE_STATUS, &status);

    return (status == GL_TRUE);
}

std::string
glShader::infolog()
{
    GLint infoLogLength = 0;
    glGetShaderiv(m_shader_obj, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLsizei logLength;

    std::unique_ptr<GLchar> infoLog(new GLchar[infoLogLength]);
    glGetShaderInfoLog(m_shader_obj, infoLogLength, &logLength, infoLog.get());

    return std::string(infoLog.get());
}

glVertexShader::glVertexShader()
{
    m_shader_obj = glCreateShader(GL_VERTEX_SHADER);
}

glFragmentShader::glFragmentShader()
{
    m_shader_obj = glCreateShader(GL_FRAGMENT_SHADER);
}

glGeometryShader::glGeometryShader()
{
    m_shader_obj = glCreateShader(GL_GEOMETRY_SHADER);
}

glProgram::glProgram()
    : m_program_obj(glCreateProgram())
{
}

glProgram::~glProgram()
{
    detach_all();
    glDeleteProgram(m_program_obj);
}

void
glProgram::use() const
{
    glUseProgram(m_program_obj);
}

void
glProgram::unuse() const
{
    glUseProgram(0);
}

void
glProgram::link()
{
    glLinkProgram(m_program_obj);

    if (!is_linked())
        throw shader_link_error(infolog());
}

void
glProgram::attach_shader(glShader& shader)
{
    glAttachShader(m_program_obj, shader.m_shader_obj);
}

void
glProgram::detach_shader(glShader& shader)
{
    glDetachShader(m_program_obj, shader.m_shader_obj);
}

void
glProgram::detach_all()
{
    GLsizei count;
    GLuint shader[64];

    glGetAttachedShaders(m_program_obj, 64, &count, shader);
    
    for (GLsizei i(0); i < count; ++i)
    {
        glDetachShader(m_program_obj, shader[i]);
    }
}

bool
glProgram::is_linked()
{
    GLint status;
    glGetProgramiv(m_program_obj, GL_LINK_STATUS, &status);

    return (status == GL_TRUE);
}

bool
glProgram::is_attached(glShader const& shader)
{
    GLint number_shader_attached;
    glGetProgramiv(m_program_obj, GL_ATTACHED_SHADERS,
        &number_shader_attached);

    std::unique_ptr<GLuint> shader_list(
        new GLuint[number_shader_attached]);

    GLsizei count;
    glGetAttachedShaders(m_program_obj,
        static_cast<GLsizei>(number_shader_attached), &count,
        shader_list.get());

    for (unsigned int i = 0; i < static_cast<GLuint>(count); ++i)
    if ((shader_list.get())[i] == shader.m_shader_obj)
        return true;

    return false;
}

std::string
glProgram::infolog()
{
    GLint infoLogLength = 0;
    glGetProgramiv(m_program_obj, GL_INFO_LOG_LENGTH, &infoLogLength);

    GLsizei logLength;

    std::unique_ptr<GLchar> infoLog(new GLchar[infoLogLength]);
    glGetProgramInfoLog(m_program_obj, infoLogLength, &logLength,
        infoLog.get());

    return std::string(infoLog.get());
}

void
glProgram::set_uniform_1i(GLchar const* name, GLint value)
{
    GLint location = glGetUniformLocation(m_program_obj, name);
    if (location == -1)
    {
        throw uniform_not_found_error(name);
    }

    glUniform1i(location, value);
}

void
glProgram::set_uniform_block_binding(GLchar const* name, GLuint block_binding)
{
    GLuint block_index = glGetUniformBlockIndex(m_program_obj, name);
    if (block_index == GL_INVALID_INDEX)
    {
        throw uniform_not_found_error(name);
    }

    glUniformBlockBinding(m_program_obj, block_index, block_binding);
}
