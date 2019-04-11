# Manual

home page: https://github.com/skramm/homog2d


All the code is in the namespace `homog2d`, so either add `using namespace homog2d`, either use it as a prefix on each type.

## 2D lines and points

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




## Technical details

- Points are stored as non-normalized values, any computation will keep the resulting values
- Lines are always stored as normalized values (a^2+b^2 = 1)
- Homographies are stored as normalized values, either ass h33=0, or (if null) as h23=0

