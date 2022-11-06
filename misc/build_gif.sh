#!/bin/bash

# This file is used to build the final gif for showcase demos.
# It relies on Imagemagick, so must be installed.

# This script is called by `make` for target `showcase`.

set +x
echo " -processing $1"

filename=$(basename -- "$1")
ext="${filename##*.}"
fn="${filename%.*}"

#echo "filename=$filename"
#echo "ext=$filename"
#echo "fn=$filename"

convert -delay 12 -loop 0 BUILD/showcase/${fn}_*.png $1
rm BUILD/showcase/${fn}_*.png

