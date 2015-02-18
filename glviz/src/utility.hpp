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

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <GL/glew.h>
#include <Eigen/Core>

#include <string>
#include <vector>
#include <array>

namespace GLviz
{

void load_raw(std::string const& filename, std::vector<Eigen::Vector3f>
    &vertices, std::vector<std::array<unsigned int, 3> >& faces);

void save_raw(std::string const& filename, std::vector<Eigen::Vector3f>
    const& vertices, std::vector<std::array<unsigned int, 3> >& faces);

void set_vertex_normals_from_triangle_mesh(std::vector<Eigen::Vector3f>
    const& vertices, std::vector<std::array<unsigned int, 3> > const& faces,
    std::vector<Eigen::Vector3f>& normals);

std::string get_gl_error_string(GLenum gl_error);
std::string get_gl_framebuffer_status_string(GLenum framebuffer_status);

}

#endif // UTILITY_HPP
