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

