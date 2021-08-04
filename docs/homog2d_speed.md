# Computation time information

A detailed [in manual](homog2d_manual.md#build_options), the
`HOMOG2D_OPTIMIZE_SPEED` build option will favor speed over memory footprint.

With current commit (2021-08-04), and using the
`$ make speed_test`
command that builds and runs the program [ellipse_speed_test.cpp](misc/ellipse_speed_test.cpp) with different options, the results are as follow:

| Build Option | Ellipse footprint (bytes) | Duration (s) |
|--------------|---------------------------|--------------|
| (none)        |      80                  |  7.09 |
| HOMOG2D_OPTIMIZE_SPEED | 152 | 4.77 |
| HOMOG2D_OPTIMIZE_SPEED <br> HOMOG2D_NOCHECKS | 152 | 3.33 |

- Test machine: Dell 5510 core I7, Ubuntu 18.04
- Number of runs: 1E7 (default value)

Those values are just to have an idea and are variable with several runs.
(And are of course dependent of machine.)
