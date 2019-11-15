.PHONY: doc test install demo


CFLAGS += -std=c++11 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic

ifeq "$(USE_OPENCV)" ""
	USE_OPENCV=N
endif

ifeq ($(USE_OPENCV),Y)
	CFLAGS += -DHOMOG2D_USE_OPENCV
	LDFLAGS += `pkg-config --libs opencv`
endif


test: homog2d_test demo_check
	./homog2d_test

# compute code coverage (EXPERIMENTAL !)
cov:
	gcov -m -f -r -i homog2d_test.cpp >gcov_stdout

demo_check: misc/demo_check.cpp homog2d.hpp Makefile
	$(CXX) $(CFLAGS) -I. -o demo_check misc/demo_check.cpp

# 2019-11-15: added options for code coverage with gcov
homog2d_test: misc/homog2d_test.cpp homog2d.hpp Makefile
#	$(CXX) $(CFLAGS) -o homog2d_test misc/homog2d_test.cpp $(LDFLAGS)
	$(CXX) $(CFLAGS) -O0 -g --coverage -o homog2d_test misc/homog2d_test.cpp $(LDFLAGS)

doc: html/index.html
	@echo "done !"

html/index.html: misc/homog2d_test.cpp homog2d.hpp misc/doxyfile README.md manual.md
	doxygen misc/doxyfile

install:
	cp homog2d.hpp /usr/local/include

demo: demo_opencv
	./demo_opencv

diff:
	git diff | colordiff | aha > diff.html
	xdg-open diff.html

# this target REQUIRES Opencv, no will attempt to build even when USE_OPENCV not given
demo_opencv: misc/demo_opencv.cpp homog2d.hpp
	$(CXX) $(CFLAGS) -I. -o demo_opencv misc/demo_opencv.cpp `pkg-config --libs opencv`

clean:
	-rm -r html/*
	-rm homog2d_test
	-rm demo_opencv
	-rm *.gcov
