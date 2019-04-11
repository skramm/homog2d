

test: homog2d_test demo
	./homog2d_test


demo: demo.cpp homog2d.hpp
	$(CXX) -std=c++11 -o demo demo.cpp

homog2d_test: homog2d_test.cpp homog2d.hpp
	$(CXX) -std=c++11 -o homog2d_test homog2d_test.cpp

doc: html/index.html
	@echo "done !"

html/index.html: homog2d_test.cpp homog2d.hpp doxyfile
	doxygen doxyfile

install:
	cp homog2d.hpp /usr/local/include

demo: demo_opencv

demo_opencv: demo_opencv.cpp homog2d.hpp
	$(CXX) -std=c++11 -o demo_opencv demo_opencv.cpp `pkg-config --libs opencv`

