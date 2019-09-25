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

#define ATTR_POSITION 0
layout(location = ATTR_POSITION) in vec3 position;

#define ATTR_TEXTURE_UV 1
layout(location = ATTR_TEXTURE_UV)
    in vec2 texture_uv;

out block
{
    vec2 texture_uv;
}
Out;

void main()
{
    gl_Position = vec4(position, 1.0);
    Out.texture_uv = texture_uv;
}
