set xlabel "Amount of allocations"
set ylabel "Time in ns"
set title "Pointers to next on both vs pointer only when free"

set term png
set datafile separator ","
set output "assets/ptr-vs-no-ptr.png"

set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5

set style line 2 \
    linecolor rgb '#ccc0ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5




plot "assets/bench-no-ptr.csv"        with linespoints linestyle 2 title 'No pointers on taken',    \
     "assets/bench-merge.csv"           with linespoints linestyle 1 title 'Pointers on both taken and headers' \

