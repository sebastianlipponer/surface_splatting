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
