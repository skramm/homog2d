# Homog2d: history

See [Release page](https://github.com/skramm/homog2d/releases).

- current master branch:
 - Major release, some API change
 - added a template class `KeyboardLoop` to handle keyboard inputs and loop in Opencv demo
 - added some static "no build" checking for illegal code instanciation (see target `nobuild` in makefile)
 - added operator * to apply homography to all elements of a container holding points or lines
 - added templated types `Circle_`, `Polyline_`, `FRect_` and associated code

- [v2.3](https://github.com/skramm/homog2d/releases/tag/v2.3), released on 2021-06-18
  - switch to MPLv2 licence
  - all computations are now done using default numerical type `HOMOG2D_INUMTYPE`
  - added `buildFrom4Points()` to Homography class
  - added templated conversion free functions and member function to Opencv point types
  - added full testing with all three arithmetic types (`make testall`)
  - added segment type and associated features
  - demo code heavy refactoring (requires Opencv)

- [v2.2](https://github.com/skramm/homog2d/releases/tag/v2.2), released on 2021-06-01
  - added 2 constructors to `Homogr`
  - added `isParallelTo()`
  - API change:
    - renamed `drawCvMat()` to `draw()` (to make transition to other graphical backends easier)
    - matrices: renamed `setValue()` to `set()` and `getValue()` to `get()`
    - for OpenCv matrices, replaced `getFrom()` by assignment operator
    - replaced enums `En_OffsetDir` and `En_GivenCoord` with class enums `LineOffset` and `GivenCoord`
  - added `Segment` type and associated code
  - Licence change to MPLv2
  - replaced `HOMOG2D_SAFE_MODE` with `HOMOG2D_NOCHECKS`, so that checking is enabled by default.

- [v2.1](https://github.com/skramm/homog2d/releases/tag/v2.1), released on 2020-04-27.
  - added `intersectsCircle()`, to get intersection with circles
  - intersection data structure name changed, now `Intersect`
  - the intersection points are now private, they must be fetched with `get()`:
  this `Intersect` member function will return a `std::pair` holding the two intersection points.
  - added `getPoints()`
  - added single argument constructors
  - renamed `distToPoints()` member function: now `distTo()`, and can be used with lines as argument.
  - added new matrix type: `Hmatrix`, for point to line (or line to point) mapping.
  - renamed `clear()` to `init()` for matrices
  - added `getParallelLine()`


- [v2.0](https://github.com/skramm/homog2d/releases/tag/v2.0): fully templated.

- [v1.0](https://github.com/skramm/homog2d/releases/tag/v1.0): initial release, not templated by numerical data type.
Same API, works fine. A bit lighter on template stuff.







### Footnotes

[(1)](#paedfapol)
<a name="fn1"></a> The rationale behind this interface is that if we had choosen to pass a point as argument, no guarantee would have been given that the point is effectively lying on the line.
With this function signature, the function can compute the point itself.
