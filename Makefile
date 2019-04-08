

test: homog2d_test
	./homog2d_test


homog2d_test: homog2d_test.cpp homog2d.hpp
	$(CXX) -o homog2d_test homog2d_test.cpp

doc: html/index.html
	@echo "done !"

html/index.html: homog2d_test.cpp homog2d.hpp doxyfile
	doxygen doxyfile

install:
	cp homog2d.hpp /usr/local/include
