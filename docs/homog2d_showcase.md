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
Also show the rectangle's bounding circle.

![showcase4](showcase/showcase4a.gif)


* [Showcase 2](../misc/showcase/showcase2.cpp):
Intersection and union areas of two rectangles.

![Example of intersection area](showcase/showcase2a.gif)
![Example of union area](showcase/showcase2b.gif)


* [Showcase 5](../misc/showcase/showcase5.cpp):
A rotating ellipse with its center point (green), its axis lines, and its two bounding boxes (one aligned with its axis, the other xy-aligned).

![showcase5](showcase/showcase5.gif)


* [Showcase 6](../misc/showcase/showcase6.cpp):
Building circles from two (blue) or three (red) points, with one of them rotating in the image.
Also shows intersection of circles with a line segment.

![showcase6](showcase/showcase6.gif)
