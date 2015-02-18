# GLviz

GLviz is a small collection of C++ classes and GLSL shaders to facilitate the development of OpenGL demos. It is built on top of [CMake](http://www.cmake.org/), [SDL](http://libsdl.org), [GLEW](http://glew.sourceforge.net), [Eigen](http://eigen.tuxfamily.org/), and [AntTweakBar](http://anttweakbar.sourceforge.net/) and requires at least OpenGL 3.3. GLviz has been tested on a NVIDIA GTX 680 GPU using driver version 347.52 on Windows (compiled with MSVC 2013) and 340.65 on Linux (compiled with GCC 4.9). It includes all external dependencies to make compilation on either Windows or Linux as simple and convenient as possible.

**Author**: [Sebastian Lipponer](http://sebastianlipponer.de), **License**: MIT

## Features

* Camera with trackball navigation.
* Shader management.
* Triangle mesh shader (supports flat or Phong shading and an optional high-quality wireframe<sup>1</sup> overlay).
* Sphere shader (for the sake of performance not perspectively correct spheres).
* Supports embedding of shaders in the executable.

## Screenshots

[![](http://sebastianlipponer.github.io/glviz/dragon_thumbnail.png)](http://sebastianlipponer.github.io/glviz/dragon.png)[![](http://sebastianlipponer.github.io/glviz/dragon_wireframe_thumbnail.png)](http://sebastianlipponer.github.io/glviz/dragon_wireframe.png)[![](http://sebastianlipponer.github.io/glviz/dragon_spheres_thumbnail.png)](http://sebastianlipponer.github.io/glviz/dragon_spheres.png)

## Documentation

Currently there is no documentation, but the code includes an example. From the source code it should become apparent how GLviz is intended to be used.

## References

[1] Bærentzen, J. A., Munk-Lund, S., Gjøl, M., Larsen, B. D.: **Two Methods for Antialiased Wireframe Drawing with Hidden Line Removal**. In Proceedings of the Spring Conference in Computer Graphics, 2008.
