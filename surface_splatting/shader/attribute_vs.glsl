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

#define VISIBILITY_PASS    0
#define BACKFACE_CULLING   0
#define SMOOTH             0
#define COLOR_MATERIAL     0
#define EWA_FILTER         0
#define POINTSIZE_METHOD   0

layout(std140, column_major) uniform Camera
{
    mat4 modelview_matrix;
    mat4 projection_matrix;
};

layout(std140, column_major) uniform Raycast
{
    mat4 projection_matrix_inv;
    vec4 viewport;
};

layout(std140) uniform Frustum
{
    vec4 frustum_plane[6];
};

layout(std140) uniform Parameter
{
    vec3 material_color;
    float material_shininess;
    float radius_scale;
    float ewa_radius;
    float epsilon;
};

#define ATTR_CENTER 0
layout(location = ATTR_CENTER) in vec3 c;

#define ATTR_T1 1
layout(location = ATTR_T1) in vec3 u;

#define ATTR_T2 2
layout(location = ATTR_T2) in vec3 v;

#define ATTR_PLANE 3
layout(location = ATTR_PLANE) in vec3 p;

#define ATTR_COLOR 4
layout(location = ATTR_COLOR) in vec4 rgba;

out block
{
    flat out vec3 c_eye;
    flat out vec3 u_eye;
    flat out vec3 v_eye;
    flat out vec3 p;
    flat out vec3 n_eye;

    #if !VISIBILITY_PASS
        #if EWA_FILTER
            flat out vec2 c_scr;
        #endif
        flat out vec3 color;
    #endif
}
Out;

#if !VISIBILITY_PASS
    vec3 lighting(vec3 n_eye, vec3 v_eye, vec3 color, float shininess);
#endif

void
intersect(in vec4 v1, in vec4 v2, in int p,
          out int n_pts, out vec4[2] pts)
{
    int i = p / 2;
    float j = float(-1 + 2 * (p % 2));

    float b1 = v1.w + float(j) * v1[i];
    float b2 = v2.w + float(j) * v2[i];

    bool tb1 = b1 > 0.0;
    bool tb2 = b2 > 0.0;

    n_pts = 0;

    if (tb1 && tb2)
    {
        pts[0] = v2;
        n_pts = 1;
    }
    else
    if (tb1 && !tb2)
    {
        float a = b1 / (b1 - b2);
        pts[0] = (1.0 - a) * v1 + a * v2;
        n_pts = 1;
    }
    else
    if (!tb1 && tb2)
    {
        float a = b1 / (b1 - b2);
        pts[0] = (1.0 - a) * v1 + a * v2;
        pts[1] = v2;
        n_pts = 2;
    }
}

void clip_polygon(in vec4 p0[4], out int n_pts, out vec4 p1[8])
{
    vec4 p[8];
    int n = 4;

    p[0] = p0[0];
    p[1] = p0[1];
    p[2] = p0[2];
    p[3] = p0[3];

    for (int i = 0; i < 6; ++i)
    {
        int k = 0;

        for (int j = 0; j < n; ++j)
        {
            int n_pts;
            vec4 pts[2];

            intersect(p[j], p[(j + 1) % n], i, n_pts, pts);

            if (n_pts == 1)
            {
                p1[k++] = pts[0];
            }
            else
            if (n_pts == 2)
            {
                p1[k++] = pts[0];
                p1[k++] = pts[1];
            }
        }

        for (int j = 0; j < k; ++j)
        {
            p[j] = p1[j];
        }

        n = k;
    }

    n_pts = n;
}

void
conic_Q(in vec3 u, in vec3 v, in vec3 c,
        out mat3 Q1)
{
    mat3 Q0 = mat3(vec3(1.0, 0.0, 0.0),
        vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, -1.0));

    mat3 Sinv = transpose(mat3(cross(v, c),
        cross(u, c), cross(u, v)));
    Sinv[0][1] = -Sinv[0][1];
    Sinv[1][1] = -Sinv[1][1];
    Sinv[2][1] = -Sinv[2][1];
    
    mat3 Pinv = mat3(
        vec3(projection_matrix_inv[0]),
        vec3(projection_matrix_inv[1]),
        vec3(projection_matrix_inv[2]));
    Pinv[2][2] = -1.0;

    mat3 Minv = Sinv * Pinv;
    Q1 = transpose(Minv) * Q0 * Minv;
}

void
pointsprite(in vec3 c, in vec3 u, in vec3 v,
    out vec4 p_scr, out vec2 w)
{
#if POINTSIZE_METHOD == 0

    // This method obtains the position and bounds of a splat by
    // clipping and perspectively projecting a bounding polygon.
    mat4 M = projection_matrix * mat4(
        vec4(u, 0.0), vec4(v, 0.0), vec4(c, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0));

    vec4 p0[4];
    p0[0] = M * vec4(1.0);
    p0[1] = M * vec4(1.0, -1.0, 1.0, 1.0);
    p0[2] = M * vec4(-1.0, -1.0, 1.0, 1.0);
    p0[3] = M * vec4(-1.0, 1.0, 1.0, 1.0);

    int n_pts;
    vec4 p1[8];

    clip_polygon(p0, n_pts, p1);

    if (n_pts == 0)
    {
        p_scr = vec4(1.0, 0.0, 0.0, 0.0);
        w = vec2(0.0);
    }
    else
    {
        vec2 p_min = vec2(1.0);
        vec2 p_max = vec2(-1.0);

        for (int i = 0; i < n_pts; ++i)
        {
            vec2 p1i = p1[i].xy / p1[i].w;
            p_min = min(p_min, p1i);
            p_max = max(p_max, p1i);
        }

        w = 0.5 * (p_max - p_min);

        p_scr.xy = p_min + w;
        p_scr.z = 0.0;
        p_scr.w = 1.0;
    }

#elif POINTSIZE_METHOD == 1

    // BHZK05.
    p_scr = projection_matrix * vec4(c, 1.0);

    float p11 = projection_matrix[1][1];
    float r = max(length(u), length(v));

    w = vec2(0.0, r * p11 / abs(c.z));

#elif POINTSIZE_METHOD == 2

    // WHA+07.
    float r = max(length(u), length(v));
    vec3 pl = vec3(
        dot(frustum_plane[0], vec4(c, 1.0)),
        dot(frustum_plane[2], vec4(c, 1.0)),
        dot(frustum_plane[4], vec4(c, 1.0)));
    vec3 pr = vec3(
        dot(frustum_plane[1], vec4(c, 1.0)),
        dot(frustum_plane[3], vec4(c, 1.0)),
        dot(frustum_plane[5], vec4(c, 1.0)));

    bool t_lr = (pl.x + r) > 0.0 && (pr.x + r) > 0.0;
    bool t_bt = (pl.y + r) > 0.0 && (pr.y + r) > 0.0;
    bool t_nf = (pl.z + r) > 0.0 && (pr.z + r) > 0.0;

    if (t_lr && t_bt && t_nf)
    {
        mat4x3 T = transpose(projection_matrix * mat3x4(
            vec4(u, 0.0), vec4(v, 0.0), vec4(c, 1.0)
            ));

        float d = dot(vec3(1.0, 1.0, -1.0), T[3] * T[3]);
        vec3 f = (1.0 / d) * vec3(1.0, 1.0, -1.0);

        vec3 p = vec3(dot(f, T[0] * T[3]),
            dot(f, T[1] * T[3]), dot(f, T[2] * T[3]));

        vec3 h0 = p * p - vec3(dot(f, T[0] * T[0]),
            dot(f, T[1] * T[1]), dot(f, T[2] * T[2]));
        vec3 h = sqrt(max(vec3(0.0), h0)) + vec3(0.0, 0.0, 1e-2);

        w = h.xy;
        p_scr = vec4(p.xy, 0.0, 1.0);
    }
    else
    {
        p_scr = vec4(1.0, 0.0, 0.0, 0.0);
        w = vec2(0.0);
    }

#elif POINTSIZE_METHOD == 3

    // ZRB+04.
    mat3 Q;
    conic_Q(u, v, c, Q);

    float Qa = Q[0][0]; float Qb = Q[1][0];
    float Qc = Q[1][1]; float Qd = Q[2][0];
    float Qe = Q[2][1]; float Qf = -Q[2][2];

    float delta = Qa * Qc - Qb * Qb;

    if (delta > 0.0)
    {
        vec2 p = (Qb * vec2(Qe, Qd) - vec2(Qc * Qd, Qa * Qe)) / delta;
        float bb = Qf - dot(vec2(Qd, Qe), p);

        mat3 Q2 = mat3(
            vec3(Qa, Qb, 0.0),
            vec3(Qb, Qc, 0.0),
            vec3(0.0, 0.0, -bb)) / bb;

        float delta2 = Q2[0][0] * Q2[1][1] - Q2[0][1] * Q2[0][1];
        vec2 h = sqrt(vec2(Q2[1][1], Q2[0][0]) / delta2);

        // Put an upper bound on the point size since the
        // centralized conics method is numerically unstable.
        w = clamp(h, 0.0, 0.1);
        p_scr = vec4(p, 0.0, 1.0);
    }
    else
    {
        p_scr = vec4(1.0, 0.0, 0.0, 0.0);
        w = vec2(0.0);
    }
#endif
}

void main()
{
    vec4 c_eye = modelview_matrix * vec4(c, 1.0);
    vec3 u_eye = radius_scale * mat3(modelview_matrix) * u;
    vec3 v_eye = radius_scale * mat3(modelview_matrix) * v;
    vec3 n_eye = normalize(cross(u_eye, v_eye));

    vec4 p_scr;
    vec2 w;
    pointsprite(c_eye.xyz, u_eye, v_eye, p_scr, w);

#if !VISIBILITY_PASS
    #if SMOOTH
        #if COLOR_MATERIAL
            Out.color = material_color;
        #else
            Out.color = vec3(rgba);
        #endif
    #else
        #if COLOR_MATERIAL
            Out.color = lighting(n_eye, vec3(c_eye), material_color,
                                 material_shininess);
        #else
            Out.color = lighting(n_eye, vec3(c_eye), vec3(rgba),
                                 material_shininess);
        #endif
    #endif
#endif

#if BACKFACE_CULLING
    // Backface culling
    if (dot(n_eye, -vec3(c_eye)) > 0.0)
    {
#endif
        // Pointsprite position.
        gl_Position = p_scr;
        
        Out.c_eye = vec3(c_eye);
        Out.u_eye = u_eye;
        Out.v_eye = v_eye;
        Out.p = p;
        Out.n_eye = n_eye;

        // Pointsprite size. One additional pixel
        // avoids artifacts.
        float point_size = max(w[0] * viewport.z,
            w[1] * viewport.w) + 1.0;

#if !VISIBILITY_PASS && EWA_FILTER
        Out.c_scr = vec2((p_scr.xy + 1.0) * viewport.zw * 0.5);
        gl_PointSize = max(2.0, point_size);
#else
        gl_PointSize = point_size;
#endif

#if BACKFACE_CULLING
    }
    else
    {
        gl_Position = vec4(1.0, 0.0, 0.0, 0.0);
    }
#endif
}
