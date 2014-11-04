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

#ifndef TRACKBALL_HPP
#define TRACKBALL_HPP

#include <Eigen/Dense>

class Trackball
{

public:
    Eigen::Quaternionf const& operator()(float u0_x, float u0_y,
        float u1_x, float u1_y);

protected:
    float project_to_sphere(float r, float x, float y) const;

protected:
    Eigen::Quaternionf rotation;
};

#endif // TRACKBALL_HPP
