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
