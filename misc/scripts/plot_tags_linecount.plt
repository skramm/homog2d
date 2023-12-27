#!/usr/bin/env gnuplot

# tested with gnuplot 5

set terminal pngcairo
set datafile separator ";"
set boxwidth 0.5
set style fill solid
set style data histogram
set grid
#set logscale y
#set yrange [0.5:30]
set key on inside left top
set xtics rotate

set title "homog2d: KLOC per release"
set output "linecount.png"
plot "/tmp/homog2d/tags_linecount.csv" using (column(2)/1000):xtic(1) with histogram ti "Library file", \
	"" using (column(3)/1000):xtic(1) with histogram ti "Test file"

set title "homog2d: KBytes per release"
set output "bytecount.png"
plot "/tmp/homog2d/tags_linecount.csv" using (column(4)/1000):xtic(1) with histogram ti "Library file", \
	"" using (column(5)/1000):xtic(1) with histogram ti "Test file"
	


