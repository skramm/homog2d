.PHONY: doc test install demo


CFLAGS += -std=c++11 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic

ifeq "$(USE_OPENCV)" ""
	USE_OPENCV=N
endif

ifeq ($(USE_OPENCV),Y)
	CFLAGS += -DHOMOG2D_USE_OPENCV
	LDFLAGS += `pkg-config --libs opencv`
endif


test: homog2d_test #demo_check
	@echo "Run tests"
	./homog2d_test

# compute code coverage (EXPERIMENTAL !)
cov:
	gcov -m -f -r -i homog2d_test.cpp >gcov_stdout

check:
	cppcheck . --enable=all --std=c++11 2>cppcheck.log
	xdg-open cppcheck.log


demo_check: misc/demo_check.cpp homog2d.hpp Makefile
	$(CXX) $(CFLAGS) -I. -o demo_check misc/demo_check.cpp

# 2019-11-15: added options for code coverage with gcov
homog2d_test: misc/homog2d_test.cpp homog2d.hpp Makefile
#	$(CXX) $(CFLAGS) -O0 -g --coverage -o homog2d_test $< $(LDFLAGS)
	$(CXX) $(CFLAGS) -O2 -o $@ $< $(LDFLAGS)

doc: html/index.html
	xdg-open html/index.html

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
	$(CXX) $(CFLAGS) `pkg-config --cflags opencv` -I. -o demo_opencv $< `pkg-config --libs opencv`

demo_sdl2: misc/demo_sdl2.cpp homog2d.hpp
	$(CXX) $(CFLAGS) `pkg-config --cflags sdl2` -I. -o demo_sdl2 $< `pkg-config --libs sdl2`

clean:
	-rm -r html/*
	-rm homog2d_test
	-rm demo_opencv
	-rm *.gcov
