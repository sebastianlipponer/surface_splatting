# Surface Splatting

This demo implements a point rendering and texture filtering technique called *Surface Splatting*<sup>1</sup>. More specifically, it implements the GPU accelerated surface splatting approach by Botsch et al.<sup>2</sup> using OpenGL 3.3 (core profile). This basically comprises a raycasting based rasterization of elliptical splats, a deferred shading pipeline and an approximation to the original EWA filter<sup>1</sup>. The demo has been tested on a NVIDIA GTX 1080 Ti GPU using driver version 436.02 on Windows 10 (compiled with MSVC 2019) and 418.74 on Linux (compiled with GCC 8.3). It is built on top of [GLviz](https://github.com/sebastianlipponer/glviz) and is therefore rather simple to compile.

**Author**: [Sebastian Lipponer](http://sebastianlipponer.de), **License**: GPL v3

[![](http://sebastianlipponer.github.io/surface_splatting/dragon_thumbnail.png)](http://sebastianlipponer.github.io/surface_splatting/dragon.png)[![](http://sebastianlipponer.github.io/surface_splatting/dragon_splats_tail_thumbnail.png)](http://sebastianlipponer.github.io/surface_splatting/dragon_splats_tail.png)

_**Left**: Surface splatting of the Stanford Dragon model. **Right**: Closeup showing artificially shrunk splats to illustrate the splat distribution._

## Build

Before running CMake run either build-extern.cmd or build-extern.sh to download and build the necessary external dependencies in the .extern directory.

## Basic Principle

Surface splatting<sup>1</sup> renders point-sampled surfaces using a combination of an object-space reconstruction filter and a screen-space pre-filter for each point sample. This effectively avoids aliasing artifacts and it guarantees a hole-free reconstruction of a point-sampled surface even for moderate sampling densities. The object-space reconstruction filter resembles an elliptical disk, also referred to as a *splat*, whose position, orientation, major axis, and semi-major axis are usually chosen to provide a good approximation to a given geometry. After a perspective projection of all splats to screen-space, rendering proceeds by applying a bandlimiting prefilter to avoid frequencies higher than the Nyquist frequency of the pixel sampling grid and summing up all contributions from the overlapping splats for each individual pixel with a subsequent normalization.

[![](http://sebastianlipponer.github.io/surface_splatting/checkerboard_ewa_filter_thumbnail.png)](http://sebastianlipponer.github.io/surface_splatting/checkerboard_ewa_filter.png)[![](http://sebastianlipponer.github.io/surface_splatting/checkerboard_thumbnail.png)](http://sebastianlipponer.github.io/surface_splatting/checkerboard.png)

_Surface splatting of a checkerboard. **Left**: EWA filter approximation enabled. **Right**: EWA filter approximation disabled._

## Splat Rasterization

Since today's GPUs are mostly optimized for triangle based rendering and do not provide any specialized fixed-function hardware for splat rasterization, it is required to use the programmable vertex and fragment shader units for this task. The most performant way to do so, at least on NVIDIA hardware, seems still to be to draw point primitives as screen-space squares of a certain size, and to cast a ray for each fragment to test whether it is part of the splat. While the corresponding formulas proposed by Botsch et al.<sup>3,2</sup> are efficient to compute, they constitute only an approximation to the exact screen-space position and extents of a splat. Moreover, under certain conditions, the screen-space extents of a splat are underestimated by this approximation, which then causes rendering artifacts.

[![](http://sebastianlipponer.github.io/surface_splatting/plane_splats_pch_thumbnail.png)](http://sebastianlipponer.github.io/surface_splatting/plane_splats_pch.png)[![](http://sebastianlipponer.github.io/surface_splatting/plane_splats_bhzk05_thumbnail.png)](http://sebastianlipponer.github.io/surface_splatting/plane_splats_bhzk05.png)

_**Left**: Correct rendering. **Right**: Some splats are chopped off due to Botsch et al.'s<sup>2,1</sup> approximation of the screen-space extents of a splat._

A very simple fix for this problem consists in multiplying the estimated splat extents with a safety factor larger than one, but this causes the generation of large number of unnecessary fragments and since the fragment stage is the performance bottleneck anyway, this is not a particularly good solution.

Computing the exact screen-space position and extents of a splat in a reliable and robust way turns out to be difficult in practice. For example, using the centralized conics approach of Zwicker et al.<sup>4</sup>, the resulting formulas involve the inverse of a 2D-to-2D projective mapping from a splats parameter space (given by the position of its center, and two vectors spanning the ellipse) to screen-space. The condition number of this mapping increases rapidly as the projection of a splat approaches a line. Zwicker et al.'s<sup>4</sup> approach is therefore numerically unstable and plagued by large roundoff errors. The demo nevertheless also includes an implementation of this method.

In the context of GPU-based raycasting of quadric surfaces Sigg et al.<sup>5</sup> noted that a reliable and robust solution to this problem must not use the inverse of the projective mapping. This is indeed possible by exploiting the fact that normal vectors, as opposed to ordinary vectors, transform by the inverse transpose of a mapping. The underlying idea is to find a plane in screen-space which is tangent to a quadric in its parameter space, i.e. tangent to the unit sphere. Mathematically, this requires the solution of a quadratic equation for each dimension and does not involve the inverse projective mapping. Weyrich et al.<sup>6</sup> later adapted this method to the special case of surface splatting. It is important to note that this approach yields a splats position and extents in screen-space, i.e. normalized device coordinates. It is therefore not possible to clip the obtained bounding box by the view-frustum. Furthermore, while conics are closed under projective mappings, the perspective projection of an ellipse may yield a parabola or hyperbola, in which case the proposed method does not work anymore. To avoid these problems, my implementation considers only those splats whose bounding sphere intersects with the view-frustum.

[![](http://sebastianlipponer.github.io/surface_splatting/cube_closeup_pbp_thumbnail.png)](http://sebastianlipponer.github.io/surface_splatting/cube_closeup_pbp.png)[![](http://sebastianlipponer.github.io/surface_splatting/cube_closeup_whaf07_thumbnail.png)](http://sebastianlipponer.github.io/surface_splatting/cube_closeup_whaf07.png)

_Closeup of a cube consisting of 24 clipped splats. **Left**: Using my method of a projected bounding polygon produces a correct image. **Right**: Using the method proposed by Weyrich et al.<sup>6</sup> produces artifacts._

In the course of implementing and analyzing all these methods, I also devised my own approach. The idea is to bound a splat by a polygon in its parameter space and to only employ this bounding polygon to compute the screen-space position and extents of a splat. To this end, the polygon is first transformed to clip space where it is clipped by the view-frustum using the Sutherland-Hodgman algorithm. A perspective division of each vertex then yields the clipped polygon in screen-space where the desired quantities are simple to compute. This method is not exact, but it produces a correct upper bound to the extents of a splat and unlike previous methods also works for large splats being close to the near plane.

## Sharp Features

The demo also implements clipped splats<sup>4</sup> to facilitate the rendering of sharp features like edges and corners of a cube.

[![](http://sebastianlipponer.github.io/surface_splatting/cube_thumbnail.png)](http://sebastianlipponer.github.io/surface_splatting/cube.png)[![](http://sebastianlipponer.github.io/surface_splatting/cube_splats_thumbnail.png)](http://sebastianlipponer.github.io/surface_splatting/cube_splats.png)

_**Left**: Surface splatting of a cube consisting of 24 clipped splats. **Right**: Cube with artificially shrunk splats to illustrate the splat distribution._


## References

[1] Zwicker M., Pfister H., van Baar J., Gross M.: **Surface Splatting**. In Proceedings of the 28th Annual Conference on Computer Graphics and Interactive Techniques, SIGGRAPH '01, pp. 371-378.

[2] Botsch, M., Hornung, A., Zwicker, M., Kobbelt, L.: **High-Quality Surface Splatting on Today's GPUs**. In Proceedings of Eurographics Symposium on Point-Based Graphics, 2005, 17-24.

[3] Botsch, M., Spernat, M., Kobbelt, L.: **Phong Splatting**. In Proceedings of the First Eurographics Conference on Point-Based Graphics 2004, SPBG '04, 25-32.

[4] Zwicker, M., Räsänen, J., Botsch, M., Dachsbacher, C., Pauly, M.: **Perspective Accurate Splatting**. Proceedings of Graphics Interface, 2004, GI '04, 247-254.

[5] Sigg, C., Weyrich, T., Botsch, M., Gross, M.: **GPU-based Ray-casting of Quadratic Surfaces**. Proceedings of the 3rd Eurographics / IEEE VGTC conference on Point-Based Graphics, 2006, SPBG '06, 59-65.

[6] Weyrich, T., Heinzle, S., Aila, T., Fasnacht, D. B., Oetiker, S., Botsch, M., Flaig, C., Mall, S., Rohrer, K., Felber, N., Kaeslin, H., Gross, M.: **A Hardware Architecture for Surface Splatting**. ACM Transactions on Graphics, 2007.
