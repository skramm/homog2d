# homog2d

A very small single-file header-only C++ library dedicated to
handling 2d lines, points and homographies (2D transformations),
using (internally) homogeneous coordinates.

- Language: C++11
- Home: https://github.com/skramm/homog2d
- Usage: just fetch the file `homog2d.hpp`, put it somewhere, and get going.
- Status: beta
- Author: S. Kramm
- Licence: GPL v3
- Dependencies: none, other than standard library

A test file is provided, needs [Catch](https://github.com/catchorg/Catch2).
When installed, run `make` (or `make test`).

To install on your machine, copy `homog2d.hpp` somewhere, or `sudo make install`.
This will copy in `/usr/local/include`.

Usage: see [manual](manual.md)

Rationale: usage simplicity (see manual), and no dependency!

[![Build Status](https://travis-ci.com/skramm/homog2d.svg?branch=master)](https://travis-ci.com/skramm/homog2d)

