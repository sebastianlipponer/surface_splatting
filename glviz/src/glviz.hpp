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

#ifndef GLVIZ_HPP
#define GLVIZ_HPP

#include "camera.hpp"

#include <AntTweakBar.h>

namespace GLviz
{

int      screen_width();
int      screen_height();

TwBar*   twbar();

Camera*  camera();
void     set_camera(Camera& camera);

void     display_callback(std::function<void ()> display_callback);
void     timer_callback(std::function<void (unsigned int)> timer_callback,
             unsigned int timer_msec);
void     reshape_callback(std::function<void (int width, int height)>
             reshape_callback);
void     close_callback(std::function<void ()> close_callback);

void     cout_opengl_version();
void     cout_glew_version();

void     init(int argc, char* argv[]);
int      exec(Camera& camera);

}

#endif // GLVIZ_HPP
