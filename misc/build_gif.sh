#!/bin/bash

# This file is used to build the final gif for showcase demos.
# It relies on Imagemagick, so must be installed.

# This script is called by `make` for target `showcase`.

# first argument is the filename, second (optional) is the frame duration

set +x
echo " -processing $1 with frame duration=$2"

filename=$(basename -- "$1")
ext="${filename##*.}"
fn="${filename%.*}"

d=14
if [ "$2" != "" ]; then
	d=$2
fi

#echo "filename=$filename"
#echo "ext=$filename"
#echo "fn=$filename"

convert -delay $d -loop 0 BUILD/showcase/${fn}_*.png $1
#mv BUILD/showcase/${fn}.gif BUILD/showcase/gif/
#rm BUILD/showcase/${fn}_*.png

