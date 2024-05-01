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
