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

layout(std140, column_major) uniform Camera
{
    mat4 modelview_matrix;
    mat4 projection_matrix;
};

layout(std140) uniform Sphere
{
    float sphere_radius;
    float projection_radius;
};

#define ATTR_POSITION 0
layout(location = ATTR_POSITION) in vec3 center;

out block
{
    flat vec3 center_eye;
}
Out;

void main()
{
    vec4 center_eye = modelview_matrix * vec4(center, 1.0);
    gl_Position = projection_matrix * center_eye;

    Out.center_eye = vec3(center_eye);
    
    gl_PointSize = 2.0 * (sphere_radius
        / -center_eye.z) * projection_radius;
}
