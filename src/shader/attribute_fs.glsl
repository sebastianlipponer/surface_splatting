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

#version 330

#define VISIBILITY_PASS  0
#define SMOOTH           0
#define EWA_FILTER       0

layout(std140, column_major) uniform Camera
{
    mat4 modelview_matrix;
    mat4 modelview_matrix_it;
    mat4 projection_matrix;
};

layout(std140, column_major) uniform Raycast
{
    mat4 projection_matrix_inv;
    vec4 viewport;
};

layout(std140) uniform Parameter
{
    vec3 material_color;
    float material_shininess;
    float radius_scale;
    float ewa_radius;
    float epsilon;
};

uniform sampler1D filter_kernel;

in block
{
    flat in vec3 c_eye;
    flat in vec3 u_eye;
    flat in vec3 v_eye;
    flat in vec3 p;
    flat in vec3 n_eye;

    #if !VISIBILITY_PASS
        #if EWA_FILTER
            flat in vec2 c_scr;
        #endif
        flat in vec3 color;
    #endif
}
In;

#define FRAG_COLOR 0
layout(location = FRAG_COLOR) out vec4 frag_color;

#if !VISIBILITY_PASS
    #if SMOOTH
        #define FRAG_NORMAL 1
        layout(location = FRAG_NORMAL) out vec4 frag_normal;
    #endif
#endif

void main()
{
    vec4 p_ndc = vec4(2.0 * (gl_FragCoord.xy - viewport.xy)
        / (viewport.zw) - 1.0, -1.0, 1.0);
    vec4 p_eye = projection_matrix_inv * p_ndc;
    vec3 qn = p_eye.xyz / p_eye.w;

    vec3 q = qn * dot(In.c_eye, In.n_eye) / dot(qn, In.n_eye);
    vec3 d = q - In.c_eye;

    vec2 u = vec2(dot(In.u_eye, d) / dot(In.u_eye, In.u_eye),
                  dot(In.v_eye, d) / dot(In.v_eye, In.v_eye));

    if (dot(vec3(u, 1.0), In.p) < 0)
    {
        discard;
    }

    float w3d = length(u);
    float zval = q.z;

    #if !VISIBILITY_PASS && EWA_FILTER
        float w2d = distance(gl_FragCoord.xy, In.c_scr) / ewa_radius;
        float dist = min(w2d, w3d);

        // Avoid visual artifacts due to wrong z-values for fragments
        // being part of the low-pass filter, but outside of the
        // reconstruction filter.
        if (w3d > 1.0)
        {
            zval = In.c_eye.z;
        }
    #else
        float dist = w3d;
    #endif

    if (dist > 1.0)
    {
        discard;
    }

    #if !VISIBILITY_PASS
        #if EWA_FILTER
            float alpha = texture(filter_kernel, dist).r;
        #else
            float alpha = 1.0;
        #endif

        frag_color = vec4(In.color, alpha);

        #if SMOOTH
            frag_normal = vec4(In.n_eye, alpha);
        #endif
    #endif

    #if VISIBILITY_PASS
        zval -= epsilon;
    #endif

    float depth = -projection_matrix[3][2] * (1.0 / zval) -
        projection_matrix[2][2];

    gl_FragDepth = (depth + 1.0) / 2.0;
}
