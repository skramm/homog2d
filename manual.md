# Manual

Home page: https://github.com/skramm/homog2d

- [Lines and points](#basic)
- [2D transformation (aka homographies)](#matrix)
- [Bindings](#bind)
- [Build options](#options)
- [Technical details](#tech)

## General

All the code is in the namespace `homog2d`, so either add `using namespace homog2d`, either use it as a prefix on each type.

## 2D lines and points
<a name="basic"></a>

- Create a 2D point:
```
Point2d pt1;       // 0,0
Point2d pt2(3,4);
```

- Create a 2D line:
```C++
Line2d li1;             // vertical line at x=0
Line2d li2( 3, 4 );     // line passing through (0,0) and (3,4)
```

- Create a point from two lines, and a line from two points:
```
Point2d pt3( li1, li2 );
Line2d  li3( pt1, pt2 );
```

- Get line from two points, and point from two lines:
```
pt1 = li1 * li2;
li1 = pt1 * pt2;
```

Beware, two parallel lines will never cross, and two identical points do not define a line.
So if you code attempts to do so, this will trigger a
[std::runtime_error](https://en.cppreference.com/w/cpp/error/runtime_error)
exception.

- Add some offset to a line
```
li1.addOffset( OD_Horiz, 5 ); // horizontal offset
li1.addOffset( OD_Vert, 5 ); // vertical offset
```


## Homographies
<a name="matrix"></a>

You can manipulate 2D transformations as 3x3 homogeneous matrices (aka "Homography").
The three basic transformations (rotation, translation, scaling) are available.
Each of these is available in two forms: "`setXxxx()`" and "`addXxxx()`".
The first one starts from the identity transformation and applies the requested one.
The second form adds the requested transformation to the matrix.

- First example:

```
Homogr h; // unit transformation
h.setTranslation(3,4);
Point2d pt1(1,2);
Point2d pt2 = h * pt1; // pt2 is now (4,6)
```

- You can build some complex transformation by multiplying these:
```
Homogr h; // unit transformation
h.setTranslation(3,4);
Homogr h2( 45. * M_PI / 180.); // 45° rotation matrix
Homogr h3a = h1*h2; // first, rotation, then translation
Homogr h3b = h2*h1; // first, rotation, then translation
```

- But you can also used "chained syntax":
```
Homogr h; // unit transformation
h.addTranslation(3,4).addRotation( 45. * M_PI / 180.).addTranslation(-3,-4);
```

- You can access individual values of the matrix:
```
h.setValue( 0, 0, 3.14 );
auto v = h.getValue( 0, 0 ); // 3.14
```
However, when using setValue(), no guarantee is given that the result will be a valid matrix!

- You can apply the homography to a set of points or lines:
```
std::vector<Point2d> v_pts;
... // fill with values
h.applyTo( v_pts );
```
This actually works with any other container on whom one can iterate, such as `std::array` or `std::list`.

- You can compute the inverse and/or the transpose of the matrix:
```
h.inverse();
h.transpose();
```

## Bindings with other libraries
<a name="bind"></a>

Import from other types is pretty much straight forward.
For points, a templated constructor is provided that can be used with any type having an 'x' and 'y' member.
For homographies, you can import directly from
`std::vector<std::vector<T>>` or `std::array<std::array<T,3>,3>`

For export, additional functions are provided to interface with [Opencv](https://opencv.org).
This is enabled by defining the symbol HOMOG2D_USE_OPENCV.
You can then write this:
```
Point2d pt;
...
cv::Point2d ptcv1 = pt.getCvPtd();
cv::Point2f ptcv2 = pt.getCvPtf()
```

You can also directly draw points and lines on an image (cv::Mat):
```
Point2d pt( ... );
Line2d li( ... );
li.drawCvMat( mat );
pt.drawCvMat( mat );
```

Please note that for lines, this will draw a line spanning the whole image, as opposed to
the Opencv function `cv::line()` that actually draws a **segment** only.
For points, this will just draw a small cross: 2 H/V lines.

These two functions support a second optional argument of type `CvDrawParams` that holds various parameters for drawing.
So you can for example set the color and line width with:
```
li.drawCvMat( mat, CvDrawParams().setThickness(2 /* pixels */).setColor( r,g,b) );
```
with r,g,b as bytes (`uint8_t`) in the range [0,255].

A demo demonstrating this Opencv binding is provided, try it with
`make demo` (requires of course that Opencv is installed on your machine).

## Build options
<a name="options"></a>

Below are some options that can be passed, to activate them, just define the symbol.
You can do that in the makefile or just add a `#define` on top of your program,
**before** the `#include "homog2d"`

- `HOMOG2D_USE_OPENCV`: enable the Opencv binding, see [Bindings](#bind).

(TO BE EXPANDED)


## Technical details
<a name="tech"></a>

- The library is fully templated, the two types (Point2d and Line2d) are actually the same datatype,
behavior differs due to some policy-based design.
- Points are stored as non-normalized values, any computation will keep the resulting values.
Normalization is done for comparison but not saved.
- Lines are always stored as normalized values (a^2+b^2 = 1)
- Homographies are stored as normalized values, either as h33=1, or (if null) as h23=1

