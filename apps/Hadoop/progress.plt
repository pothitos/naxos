#!/usr/bin/gnuplot

set terminal svg size 1680,844 name "Search_Tree"
set output "progress.svg"
#set terminal epslatex color solid
#set output "progress.tex"
#set size 0.96,1

set tics nomirror
set ytics scale 0
unset xtics
set border 0

set yrange [] reverse
set ylabel "Time (seconds)"
set x2label "Search tree nodes"
set format x2 ""

plot "progress.dat" \
     index 0 using 2:1:($4-$2):($3-$1) with vectors title "Solve", \
  "" index 1 using 2:1:($4-$2):($3-$1) with vectors title "Simulate"
