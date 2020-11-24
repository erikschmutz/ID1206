set xlabel "Amount of allocations"
set ylabel "Length of list"
set title "When merging vs not merging"

set term png
set datafile separator ","
set output "assets/free-list-size.png"

set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5

set style line 2 \
    linecolor rgb '#ccc0ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5

plot "assets/length-bench-no-merge.csv"           with linespoints linestyle 2 title 'No pointers on taken',              \
     "assets/length-bench-merge.csv"            with linespoints linestyle 1 title 'Pointers on both taken and headers'  \

