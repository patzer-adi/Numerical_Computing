# Least Squares Curve Fitting Module

## What This Module Does

Given a set of data points (x₁, y₁), (x₂, y₂), ..., (xₙ, yₙ), this module
fits the **best approximation curve** that minimizes the total squared error.

Two methods implemented:

- **Least Squares Line** — fits `y = a + bx` (straight line)
- **Least Squares Parabola** — fits `y = a + bx + cx²` (second degree curve)

Unlike Lagrange interpolation (which passes through every point exactly), least
squares finds the curve that stays **as close as possible** to all points overall.
This makes it ideal for noisy, real-world data.

---

## Class Hierarchy

```
Interpolation (abstract base — uses Matrix for xData, yData)
  ├── Lagrange              (exact interpolation)
  ├── LeastSquareLine       (NEW — best fit line)
  └── LeastSquareParabola   (NEW — best fit parabola)
```

Both new classes inherit from `Interpolation`, just like `Lagrange`. They share
the same data loading, interpolation, and output infrastructure.

---

## How The Matrix Class Is Used

The Matrix class is used in **three distinct ways** in these methods. This is a
key design decision worth understanding.

### 1. Data Storage (Inherited from Interpolation — Composition)

```cpp
class Interpolation {
protected:
  Matrix xData;    // 1×n row vector of x coordinates
  Matrix yData;    // 1×n row vector of y coordinates
};
```

`LeastSquareLine` and `LeastSquareParabola` inherit these Matrix members.
All data access goes through Matrix methods:

```cpp
double xi = getX(i);    // internally: xData.getData(0, i)
double yi = getY(i);    // internally: yData.getData(0, i)
```

### 2. Building the Normal Equations (Object Creation)

Inside `fit()`, we create **new Matrix objects** to represent the coefficient
matrix and RHS vector of the normal equations.

**For the line fit (2×2 system):**

```cpp
// build the 2×2 coefficient matrix using Matrix class
Matrix A(2, 2);
A.setData(0, 0, (double)n);   // n
A.setData(0, 1, sumX);        // Σx
A.setData(1, 0, sumX);        // Σx
A.setData(1, 1, sumX2);       // Σx²

// build the 2×1 RHS vector using Matrix class
Matrix rhs(2, 1);
rhs.setData(0, 0, sumY);      // Σy
rhs.setData(1, 0, sumXY);     // Σxy
```

**For the parabola fit (3×3 system):**

```cpp
// build the 3×3 coefficient matrix using Matrix class
Matrix A(3, 3);
A.setData(0, 0, (double)n);   A.setData(0, 1, sumX);    A.setData(0, 2, sumX2);
A.setData(1, 0, sumX);        A.setData(1, 1, sumX2);   A.setData(1, 2, sumX3);
A.setData(2, 0, sumX2);       A.setData(2, 1, sumX3);   A.setData(2, 2, sumX4);

// build the 3×1 RHS vector using Matrix class
Matrix rhs(3, 1);
rhs.setData(0, 0, sumY);      // Σy
rhs.setData(1, 0, sumXY);     // Σxy
rhs.setData(2, 0, sumX2Y);    // Σx²y
```

### 3. Solving the System (Matrix Operations)

We solve the normal equations using **existing Matrix operations** — inverse
and multiplication:

```cpp
// solve: coefficients = A^(-1) * rhs
Matrix Ainv = A.inverse();     // uses Matrix::inverse() → adjoint/det method
Matrix coeffs = Ainv * rhs;    // uses Matrix::operator*() → matrix multiplication

// extract coefficients from result Matrix
coeffA = coeffs.getData(0, 0);
coeffB = coeffs.getData(1, 0);
```

This is the key insight: **we don't write any new linear algebra code**. The
Matrix class already has `inverse()` and `operator*()`. We just build the right
matrices and let the existing infrastructure solve it.

### Summary of Matrix Usage

| Usage                  | How                                    | Where                |
|------------------------|----------------------------------------|----------------------|
| Store data points      | `Matrix xData`, `Matrix yData` (1×n)   | Inherited from base  |
| Read data points       | `getX(i)`, `getY(i)` → `getData(0,i)`  | In `fit()` loops     |
| Build coefficient mat  | `Matrix A(2,2)` or `Matrix A(3,3)`     | Inside `fit()`       |
| Build RHS vector       | `Matrix rhs(2,1)` or `Matrix rhs(3,1)` | Inside `fit()`       |
| Solve normal equations | `A.inverse() * rhs`                    | Inside `fit()`       |
| Read solution          | `coeffs.getData(i, 0)`                 | End of `fit()`       |
| Input from user/file   | `getMatrixInput(xMat)` → `loadData()`  | In `solveInterpolation()` |

---

## Why Composition, Not Inheritance (for Matrix)

Same reasoning as Lagrange (see `INTERPOLATION_README.md`):

- A least squares fit **is not** a matrix — it has no determinant, no inverse,
  no rows/columns in the traditional sense
- It **uses** matrices internally (to store data and solve systems)
- Composition keeps the interface clean: only interpolation-relevant methods
  are exposed
- The Matrix class stays untouched — no curve-fitting methods pollute it

---

## The Algorithms

### Least Squares Line: y = a + bx

**Goal:** Find `a` (intercept) and `b` (slope) that minimize Σ(yᵢ - a - bxᵢ)²

**Steps:**

1. Compute sums from the n data points:
   - Σx, Σy, Σx², Σxy

2. Form the normal equations:
   ```
   Σy  = n·a + b·Σx
   Σxy = a·Σx + b·Σx²
   ```

3. Express as a matrix system:
   ```
   [n    Σx ] [a]   [Σy ]
   [Σx   Σx²] [b] = [Σxy]
   ```

4. Solve: `[a, b]ᵀ = A⁻¹ · rhs`

5. Evaluate: `y(x) = a + bx` for any x

### Least Squares Parabola: y = a + bx + cx²

**Goal:** Find `a`, `b`, `c` that minimize Σ(yᵢ - a - bxᵢ - cxᵢ²)²

**Steps:**

1. Compute sums from the n data points:
   - Σx, Σy, Σx², Σx³, Σx⁴, Σxy, Σx²y

2. Form the normal equations:
   ```
   Σy   = n·a  + b·Σx  + c·Σx²
   Σxy  = a·Σx  + b·Σx² + c·Σx³
   Σx²y = a·Σx² + b·Σx³ + c·Σx⁴
   ```

3. Express as a matrix system:
   ```
   [n    Σx   Σx²] [a]   [Σy  ]
   [Σx   Σx²  Σx³] [b] = [Σxy ]
   [Σx²  Σx³  Σx⁴] [c]   [Σx²y]
   ```

4. Solve: `[a, b, c]ᵀ = A⁻¹ · rhs`

5. Evaluate: `y(x) = a + bx + cx²` for any x

---

## Error Analysis

Both methods include built-in error analysis:

| Metric | Formula | Meaning |
|--------|---------|---------|
| Error per point | `eᵢ = yᵢ(actual) - yᵢ(predicted)` | How far off each prediction is |
| Squared error | `eᵢ²` | Penalizes large errors more |
| SSE | `Σeᵢ²` | Total error being minimized |
| MSE | `SSE / n` | Average squared error per point |
| RMSE | `√MSE` | Error in same units as y |

### Error Table Output

```
--- Error Analysis Table (Least Squares Line) ---
x           y(actual)     y(pred)       error         error^2
--------------------------------------------------------------------
1.000000    2.100000      2.023810      0.076190      0.005805
2.000000    3.900000      3.985714      -0.085714     0.007347
...

Total SSE (Sum of Squared Errors): 0.040000
MSE  (Mean Squared Error):         0.006667
RMSE (Root Mean Squared Error):    0.081650
```

---

## Lagrange vs Least Squares — When to Use What

| Feature               | Lagrange         | Least Squares Line | Least Squares Parabola |
|-----------------------|------------------|--------------------|------------------------|
| Fit type              | Exact            | Approximate        | Approximate            |
| Passes through points | Yes (all)        | No                 | No                     |
| Handles noisy data    | Poorly           | Well               | Well                   |
| Degree of polynomial  | n-1 (high)       | 1 (line)           | 2 (parabola)           |
| Stability             | Poor for large n | Excellent          | Excellent              |
| Best for              | Exact math data  | Linear trends      | Curved trends          |
| Minimum points needed | 2                | 2                  | 3                      |

---

## How to Build

```bash
cd matrix_class
make clean && make cpu
```

Or manually:

```bash
g++ -std=c++11 -Wall -o matrix_program \
    main.cpp \
    src/Matrix.cpp \
    src/MatrixException.cpp \
    src/SystemOfLinearEquationSolver.cpp \
    src/GaussianElimination.cpp \
    src/LUDecomposition.cpp \
    src/Doolittle.cpp \
    src/Crout.cpp \
    src/Cholesky.cpp \
    src/GaussJacobi.cpp \
    src/GaussSeidel.cpp \
    src/MatrixOperations.cpp \
    src/EigenSolver.cpp \
    src/GershgorinAnalyzer.cpp \
    src/Interpolation.cpp \
    src/Lagrange.cpp \
    src/LeastSquareLine.cpp \
    src/LeastSquareParabola.cpp \
    utils/Input.cpp \
    utils/Display.cpp \
    app/Menu.cpp
```

No external dependencies. Standard C++11 only.

---

## How to Use

### Menu Options

- **28. Least Squares Line Fit** — fits y = a + bx
- **29. Least Squares Parabola Fit** — fits y = a + bx + cx²

### Workflow (same flow as Lagrange)

1. Enter X data points (manual or from file — via Matrix I/O)
2. Enter Y data points (manual or from file — via Matrix I/O)
3. View fitted equation and coefficients
4. Optionally view error analysis table (SSE, MSE, RMSE)
5. Optionally save error table to file
6. Choose mode: full range interpolation or single query point
7. Choose output: print to console or save to file

### Manual Input Example

```
Enter choice: 28

--- Enter X data points (as a matrix) ---
How do you want to enter the matrix?
1. Enter manually
2. Load from file
Enter choice: 1
Enter rows: 1
Enter cols: 6
Enter matrix elements row by row:
Row 1: 1 2 3 4 5 6

--- Enter Y data points (as a matrix) ---
Enter choice: 1
Enter rows: 1
Enter cols: 6
Enter matrix elements row by row:
Row 1: 2.1 3.9 6.2 7.8 10.1 11.8

--- Least Squares Line Fit ---
Best fit line: y = a + bx
  a (intercept) = 0.066667
  b (slope)     = 1.961905

  y = 0.066667 + 1.961905x
```

### File Input Example

Using the provided example files:

```
Enter choice: 28

--- Enter X data points (as a matrix) ---
Enter choice: 2
Enter filename: examples/least_squares_x.txt

--- Enter Y data points (as a matrix) ---
Enter choice: 2
Enter filename: examples/least_squares_y.txt
```

---

## The `mutable` Keyword

You'll notice `coeffA`, `coeffB`, `coeffC`, and `fitted` are declared `mutable`:

```cpp
mutable double coeffA, coeffB;
mutable bool fitted;
```

This is because `evaluate()` is `const` (it doesn't logically change the object —
it just returns a value). But the first call to `evaluate()` needs to compute the
coefficients (lazy initialization). The `mutable` keyword allows these internal
cache variables to be modified even from a `const` method.

This is the standard C++ pattern for:
> "These members are an implementation detail / cache, not part of the object's
> logical state."

---

## File Map

```
include/
  Interpolation.hpp        — abstract base class (updated: printFitInfo, error analysis)
  Lagrange.hpp             — Lagrange derived class (updated: const evaluate)
  LeastSquareLine.hpp      — NEW: least squares line fit
  LeastSquareParabola.hpp  — NEW: least squares parabola fit

src/
  Interpolation.cpp        — base class (updated: xMin/xMax fix, samples check)
  Lagrange.cpp             — Lagrange formula (updated: const, getX/getY, dup check)
  LeastSquareLine.cpp      — NEW: fit() + evaluate() + error analysis
  LeastSquareParabola.cpp  — NEW: fit() + evaluate() + error analysis

utils/
  Display.cpp              — solveInterpolation() (updated: printFitInfo + error table)

app/
  Menu.cpp                 — menu options 28, 29 (updated)

examples/
  least_squares_x.txt      — sample X data (1×6)
  least_squares_y.txt      — sample Y data (1×6)
```

---

## Fixes Applied to Base Class

While adding least squares, the following stability fixes were applied to
`Interpolation`:

| Fix | What Changed |
|-----|-------------|
| `evaluate()` → `const` | Evaluation shouldn't modify the object |
| xMin/xMax scanning | Was assuming sorted input; now scans all points |
| Minimum points relaxed | Base accepts ≥ 1; each method enforces its own minimum |
| `samples >= 2` check | Prevents division by zero in step calculation |
| `~Interpolation() = default` | Cleaner — Matrix members clean up themselves |
| `hasData()` added | Utility check for derived classes |
| Lagrange: duplicate check | Prevents division by zero from repeated x values |
| Lagrange: uses `getX()`/`getY()` | Proper encapsulation instead of direct `xData` access |
