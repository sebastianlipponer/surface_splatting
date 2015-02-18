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

layout(std140) uniform Material
{
    vec3 color;
    float shininess;
}
material;

in block
{
    flat vec3 center_eye;
}
In;

#define FRAG_COLOR 0
layout(location = FRAG_COLOR, index = 0) out vec4 frag_color;

void main()
{
    vec2 p = gl_PointCoord * 2.0 - vec2(1.0);
    float z2 = 1.0 - p.x * p.x - p.y * p.y;

    if (z2 < 0.0)
        discard;

    float z = sqrt(z2);
    vec3 normal_eye = vec3(p.x, p.y, z);

    vec3 position_eye = In.center_eye + vec3(0.0, 0.0, sphere_radius * z);

    float depth = -projection_matrix[3][2] * (1.0 / position_eye.z) -
        projection_matrix[2][2];

    gl_FragDepth = (depth + 1.0) / 2.0;

    const vec3 light_eye = vec3(0.0, 0.0, 1.0);
    
    float dif = max(dot(light_eye, normal_eye), 0.0);
    vec3 view_eye = normalize(position_eye);
    vec3 refl_eye = reflect(light_eye, normal_eye);

    float spe = pow(clamp(dot(refl_eye, view_eye), 0.0, 1.0),
        material.shininess);
    float rim = pow(1.0 + dot(normal_eye, view_eye), 3.0);

    vec3 color = 0.15 * material.color;
    color += 0.6 * dif * material.color;
    color += 0.1 * spe * vec3(1.0);
    color += 0.1 * rim * vec3(1.0);

    // Gamma correction. Assuming gamma of 2.0 rather than 2.2.
    frag_color = vec4(sqrt(color), 1.0);
}
