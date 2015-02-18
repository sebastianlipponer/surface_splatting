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

#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <string>
#include <map>
#include <stdexcept>

struct file_open_error : public std::runtime_error
{
    file_open_error(const std::string& errmsg)
        : runtime_error(errmsg)
    {
    }
};

struct shader_compilation_error : public std::logic_error
{
    shader_compilation_error(const std::string& errmsg)
        : logic_error(errmsg)
    {
    }
};

struct shader_link_error : public std::logic_error
{
    shader_link_error(std::string const& errmsg)
        : logic_error(errmsg)
    {
    }
};

struct uniform_not_found_error : public std::logic_error
{
    uniform_not_found_error(std::string const& errmsg)
        : logic_error(errmsg)
    {
    }
};

class glShader
{

public:
    virtual ~glShader();

    void load_from_file(std::string const& filename);
    void load_from_cstr(char const* source_cstr);

    void compile(std::map<std::string, int> const&
        define_list = std::map<std::string, int>());
    bool is_compiled() const;

    std::string infolog();

protected:
    glShader();

protected:
    GLuint m_shader_obj;
    std::string m_source;

    friend class glProgram;
};

class glVertexShader : public glShader
{

public:
    glVertexShader();
};

class glFragmentShader : public glShader
{

public:
    glFragmentShader();
};

class glGeometryShader : public glShader
{

public:
    glGeometryShader();
};

class glProgram
{

public:
    glProgram();
    virtual ~glProgram();

    void use() const;
    void unuse() const;
    void link();
    
    void attach_shader(glShader& shader);
    void detach_shader(glShader& shader);

    void detach_all();

    bool is_linked();
    bool is_attached(glShader const& shader);
    std::string infolog();

    void set_uniform_1i(GLchar const* name, GLint value);
    void set_uniform_block_binding(GLchar const* name, GLuint block_binding);

protected:
    GLuint m_program_obj;
};

#endif // SHADER_HPP
