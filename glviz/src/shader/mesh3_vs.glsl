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
