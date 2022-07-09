#!/bin/bash
# S. Kramm, LITIS Rouen - 20220709

# This script will proceed a clone of the repo,
# then iterate throuh all the commits of the master branch, and count the
# number of lines of the main library file and the test file, and store
# these in file: BUILD/git_log_1.dat
# That file will be plotted with the script:
# misc/plot_line_count.plt

git log --pretty=format:"%h %ad" --date=short >BUILD/git_log_1.dat
mkdir -p BUILD/tmp
cd BUILD/tmp
pwd
git clone https://github.com/skramm/homog2d.git
echo "# commit number of lines "> line_count.dat
cd homog2d
while IFS=' ' read -r commit cdate;
do
	echo "processing2 $commit"
	git checkout $commit
	nb1=$(wc -l <homog2d.hpp)
	nb2=$(wc -l <misc/homog2d_test.cpp)
	echo "$commit $cdate $nb1 $nb2">>../line_count.dat
	read
done < ../../git_log_1.dat

pwd
echo "reading to erase?"
read
rm -r *

