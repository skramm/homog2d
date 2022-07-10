# Graphical showcase

[Manual main page](homog2d_manual.md)

On this page, some graphical demos, all generated with programs in misc/showcase, with `$ make showcase`
<br>
Requires Opencv (to generate images) and ImageMagick (to generate the final gif).


* [Showcase 1](../misc/showcase/showcase1.cpp):The front page demo. The moving circle turns red when it is detected as inside the rectangle or the other circle.

![showcase1](showcase/showcase1.gif)


* [Showcase 3](../misc/showcase/showcase3.cpp):
A rotating Polyline object, its bounding box, and its intersection with a line.

![showcase3](showcase/showcase3.gif)


* [Showcase 4](../misc/showcase/showcase4a.cpp):
Two dancing objects (Circle and FRect) and their common bounding box.
Also show the rectangle's bounding circle and the circle's bounding box.

![showcase4](showcase/showcase4a.gif)


* [Showcase 2a](../misc/showcase/showcase2a.cpp) and [Showcase 2b](../misc/showcase/showcase2b.cpp):

Intersection and union areas of two rectangles.

![Example of intersection area](showcase/showcase2a.gif)
![Example of union area](showcase/showcase2b.gif)


* [Showcase 5](../misc/showcase/showcase5.cpp):
A rotating ellipse with its center point (green), its axis lines, and its two bounding boxes (one aligned with its axis, the other xy-aligned).

![showcase5](showcase/showcase5.gif)


* [Showcase 6](../misc/showcase/showcase6.cpp):
Building circles from two (blue) or three (red) points, with one of the points rotating in the image.
Also shows intersection of circles with a segment.

![showcase6](showcase/showcase6.gif)


* [Showcase 7](../misc/showcase/showcase7.cpp):
<a id="sc7"></a>
A demo showing when a closed polyline and a circle are inside each other.
Red and bold drawing when inside the other.
Also shows the intersection points between these two primitives.

![showcase7](showcase/showcase7.gif)

* [Showcase 7b](../misc/showcase/showcase7b.cpp):
<a id="sc7b"></a>
Same as above but with a rectangle and a cirle.

![showcase7b](showcase/showcase7b.gif)


* [Showcase 7c](../misc/showcase/showcase7c.cpp):
<a id="sc7c"></a>
Shows a moving segment that turns red and bold when inside a rectangle, a circle, and a closed polyline.

![showcase7c](showcase/showcase7c.gif)



* [Showcase 8](../misc/showcase/showcase8.cpp):
<a name="sc8"></a>
A rotating point, a fixed segment and a fixed line.
The blue segment is generated between the point and the line, using `getOrthogSegment()`.
The other segment shows the distance between the point and the fixed segment, with two alternate colors showing the case (orthogonal distance or distance to segment edge).

![showcase8](showcase/showcase8.gif)

