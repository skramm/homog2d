# homog2d

Current test status:
[![Build Status](https://travis-ci.com/skramm/homog2d.svg?branch=master)](https://travis-ci.com/skramm/homog2d)

[![License: MPLv2](https://img.shields.io/badge/licence-MPLv2-brightgreen)](https://www.mozilla.org/en-US/MPL/)

A small single-file header-only C++ library dedicated to
handling 2D lines, points and homographies (2D transformations),
using (internally) homogeneous coordinates.

- Language: C++11 (or later)
- Home: https://github.com/skramm/homog2d
- Usage: just fetch the file `homog2d.hpp`, put it somewhere, and "#include" it in your source file.
- Status: beta
- Author: S. Kramm
- Licence: MPLv2
- Dependencies: none, other than standard library

Short preview:

```C++
#include "homog2d.hpp"
int main()
{
	Line2d l1( Point2d(),     Point2d(10,10) );  // a line from (0,0) to (10,10)
	Line2d l2( Point2d(0,10), Point2d(10,0) );   // a line from (0,10) to (10,0)
	Point2d pt = l1 * l2;                        // intersection point (5,5)
	Homogr H(2,3);                               // a translation matrix
	std::cout << H * pt;                         // prints [7,8]
}
```

A test file is provided, needs [Catch](https://github.com/catchorg/Catch2).
When installed, run `make` (or `make test`).

To install on your machine, copy `homog2d.hpp` somewhere, or `sudo make install`.
This will copy that file in `/usr/local/include`.

Usage: see [manual](manual.md)

Reference: once downloaded, enter `make doc` (requires Doxygen).

Rationale:
- Usage simplicity (see manual)
- No dependency
- Modern C++, using policy-based design and tag dispatching
- Direct bindings with [OpenCv](https://opencv.org/) (optional)

Related libraries:
 - [Wykobi](http://www.wykobi.com/), has much more features but maybe harder to dive in.

