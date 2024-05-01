// This file is part of Surface Splatting.
//
// Copyright (C) 2010, 2015 by Sebastian Lipponer.
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

#ifndef PROGRAM_FINALIZATION_HPP
#define PROGRAM_FINALIZATION_HPP

#include <GLviz/program.hpp>

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
