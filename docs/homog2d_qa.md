# Questions & Answers

Home page: https://github.com/skramm/homog2d


<dl>
<dt>
Q: Is this library usable on a Windows platform?
</dt>
<dd>
A: I don't see any reason why that would not be the case, as long as you are able to handle the inclusion of a file in your build process.
However, all the additional stuff here (Opencv graphical demo, test files, ...) will probably not build "out of the box", and will require some build tweaking.
</dd>

<a name="assert_trigger"></a>
<dt>
Q: I have an unexpected assert that gets triggered when I use this library in my code.
What shall I do?
</dt>
<dd>
A: Unfortunately, this can still happen at this time, and it is probably due to some numerical issue.
It is difficult to consider all the possible situations in test cases, so that one just got through.<br>
To get this solved, please add <code>#define HOMOG2D_DEBUGMODE</code> just above `#include "homog2d.hpp"`, and log the error stream of your program:<br>
`./myapp 2>stderr` <br>
then open an issue [here](https://github.com/skramm/homog2d/issues) and include that output (after removing non relevant parts), so this can be checked and corrected.
</dd>

<dt>
Q: I get a lot of warnings (`...may be used uninitialized in this function`) when running the test suite with GCC, why?
</dt>
<dd>
A: This seems to be an issue whith Catch, regular program shouldn't issue those, no worry
(this doesn't seem to happen with clang).
</dd>


<dt>
Q: why do
`unionArea()` and `intersectArea()` not return the same data type?
</dt>
<dd>
A: because the first function returns a (flat) rectangle, and the second returns a polygon (as a closed Polyline object).
However, both can fail (is there is no common part of course), but you cannot return an invalid FRect object.
Thus the first function returns a type that can be checked for success.
A contrario, the second function will return a Polyline, and in case of failure, it will just be empty, which is perfectly valid.
</dd>

</dl>

