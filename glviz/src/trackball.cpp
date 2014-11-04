// (c) Copyright 1993, 1994, Silicon Graphics, Inc.
// ALL RIGHTS RESERVED
// Permission to use, copy, modify, and distribute this software for
// any purpose and without fee is hereby granted, provided that the above
// copyright notice appear in all copies and that both the copyright notice
// and this permission notice appear in supporting documentation, and that
// the name of Silicon Graphics, Inc. not be used in advertising
// or publicity pertaining to distribution of the software without specific,
// written prior permission.
//
// THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
// AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
// INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
// FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
// GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
// SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY
// KIND, OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION,
// LOSS OF PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF
// THIRD PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN
// ADVISED OF THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON
// ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE
// POSSESSION, USE OR PERFORMANCE OF THIS SOFTWARE.
//
// US Government Users Restricted Rights
// Use, duplication, or disclosure by the Government is subject to
// restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
// (c)(1)(ii) of the Rights in Technical Data and Computer Software
// clause at DFARS 252.227-7013 and/or in similar or successor
// clauses in the FAR or the DOD or NASA FAR Supplement.
// Unpublished-- rights reserved under the copyright laws of the
// United States.  Contractor/manufacturer is Silicon Graphics,
// Inc., 2011 N.  Shoreline Blvd., Mountain View, CA 94039-7311.
//
// OpenGL(TM) is a trademark of Silicon Graphics, Inc.

// Trackball code:
// 
// Implementation of a virtual trackball.
// Implemented by Gavin Bell, lots of ideas from Thant Tessman and
//   the August '88 issue of Siggraph's "Computer Graphics," pp. 121-129.*

#include "trackball.hpp"

#include <Eigen/Core>

using namespace Eigen;

// This size should really be based on the distance from the center of
// rotation to the point on the object underneath the mouse.  That
// point would then track the mouse as closely as possible.  This is a
// simple example, though, so that is left as an Exercise for the
// Programmer.
const float trackball_size = 1.0f;

// Ok, simulate a track-ball.  Project the points onto the virtual
// trackball, then figure out the axis of rotation, which is the cross
// product of P1 P2 and O P1 (O is the center of the ball, 0,0,0)
// Note:  This is a deformed trackball-- is a trackball in the center,
// but is deformed into a hyperbolic sheet of rotation away from the
// center.  This particular function was chosen after trying out
// several variations.
//
// It is assumed that the arguments to this routine are in the range
// (-1.0 ... 1.0)
Quaternionf const&
Trackball::operator()(float u0_x, float u0_y, float u1_x, float u1_y)
{
    if (u0_x == u1_x && u0_y == u1_y)
    {
        // Zero rotation.
        rotation = Quaternionf::Identity();
        return rotation;
    }

    // First, figure out z-coordinates for projection of P1 and P2 to
    // deformed sphere.
    Vector3f u0(u0_x, u0_y, project_to_sphere(trackball_size, u0_x, u0_y));
    Vector3f u1(u1_x, u1_y, project_to_sphere(trackball_size, u1_x, u1_y));

    // Calculate rotation axis.
    Vector3f axis = u1.cross(u0);
    axis.normalize();

    // Calculate rotation angle.
    Vector3f d = u0 - u1;
    float t = d.norm() / (2.0f * trackball_size);

    // Clamp t to [-1.0, 1.0].
    if (t > 1.0f)
    {
        t = 1.0f;
    }
    else if (t < -1.0f)
    {
        t = -1.0f;
    }

    rotation = AngleAxisf(2.0f * std::asin(t), axis);

    return rotation;
}

// Project an x,y pair onto a sphere of radius r OR a hyperbolic sheet
// if we are away from the center of the sphere.
inline float
Trackball::project_to_sphere(float r, float x, float y) const
{
    const float sqrt2_div2 = 0.70710678118654752440f;
    float d = std::sqrt(x * x + y * y);
    float z;

    if (d < r * sqrt2_div2)
    {
        // Inside sphere.
        z = std::sqrt(r * r - d * d);
    }
    else
    {
        // On hyperbola.
        z = r * r / (2.0f * d);
    }

    return z;
}
