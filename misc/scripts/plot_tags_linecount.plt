#!/usr/bin/env gnuplot

set terminal pngcairo
set output "linecount.png"
set datafile separator ";"
set boxwidth 0.5
set style fill solid
set style data histogram
set key on inside left top
set title "# lines per release"
plot "stdout" using 2:xtic(1) with histogram ti "homog2d.hpp", \
	"" using 3:xtic(1) with histogram ti "homog2d\_test.cpp"


