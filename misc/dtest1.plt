#!/usr/bin/env gnuplot

set terminal pngcairo size 800,500
set output "BUILD/dtest1.png"

set datafile commentschars "#"
set datafile separator ";"

set title "Distance value from a line to the points its was generated from"
set logscale xy
set xlabel "Point coordinates range"
set ylabel "Distance value"
set key left top box

set format y "%.0e"
set format x "%.0e"
set grid
plot "BUILD/dtest.data" \
	using 1:6 with linespoints lw 2 ti "Max value", \
	"" using 1:7 with linespoints lw 2 ti "Mean value"


