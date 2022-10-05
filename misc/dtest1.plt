#!/usr/bin/env gnuplot

set terminal pngcairo size 800,500

set datafile commentschars "#"
set datafile separator ";"

set logscale xy
set xlabel "Point coordinates range"
set ylabel "Distance value"
set key left top box

set format y "%.0e"
set format x "%.0e"
set grid

set title "Distance value from a line to the points its was generated from\n(using 'float' as internal type)"
set output "BUILD/dtest1_f.png"
plot "BUILD/dtest1_f.data" \
	using 1:5 with linespoints lw 2 ti "Max value", \
	"" using 1:6 with linespoints lw 2 ti "Mean value"

set title "Distance value from a line to the points its was generated from\n(using 'double' as internal type)"
set output "BUILD/dtest1_d.png"
plot "BUILD/dtest1_d.data" \
	using 1:5 with linespoints lw 2 ti "Max value", \
	"" using 1:6 with linespoints lw 2 ti "Mean value"

set title "Distance value from a line to the points its was generated from\n(using 'long double' as internal type)"
set output "BUILD/dtest1_l.png"
plot "BUILD/dtest1_l.data" \
	using 1:5 with linespoints lw 2 ti "Max value", \
	"" using 1:6 with linespoints lw 2 ti "Mean value"



