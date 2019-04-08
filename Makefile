

test: homog2d_test
	./homog2d_test


homog2d_test: homog2d_test.cpp
	$(CXX) -o homog2d_test homog2d_test.cpp
