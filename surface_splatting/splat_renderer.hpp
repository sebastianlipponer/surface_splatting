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

#ifndef SPLATRENDER_HPP
#define SPLATRENDER_HPP

#include "program_attribute.hpp"
#include "program_finalization.hpp"

#include <GLviz>

#include "framebuffer.hpp"

#include <Eigen/Core>
#include <string>
#include <vector>

struct Surfel
{
    Surfel() { }

    Surfel(Eigen::Vector3f c_, Eigen::Vector3f u_, Eigen::Vector3f v_,
           Eigen::Vector3f p_, unsigned int rgba_)
        : c(c_), u(u_), v(v_), p(p_), rgba(rgba_) { }

    Eigen::Vector3f c,      // Position of the ellipse center point.
                    u, v,   // Ellipse major and minor axis.
                    p;      // Clipping plane.

    unsigned int    rgba;   // Color.
};

class UniformBufferRaycast : public GLviz::glUniformBuffer
{

public:
    UniformBufferRaycast();

    void set_buffer_data(Eigen::Matrix4f const& projection_matrix_inv,
        GLint const* viewport);
};

class UniformBufferFrustum : public GLviz::glUniformBuffer
{

public:
    UniformBufferFrustum();

    void set_buffer_data(Eigen::Vector4f const* frustum_plane);
};

class UniformBufferParameter : public GLviz::glUniformBuffer
{

public:
    UniformBufferParameter();

    void set_buffer_data(Eigen::Vector3f const& color, float shininess,
        float radius_scale, float ewa_radius, float epsilon);
};

class SplatRenderer
{

public:
    SplatRenderer(GLviz::Camera const& camera);
    virtual ~SplatRenderer();

    void render_frame(std::vector<Surfel> const& visible_geometry);

    bool smooth() const;
    void set_smooth(bool enable = true);

    bool color_material() const;
    void set_color_material(bool enable = true);

    bool backface_culling() const;
    void set_backface_culling(bool enable = true);

    bool soft_zbuffer() const;
    void set_soft_zbuffer(bool enable = true);

    float soft_zbuffer_epsilon() const;
    void set_soft_zbuffer_epsilon(float epsilon);

    unsigned int pointsize_method() const;
    void set_pointsize_method(unsigned int pointsize_method);

    bool ewa_filter() const;
    void set_ewa_filter(bool enable = true);

    bool multisample() const;
    void set_multisample(bool enable = true);

    float const* material_color() const;
    void set_material_color(float const* color_ptr);
    float material_shininess() const;
    void set_material_shininess(float shininess);

    float radius_scale() const;
    void set_radius_scale(float radius_scale);

    float ewa_radius() const;
    void set_ewa_radius(float ewa_radius);

    void reshape(int width, int height);

private:
    void setup_program_objects();
    void setup_filter_kernel();
    void setup_screen_size_quad();
    void setup_vertex_array_buffer_object();

    void setup_uniforms(glProgram& program);

    void begin_frame();
    void end_frame();
    void render_pass(bool depth_only = false);

private:
    GLviz::Camera const& m_camera;

    GLuint m_rect_vertices_vbo, m_rect_texture_uv_vbo,
        m_rect_vao, m_filter_kernel;

    GLuint m_vbo, m_vao;
    unsigned int m_num_pts;

    ProgramAttribute m_visibility, m_attribute;
    ProgramFinalization m_finalization;

    Framebuffer m_fbo;

    bool m_soft_zbuffer, m_backface_culling, m_smooth,
        m_color_material, m_ewa_filter, m_multisample;
    unsigned int m_pointsize_method;
    Eigen::Vector3f m_color;
    float m_epsilon, m_shininess, m_radius_scale,
        m_ewa_radius;

    GLviz::UniformBufferCamera m_uniform_camera;
    UniformBufferRaycast m_uniform_raycast;
    UniformBufferFrustum m_uniform_frustum;
    UniformBufferParameter m_uniform_parameter;
};

#endif // SPLATRENDER_HPP
