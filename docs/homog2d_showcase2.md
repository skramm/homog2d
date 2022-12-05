# Other graphical showcase demos

[Manual main page](homog2d_manual.md)

On this page, some more graphical demos as images, all generated with programs in
[misc/showcase_v](../misc/showcase_v), with
```
$ make showcase2
```

This latter target only build the programs, but you can build png image files and corresponding videos with
```
$ misc/showcase_v/build_vid.sh
```
(Requires Opencv and ffmpeg)


1 - Spirograph-like generated images
<br>
[source](../misc/showcase_v/showcase_v_01.cpp)
<br>
[See here](../misc/showcase_v/run_parameters.txt) for argument examples

* Demo 1: (arguments: 10 0.4 1.6)

![Demo 1](showcase2/showcase_v_01_01_250.png)

* Demo 2: (arguments: 10 0.4 1.4)

![Demo 2](showcase2/showcase_v_01_02_200.png)
