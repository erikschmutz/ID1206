set xlabel "Amount of allocations"
set ylabel "Time in ns"
set title "Merging vs Not Merging 75% of the time free before end"

set term png
set datafile separator ","
set output "assets/merge-vs-no-merge.png"

set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5

set style line 2 \
    linecolor rgb '#ccc0ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5




plot "assets/bench-no-merge.csv"        with linespoints linestyle 2 title 'No Merge',    \
     "assets/bench-merge.csv"           with linespoints linestyle 1 title 'Merge'                  \

