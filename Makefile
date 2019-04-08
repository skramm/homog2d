

test: homog2d_test
	./homog2d_test


homog2d_test: homog2d_test.cpp homog2d.hpp
	$(CXX) -o homog2d_test homog2d_test.cpp

