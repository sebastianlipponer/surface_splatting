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

#include "splat_renderer.hpp"

#include <GLviz/glviz.hpp>
#include <GLviz/utility.hpp>

#include <iostream>
#include <cmath>

using namespace Eigen;

UniformBufferRaycast::UniformBufferRaycast()
    : glUniformBuffer(sizeof(Matrix4f) + sizeof(Vector4f))
{
}

void
UniformBufferRaycast::set_buffer_data(Matrix4f const&
    projection_matrix_inv, GLint const* viewport)
{
    float viewportf[4] = {
        static_cast<float>(viewport[0]),
        static_cast<float>(viewport[1]),
        static_cast<float>(viewport[2]),
        static_cast<float>(viewport[3])
    };

    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f),
        projection_matrix_inv.data());
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(Matrix4f),
        4 * sizeof(float), viewportf);
    unbind();
}

UniformBufferFrustum::UniformBufferFrustum()
    : glUniformBuffer(6 * sizeof(Vector4f))
{
}

void
UniformBufferFrustum::set_buffer_data(Vector4f const* frustum_plane)
{
    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 6 * sizeof(Vector4f),
        static_cast<void const*>(frustum_plane));
    unbind();
}

UniformBufferParameter::UniformBufferParameter()
    : glUniformBuffer(8 * sizeof(float))
{
}

void
UniformBufferParameter::set_buffer_data(Vector3f const& color, float shininess,
    float radius_scale, float ewa_radius, float epsilon)
{
    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, 0, 3 * sizeof(float), color.data());
    glBufferSubData(GL_UNIFORM_BUFFER, 12, sizeof(float), &shininess);
    glBufferSubData(GL_UNIFORM_BUFFER, 16, sizeof(float), &radius_scale);
    glBufferSubData(GL_UNIFORM_BUFFER, 20, sizeof(float), &ewa_radius);
    glBufferSubData(GL_UNIFORM_BUFFER, 24, sizeof(float), &epsilon);
    unbind();
}


SplatRenderer::SplatRenderer(GLviz::Camera const& camera)
    : m_camera(camera), m_soft_zbuffer(true), m_smooth(false),
      m_color_material(true), m_ewa_filter(false), m_multisample(false),
      m_pointsize_method(0), m_backface_culling(false),
      m_color(Vector3f(0.0, 0.25f, 1.0f)), m_epsilon(1.0f * 1e-3f),
      m_shininess(8.0f), m_radius_scale(1.0f), m_ewa_radius(1.0f)
{
    m_uniform_camera.bind_buffer_base(0);
    m_uniform_raycast.bind_buffer_base(1);
    m_uniform_frustum.bind_buffer_base(2);
    m_uniform_parameter.bind_buffer_base(3);

    setup_program_objects();
    setup_filter_kernel();
    setup_screen_size_quad();
    setup_vertex_array_buffer_object();
}

SplatRenderer::~SplatRenderer()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);

    glDeleteBuffers(1, &m_rect_vertices_vbo);
    glDeleteBuffers(1, &m_rect_texture_uv_vbo);
    glDeleteVertexArrays(1, &m_rect_vao);

    glDeleteTextures(1, &m_filter_kernel);
}

void
SplatRenderer::setup_program_objects()
{
    m_visibility.set_visibility_pass();
    m_visibility.set_pointsize_method(m_pointsize_method);
    m_visibility.set_backface_culling(m_backface_culling);

    m_attribute.set_visibility_pass(false);
    m_attribute.set_pointsize_method(m_pointsize_method);
    m_attribute.set_backface_culling(m_backface_culling);
    m_attribute.set_color_material(m_color_material);
    m_attribute.set_ewa_filter(m_ewa_filter);
    m_attribute.set_smooth(m_smooth);

    m_finalization.set_multisampling(m_multisample);
    m_finalization.set_smooth(m_smooth);
}

inline void
SplatRenderer::setup_filter_kernel()
{
    const float sigma2 = 0.316228f; // Sqrt(0.1).

    GLfloat yi[256];
    for (unsigned int i = 0; i < 256; ++i)
    {
        float x = static_cast<GLfloat>(i) / 255.0f;
        float const w = x * x / (2.0f * sigma2);
        yi[i] = std::exp(-w);
    }

    glGenTextures(1, &m_filter_kernel);
    glBindTexture(GL_TEXTURE_1D, m_filter_kernel);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 256, 0, GL_RED, GL_FLOAT, yi);
}

inline void
SplatRenderer::setup_screen_size_quad()
{
    float rect_vertices[12] = {
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f
    };

    float rect_texture_uv[8] = {
        1.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        0.0f, 0.0f
    };

    glGenBuffers(1, &m_rect_vertices_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_vertices_vbo);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), rect_vertices,
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &m_rect_texture_uv_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_rect_texture_uv_vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), rect_texture_uv,
        GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &m_rect_vao);
    glBindVertexArray(m_rect_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_rect_vertices_vbo);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        3 * sizeof(float), reinterpret_cast<const GLvoid*>(0));

    glBindBuffer(GL_ARRAY_BUFFER, m_rect_texture_uv_vbo);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
        2 * sizeof(float), reinterpret_cast<const GLvoid*>(0));

    glBindVertexArray(0);
}

void
SplatRenderer::setup_vertex_array_buffer_object()
{
    glGenBuffers(1, &m_vbo);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    // Center c.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Surfel), reinterpret_cast<const GLfloat*>(0));

    // Tagent vector u.
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Surfel), reinterpret_cast<const GLfloat*>(12));

    // Tangent vector v.
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
        sizeof(Surfel), reinterpret_cast<const GLfloat*>(24));

    // Clipping plane p.
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE,
        sizeof(Surfel), reinterpret_cast<const GLfloat*>(36));
    
    // Color rgba.
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE, GL_TRUE,
        sizeof(Surfel), reinterpret_cast<const GLbyte*>(48));

    glBindVertexArray(0);
}

bool
SplatRenderer::smooth() const
{
    return m_smooth;
}

void
SplatRenderer::set_smooth(bool enable)
{
    if (m_smooth != enable)
    {
        m_smooth = enable;

        m_attribute.set_smooth(enable);
        m_finalization.set_smooth(enable);

        if (m_smooth)
        {
            m_fbo.enable_depth_texture();
            m_fbo.attach_normal_texture();
        }
        else
        {
            m_fbo.disable_depth_texture();
            m_fbo.detach_normal_texture();
        }
    }
}

bool
SplatRenderer::color_material() const
{
    return m_color_material;
}

void
SplatRenderer::set_color_material(bool enable)
{
    if (m_color_material != enable)
    {
        m_color_material = enable;
        m_attribute.set_color_material(enable);
    }
}

bool
SplatRenderer::backface_culling() const
{
    return m_backface_culling;
}

void
SplatRenderer::set_backface_culling(bool enable)
{
    if (m_backface_culling != enable)
    {
        m_backface_culling = enable;
        m_visibility.set_backface_culling(enable);
        m_attribute.set_backface_culling(enable);
    }
}

bool
SplatRenderer::soft_zbuffer() const
{
    return m_soft_zbuffer;
}

void
SplatRenderer::set_soft_zbuffer(bool enable)
{
    if (m_soft_zbuffer != enable)
    {
        if (!enable)
        {
            m_ewa_filter = false;
            m_attribute.set_ewa_filter(false);
        }

        m_soft_zbuffer = enable;
    }
}

float
SplatRenderer::soft_zbuffer_epsilon() const
{
    return m_epsilon;
}

void
SplatRenderer::set_soft_zbuffer_epsilon(float epsilon)
{
    m_epsilon = epsilon;
}

unsigned int
SplatRenderer::pointsize_method() const
{
    return m_pointsize_method;
}

void
SplatRenderer::set_pointsize_method(unsigned int pointsize_method)
{
    if (m_pointsize_method != pointsize_method)
    {
        m_pointsize_method = pointsize_method;
        m_visibility.set_pointsize_method(pointsize_method);
        m_attribute.set_pointsize_method(pointsize_method);
    }
}

bool
SplatRenderer::ewa_filter() const
{
    return m_ewa_filter;
}

void
SplatRenderer::set_ewa_filter(bool enable)
{
    if (m_soft_zbuffer && m_ewa_filter != enable)
    {
        m_ewa_filter = enable;
        m_attribute.set_ewa_filter(enable);
    }
}

bool
SplatRenderer::multisample() const
{
    return m_multisample;
}

void
SplatRenderer::set_multisample(bool enable)
{
    if (m_multisample != enable)
    {
        m_multisample = enable;
        m_finalization.set_multisampling(enable);
        m_fbo.set_multisample(enable);
    }
}

float const*
SplatRenderer::material_color() const
{
    return m_color.data();
}

void
SplatRenderer::set_material_color(float const* color_ptr)
{
    Map<const Vector3f> color(color_ptr);
    m_color = color;
}

float
SplatRenderer::material_shininess() const
{
    return m_shininess;
}

void
SplatRenderer::set_material_shininess(float shininess)
{
    m_shininess = shininess;
}

float
SplatRenderer::radius_scale() const
{
    return m_radius_scale;
}

void
SplatRenderer::set_radius_scale(float radius_scale)
{
    m_radius_scale = radius_scale;
}

float
SplatRenderer::ewa_radius() const
{
    return m_ewa_radius;
}

void
SplatRenderer::set_ewa_radius(float ewa_radius)
{
    m_ewa_radius = ewa_radius;
}

void
SplatRenderer::reshape(int width, int height)
{
    m_fbo.reshape(width, height);
}

void
SplatRenderer::setup_uniforms(glProgram& program)
{
    m_uniform_camera.set_buffer_data(m_camera);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLviz::Frustum view_frustum = m_camera.get_frustum();
        
    m_uniform_raycast.set_buffer_data(
        m_camera.get_projection_matrix().inverse(),
        viewport);

    Vector4f frustum_plane[6];

    Matrix4f const& projection_matrix = m_camera.get_projection_matrix();
    for (unsigned int i(0); i < 6; ++i)
    {
        frustum_plane[i] = projection_matrix.row(3) + (-1.0f + 2.0f
            * static_cast<float>(i % 2)) * projection_matrix.row(i / 2);
    }
    
    for (unsigned int i(0); i < 6; ++i)
    {
        frustum_plane[i] = (1.0f / frustum_plane[i].block<3, 1>(
            0, 0).norm()) * frustum_plane[i];
    }

    m_uniform_frustum.set_buffer_data(frustum_plane);

    m_uniform_parameter.set_buffer_data(
        m_color, m_shininess, m_radius_scale, m_ewa_radius, m_epsilon
    );
}

void
SplatRenderer::render_pass(bool depth_only)
{ 
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    if (!depth_only && m_soft_zbuffer)
    {
        glEnable(GL_BLEND);
        glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE, GL_ONE, GL_ONE);
    }

    glProgram &program = depth_only ? m_visibility : m_attribute;

    program.use();

    if (depth_only)
    {
        glDepthMask(GL_TRUE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    }
    else
    {
        if (m_soft_zbuffer)
            glDepthMask(GL_FALSE);
        else
            glDepthMask(GL_TRUE);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    setup_uniforms(program);

    if (!depth_only && m_soft_zbuffer && m_ewa_filter)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_1D, m_filter_kernel);

        program.set_uniform_1i("filter_kernel", 1);
    }

    glBindVertexArray(m_vao);
    glDrawArrays(GL_POINTS, 0, m_num_pts);
    glBindVertexArray(0);

    program.unuse();

    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
}

void
SplatRenderer::begin_frame()
{
    m_fbo.bind();

    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void
SplatRenderer::end_frame()
{
    m_fbo.unbind();

    if (m_multisample)
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_fbo.color_texture());

        if (m_smooth)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_fbo.normal_texture());

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_fbo.depth_texture());
        }
    }
    else
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_fbo.color_texture());

        if (m_smooth)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_fbo.normal_texture());

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, m_fbo.depth_texture());
        }
    }

    m_finalization.use();
    
    try
    {
        setup_uniforms(m_finalization);
        m_finalization.set_uniform_1i("color_texture", 0);

        if (m_smooth)
        {
            m_finalization.set_uniform_1i("normal_texture", 1);
            m_finalization.set_uniform_1i("depth_texture", 2);
        }
    }
    catch (uniform_not_found_error const& e)
    {
        std::cerr << "Warning: Failed to set a uniform variable." << std::endl
            << e.what() << std::endl;
    }

    glBindVertexArray(m_rect_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void
SplatRenderer::render_frame(std::vector<Surfel> const& visible_geometry)
{
    begin_frame();

    m_num_pts = static_cast<unsigned int>(visible_geometry.size());

    if (m_num_pts > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Surfel) * m_num_pts,
            &visible_geometry.front(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (m_multisample)
        {
            glEnable(GL_MULTISAMPLE);
            glEnable(GL_SAMPLE_SHADING);
            glMinSampleShading(4.0);
        }

        if (m_soft_zbuffer)
        {
            render_pass(true);
        }

        render_pass(false);

        if (m_multisample)
        {
            glDisable(GL_MULTISAMPLE);
            glDisable(GL_SAMPLE_SHADING);
        }
    }

    end_frame();

#ifndef NDEBUG
    GLenum gl_error = glGetError();
    if (GL_NO_ERROR != gl_error)
    {
        std::cerr << __FILE__ << "(" << __LINE__ << "): "
            << GLviz::get_gl_error_string(gl_error) << std::endl;
    }
#endif
}
