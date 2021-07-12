# Questions & Answers

Home page: https://github.com/skramm/homog2d

---

- Q: Is this library usable on a Windows platform?
- A: I don't see any reason why that would not be the case, as long as you are able to handle the inclusion of a file in your build process.
However, all the additional stuff here (Opencv graphical demo, test files, ...) will probably not build "out of the box", and will require some build tweaking.
---

- Q: I have an unexpected assert that gets triggered when I use this in my code.
What shall I do?
- A: Unfortunately, this can still happen at this time, and it is probably due to some numerical issue.
The best thing to do is to add `#define HOMOG2D_DEBUGMODE` just above `#include "homog2d.hpp"` before and log the error stream of your program:<br>
`./myapp 2>stderr` <br>
and open an issue [here](https://github.com/skramm/homog2d/issues) with this output (after removing non relevant parts), so this can be checked and corrected.
---

- Q: I get a lot of warnings (`...may be used uninitialized in this function`) when running the test suite with GCC, why?
- A: This seems to be an issue whith Catch, regular program shouldn't issue those, no worry
(this doesn't seem to happen with clang).

---

- Q: what is the difference between the types `Homog2d` and `Epipmat`?
- A: the two hold a 3x3 homogeneous matrix. The differences is on the usage and the default values:
  - With `Homog2d`, the default is a unit transformation, i.e. an "eye" matrix (null everywhere except the diagonal with ones).
  - With `Epipmat`, the default is a standard horizontally aligned epipolar geometry:
```
0  0  0
0  0  1
0  1  0
```
On usage:
  - `Homog2d` is designed to hold a planar transformation, i.e. mapping a point to a point and a line to a line.
  - `Epipmat` is designed to handle epipolar mapping in the case of stereovision situations for example, i.e. a point will be mapped to a line,
 and a line will be mapped to a point.

 ```C++
Epipmat mat;
Point2d pt;
Line2d li;
auto a = mat * pt; // a is a line !
auto b = mat * li; // b is a point !
```
---
