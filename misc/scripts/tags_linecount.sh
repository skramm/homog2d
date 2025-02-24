#!/usr/bin/env bash

# generates a plot of the number of LOC per release

set +x

rm -rf /tmp/homog2d/*
rm -rf /tmp/homog2d/.*
rmdir /tmp/homog2d/
git clone git@github.com:skramm/homog2d.git /tmp/homog2d
pushd /tmp/homog2d

if [ -e tags_linecount1.csv ]
then
	rm tags_linecount1.csv
fi
	
taglist=$(git tag -l)
for t in $taglist
do
#	echo "tag=$t"
	p=""
	if [ "$t" != "v1.0" ]; then p=misc/; fi
	git checkout $t
	c1=$(cat homog2d.hpp|wc -l)
	c2=$(cat ${p}homog2d_test.cpp|wc -l)
	c3=$(du --bytes homog2d.hpp | cut -f1)
	c4=$(du --bytes ${p}homog2d_test.cpp | cut -f1)
	c5=$(grep TEST_CASE ${p}homog2d_test.cpp | wc -l)
	c6=$(grep CHECK ${p}homog2d_test.cpp | wc -l)
	echo "$t;$c1;$c2;$c3;$c4;$c5;$c6">>tags_linecount1.csv
done

# -V: enables sorting by version number (i.e. v2.10 if AFTER v2.9)
sort -V tags_linecount1.csv > tags_linecount.csv

# get dates of tags
# preliminar: need to check how to get date as ISO
git for-each-ref --format="%(refname:short) | %(creatordate)" "refs/tags/*" > tags_dates.csv

popd
./plot_tags_linecount.plt



