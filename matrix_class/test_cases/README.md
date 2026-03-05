# Test Cases & Plotting Tools

## What's in this folder

| File | What it does |
|------|-------------|
| `generate_matrices.sage` | SageMath script — generates random test matrices |
| `plot_solution.gp` | Gnuplot script — plots your solution as x vs y |

---

## Generating Test Matrices (SageMath)

```bash
cd test_cases
sage generate_matrices.sage
```

Follow the menu. It can generate:
- **Random** matrices (may be singular)
- **Diagonally dominant** matrices (always solvable — use this for testing)
- **Symmetric positive definite** matrices (needed for Cholesky)
- **Quick symmetric tests** (option 6) — creates ready-to-use folders like `symmetric_49/49l.txt`

### Quick Cholesky Test Generation (Option 6)

Choose option 6 from the menu, then enter sizes like `5 10 49`. It will:
1. Create folders `symmetric_5/`, `symmetric_10/`, `symmetric_49/` in the `matrix_class/` directory
2. Each folder gets `Nl.txt` (matrix A), `Nr.txt` (vector b), `N_true_solution.txt` (correct answer)
3. Verify symmetry and positive definiteness (checks eigenvalues)
4. Print usage instructions

Then in the C++ program:
```
Option 9 (Cholesky) → Load files: symmetric_49/49l.txt and symmetric_49/49r.txt
```

Output files: `name_left.txt` (matrix A), `name_right.txt` (vector b), `name_true_solution.txt` (correct answer to check against).

---

## Plotting with Gnuplot

### What is Gnuplot?

Gnuplot is a command-line tool that makes graphs from data files. You give it a data file and a script, and it outputs a PNG (or displays on screen).

### Installing Gnuplot

**macOS:**
```bash
brew install gnuplot
```

**Ubuntu/Debian:**
```bash
sudo apt install gnuplot
```

**Check it's installed:**
```bash
gnuplot --version
```

### How to plot your solution

**Step 1:** Solve a system in the C++ program and save the solution:

```
Enter choice: 5
How do you want to input: 3
Enter matrix (left) file: 49/49l.txt
Enter RHS (right) file: 49/49r.txt

Save solution to file? 1
Enter output filename: solution_49.txt
```

**Step 2:** Run the gnuplot script:

```bash
cd test_cases
gnuplot plot_solution.gp
```

**Step 3:** Open the output images:

```bash
open solution_plot.png         # x vs y line plot
open solution_heatmap.png      # 2D color map
```

### Using a different solution file

By default, the script looks for `../solution_49.txt`. To use a different file:

```bash
gnuplot -e "datafile='../solution_225.txt'" plot_solution.gp
```

Or for the 225×225 system (15×15 grid):

```bash
gnuplot -e "datafile='../solution_225.txt'; gridsize=15" plot_solution.gp
```

### What the plots show

**`solution_plot.png`** — A line plot:
- **x-axis** = index i (0, 1, 2, ...)
- **y-axis** = solution value x[i]
- Shows you the overall shape of the solution

**`solution_heatmap.png`** — A 2D color map:
- Reshapes the 1D solution vector onto the grid (7×7 or 15×15)
- Colors show the magnitude at each grid point
- Blue = low values, Red = high values

### Running gnuplot interactively

You can also type gnuplot commands one at a time:

```bash
gnuplot
```

Then inside gnuplot:

```gnuplot
gnuplot> plot "solution_49.txt" using ($0):1 with linespoints title "solution"
```

This opens a window with the plot. Close the window to go back to the prompt.

Some useful commands inside gnuplot:

```gnuplot
# basic x vs y plot from a file
plot "data.txt" using 1:2 with lines

# plot with points instead of lines
plot "data.txt" using 1:2 with points

# plot with both
plot "data.txt" using 1:2 with linespoints

# set axis labels
set xlabel "x"
set ylabel "y"

# add a title
set title "My Plot"

# turn on grid
set grid

# save to PNG instead of showing on screen
set terminal pngcairo size 1200,600
set output "my_plot.png"
plot "data.txt" using 1:2 with lines
set output

# go back to showing on screen
set terminal qt

# quit
quit
```

### Gnuplot data file format

Gnuplot reads plain text files. The simplest format is one number per line:

```
1.5
2.3
4.1
-0.7
```

Gnuplot auto-assigns x = 0, 1, 2, 3... for each line. That's what `using ($0):1` means: `$0` = line number (auto x), `1` = first column (your y value).

For two-column data (explicit x and y):

```
0.0  1.5
1.0  2.3
2.0  4.1
3.0  -0.7
```

Then use `using 1:2` — column 1 = x, column 2 = y.

### Common gnuplot mistakes

1. **File not found** — make sure you're in the right directory, or use the full path
2. **No output** — if using `set terminal png`, you must also do `set output "file.png"` BEFORE plotting, and `set output` (no args) AFTER to flush the file
3. **Blank plot** — your data file might have extra text. Gnuplot skips lines starting with `#` but chokes on other text
4. **Plot shows nothing on macOS** — you might need `set terminal qt` or `set terminal aqua` for interactive plots

### Requirements

- **Gnuplot**: `brew install gnuplot` (macOS) or `sudo apt install gnuplot` (Linux)
- **SageMath**: [sagemath.org](https://www.sagemath.org) (for generating test matrices)
