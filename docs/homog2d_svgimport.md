## Technical details on svg file import


SVG Reference: https://www.w3.org/TR/SVG2/shapes.html

A minimal import code is present, it relies on the well-known
[tinyxml2](https://github.com/leethomason/tinyxml2)
library, thus it needs to be present on machine to build an application using this.
On Debian-based distros, this can be easily done with
`$ sudo apt install libtinyxml2-dev`

To enable its usage in your code, you need to do two things:
* define the symbol `HOMOG2D_USE_SVG_IMPORT` before including `homog2d.hpp`
* add the library to the compile line.
That can be done by adding this to the linking command-line:
```
$(pkg-config --libs tinyxml2)
```

The following points must be considered:

* All the color, style,etc. Svg attributes present in file are lost, as this library does not store them.
* All groups (`<g>` tag) are ignored.
* Svg has no "line" object, what is called a line is actually a segment and it will be imported as such.
* Svg has no "point" object, thus it cannot be imported as such.
However, if you try to import a file that was created with the Svg drawing subsystem, points will be plotted with a shape defined by
[its parameters](https://github.com/skramm/homog2d/blob/master/docs/homog2d_manual.md#83---drawing-parameters).
* Svg has two ways to define a polyline (or polygon):
either with the dedicated keywords
([`polyline`](https://www.w3.org/TR/SVG2/shapes.html#PolylineElement)
or [`polygon`](https://www.w3.org/TR/SVG2/shapes.html#PolygonElement))
, or by using the
[`path`](https://www.w3.org/TR/SVG2/paths.html#PathElement) element.
<br>
At present this import subsystem only handles the `polyline`/`polygon` elements.


