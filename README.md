# homog2d

Current test status:
[![Build Status](https://travis-ci.com/skramm/homog2d.svg?branch=master)](https://travis-ci.com/skramm/homog2d)

[![License: LGPL v3](https://img.shields.io/badge/License-LGPL%20v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)

A small single-file header-only C++ library dedicated to
handling 2D lines, points and homographies (2D transformations),
using (internally) homogeneous coordinates.

- Language: C++11
- Home: https://github.com/skramm/homog2d
- Usage: just fetch the file `homog2d.hpp`, put it somewhere, and "#include" it in your source file.
- Status: beta
- Author: S. Kramm
- Licence: LGPL v3
- Dependencies: none, other than standard library

A test file is provided, needs [Catch](https://github.com/catchorg/Catch2).
When installed, run `make` (or `make test`).

To install on your machine, copy `homog2d.hpp` somewhere, or `sudo make install`.
This will copy in `/usr/local/include`.

Usage: see [manual](manual.md)

Reference: once downloaded, enter `make doc` (requires Doxygen).

Rationale:
- Usage simplicity (see manual)
- No dependency
- Modern C++ policy-based design


