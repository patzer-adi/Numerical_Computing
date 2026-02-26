# Gnuplot Script — Plot x vs y from solution file
# ==================================================
#
# Usage:
#   gnuplot plot_solution.gp
#
# Custom file:
#   gnuplot -e "datafile='../solution_49.txt'" plot_solution.gp
#
# For 225 system (15x15 grid):
#   gnuplot -e "datafile='../solution_225.txt'; gridsize=15" plot_solution.gp

# === Input file ===
if (!exists("datafile")) datafile = "../solution_right.txt"

# === Plot 1: x vs y (index vs solution value) ===

set terminal pngcairo size 1200,600 enhanced font "Arial,14"
set output "solution_plot.png"

set title "Solution: x vs y" font ",18"
set xlabel "x  (index i)" font ",14"
set ylabel "y  (solution value x_i)" font ",14"
set grid
set key top right

plot datafile using ($0):1 title "y = f(x)" \
     with linespoints pt 7 ps 0.6 lw 1.5 lc rgb "#0066CC"

set output
print "Saved: solution_plot_225.png"


# === Plot 2: Heatmap (for grid-based solutions) ===

if (!exists("gridsize")) gridsize = 7

set terminal pngcairo size 800,800 enhanced font "Arial,14"
set output "solution_heatmap.png"

set title sprintf("Solution Heatmap (%dx%d grid)", gridsize, gridsize) font ",18"
set xlabel "x (column)" font ",14"
set ylabel "y (row)" font ",14"
set cblabel "value"

set palette defined (0 "dark-blue", 1 "blue", 2 "cyan", 3 "yellow", 4 "red")
set size square
set xrange [-0.5:gridsize-0.5]
set yrange [-0.5:gridsize-0.5]
unset key

plot datafile using (int($0) % gridsize):(int($0) / gridsize):1 \
     with points pt 5 ps 3 palette

set output
print "Saved: solution_heatmap_225.png"
print "Done!"
