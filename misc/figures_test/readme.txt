This folder holds some code use in the test app.
The goal is to both:
- include these lines in the test app, followed by some checkings
- use these lines to build the associated figure, to make it more understandable

To build the associated figures, enter `$ make test_fig`
and check BUILD/figures_test

This target will produce a valid cpp file, by concatenating
a header, a footer, and the considered *.code file,
then compile it, then run it to generate the .svg file.


This target DOES NOT require Opencv, as it only builds "text" files (SVG images).

