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

#ifndef PROGRAM_FINALIZATION_HPP
#define PROGRAM_FINALIZATION_HPP

#include <GLviz>

class ProgramFinalization : public glProgram
{

public:
    ProgramFinalization();

    void set_multisampling(bool enable);
    void set_smooth(bool enable);

private:
    void initialize_shader_obj();
    void initialize_program_obj();

private:
    glVertexShader    m_finalization_vs_obj;
    glFragmentShader  m_finalization_fs_obj, m_lighting_fs_obj;

    bool m_smooth, m_multisampling;
};

#endif // PROGRAM_FINALIZATION_HPP
