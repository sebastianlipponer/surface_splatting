// This file is part of Surface Splatting.
//
// Copyright (C) 2010-2018 by Sebastian Lipponer.
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

#include <GLviz/glviz.hpp>
#include <GLviz/utility.hpp>

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

GLviz::Camera g_camera;

int g_model(1);

std::unique_ptr<SplatRenderer>  viz;
std::vector<Surfel>             g_surfels;

void load_triangle_mesh(std::string const& filename, std::vector<
    Eigen::Vector3f>& vertices, std::vector<std::array<
    unsigned int, 3>>& faces);

void mesh_to_surfel(std::vector<Eigen::Vector3f> const& vertices,
    std::vector<std::array<unsigned int, 3>> const& faces,
    std::vector<Surfel>& surfels);

void
load_plane(unsigned int n)
{
    const float d = 1.0f / static_cast<float>(2 * n);

    Surfel s(Vector3f::Zero(),
             2.0f * d * Vector3f::UnitX(),
             2.0f * d * Vector3f::UnitY(),
             Vector3f::Zero(),
             0);

    g_surfels.resize(4 * n * n);
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
                g_surfels[m] = s;

                // Clip border surfels.
                if (j == 2 * n)
                {
                    g_surfels[m].p = Vector3f(-1.0f, 0.0f, 0.0f);
                    g_surfels[m].rgba = ~s.rgba;
                }
                else if (i == 2 * n)
                {
                    g_surfels[m].p = Vector3f(0.0f, -1.0f, 0.0f);
                    g_surfels[m].rgba = ~s.rgba;
                }
                else if (j == 0)
                {
                    g_surfels[m].p = Vector3f(1.0f, 0.0f, 0.0f);
                }
                else if (i == 0)
                {
                    g_surfels[m].p = Vector3f(0.0f, 1.0f, 0.0f);
                }
                else
                {
                    // Duplicate and clip inner surfels.
                    if (j % 2 == 0)
                    {
                        g_surfels[m].p = Vector3f(1.0, 0.0f, 0.0f);

                        g_surfels[++m] = s;
                        g_surfels[m].p = Vector3f(-1.0, 0.0f, 0.0f);
                        g_surfels[m].rgba = ~s.rgba;
                    }

                    if (i % 2 == 0)
                    {
                        g_surfels[m].p = Vector3f(0.0, 1.0f, 0.0f);

                        g_surfels[++m] = s;
                        g_surfels[m].p = Vector3f(0.0, -1.0f, 0.0f);
                        g_surfels[m].rgba = ~s.rgba;
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

    g_surfels = std::vector<Surfel>(cube, cube + 24);
}

void
load_dragon()
{
    std::vector<Eigen::Vector3f>              vertices, normals;
    std::vector<std::array<unsigned int, 3>>  faces;

    try
    {
        load_triangle_mesh("stanford_dragon_v344k_f688k.raw",
            vertices, faces);
    }
    catch (std::runtime_error const& e)
    {
        std::cerr << e.what() << std::endl;
        std::exit(EXIT_FAILURE);
    }

    GLviz::set_vertex_normals_from_triangle_mesh(
        vertices, faces, normals);

    mesh_to_surfel(vertices, faces, g_surfels);
}

void
load_model()
{
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
load_triangle_mesh(std::string const& filename, std::vector<
    Eigen::Vector3f>& vertices, std::vector<std::array<
    unsigned int, 3>>& faces)
{
    std::cout << "\nRead " << filename << "." << std::endl;
    std::ifstream input(filename);

    if (input.good())
    {
        input.close();
        GLviz::load_raw(filename, vertices, faces);
    }
    else
    {
        input.close();

        std::ostringstream fqfn;
        fqfn << path_resources;
        fqfn << filename;
        GLviz::load_raw(fqfn.str(), vertices, faces);
    }

    std::cout << "  #vertices " << vertices.size() << std::endl;
    std::cout << "  #faces    " << faces.size() << std::endl;
}

void
steiner_circumellipse(float const* v0_ptr, float const* v1_ptr,
    float const* v2_ptr, float* p0_ptr, float* t1_ptr, float* t2_ptr)
{
    Matrix2f Q;
    Vector3f d0, d1, d2;
    {
        using Vec = Map<const Vector3f>;
        Vec v[] = { Vec(v0_ptr), Vec(v1_ptr), Vec(v2_ptr) };

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
mesh_to_surfel(std::vector<Eigen::Vector3f> const& vertices,
    std::vector<std::array<unsigned int, 3>> const& faces,
    std::vector<Surfel>& surfels)
{
    surfels.resize(faces.size());

    for (unsigned int i(0); i < static_cast<unsigned int>(
        faces.size()); ++i)
    {
        std::array<unsigned int, 3> face = faces[i];
        Vector3f v[3] = {
            vertices[face[0]],
            vertices[face[1]],
            vertices[face[2]]
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

        surfels[i].c = p0;
        surfels[i].u = t1;
        surfels[i].v = t2;
        surfels[i].p = Vector3f::Zero();

        float h = std::min((std::abs(p0.x()) / 0.45f) * 360.0f, 360.0f);
        float r, g, b;
        hsv2rgb(h, 1.0f, 1.0f, r, g, b);
        surfels[i].rgba = static_cast<unsigned int>(r * 255.0f)
            | (static_cast<unsigned int>(g * 255.0f) << 8)
            | (static_cast<unsigned int>(b * 255.0f) << 16);
    }
}

void
display()
{
    viz->render_frame(g_surfels);
}

void
reshape(int width, int height)
{
    const float aspect = static_cast<float>(width) /
        static_cast<float>(height);

    glViewport(0, 0, width, height);
    g_camera.set_perspective(60.0f, aspect, 0.005f, 5.0f);
}

void
close()
{
    viz = nullptr;
}

void
gui()
{
    ImGui::Begin("Surface Splatting", nullptr);
    ImGui::SetWindowPos(ImVec2(3.0f, 3.0f), ImGuiCond_Once);
    ImGui::SetWindowSize(ImVec2(350.0f, 415.0f), ImGuiCond_Once);

    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.55f);

    ImGui::Text("fps \t %.1f fps", ImGui::GetIO().Framerate);

    ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
    if (ImGui::CollapsingHeader("Scene"))
    {
        if (ImGui::Combo("Models", &g_model, "Dragon\0Plane\0Cube"))
        {
            load_model();
        }
    }

    ImGui::SetNextTreeNodeOpen(true, ImGuiCond_Once);
    if (ImGui::CollapsingHeader("Surface Splatting"))
    {
        int shading_method = viz->smooth() ? 1 : 0;
        if (ImGui::Combo("Shading", &shading_method, "Flat\0Smooth\0\0"))
        {
            viz->set_smooth(shading_method > 0 ? true : false);
        }

        ImGui::Separator();

        int color_material = viz->color_material() ? 1 : 0;
        if (ImGui::Combo("Color", &color_material, "Surfel\0Material\0\0"))
        {
            viz->set_color_material(color_material > 0 ? true : false);
        }

        float material_color[3];
        std::copy(viz->material_color(), viz->material_color() + 3,
            material_color);
        if (ImGui::ColorEdit3("Material color", material_color))
        {
            viz->set_material_color(material_color);
        }

        float material_shininess = viz->material_shininess();
        if (ImGui::DragFloat("Material shininess",
            &material_shininess, 0.05f, 1e-12f, 1000.0f))
        {
            viz->set_material_shininess(std::min(std::max(
                1e-12f, material_shininess), 1000.0f));
        }

        ImGui::Separator();

        bool soft_zbuffer = viz->soft_zbuffer();
        if (ImGui::Checkbox("Soft z-buffer", &soft_zbuffer))
        {
            viz->set_soft_zbuffer(soft_zbuffer);
        }

        float soft_zbuffer_epsilon = viz->soft_zbuffer_epsilon();
        if (ImGui::DragFloat("Soft z-buffer epsilon",
            &soft_zbuffer_epsilon, 1e-5f, 1e-5f, 1.0f, "%.5f"))
        {
            viz->set_soft_zbuffer_epsilon(std::min(std::max(
                1e-5f, soft_zbuffer_epsilon), 1.0f));
        }

        ImGui::Separator();

        bool ewa_filter = viz->ewa_filter();
        if (ImGui::Checkbox("EWA filter", &ewa_filter))
        {
            viz->set_ewa_filter(ewa_filter);
        }

        float ewa_radius = viz->ewa_radius();
        if (ImGui::DragFloat("EWA radius",
            &ewa_radius, 1e-3f, 0.1f, 4.0f))
        {
            viz->set_ewa_radius(ewa_radius);
        }

        ImGui::Separator();

        int point_size = viz->pointsize_method();
        if (ImGui::Combo("Point size", &point_size,
            "PBP\0BHZK05\0WHA+07\0ZRB+04\0\0"))
        {
            viz->set_pointsize_method(point_size);
        }

        float radius_scale = viz->radius_scale();
        if (ImGui::DragFloat("Radius scale",
            &radius_scale, 0.001f, 1e-6f, 2.0f))
        {
            viz->set_radius_scale(std::min(std::max(
                1e-6f, radius_scale), 2.0f));
        }

        ImGui::Separator();

        bool multisample_4x = viz->multisample();
        if (ImGui::Checkbox("Multisample 4x", &multisample_4x))
        {
            viz->set_multisample(multisample_4x);
        }

        bool backface_culling = viz->backface_culling();
        if (ImGui::Checkbox("Backface culling", &backface_culling))
        {
            viz->set_backface_culling(backface_culling);
        }
    }

    ImGui::End();
}

void
keyboard(SDL_Keycode key)
{
    switch (key)
    {
        case SDLK_5:
            viz->set_smooth(!viz->smooth());
            break;
        case SDLK_c:
            viz->set_color_material(!viz->color_material());
            break;
        case SDLK_z:
            viz->set_soft_zbuffer(!viz->soft_zbuffer());
            break;
        case SDLK_u:
            viz->set_ewa_filter(!viz->ewa_filter());
            break;
        case SDLK_t:
            viz->set_pointsize_method((viz->pointsize_method() + 1) % 4);
            break;
    }
}

}

int
main(int argc, char* argv[])
{
    GLviz::GLviz();

    g_camera.translate(Eigen::Vector3f(0.0f, 0.0f, -2.0f));
    viz = std::unique_ptr<SplatRenderer>(new SplatRenderer(g_camera));

    load_model();

    GLviz::display_callback(display);
    GLviz::reshape_callback(reshape);
    GLviz::close_callback(close);
    GLviz::gui_callback(gui);
    GLviz::keyboard_callback(keyboard);

    return GLviz::exec(g_camera);
}
