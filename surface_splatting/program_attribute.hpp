// This file is part of Surface Splatting.
//
// Copyright (C) 2010, 2015 by Sebastian Lipponer.
// 
// Surface Splatting is free software: you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Surface Splatting is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Surface Splatting. If not, see <http://www.gnu.org/licenses/>.

#ifndef PROGRAM_RENDER_HPP
#define PROGRAM_RENDER_HPP

#include <GLviz>

class ProgramAttribute : public glProgram
{

public:
    ProgramAttribute();

    void set_ewa_filter(bool enable = true);
    void set_pointsize_method(unsigned int pointsize_method);
    void set_backface_culling(bool enable = true);
    void set_visibility_pass(bool enable = true);
    void set_smooth(bool enable = true);
    void set_color_material(bool enable = true);

private:
    void initialize_shader_obj();
    void initialize_program_obj();

private:
    glVertexShader m_attribute_vs_obj, m_lighting_vs_obj;
    glFragmentShader m_attribute_fs_obj;

    bool m_ewa_filter, m_backface_culling,
         m_visibility_pass, m_smooth, m_color_material;
    unsigned int m_pointsize_method;
};

#endif // PROGRAM_RENDER_HPP
