#!/usr/bin/env gnuplot

# S. Kramm, LITIS Rouen - 20220709

# this file will generate a plot of the line count
# input data: file BUILD/tmp/line_count.dat
# that is generated with the script: generate_line_count_graph.sh

set datafile separator " "
set terminal pngcairo size 900,600
set output "BUILD/line_count.png"

#set terminal qt
#set grid xtics linetype 1 lw 1, linetype 1
set grid

#set logscale y
set key on left top box

set title "
set xtics rotate by 80 right
set format x "%b %y"

set timefmt "%Y-%m"
set xdata time

plot "BUILD/tmp/line_count.dat" \
	using 2:3 with linespoints pt 6 lw 1 title "Library file", \
	"" using 2:4 with linespoints pt 6 title "Test file"


