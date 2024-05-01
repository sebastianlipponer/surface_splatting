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

vec3 lighting(vec3 normal_eye, vec3 v_eye, vec3 color, float shininess)
{
    const vec3 light_eye = vec3(0.0, 0.0, 1.0);

    float dif = max(dot(light_eye, normal_eye), 0.0);
    vec3 refl_eye = reflect(light_eye, normal_eye);

    vec3 view_eye = normalize(v_eye);
    float spe = pow(clamp(dot(refl_eye, view_eye), 0.0, 1.0),
            shininess);
    float rim = pow(1.0 + dot(normal_eye, view_eye), 3.0);

    vec3 res = 0.15 * color;
    res += 0.6 * dif * color;
    res += 0.1 * spe * vec3(1.0);
    res += 0.1 * rim * vec3(1.0);

    return res;
}
