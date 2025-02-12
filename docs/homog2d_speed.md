## Computation time information

[Manual main page](homog2d_manual.md)

As detailed [in manual](homog2d_manual.md#build_options), the
`HOMOG2D_OPTIMIZE_SPEED` build option will favor speed over memory footprint.
At present, this only has an impact on the `Ellipse_` class behavior.

With release 2.12 and using the
`speed-test` make target that
builds and runs the program [`ellipse_speed_test.cpp`](../misc/ellipse_speed_test.cpp) with different options, the results are as follow:

| Build Option | Ellipse footprint (bytes) |  Duration (s)  |
|--------------|---------------------------|----------------|
| (none)                                       |  80 | 4.75 |
| HOMOG2D_OPTIMIZE_SPEED                       | 152 | 3.32 |
| HOMOG2D_OPTIMIZE_SPEED <br> HOMOG2D_NOCHECKS | 152 | 2.26 |

- Test machine: Dell 5510 core I7, Ubuntu 22.04
- Number of runs: 1E7 (default value)

Those values are just to have an idea and are variable with several runs.
(And are of course dependent of machine.)

