## Computation time information

[Manual main page](homog2d_manual.md)

As detailed [in manual](homog2d_manual.md#build_options), the
`HOMOG2D_OPTIMIZE_SPEED` build option will favor speed over memory footprint.
At present, this only has an impact on the `Ellipse_` class behavior.

With current commit (2022-11-10), and using the
`$ make speed_test`
command that builds and runs the program `ellipse_speed_test.cpp` with different options, the results are as follow:

| Build Option | Ellipse footprint (bytes) |  Duration (s)  |
|--------------|---------------------------|----------------|
| (none)                                       |  88 | 4.75 |
| HOMOG2D_OPTIMIZE_SPEED                       | 160 | 3.32 |
| HOMOG2D_OPTIMIZE_SPEED <br> HOMOG2D_NOCHECKS | 160 | 2.26 |

- Test machine: Dell 5510 core I7, Ubuntu 20.04
- Number of runs: 1E7 (default value)

Those values are just to have an idea and are variable with several runs.
(And are of course dependent of machine.)

