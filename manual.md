# Manual

# THIS IS PRELIMINAR !

Home page: https://github.com/skramm/homog2d

- [Lines and points](#basic)
- [2D transformation (aka homographies)](#matrix)
- [Bindings](#bind)
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
Line2d li2( pt1, pt2 ); // from two points:
Line2d li2( 3, 4 );     // line passing through (0,0) and (3,4)
```

- Create a point from two lines:
```
Point2d pt3(li1,li2);
```

- Get line from two points, and point from two lines:
```
pt1 = li1 * li2;
li1 = pt1 * pt2;
```
- Add some offset to a line
```
li1.addOffset( OD_Horiz, 5 ); // horizontal offset
li1.addOffset( OD_Vert, 5 ); // vertical offset
```

## Homographies
<a name="matrix"></a>
- You can manipulate 2D transformations as 3x3 homogeneous matrices (aka "Homography"):

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

- But you can also add some transformation to a given matrix:
```
Homogr h; // unit transformation
h.setTranslation(3,4);
h.addRotation( 45. * M_PI / 180.); // 45° rotation
```

- You can also access individual values of the matrix:
```
h.setValue( 0, 0, 3.14 );
auto v = h.getValue( 0, 0 ); // 3.14
```
However, when using setValue(), no guarantee is given that the result will be a valid matrix!


## Binding with other libraries
<a name="bind"></a>

Import from other types is pretty much straight forward.
For points, a templated constructor is provided that can be used with any type having an 'x' and 'y' member.
For homographies, you can import directly from
`std::vector<std::vector<T>>` or `std::array<std::array<T,3>,3>`

For export, additional functions are provided to interface with [Opencv](https://opencv.org).
This is enable by defining the symbol HOMOG2D_USE_OPENCV.
You can then write this:
```
Point2d pt;
...
cv::Point2d ptcv1 = pt.getCvPtd();
cv::Point2f ptcv2 = pt.getCvPtf()
```

You can also directly points and lines:
```
Point2d pt( ... );
Line2d li( ... );
li.drawCvMat( mat, cv::Scalar(b,g,r), thickness, linetype );
pt.drawCvMat( mat, cv::Scalar(b,g,r), delta );
```

Dor lines, the API follows the Opencv interface (color, line thickness, line type),
[see here](https://docs.opencv.org/3.4.5/d6/d6e/group__imgproc__draw.html#ga7078a9fae8c7e7d13d24dac2520ae4a2).
Please note that the Opencv function actually draws a **segment** while this function here will draw a line spanning the whole image.

For points, this will just draw a small cross: 2 H/V lines, length is `delta` (integer).

A demo demonstrating this Opencv binding is provided, try it with
`make demo` (requires of course that Opencv is installed on your machine).


## Technical details
<a name="tech"></a>

- The library is fully templated, the two types (Point2d and Line2d) are actually the same datatype,
behavior differs due to some policy-based design.
- Points are stored as non-normalized values, any computation will keep the resulting values.
Normalization is done for comparison but not saved.
- Lines are always stored as normalized values (a^2+b^2 = 1)
- Homographies are stored as normalized values, either as h33=1, or (if null) as h23=1

