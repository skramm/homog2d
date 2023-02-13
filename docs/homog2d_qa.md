# Questions & Answers

[Manual main page](homog2d_manual.md)

<dl>
<dt>
Q: Is this library usable on a Windows platform?
</dt>
<dd>
A: I don't see any reason why that would not be the case, as long as you are able to handle the inclusion of a file in your build process.
However, all the additional stuff here (Opencv graphical demo, test files, ...) will probably not build "out of the box", and will require some build tweaking.
</dd>

<dt>
Q: How do I know the version that I have installed on my machine?
</dt>
<dd>
A: easy, add this in your app (or check for that symbol in the file).
<pre>
   std::cout << "version: " << HOMOG2D_VERSION << '\\n';
</pre>
</dd>

<dt>
Q: Why the choice of <tt>ttmath</tt> (https://www.ttmath.org/) as "big numbers" library?
</dt>
<dd>
A: Sure, other choices were possible. But it matched all these criterions:<br>
- C++ (GMP and MPFR have a C API)<br>
- header only (so really easy to install)<br>
- compatible licence (BSD)<br>
- used by other projects (Boost)<br>
- reasonnably easy API
</dd>

<a name="assert_trigger"></a>
<dt>
Q: I have an unexpected assert that gets triggered when I use this library in my code.
What shall I do?
</dt>
<dd>
A: Unfortunately, this can still happen at this time, and it is probably due to some numerical issue.
It is difficult to consider all the possible situations in test cases, so that one just got through.<br>
To get this solved,
please add <code>#define HOMOG2D_DEBUGMODE</code> just above <code>#include "homog2d.hpp"</code>,
and log the error stream of your program:<br>
<code>./myapp 2>stderr</code><br>
then open an issue (https://github.com/skramm/homog2d/issues) and include that output after removing non relevant parts, so this can be checked and corrected.
</dd>

<dt>
Q: I get a lot of warnings (`...may be used uninitialized in this function`) when running the test suite with GCC with `$ make test`, why?
</dt>
<dd>
A: This seems to be an issue with Catch, regular program shouldn't issue those, no worry
(this doesn't seem to happen with clang).
</dd>


<dt>
Q: why do
<code>unionArea()</code> and <code>intersectArea()</code> not return the same data type?
</dt>
<dd>
A: because the first function returns a (flat) rectangle, and the second returns a polygon (as a closed <code>Polyline</code> object).
However, both can fail (if there is no common part of course), but you cannot return an invalid <code>FRect</code> object.
Thus the first function returns a type that can be checked for success.
A contrario, the second function will return a <code>Polyline</code> object, and in case of failure, it will just be empty, which is perfectly valid.
</dd>


<dt>
Q: Circle_::center() and FRect_::getCenter()? Why not the same identifier?
</dt>
<dd>
A: because the first function returns a reference, and can be used to edit the value.
A contrario, the second function returns a value, and cannot be used to edit the value.
Thus, the intent is clearer.
</dd>





<dt>
Q: I notice the repo has a lot of branches. What's the point with all these branches?
</dt>
<dd>
A: Probably bad branch managment...
As "branching is free", I tend to create branches on the fly, but sometimes forget, and/or they are too different to sync up, so I just let them go.
But the point to remember is that "master" will always stay clean (build and tests ok), and will regularly be updated with bugfixes and new features.
</dd>

<dt>
Q: Any plans to deliver this as a (deb or rpm) package?
</dt>
<dd>
A: As the whole library is contained in a single file, there's not much difference between downloading  it or a package, so, no, not at present due to lack of time.
<br>
But if you feel like adding required code to build a package through a PR, you are welcome!
</dd>


<dt>
Q: Why Opencv as drawing backend?
</dt>
<dd>
-Short answer: because it works fine!
<br>
-Long(er) answer:
Besides the fact that I have already used it with other projects, it also has the great advantage of having all the drawing code for "advanced" primitives (circle, ellipse).
This doesn't seem to be the case for, say SDL2 (https://www.libsdl.org/) or other backends.
<br>
Alternatively, the library provides also SVG drawing.
</dd>

<dt>
Q: Why do yo use (Gnu)Make as build tool (and not CMake or another fancy thing)?
</dt>
<dd>
A: Because:<br>
 1. it is really powerful<br>
 2. there is no need for more<br>
 3. I like it

(Actually, I have the feeling that most people criticizing that tool never really understood how it works.)

Anyway, the library user doesn't need to use it:
as this is a "header-only" lib, he can use the tool he wants.
</dd>


</dl>

