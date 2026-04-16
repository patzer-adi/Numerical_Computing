# Interpolation Module

## What This Module Does

Given a set of known data points (x₀, y₀), (x₁, y₁), ..., (xₙ₋₁, yₙ₋₁), the
Interpolation module constructs a polynomial that passes through all points and
evaluates it at any x value.

Currently implemented: **Lagrange Interpolation**

---

## Class Hierarchy

```
Interpolation (abstract base)
  └── Lagrange
  └── (future: Newton, Hermite, Spline, etc.)
```

This is a **separate hierarchy** from the solver tree. It does not touch or
extend the Matrix → SystemOfLinearEquationSolver chain.

---

## How Matrix Class Is Used

The Interpolation module uses the Matrix class through **composition**. This means
Interpolation creates and holds Matrix objects as members — it does not inherit
from Matrix.

### Data Storage

```cpp
class Interpolation {
protected:
  Matrix xData;    // 1×n row vector of x coordinates
  Matrix yData;    // 1×n row vector of y coordinates
  int numPoints;
};
```

`xData` and `yData` are **Matrix objects**, not raw `double*` arrays. Every
data access goes through Matrix methods:

| Operation | How it's done |
|-----------|---------------|
| Read x[i] | `xData.getData(0, i)` |
| Read y[i] | `yData.getData(0, i)` |
| Write x[i] | `xData.setData(0, i, value)` |
| Create storage | `xData = Matrix(1, numPoints)` |
| Memory cleanup | Automatic — Matrix destructor handles it |

### Data Input

The user enters X and Y values as matrices using the existing `getMatrixInput()`:

```cpp
Matrix xMat;
getMatrixInput(xMat);   // manual or from file — Matrix handles both

Matrix yMat;
getMatrixInput(yMat);

interp.loadData(xMat, yMat);  // pass Matrix objects directly
```

This reuses the entire Matrix I/O infrastructure — console input, file reading,
auto-format detection — without writing any new I/O code.

### In Lagrange::evaluate()

The Lagrange formula accesses data through Matrix getters:

```cpp
double Lagrange::evaluate(double x) {
  for (int i = 0; i < numPoints; i++) {
    double Li = 1.0;
    for (int j = 0; j < numPoints; j++) {
      if (j != i) {
        Li *= (x - xData.getData(0, j)) / (xData.getData(0, i) - xData.getData(0, j));
      }
    }
    result += yData.getData(0, i) * Li;
  }
}
```

Every `xData[j]` and `yData[i]` access goes through `Matrix::getData()` which
includes bounds checking — no raw pointer arithmetic.

---

## Why Composition, Not Inheritance

### What is composition?

Composition means: "Interpolation **HAS-A** Matrix" (it contains Matrix objects
as members). Inheritance would mean: "Interpolation **IS-A** Matrix".

### Why this choice?

An Interpolation is **not** a matrix. It doesn't have rows, columns, determinants,
or inverses. It has data points and a polynomial. Making Interpolation inherit from
Matrix would be semantically wrong:

```cpp
// If Interpolation inherited Matrix, this would compile:
Lagrange lag(xMat, yMat);
lag.determinant();    // meaningless — an interpolation doesn't have a determinant
lag.inverse();        // meaningless — what is the inverse of an interpolation?
lag + someMatrix;     // meaningless — can't add an interpolation to a matrix
```

By using composition instead:

- Interpolation only exposes methods that make sense for interpolation
- Matrix stays clean — no interpolation-specific methods pollute it
- The Matrix hierarchy (`Matrix → SystemOfLinearEquationSolver → GE/LU/...`)
  remains untouched

### Compare with the solver hierarchy

The solvers DO inherit from Matrix because a solver **IS** a matrix. The `data[][]`
inside a GaussianElimination object IS the coefficient matrix A. The solver reads
data into itself via `getMatrixInput(ge)` and operates on `this->data` directly.

Interpolation is different — it USES matrices to store data points but it is not
itself a matrix.

---

## Why the Destructor Is Empty

```cpp
Interpolation::~Interpolation() {}
```

Because `xData` and `yData` are Matrix objects (not pointers). When Interpolation
is destroyed, the compiler automatically calls the destructor for each member.
Matrix's destructor frees the `double**` data. No manual cleanup needed.

This is one of the benefits of composition with proper classes — the Rule of 5
implemented in Matrix handles everything.

---

## Function Overloading

Two versions of `interpolate()`:

```cpp
void interpolate(int samples, bool saveToFile, string filename);    // full range
void interpolate(double queryX, bool saveToFile, string filename);  // single point
```

Same name, different first parameter type. The compiler picks the right one based
on the argument:

```cpp
interp.interpolate(100, true, "output.txt");     // calls int version
interp.interpolate(2.5, false, "");              // calls double version
```

---

## Output Format (gnuplot-Compatible)

### Full interpolation output

Two blocks separated by blank lines (gnuplot `index` feature):

```
# Interpolation Result
# Data points: 5
# Sample points: 100
# Range: [1.000000, 5.000000]
# x  y
1.000000  1.000000
1.040816  1.125095
...
5.000000  125.000000


# Original data points
1.000000  1.000000
2.000000  8.000000
3.000000  27.000000
4.000000  64.000000
5.000000  125.000000
```

### gnuplot commands

```gnuplot
set title "Lagrange Interpolation"
set xlabel "x"
set ylabel "y"
plot "output.txt" index 0 with lines title "Interpolation", \
     "output.txt" index 1 with points pt 7 ps 2 title "Data Points"
```

---

## File Map

```
include/
  Interpolation.hpp   — abstract base class (Matrix xData, Matrix yData)
  Lagrange.hpp        — Lagrange derived class

src/
  Interpolation.cpp   — loadData, interpolate overloads, getters
  Lagrange.cpp        — evaluate() using Lagrange formula

utils/
  Display.cpp         — solveInterpolation() workflow

app/
  Menu.cpp            — menu option 27
```

---

## Menu Integration

Option **27. Lagrange Interpolation** in the main menu.

Flow:
1. Enter X data points (manual or from file — via Matrix I/O)
2. Enter Y data points (manual or from file — via Matrix I/O)
3. Choose mode: full range interpolation or single query point
4. Choose output: print to console or save to file
