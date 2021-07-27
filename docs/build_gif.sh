#!/bin/bash
function conv
{
	echo " -processing $1"
	convert -delay 12 -loop 0 BUILD/$1_*.png BUILD/$1.gif
}

for f in BUILD/showcase/*
do
	conv $(basename $f)
done
