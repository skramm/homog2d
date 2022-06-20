#!/bin/bash
set +x
echo " -processing $1"

filename=$(basename -- "$1")
ext="${filename##*.}"
fn="${filename%.*}"

#echo "filename=$filename"
#echo "ext=$filename"
#echo "fn=$filename"

convert -delay 12 -loop 0 BUILD/showcase/${fn}_*.png $1
#rm BUILD/showcase/${fn}_*.png

