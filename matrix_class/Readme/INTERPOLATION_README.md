# Interpolation Module — Design & Usage

## What This Module Does

The Interpolation module adds **numerical interpolation** capabilities to the
Matrix Operations Library. Given a set of known data points `(x₀, y₀), (x₁, y₁), ..., (xₙ₋₁, yₙ₋₁)`,
it constructs a polynomial that passes through all the points and can evaluate it
at any x value — including points between, before, or after the given data.

Currently implemented:
- **Lagrange Interpolation** — uses the Lagrange basis polynomial formula

---

## Architecture: Why Composition, Not Inheritance

### The Existing Hierarchy

The Matrix class has a well-defined inheritance tree for solving systems of
linear equations:

```
Matrix
  └── SystemOfLinearEquationSolver (abstract)
        ├── GaussianElimination
        ├── LUDecomposition (abstract)
        │     ├── Doolittle
        │     ├── Crout
        │     └── Cholesky
        ├── GaussJacobi
        └── GaussSeidel
```

Each solver **IS-A** Matrix — it holds the coefficient matrix `A` as its own data,
extends the Matrix class, and operates on `this->data` directly. This makes sense
because the solvers literally ARE matrices with extra behavior.

### Why Interpolation Does NOT Inherit Matrix

Interpolation is **fundamentally different** from a system solver:

1. **An interpolation is NOT a matrix.** It's a set of `(x, y)` data points and
   a polynomial. Saying a Lagrange interpolation "is-a" Matrix would be
   semantically wrong.

2. **It USES matrices.**  The Interpolation class creates and uses `Matrix`
   objects to read data points from console or file using the existing I/O
   infrastructure (`getMatrixInput`, `readFromFile`, etc.). This is the textbook
   definition of **composition**: "has-a" vs "is-a".

3. **It doesn't pollute the Matrix hierarchy.** Adding interpolation into the
   `Matrix → SystemOfLinearEquationSolver` tree would confuse the abstraction.
   Interpolation has nothing to do with solving `Ax = b`.

4. **It keeps the Matrix class clean.** The Matrix class is the foundation of
   the entire library. Adding unrelated methods (like `evaluate(x)`) to it
   would violate the Single Responsibility Principle.

### The Interpolation Hierarchy

```
Interpolation (abstract base)
  └── Lagrange
  └── (future: Newton, Hermite, Spline, etc.)
```

This is a **separate, parallel hierarchy** that lives alongside the Matrix
hierarchy. It can be extended independently without touching any existing code.

---

## Data Storage: Raw `double*`, Not `std::vector`

The library uses raw `double*` arrays with manual `new[]`/`delete[]` throughout:

- `Matrix::data` is `double**`
- `SolverResult::x` is `double*`
- All solver methods take `double* b`

The Interpolation module follows the same convention:

```cpp
class Interpolation {
protected:
  double *xData;   // x coordinates (owned, new[])
  double *yData;   // y coordinates (owned, new[])
  int numPoints;
};
```

This ensures consistency across the library and avoids mixing paradigms.
The destructor handles cleanup:

```cpp
Interpolation::~Interpolation() {
  if (xData != nullptr) delete[] xData;
  if (yData != nullptr) delete[] yData;
}
```

---

## Class Reference

### `Interpolation` (abstract base class)

| Method | Description |
|--------|-------------|
| `Interpolation()` | Default constructor — empty data |
| `Interpolation(double *x, double *y, int n)` | Constructor — copies `n` data points |
| `~Interpolation()` | Destructor — frees xData and yData |
| `loadData(double *x, double *y, int n)` | Load/replace data points |
| `getNumPoints()` | Returns number of data points |
| `getX(int i)` / `getY(int i)` | Get individual data point values |
| `evaluate(double x)` | **Pure virtual** — evaluate polynomial at x |
| `interpolate(int samples, bool save, string file)` | Evaluate over full range |
| `interpolate(double queryX, bool save, string file)` | Evaluate at one point |

### `Lagrange` (concrete class)

| Method | Description |
|--------|-------------|
| `Lagrange()` | Default constructor |
| `Lagrange(double *x, double *y, int n)` | Constructor with data |
| `evaluate(double x)` | Lagrange polynomial evaluation |

The Lagrange formula:

```
P(x) = Σᵢ yᵢ · Lᵢ(x)

where Lᵢ(x) = Πⱼ≠ᵢ (x - xⱼ) / (xᵢ - xⱼ)
```

---

## Usage

### From the Menu

Select option **27. Lagrange Interpolation** from the main menu. You will be
prompted to:

1. Enter X data points (as a matrix — manually or from file)
2. Enter Y data points (as a matrix — manually or from file)
3. Choose mode:
   - **Full interpolation** — evaluates over the entire range with N sample points
   - **Query point** — evaluates at a single specific x
4. Choose output:
   - Print to console
   - Save to file (gnuplot-compatible format)

### From Code

```cpp
double x[] = {1, 2, 3, 4, 5};
double y[] = {1, 8, 27, 64, 125};

Lagrange lag(x, y, 5);

// evaluate at a single point
double val = lag.evaluate(2.5);  // ≈ 15.625

// generate full curve to file
lag.interpolate(100, true, "output/lagrange.txt");

// query a specific point to console
lag.interpolate(2.5, false, "");
```

---

## Output Format (gnuplot-Compatible)

The output text file has two blocks separated by blank lines:

### Block 1: Interpolated Curve

```
# Interpolation Result
# Data points: 5
# Sample points: 100
# Range: [1.000000, 5.000000]
# x  y
1.000000  1.000000
1.040816  1.125095
1.081633  1.258652
...
5.000000  125.000000
```

### Block 2: Original Data Points

```
# Original data points
1.000000  1.000000
2.000000  8.000000
3.000000  27.000000
4.000000  64.000000
5.000000  125.000000
```

### gnuplot Commands

Plot both the curve and data points:

```gnuplot
set title "Lagrange Interpolation"
set xlabel "x"
set ylabel "y"
plot "output/lagrange.txt" index 0 with lines title "Interpolation", \
     "output/lagrange.txt" index 1 with points pt 7 ps 2 title "Data Points"
```

---

## File Layout

```
include/
  Interpolation.hpp    — abstract base class
  Lagrange.hpp         — Lagrange method

src/
  Interpolation.cpp    — base class: loadData, interpolate overloads
  Lagrange.cpp         — Lagrange: evaluate() implementation

app/
  Menu.cpp             — menu option 27

utils/
  Display.hpp/cpp      — solveInterpolation() workflow
```
