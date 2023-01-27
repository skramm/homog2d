#!/usr/bin/env bash

# generates a plot of the number of LOC per release

set +x

git clone git@github.com:skramm/homog2d.git /tmp/homog2d
pushd /tmp/homog2d

rm tags_linecount.csv
taglist=$(git tag -l)
for t in $taglist
do
#	echo "tag=$t"
	p=""
	if [ "$t" != "v1.0" ]; then p=misc/; fi
	git checkout $t
	c1=$(cat homog2d.hpp|wc -l)
	c2=$(cat ${p}homog2d_test.cpp|wc -l)
	echo "$t;$c1;$c2">>tags_linecount.csv
done

popd
./plot_tags_linecount.plt
