.PHONY: doc test install demo

#TOOL=$(CXX)
TOOL=clang++
CFLAGS += -std=c++11 -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic

ifeq "$(USE_OPENCV)" ""
	USE_OPENCV=N
endif

ifeq ($(USE_OPENCV),Y)
	CFLAGS += -DHOMOG2D_USE_OPENCV
	LDFLAGS += `pkg-config --libs opencv`
endif

ifeq ($(USE_EIGEN),Y)
	CFLAGS += -DHOMOG2D_USE_EIGEN
endif

DOC_IMAGES_LOC:=docs/figures_src
DOC_IMAGES_SRC:=$(wildcard $(DOC_IMAGES_LOC)/*.cpp)
DOC_IMAGES_PNG:=$(patsubst $(DOC_IMAGES_LOC)/%.cpp,$(DOC_IMAGES_LOC)/%.png, $(DOC_IMAGES_SRC))

show:
	@echo "DOC_IMAGES_LOC=$(DOC_IMAGES_LOC)"
	@echo "DOC_IMAGES_SRC=$(DOC_IMAGES_SRC)"
	@echo "DOC_IMAGES_PNG=$(DOC_IMAGES_PNG)"

test: homog2d_test nobuild #demo_check
	@echo "Make: run test, build using $(CXX)"
	./homog2d_test

testall: homog2d_test_f homog2d_test_d homog2d_test_l
	@echo "Make: run testall, build using $(CXX)"
	misc/test_all.sh

# compute code coverage (EXPERIMENTAL !)
cov:
	gcov -m -f -r -i homog2d_test.cpp >gcov_stdout

check:
	cppcheck . --enable=all -DHOMOG2D_INUMTYPE=double --std=c++11 2>cppcheck.log
	xdg-open cppcheck.log


demo_check: misc/demo_check.cpp homog2d.hpp Makefile
	$(CXX) $(CFLAGS) -I. -o demo_check misc/demo_check.cpp

# 2019-11-15: added options for code coverage with gcov
homog2d_test: misc/homog2d_test.cpp homog2d.hpp Makefile
	$(CXX) $(CFLAGS) -O2 -o homog2d_test $< $(LDFLAGS)
#	$(CXX) $(CFLAGS) -O0 -g --coverage -o homog2d_test $< $(LDFLAGS)

homog2d_test_f: misc/homog2d_test.cpp homog2d.hpp
	$(CXX) $(CFLAGS) -DNUMTYPE=float -O2 -o $@ $< $(LDFLAGS)

homog2d_test_d: misc/homog2d_test.cpp homog2d.hpp
	$(CXX) $(CFLAGS) -DNUMTYPE=double -O2 -o $@ $< $(LDFLAGS)

homog2d_test_l: misc/homog2d_test.cpp homog2d.hpp
	$(CXX) $(CFLAGS) "-DHOMOG2D_INUMTYPE=long double" "-DNUMTYPE=long double" -O2 -o $@ $< $(LDFLAGS)

ptest1: precision_test1
	./precision_test1

ptest2: precision_test2
	./precision_test2

precision_test1: misc/precision_test_opencv.cpp
	$(CXX) $(CFLAGS) `pkg-config --cflags opencv` -I. -o $@ $< `pkg-config --libs opencv`

precision_test2: misc/precision_test.cpp
	$(CXX) $(CFLAGS) -I. -o $@ $<

# run the program
$(DOC_IMAGES_LOC)/%.png: $(DOC_IMAGES_LOC)/%
	$<
	mv $(notdir $@) $(DOC_IMAGES_LOC)/

# build the program
$(DOC_IMAGES_LOC)/%: $(DOC_IMAGES_LOC)/%.cpp
	$(CXX) $(CFLAGS) `pkg-config --cflags opencv` -I. -o $@ $< `pkg-config --libs opencv`

doc_fig: $(DOC_IMAGES_PNG)

doc: html/index.html doc_fig
	xdg-open html/index.html

html/index.html: misc/homog2d_test.cpp homog2d.hpp misc/doxyfile README.md docs/homog2d_manual.md
	doxygen misc/doxyfile

install:
	cp homog2d.hpp /usr/local/include

demo: demo_opencv
	./demo_opencv

# this target REQUIRES Opencv, no will attempt to build even when USE_OPENCV not given
demo_opencv: misc/demo_opencv.cpp homog2d.hpp
	$(CXX) $(CFLAGS) `pkg-config --cflags opencv` -I. -o $@ $< `pkg-config --libs opencv`

demo_sdl2: misc/demo_sdl2.cpp homog2d.hpp
	$(CXX) $(CFLAGS) `pkg-config --cflags sdl2` -I. -o demo_sdl2 $< `pkg-config --libs sdl2`

diff:
	git diff | colordiff | aha > diff.html
	xdg-open diff.html

#=================================================================
# The following is used to make sure that some constructions will not build

NOBUILD_SRC_FILES := $(notdir $(wildcard misc/no_build/*.cxx))
NOBUILD_OBJ_FILES := $(patsubst %.cxx, /tmp/%.o, $(NOBUILD_SRC_FILES))

#.PRECIOUS: /tmp/no_build_%.cpp

nobuild: $(NOBUILD_OBJ_FILES)
	@echo "done target $@"

$(NOBUILD_OBJ_FILES): rm_nb

rm_nb:
	-rm *.stdout
	-rm *.stderr

# assemble file to create a cpp program holding a main()
/tmp/no_build_%.cpp: misc/no_build/no_build_%.cxx
	@cat misc/no_build/header.txt >/tmp/$(notdir $@)
	@cat $< >>/tmp/$(notdir $@)
	@cat misc/no_build/footer.txt >>/tmp/$(notdir $@)

# compile, and return 0 if compilation fails (which is supposed to happen)
/tmp/no_build_%.o: /tmp/no_build_%.cpp
	@echo "Checking build failure of $<" >>no_build.stdout
	@echo -e "-----------------------------\nChecking build failure of $(notdir $<)\n" >>no_build.stderr
	! $(CXX) -o $@ -c $< 1>>no_build.stdout 2>>no_build.stderr

#=================================================================

clean:
	-rm -r html/*
	-rm homog2d_test
	-rm demo_opencv
	-rm *.gcov
