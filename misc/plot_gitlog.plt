#!/usr/bin/env gnuplot

# S. Kramm, LITIS Rouen - 20210714
# This script will generate a plot of the git repository commit activity


#set datafile separator " "
#set terminal pngcairo
#set output "gitlog.png"

#set terminal qt
set grid mxtics linetype 2 lw 1, linetype 3
set grid xtics linetype 2 lw 2, linetype 3

set grid mytics linetype 3 lw 1, linetype 3
set grid ytics linetype 4 lw 2, linetype 3

#set timefmt "%Y-%m-%d"
set timefmt "%Y-%m"
set xdata time
set ydata time

set style line 1 pointtype 2 pointsize 1

#set format x "%a\n%d\n%b"
set format x "%d %b %y"
set format y "%H:%M"

set xtics autofreq

set mxtics 7
set xtics "2019-01-22",3600*24*7 rotate by 80 right
set ytics 3600*2,3600*4

set xrange ["2019-01-22":*]
#set yrange [3600*6:*]

set terminal svg size 1400,500 dynamic
set output "docs/gitlog_2.svg"

#set terminal png size 1400,500
#set output "docs/gitlog_2.png"

plot "BUILD/git_log_2.dat" using (timecolumn(1, '%Y-%m-%d')):(timecolumn(2, '%H:%M:%S')) with linespoints pt 6 lw 2 notitle

#set terminal pngcairo size 1200,600
#set output "obj/gitlog_2.png"
#replot



