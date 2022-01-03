## Computation time information

[Manual main page](homog2d_manual.md)

As detailed [in manual](homog2d_manual.md#build_options), the
`HOMOG2D_OPTIMIZE_SPEED` build option will favor speed over memory footprint.
At present, this only has an impact on the `Ellipse_` class behavior.

With current commit (2021-08-04), and using the
`$ make speed_test`
command that builds and runs the program `ellipse_speed_test.cpp` with different options, the results are as follow:

| Build Option | Ellipse footprint (bytes) |  Duration (s)  |
|--------------|---------------------------|----------------|
| (none)                                       |  80 | 6.7  |
| HOMOG2D_OPTIMIZE_SPEED                       | 152 | 4.38 |
| HOMOG2D_OPTIMIZE_SPEED <br> HOMOG2D_NOCHECKS | 152 | 2.72 |

- Test machine: Dell 5510 core I7, Ubuntu 18.04
- Number of runs: 1E7 (default value)

Those values are just to have an idea and are variable with several runs.
(And are of course dependent of machine.)

