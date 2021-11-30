#!/bin/bash
function conv
{
	echo " -processing $1"
	convert -delay 12 -loop 0 BUILD/$1_*.png BUILD/$1.gif
	rm BUILD/$1_*.png
}

#for f in BUILD/showcase/*
#do
#	conv $(basename $f)
#done

conv showcase1
conv showcase2a
conv showcase2b
conv showcase3

