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

#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#include <GL/glew.h>
#include <memory>

class Framebuffer
{

public:
    Framebuffer();
    ~Framebuffer();

    GLuint color_texture();

    void enable_depth_texture();
    void disable_depth_texture();
    GLuint depth_texture();

    void attach_normal_texture();
    void detach_normal_texture();
    GLuint normal_texture();

    void set_multisample(bool enable = true);

    void bind();
    void unbind();
    void reshape(GLint width, GLint height);

private:
    void initialize();
    void remove_and_delete_attachments();

    GLuint m_fbo;
    GLuint m_color, m_normal, m_depth;

    struct Impl;
    struct Default;
    struct Multisample;

    std::unique_ptr<Impl> m_pimpl;
};

#endif // FRAMEBUFFER_HPP
