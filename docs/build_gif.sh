#!/bin/bash
function conv
{
	echo " -processing $1"
	convert -delay 12 -loop 0 BUILD/showcase/$1_*.png BUILD/showcase/$1.gif
	rm BUILD/showcase/$1_*.png
}

#for f in BUILD/showcase/*
#do
#	conv $(basename $f)
#done
#mkdir -p BUILD/showcase

conv showcase1
conv showcase2a
conv showcase2b
conv showcase3
conv showcase4

