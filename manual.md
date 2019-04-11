# Manual

home page: https://github.com/skramm/homog2d


All the code is in the namespace `homog2d`, so either add `using namespace homog2d`, either use it as a prefix on each type.

- Create a 2D point:
```
Point2d pt1;       // 0,0
Point2d pt2(3,4);
```

- Create a 2D line:
```
Line2d li1; // vertical line at x=0
Line2d li2( pt1, pt2 ); // from two points:
```

- Create a 2D point from two lines:
```
Point2d pt3(li1,li2);
```



