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

#include "utility.hpp"

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <Eigen/Dense>

namespace GLviz
{

void
load_raw(std::string const& filename, std::vector<Eigen::Vector3f>& vertices,
    std::vector<std::array<unsigned int, 3> >& faces)
{
    std::ifstream input(filename, std::ios::in | std::ios::binary);

    if (input.fail())
    {
        std::ostringstream error_message;
        error_message << "Error: Can not open "
            << filename << "." << std::endl;

        throw std::runtime_error(error_message.str().c_str());
    }

    unsigned int nv;
    input.read(reinterpret_cast<char*>(&nv), sizeof(unsigned int));
    vertices.resize(nv);

    for (unsigned int i(0); i < nv; ++i)
    {
        input.read(reinterpret_cast<char*>(vertices[i].data()),
            3 * sizeof(float));
    }

    unsigned int nf;
    input.read(reinterpret_cast<char*>(&nf), sizeof(unsigned int));
    faces.resize(nf);

    for (unsigned int i(0); i < nf; ++i)
    {
        input.read(reinterpret_cast<char*>(faces[i].data()),
            3 * sizeof(unsigned int));
    }

    input.close();
}

void
save_raw(std::string const& filename, std::vector<Eigen::Vector3f>
    const& vertices, std::vector<std::array<unsigned int, 3> >& faces)
{
    std::ofstream output(filename, std::ios::out | std::ios::binary);

    if (output.fail())
    {
        std::ostringstream error_message;
        error_message << "Error: Can not open "
            << filename << "." << std::endl;

        throw std::runtime_error(error_message.str().c_str());
    }

    unsigned int nv = static_cast<unsigned int>(vertices.size());
    output.write(reinterpret_cast<char const*>(&nv), sizeof(unsigned int));

    for (unsigned int i(0); i < nv; ++i)
    {
        output.write(reinterpret_cast<char const*>(vertices[i].data()),
            3 * sizeof(float));
    }

    unsigned int nf = static_cast<unsigned int>(faces.size());
    output.write(reinterpret_cast<char const*>(&nf), sizeof(unsigned int));

    for (unsigned int i(0); i < nf; ++i)
    {
        output.write(reinterpret_cast<char const*>(faces[i].data()),
            3 * sizeof(unsigned int));
    }

    output.close();
}

void
set_vertex_normals_from_triangle_mesh(std::vector<Eigen::Vector3f>
    const& vertices, std::vector<std::array<unsigned int, 3> > const& faces,
    std::vector<Eigen::Vector3f>& normals)
{
    unsigned int nf(static_cast<unsigned int>(faces.size())),
        nv(static_cast<unsigned int>(vertices.size()));

    normals.resize(vertices.size());
    std::fill(normals.begin(), normals.end(), Eigen::Vector3f::Zero());

    for (unsigned int i(0); i < nf; ++i)
    {
        std::array<unsigned int, 3> const& f_i = faces[i];

        Eigen::Vector3f const& p0(vertices[f_i[0]]);
        Eigen::Vector3f const& p1(vertices[f_i[1]]);
        Eigen::Vector3f const& p2(vertices[f_i[2]]);

        Eigen::Vector3f n_i = (p0 - p1).cross(p0 - p2);

        normals[f_i[0]] += n_i;
        normals[f_i[1]] += n_i;
        normals[f_i[2]] += n_i;
    }

    for (unsigned int i(0); i < nv; ++i)
    {
        if (!normals[i].isZero())
        {
            normals[i].normalize();
        }
    }
}

std::string
get_gl_error_string(GLenum gl_error)
{
    std::string error_string;

    switch (gl_error)
    {
    case GL_NO_ERROR:
        error_string = "GL_NO_ERROR";
        break;
    case GL_INVALID_ENUM:
        error_string = "GL_INVALID_ENUM";
        break;
    case GL_INVALID_VALUE:
        error_string = "GL_INVALID_VALUE";
        break;
    case GL_INVALID_OPERATION:
        error_string = "GL_INVALID_OPERATION";
        break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        error_string = "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
    case GL_OUT_OF_MEMORY:
        error_string = "GL_OUT_OF_MEMORY";
        break;
    default:
        error_string = "UNKNOWN";
    }

    return error_string;
}

std::string
get_gl_framebuffer_status_string(GLenum framebuffer_status)
{
    std::string status_string;

    switch (framebuffer_status)
    {
        case GL_FRAMEBUFFER_COMPLETE:
            status_string = "GL_FRAMEBUFFER_COMPLETE";
            break;

        case GL_FRAMEBUFFER_UNDEFINED:
            status_string = "GL_FRAMEBUFFER_UNDEFINED";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
            break;

        case GL_FRAMEBUFFER_UNSUPPORTED:
            status_string = "GL_FRAMEBUFFER_UNSUPPORTED";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
            break;

        case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
            status_string = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
            break;

        default:
            status_string = "UNKNOWN";
    }

    return status_string;
}

}
