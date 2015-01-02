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

#include <GLviz>

#include "splat_renderer.hpp"

#include "config.hpp"

#include <Eigen/Core>

#include <iostream>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <exception>

using namespace Eigen;

namespace
{

GLviz::Camera camera;
unsigned int g_model(1);
std::unique_ptr<SplatRenderer> viz;

std::vector<Eigen::Vector3f>               m_ref_vertices;
std::vector<Eigen::Vector3f>               m_ref_normals;

std::vector<Eigen::Vector3f>               m_vertices;
std::vector<std::array<unsigned int, 3> >  m_faces;
std::vector<Eigen::Vector3f>               m_normals;

std::vector<Surfel>  m_surfels;

void load_triangle_mesh(std::string const& filename);

void
displayFunc()
{
    viz->render_frame(m_surfels);
}

void
reshapeFunc(int width, int height)
{
    const float aspect = static_cast<float>(width) /
        static_cast<float>(height);

    glViewport(0, 0, width, height);
    camera.set_perspective(60.0f, aspect, 0.005f, 5.0f);

    viz->reshape(width, height);
}

void
closeFunc()
{
    viz = nullptr;
}

void
load_triangle_mesh(std::string const& filename)
{
    std::cout << "\nRead " << filename << "." << std::endl;
    std::ifstream input(filename);

    if (input.good())
    {
        input.close();
        GLviz::load_raw(filename, m_vertices, m_faces);
    }
    else
    {
        input.close();

        std::ostringstream fqfn;
        fqfn << path_resources;
        fqfn << filename;
        GLviz::load_raw(fqfn.str(), m_vertices, m_faces);
    }

    std::cout << "  #vertices " << m_vertices.size() << std::endl;
    std::cout << "  #faces    " << m_faces.size() << std::endl;

    GLviz::set_vertex_normals_from_triangle_mesh(
        m_vertices, m_faces, m_normals);

    m_ref_vertices = m_vertices;
    m_ref_normals = m_normals;
}

void
steiner_circumellipse(float const* v0_ptr, float const* v1_ptr,
    float const* v2_ptr, float* p0_ptr, float* t1_ptr, float* t2_ptr)
{
    Matrix2f Q;
    Vector3f d0, d1, d2;
    {
        Map<const Vector3f> v[3] = { v0_ptr, v1_ptr, v2_ptr };

        d0 = v[1] - v[0];
        d0.normalize();

        d1 = v[2] - v[0];
        d1 = d1 - d0 * d0.dot(d1);
        d1.normalize();

        d2 = (1.0f / 3.0f) * (v[0] + v[1] + v[2]);

        Vector2f p[3];
        for (unsigned int j(0); j < 3; ++j)
        {
            p[j] = Vector2f(
                d0.dot(v[j] - d2),
                d1.dot(v[j] - d2)
                );
        }

        Matrix3f A;
        for (unsigned int j(0); j < 3; ++j)
        {
            A.row(j) = Vector3f(
                p[j].x() * p[j].x(),
                2.0f * p[j].x() * p[j].y(),
                p[j].y() * p[j].y()
                );
        }

        FullPivLU<Matrix3f> lu(A);
        Vector3f res = lu.solve(Vector3f::Ones());

        Q(0, 0) = res(0);
        Q(1, 1) = res(2);
        Q(0, 1) = Q(1, 0) = res(1);
    }

    Map<Vector3f> p0(p0_ptr), t1(t1_ptr), t2(t2_ptr);
    {
        SelfAdjointEigenSolver<Matrix2f> es;
        es.compute(Q);

        Vector2f const& l = es.eigenvalues();
        Vector2f const& e0 = es.eigenvectors().col(0);
        Vector2f const& e1 = es.eigenvectors().col(1);

        p0 = d2;
        t1 = (1.0f / std::sqrt(l.x())) * (d0 * e0.x() + d1 * e0.y());
        t2 = (1.0f / std::sqrt(l.y())) * (d0 * e1.x() + d1 * e1.y());
    }
}

void
load_plane(unsigned int n)
{
    const float d = 1.0f / static_cast<float>(2 * n);

    Surfel s(Vector3f::Zero(),
             2.0f * d * Vector3f::UnitX(),
             2.0f * d * Vector3f::UnitY(),
             Vector3f::Zero(),
             0);

    m_surfels.resize(4 * n * n);
    unsigned int m(0);

    for (unsigned int i(0); i <= 2 * n; ++i)
    {
        for (unsigned int j(0); j <= 2 * n; ++j)
        {
            unsigned int k(i * (2 * n + 1) + j);

            if (k % 2 == 1)
            {
                s.c = Vector3f(
                    -1.0f + 2.0f * d * static_cast<float>(j),
                    -1.0f + 2.0f * d * static_cast<float>(i),
                    0.0f);
                s.rgba = (((j / 2) % 2) == ((i / 2) % 2)) ? 0u : ~0u;
                m_surfels[m] = s;

                // Clip border surfels.
                if (j == 2 * n)
                {
                    m_surfels[m].p = Vector3f(-1.0f, 0.0f, 0.0f);
                    m_surfels[m].rgba = ~s.rgba;
                }
                else if (i == 2 * n)
                {
                    m_surfels[m].p = Vector3f(0.0f, -1.0f, 0.0f);
                    m_surfels[m].rgba = ~s.rgba;
                }
                else if (j == 0)
                {
                    m_surfels[m].p = Vector3f(1.0f, 0.0f, 0.0f);
                }
                else if (i == 0)
                {
                    m_surfels[m].p = Vector3f(0.0f, 1.0f, 0.0f);
                }
                else
                {
                    // Duplicate and clip inner surfels.
                    if (j % 2 == 0)
                    {
                        m_surfels[m].p = Vector3f(1.0, 0.0f, 0.0f);

                        m_surfels[++m] = s;
                        m_surfels[m].p = Vector3f(-1.0, 0.0f, 0.0f);
                        m_surfels[m].rgba = ~s.rgba;
                    }

                    if (i % 2 == 0)
                    {
                        m_surfels[m].p = Vector3f(0.0, 1.0f, 0.0f);

                        m_surfels[++m] = s;
                        m_surfels[m].p = Vector3f(0.0, -1.0f, 0.0f);
                        m_surfels[m].rgba = ~s.rgba;
                    }
                }

                ++m;
            }
        }
    }
}

void
load_cube()
{
    Surfel cube[24];
    unsigned int color = 0;

    // Front.
    cube[0].c  = Vector3f(-0.5f, 0.0f, 0.5f);
    cube[0].u = 0.5f * Vector3f::UnitX();
    cube[0].v = 0.5f * Vector3f::UnitY();
    cube[0].p = Vector3f(1.0f, 0.0f, 0.0f);
    cube[0].rgba  = color;

    cube[1]   = cube[0];
    cube[1].c = Vector3f(0.5f, 0.0f, 0.5f);
    cube[1].p = Vector3f(-1.0f, 0.0f, 0.0f);
    
    cube[2]   = cube[0];
    cube[2].c = Vector3f(0.0f, 0.5f, 0.5f);
    cube[2].p = Vector3f(0.0f, -1.0f, 0.0f);
    
    cube[3]   = cube[0];
    cube[3].c = Vector3f(0.0f, -0.5f, 0.5f);
    cube[3].p = Vector3f(0.0f, 1.0f, 0.0f);

    // Back.
    cube[4].c = Vector3f(-0.5f, 0.0f, -0.5f);
    cube[4].u = 0.5f * Vector3f::UnitX();
    cube[4].v = -0.5f * Vector3f::UnitY();
    cube[4].p = Vector3f(1.0f, 0.0f, 0.0f);
    cube[4].rgba = color;

    cube[5] = cube[4];
    cube[5].c = Vector3f(0.5f, 0.0f, -0.5f);
    cube[5].p = Vector3f(-1.0f, 0.0f, 0.0f);

    cube[6] = cube[4];
    cube[6].c = Vector3f(0.0f, 0.5f, -0.5f);
    cube[6].p = Vector3f(0.0f, 1.0f, 0.0f);

    cube[7] = cube[4];
    cube[7].c = Vector3f(0.0f, -0.5f, -0.5f);
    cube[7].p = Vector3f(0.0f, -1.0f, 0.0f);

    // Top.
    cube[8].c = Vector3f(-0.5f, 0.5f, 0.0f);
    cube[8].u = 0.5f * Vector3f::UnitX();
    cube[8].v = -0.5f * Vector3f::UnitZ();
    cube[8].p = Vector3f(1.0f, 0.0f, 0.0f);
    cube[8].rgba = color;

    cube[9]    = cube[8];
    cube[9].c  = Vector3f(0.5f, 0.5f, 0.0f);
    cube[9].p = Vector3f(-1.0f, 0.0f, 0.0f);

    cube[10]    = cube[8];
    cube[10].c  = Vector3f(0.0f, 0.5f, 0.5f);
    cube[10].p = Vector3f(0.0f, 1.0f, 0.0f);

    cube[11] = cube[8];
    cube[11].c = Vector3f(0.0f, 0.5f, -0.5f);
    cube[11].p = Vector3f(0.0f, -1.0f, 0.0f);

    // Bottom.
    cube[12].c = Vector3f(-0.5f, -0.5f, 0.0f);
    cube[12].u = 0.5f * Vector3f::UnitX();
    cube[12].v = 0.5f * Vector3f::UnitZ();
    cube[12].p = Vector3f(1.0f, 0.0f, 0.0f);
    cube[12].rgba = color;

    cube[13] = cube[12];
    cube[13].c = Vector3f(0.5f, -0.5f, 0.0f);
    cube[13].p = Vector3f(-1.0f, 0.0f, 0.0f);

    cube[14] = cube[12];
    cube[14].c = Vector3f(0.0f, -0.5f, 0.5f);
    cube[14].p = Vector3f(0.0f, -1.0f, 0.0f);

    cube[15] = cube[12];
    cube[15].c = Vector3f(0.0f, -0.5f, -0.5f);
    cube[15].p = Vector3f(0.0f, 1.0f, 0.0f);

    // Left.
    cube[16].c = Vector3f(-0.5f, -0.5f, 0.0f);
    cube[16].u = 0.5f * Vector3f::UnitY();
    cube[16].v = -0.5f * Vector3f::UnitZ();
    cube[16].p = Vector3f(1.0f, 0.0f, 0.0f);
    cube[16].rgba = color;

    cube[17] = cube[16];
    cube[17].c = Vector3f(-0.5f, 0.5f, 0.0f);
    cube[17].p = Vector3f(-1.0f, 0.0f, 0.0f);

    cube[18] = cube[16];
    cube[18].c = Vector3f(-0.5f, 0.0f, 0.5f);
    cube[18].p = Vector3f(0.0f, 1.0f, 0.0f);

    cube[19] = cube[16];
    cube[19].c = Vector3f(-0.5f, 0.0f, -0.5f);
    cube[19].p = Vector3f(0.0f, -1.0f, 0.0f);

    // Right.
    cube[20].c = Vector3f(0.5f, -0.5f, 0.0f);
    cube[20].u = 0.5f * Vector3f::UnitY();
    cube[20].v = 0.5f * Vector3f::UnitZ();
    cube[20].p = Vector3f(1.0f, 0.0f, 0.0f);
    cube[20].rgba = color;

    cube[21] = cube[20];
    cube[21].c = Vector3f(0.5f, 0.5f, 0.0f);
    cube[21].p = Vector3f(-1.0f, 0.0f, 0.0f);

    cube[22] = cube[20];
    cube[22].c = Vector3f(0.5f, 0.0f, 0.5f);
    cube[22].p = Vector3f(0.0f, -1.0f, 0.0f);

    cube[23] = cube[20];
    cube[23].c = Vector3f(0.5f, 0.0f, -0.5f);
    cube[23].p = Vector3f(0.0f, 1.0f, 0.0f);

    m_surfels = std::vector<Surfel>(cube, cube + 24);
}

void
hsv2rgb(float h, float s, float v, float& r, float& g, float& b)
{
    float h_i = std::floor(h / 60.0f);
    float f = h / 60.0f - h_i;

    float p = v * (1.0f - s);
    float q = v * (1.0f - s * f);
    float t = v * (1.0f - s * (1.0f - f));

    switch (static_cast<int>(h_i))
    {
        case 1:
            r = q; g = v; b = p;
            break;
        case 2:
            r = p; g = v; b = t;
            break;
        case 3:
            r = p; g = q; b = v;
            break;
        case 4:
            r = t; g = p; b = v;
            break;
        case 5:
            r = v; g = p; b = q;
            break;
        default:
            r = v; g = t; b = p;
    }
}

void
load_dragon()
{
    m_surfels.resize(m_faces.size());

    for (unsigned int i(0); i < static_cast<unsigned int>(
        m_faces.size()); ++i)
    {
        std::array<unsigned int, 3> face = m_faces[i];
        Vector3f v[3] = {
            m_vertices[face[0]],
            m_vertices[face[1]],
            m_vertices[face[2]]
        };

        Vector3f p0, t1, t2;
        steiner_circumellipse(
            v[0].data(), v[1].data(), v[2].data(),
            p0.data(), t1.data(), t2.data()
            );

        Vector3f n_s = t1.cross(t2);
        Vector3f n_t = (v[1] - v[0]).cross(v[2] - v[0]);

        if (n_t.dot(n_s) < 0.0f)
        {
            t1.swap(t2);
        }

        m_surfels[i].c = p0;
        m_surfels[i].u = t1;
        m_surfels[i].v = t2;
        m_surfels[i].p = Vector3f::Zero();

        float h = std::min((std::abs(p0.x()) / 0.45f) * 360.0f, 360.0f);
        float r, g, b;
        hsv2rgb(h, 1.0f, 1.0f, r, g, b);
        m_surfels[i].rgba = static_cast<unsigned int>(r * 255.0f)
                         | (static_cast<unsigned int>(g * 255.0f) << 8)
                         | (static_cast<unsigned int>(b * 255.0f) << 16);
    }
}

void
get_model(void* value, void* data)
{
    *static_cast<unsigned int*>(value) = g_model;
}

void
set_model(void const* value, void* data)
{
    g_model = *static_cast<unsigned int const*>(value);
    
    switch (g_model)
    {
        case 1:
            load_plane(200);
            break;
        case 2:
            load_cube();
            break;
        default:
            load_dragon();
    }
}

void
set_soft_zbuffer(void const* value, void* data)
{
    viz->set_soft_zbuffer(*static_cast<bool const*>(value));

    if (viz->soft_zbuffer())
        TwDefine(" TweakBar/'EWA filter' readonly=false ");
    else
        TwDefine(" TweakBar/'EWA filter' readonly=true ");
}
void get_soft_zbuffer(void* value, void* data)
{ *static_cast<bool*>(value) = viz->soft_zbuffer(); }

void set_soft_zbuffer_epsilon(void const* value, void* data)
{ viz->set_soft_zbuffer_epsilon(*static_cast<float const*>(value)); }
void get_soft_zbuffer_epsilon(void* value, void* data)
{ *static_cast<float*>(value) = viz->soft_zbuffer_epsilon(); }

void get_pointsize_method(void* value, void* data)
{ *static_cast<unsigned int*>(value) = viz->pointsize_method(); }
void set_pointsize_method(void const* value, void* data)
{ viz->set_pointsize_method(*static_cast<unsigned int const*>(value)); }

void set_ewa_filter(void const* enable, void* data)
{ viz->set_ewa_filter(*static_cast<bool const*>(enable)); }
void get_ewa_filter(void* value, void* data)
{ *static_cast<bool*>(value) = viz->ewa_filter(); }

void set_ewa_radius(void const* value, void* data)
{ viz->set_ewa_radius(*static_cast<float const*>(value)); }
void get_ewa_radius(void* value, void* data)
{ *static_cast<float*>(value) = viz->ewa_radius(); }

void set_multisample(void const* enable, void* data)
{ viz->set_multisample(*static_cast<bool const*>(enable)); }
void get_multisample(void* value, void* data)
{ *static_cast<bool*>(value) = viz->multisample(); }

void set_backface_culling(void const* enable, void* data)
{ viz->set_backface_culling(*static_cast<bool const*>(enable)); }
void get_backface_culling(void* value, void* data)
{ *static_cast<bool*>(value) = viz->backface_culling(); }

void get_smooth(void* value, void* data)
{ *static_cast<bool*>(value) = viz->smooth(); }
void set_smooth(void const* value, void* data)
{ viz->set_smooth(*static_cast<bool const*>(value)); }

void get_color_material(void* value, void* data)
{ *static_cast<unsigned int*>(value) = viz->color_material() ? 1 : 0; }
void set_color_material(void const* value, void* data)
{ viz->set_color_material(*static_cast<unsigned int const*>(value) == 1); }

void get_material_color(void* value, void* data)
{ Map<Vector3f> color(static_cast<float*>(value));
  color = Map<const Vector3f>(viz->material_color()); }
void set_material_color(void const* value, void* data)
{ viz->set_material_color(static_cast<float const*>(value)); }

void get_material_shininess(void* value, void* data)
{ *static_cast<float*>(value) = viz->material_shininess(); }
void set_material_shininess(void const* value, void* data)
{ viz->set_material_shininess(*static_cast<float const*>(value)); }

void get_radius_scale(void* value, void* data)
{ *static_cast<float*>(value) = viz->radius_scale(); }
void set_radius_scale(void const* value, void* data)
{ viz->set_radius_scale(*static_cast<float const*>(value)); }

}

int
main(int argc, char* argv[])
{
    GLviz::init(argc, argv);
  
    camera.translate(Eigen::Vector3f(0.0f, 0.0f, -2.0f));
    viz = std::unique_ptr<SplatRenderer>(new SplatRenderer(camera));

    try
    {
        load_triangle_mesh("stanford_dragon_v40k_f80k.raw");
    }
    catch(std::runtime_error const& e)
    {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    set_model(&g_model, nullptr);

    // Setup AntTweakBar.
    {
        TwBar* bar = GLviz::twbar();

        TwType models = TwDefineEnumFromString("models",
            "Dragon,Plane,Cube");
        TwAddVarCB(bar, "Model", models, &set_model,
            &get_model, nullptr, " group=Scene ");

        TwAddSeparator(bar, nullptr, " group=Scene ");

        TwType shading_type = TwDefineEnumFromString("shading_type",
            "Flat,Phong");
        TwAddVarCB(bar, "Shading", shading_type, &set_smooth,
            &get_smooth, nullptr, " key=5 group='Surface Splatting' ");

        TwAddSeparator(bar, nullptr, " group='Surface Splatting' ");

        TwType color_src = TwDefineEnumFromString("color_src",
            "Surfel,Material");
        TwAddVarCB(bar, "Color", color_src, &set_color_material,
            &get_color_material, nullptr,
            " key='c' group='Surface Splatting' ");

        TwAddVarCB(bar, "Material Color", TW_TYPE_COLOR3F,
            &set_material_color, &get_material_color, nullptr,
            " help='Material Color' group='Surface Splatting' ");

        TwAddVarCB(bar, "Material Shininess", TW_TYPE_FLOAT,
            &set_material_shininess, &get_material_shininess, nullptr,
            " min=1e-12 max=1000 help='Material Shininess' \
            group='Surface Splatting' ");

        TwAddSeparator(bar, nullptr, " group='Surface Splatting' ");

        TwAddVarCB(bar, "Soft z-buffer", TW_TYPE_BOOLCPP,
            &set_soft_zbuffer, &get_soft_zbuffer, nullptr,
            " key=z group='Surface Splatting' ");

        TwAddVarCB(bar, "EWA filter", TW_TYPE_BOOLCPP,
            &set_ewa_filter, &get_ewa_filter, nullptr,
            " key=u help='EWA filter' group='Surface Splatting' ");

        TwAddVarCB(bar, "EWA radius", TW_TYPE_FLOAT,
            &set_ewa_radius, &get_ewa_radius, nullptr,
            " min=0.1 max=4.0 step=1e-3 group='Surface Splatting' ");

        TwAddVarCB(bar, "Soft z-buffer epsilon", TW_TYPE_FLOAT,
            &set_soft_zbuffer_epsilon, &get_soft_zbuffer_epsilon, nullptr,
            " min=0 max=1.0 step=1e-6 group='Surface Splatting' ");

        TwType point_size = TwDefineEnumFromString("Point size",
            "PBP,BHZK05,WHA+07,ZRB+04");
        TwAddVarCB(bar, "Point size", point_size, &set_pointsize_method,
            &get_pointsize_method, nullptr,
            " key='t' group='Surface Splatting' ");

        TwAddSeparator(bar, NULL, " group='Surface Splatting' ");

        TwAddVarCB(bar, "Multisample 4x", TW_TYPE_BOOLCPP,
            &set_multisample, &get_multisample, nullptr,
            " help='Multisample 4x' group='Surface Splatting' ");

        TwAddVarCB(bar, "Backface culling", TW_TYPE_BOOLCPP,
            &set_backface_culling, &get_backface_culling, nullptr,
            " group='Surface Splatting' ");

        TwAddVarCB(bar, "Radius scale", TW_TYPE_FLOAT,
            &set_radius_scale, &get_radius_scale, nullptr,
            " min=1e-6 max=2.0 step=1e-3 group='Surface Splatting' ");
    }

    TwDefine(" TweakBar size='300 400' valueswidth='100' \
        color='100 100 100' refresh=0.01 ");

    GLviz::display_callback(displayFunc);
    GLviz::reshape_callback(reshapeFunc);
    GLviz::close_callback(closeFunc);

    return GLviz::exec(camera);
}
