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

#version 330

#define SMOOTH  0

layout(std140, column_major) uniform Camera
{
    mat4 modelview_matrix;
    mat4 projection_matrix;
};

#define ATTR_POSITION 0
layout(location = ATTR_POSITION) in vec3 position;

#if SMOOTH
    #define ATTR_NORMAL 1
    layout(location = ATTR_NORMAL) in vec3 normal;
#endif

out block
{
    #if SMOOTH
        vec3 normal;
    #endif
    vec3 position;
}
Out;

void main()
{
    vec4 position_eye = modelview_matrix * vec4(position, 1.0);

    #if SMOOTH
        Out.normal = mat3(modelview_matrix) * normal;
    #endif
    Out.position = vec3(position_eye);

    gl_Position = projection_matrix * position_eye;
}
