#!/usr/bin/env gnuplot

# tested with gnuplot 5

set terminal pngcairo
set output "linecount.png"
set datafile separator ";"
set boxwidth 0.5
set style fill solid
set style data histogram
set grid
set key on inside left top
set title "KLOC per release"
plot "/tmp/homog2d/tags_linecount.csv" using (column(2)/1000):xtic(1) with histogram ti "Library file", \
	"" using (column(3)/1000):xtic(1) with histogram ti "Test file"


