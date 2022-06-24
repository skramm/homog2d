# Information on used algorithms

[Manual main page](homog2d_manual.md)

This page will give some details on how things are done.

## 1 - Intersection algorithms

### 1.1 - Line/Line, Line/Segment, Segment/Segment

For Line/Line intersection, this simply checks that lines are not parallel and if not, it returns the intersection point,
defined as the product of the two lines.

For Segment/Line, we first fetch the supporting line of the segment, and compute the intersection point using the above function.
Then, we check if the intersection point is lying on the segment.

This is done by computing the sum of the two distances between the two points of the segment and the intersection point, and comparing that value to the segment length.
If it is higher than the segment length, then the intersection point does not cross the segment.

Consider for example the figure below:

![Example of line/segment intersection](img/segment_intersect_1.png)

We compute the distances d(ac) and d(bc).
Here, their sum is obviously higher than the segment length, thus there is no intersection between segment `[ab]` and the line.

For Segment/Segment, the checking is done twice, once for each segment.

A threshold is used to compare theses distances, the computation is actually (pseudocode):
```
if( | d(ac) + d(bc) - segment_length | < thr::nullDistance() )
	... then intersection point
```

See [page on thresholds](homog2d_thresholds.md) for details on how to manage the threshold value.

### 1.2 - Line / Circle

We use here a geometric approach, described in [this document](http://skramm.lautre.net/files/misc/intersect_circle_line.pdf).
This basically restates what is [described here](https://cp-algorithms.com/geometry/circle-line-intersection.html).


### 1.3 - Segment / Circle

The first step is to check if the segments supporting line has an intersection with the circle, using the above algorithm.
  * If there is no interesection, then we can stop.
  * If these is one, then several situations need to be considered:

![segintcir1a](img/segment_intersect_circle_1a.png)
![segintcir1b](img/segment_intersect_circle_1b.png)
![segintcir1c](img/segment_intersect_circle_1c.png)

We first determine the status of each point of the segment: are they inside or outside(*) of the circle.

  * If the two points are inside, then there are no intersections.
  * If the two points are outside, then we have two intersection points, that are computed with the algorithm described above.
  * If one point is inside and the other outside, then we have only one intersection.
Determining which one of the two points is the correct one if done by checking which point of the segment is inside the circle.

(*) for clarity, we do not detail here the case where points are on the edge of the circle, but this is taken into account in the code.


### 1.4 - FRect / FRect

Each segment of one of the rectangles if compared to each segment of the other rectangle.
Thus we have between 0 and 4 intersections (equal intersection points are removed).


## 2 - Enclosing algorithms

The table below summarizes what type can be used to check if it is inside another object of same or different type.

|           | Point2d | Line2d | Segment | Frect | CPolyline | OPolyline | Circle | Ellipse |
|-----------|---------|--------|---------|-------|-----------|-----------|--------|---------|
| Point2d   |    N    |    N   |    N    |   Y   |     Y     | Y (false) |    Y   |    Y    |
| Line2d    |    N    |    N   |    N    |   N   |     N     |     N     |    N   |    N    |
| Segment   |    N    |    N   |    N    |   Y   |     Y     | Y (false) |    Y   |    Y    |
| FRect     |    N    |    N   |    N    |   Y   |     Y     | Y (false) |    Y   |    Y    |
| CPolyline |    N    |    N   |    N    |   Y   |     Y     | Y (false) |    Y   |    Y    |
| OPolyline |    N    |    N   |    N    |   Y   |     Y     | Y (false) |    Y   |    Y    |
| Circle    |    N    |    N   |    N    |   Y   |     Y     | Y (false) |    Y   |    Y    |
| Ellipse   |    N    |    N   |    N    |   Y   |     Y     | Y (false) |    Y   |    Y    |





### 2.1 - Point inside Polygon

The algorithm will provide an answer to the question "is this point inside this polygon"
So first, as polygons are implemented through the class `CPolyline_`, it will return `false`
if it does not meet the "polygon" requirements (i.e. if there are some segments crossings).

The code uses the classical "ray casting" algorithm 
(see https://en.wikipedia.org/wiki/Point_in_polygon):
we build a segment going from the considered point to infinity, and count the number of intersections.
It it is even, the point is outside, else it is inside.

However, while mathematically exact, this algorithm needs to be implemented with caution, to avoid numerical issues.

- First, we check if the point is outside the bounding box of the polyline.
If so, we return `false`.
- Second, the implementation checks if the point lies on any of the segments of the polygon.
This is done by measuring the distance between the point and the line supporting the segment.
If that distance is less than `thr::nullDistance()` (see [homog2d_thresholds.md](homog2d_thresholds.md) )  then we return `false`.
- next, we need to find some point outside the polygon to build the "test" segment.
Instead of taking a point at infinity, and to avoid any corner cases, we consider the middle point of the segment of the bounding box that is the farthest away from the considered point.






