# Information on used algorithms

[Manual main page](homog2d_manual.md)

This page will give some details on how things are done.

## Intersection algorithms

### Line/Line, Line/Segment, Segment/Segment

For Line/Line intersection, this simply checks that lines are not parallel and if not, it returns the product of the two lines.

For Segment/Line, it first fetches the supporting line of the segment, and compute the intersection point using the above function.
Then, it checks if the intersection point is lying on the segment.

This is done by computing the sum of the two distances between the two points of the segment and the intersection point, and comparing that value to the segment length.
If it is higher than the segment length, then the intersection point does not cross the segment.

Consider for example the figure below:

![Example of line/segment intersection](img/segment_intersect_1.png)

We compute the distances d(ac) and d(bc).
Here, their sum is obviously higher than the segment length, thus there is no intersection between segment `[ab]` and the line.

For Segment/Segment, the checking is done twice, once for each segment.

A threshold is used to compare theses distance, the computation is actually:
```
if( d(ac) + d(bc) - segment_length < thr::nullDistance() )
	... then intersection point
```

See [page on thresholds](homog2d_thresholds.md) for details.



### Line/Circle

We use here a geometric approach, described in [this document](http://skramm.lautre.net/files/misc/intersect_circle_line.pdf).

This basically restates what is [described here](https://cp-algorithms.com/geometry/circle-line-intersection.html).


### Segment/Circle

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






