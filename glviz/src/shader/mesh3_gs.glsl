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

#ifdef WIREFRAME
    layout(std140) uniform Wireframe
    {
        vec3 color_wireframe;
        ivec2 viewport;
    };
#endif

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in block
{
    #ifdef SMOOTH
        vec3 normal;
    #endif
    vec3 position;
}
In[];

out block
{
    #ifdef SMOOTH
        vec3 normal;
    #else
        flat vec3 normal;
    #endif

    vec3 position;
    
    #ifdef WIREFRAME
        noperspective vec3 distance;
    #endif
}
Out;

void main()
{
    #ifdef WIREFRAME
        vec2 w0 = (1.0 / gl_in[0].gl_Position.w)
            * gl_in[0].gl_Position.xy * viewport.xy;
        vec2 w1 = (1.0 / gl_in[1].gl_Position.w)
            * gl_in[1].gl_Position.xy * viewport.xy;
        vec2 w2 = (1.0 / gl_in[2].gl_Position.w)
            * gl_in[2].gl_Position.xy * viewport.xy;

        mat3 matA = mat3(vec3(1.0, w0), vec3(1.0, w1), vec3(1.0, w2));
        float area = abs(determinant(matA));
    #endif

    #ifndef SMOOTH
        vec3 normal = normalize(cross(In[1].position - In[0].position, In[2].position - In[0].position));
    #endif
    
    gl_Position = gl_in[0].gl_Position;
    #ifdef SMOOTH
        Out.normal = In[0].normal;
    #else
        Out.normal = normal;
    #endif
    Out.position = In[0].position;
    #ifdef WIREFRAME
        Out.distance = vec3(area / length(w2 - w1), 0.0, 0.0);
    #endif
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
        #ifdef SMOOTH
        Out.normal = In[1].normal;
    #else
        Out.normal = normal;
    #endif
    Out.position = In[1].position;
    #ifdef WIREFRAME
        Out.distance = vec3(0.0, area / length(w2 - w0), 0.0);
    #endif
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    #ifdef SMOOTH
        Out.normal = In[2].normal;
    #else
        Out.normal = normal;
    #endif
    Out.position = In[2].position;
    #ifdef WIREFRAME
        Out.distance = vec3(0.0, 0.0, area / length(w1 - w0));
    #endif
    EmitVertex();
}
