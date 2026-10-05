# Numerical Computing

A modular C++ library for numerical methods — matrix algebra, linear system solvers, root-finding, interpolation, numerical differentiation and integration, eigenvalue analysis, random number generation, and complex arithmetic. The repository also includes a Python port, optional CUDA support, and menu-driven interfaces.

---

## Table of Contents

- [Numerical Computing](#numerical-computing)
  - [Table of Contents](#table-of-contents)
  - [Features](#features)
  - [Project Structure](#project-structure)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Build and Run](#build-and-run)
  - [Matrix Operations Library](#matrix-operations-library)
    - [Programmatic Usage](#programmatic-usage)
  - [Linear System Solvers](#linear-system-solvers)
  - [Root-Finding Methods](#root-finding-methods)
  - [Numerical Integration](#numerical-integration)
    - [Built-in Functions](#built-in-functions)
  - [Random Number Generation](#random-number-generation)
  - [Complex Number Class](#complex-number-class)
  - [Class Hierarchy](#class-hierarchy)
  - [API Reference](#api-reference)
    - [Matrix](#matrix)
    - [SystemOfLinearEquationSolver](#systemoflinearequationsolver)
    - [Integration](#integration)
    - [RootHunter](#roothunter)
    - [RandomNumberGenerator and LCG](#randomnumbergenerator-and-lcg)
  - [GPU Acceleration](#gpu-acceleration)
  - [Testing](#testing)
  - [License](#license)

---

## Features

**Matrix Algebra**
> Addition, subtraction, multiplication, scalar multiply, transpose, determinant, inverse, adjoint, cofactor, minor matrix — with full operator overloading.

**Direct Solvers**
> Gaussian Elimination with and without partial pivoting.

**LU Decomposition**
> Three variants — Doolittle (unit lower), Crout (unit upper), and Cholesky (symmetric positive-definite).

**Iterative Solvers**
> Gauss-Jacobi and Gauss-Seidel methods for diagonally dominant systems.

**Interpolation and Curve Fitting**
> Lagrange interpolation, Newton divided differences, least-squares line fitting, and least-squares parabola fitting.

**Numerical Differentiation**
> Forward, backward, central, and Richardson-extrapolated finite differences.

**Eigenvalue Analysis**
> Gershgorin disk analysis for estimating eigenvalue regions.

**Root Finding**
> Bisection, Newton-Raphson, and Fixed-Point Iteration with configurable tolerance.

**Numerical Integration**
> Trapezoidal Rule, Simpson's 1/3 Rule, and Simpson's 3/8 Rule with 8 built-in test functions including e^x, sin(x), x², e^(-x²), 1/(1+x²), ln(x), and 1/x.

**Random Number Generation**
> Linear Congruential Generator (LCG) with configurable seed and parameters, batch generation of normalized or raw values, formatted output, and reproducible sequences.

**Complex Arithmetic**
> Full complex number class with +, -, *, /, conjugate, norm, and operator overloading.

**Flexible I/O**
> Console input, file input with auto-format detection, and solution export to file.

**GPU Support**
> Optional CUDA backend for parallelizable matrix operations.

**Error Handling**
> Custom `MatrixException` class with descriptive error messages.

---

## Project Structure

```
Numerical_Computing/
│
├── matrix_class/                  # Core matrix library
│   ├── include/                   # Header files
│   │   ├── Matrix.hpp
│   │   ├── MatrixException.hpp
│   │   ├── SystemOfLinearEquationSolver.hpp
│   │   ├── GaussianElimination.hpp
│   │   ├── LUDecomposition.hpp
│   │   ├── GaussJacobi.hpp / GaussSeidel.hpp
│   │   ├── SolverResult.hpp
│   │   ├── EigenSolver.hpp / GershgorinAnalyzer.hpp
│   │   ├── Interpolation.hpp / Lagrange.hpp
│   │   ├── NewtonDividedDifference.hpp
│   │   ├── LeastSquareLine.hpp / LeastSquareParabola.hpp
│   │   ├── Differentiation.hpp    # Numerical differentiation base
│   │   ├── Integration.hpp        # Numerical integration base
│   │   ├── TrapezoidalRule.hpp
│   │   ├── Simpsons13.hpp
│   │   └── Simpsons38.hpp
│   ├── src/                       # Implementations
│   │   ├── Matrix.cpp
│   │   ├── MatrixOperations.cpp
│   │   ├── GaussianElimination.cpp
│   │   ├── Doolittle.cpp
│   │   ├── Crout.cpp
│   │   ├── Cholesky.cpp
│   │   ├── GaussJacobi.cpp
│   │   ├── Integration.cpp
│   │   ├── TrapezoidalRule.cpp
│   │   ├── Simpsons13.cpp
│   │   └── Simpsons38.cpp
│   ├── utils/                     # Shared I/O utilities
│   │   ├── Input.hpp / Input.cpp
│   │   └── Display.hpp / Display.cpp
│   ├── cuda/                      # CUDA GPU kernels (optional)
│   ├── app/                       # Interactive menu implementation
│   ├── examples/                  # Example programs
│   ├── test_cases/                # Pre-built test matrices
│   ├── Makefile
│   └── main.cpp
│
├── examples/                      # Standalone example programs
│   ├── example_usage.cpp
│   ├── differentiation_example.cpp
│   └── integration_example.cpp
│
├── root_finding_methods/          # Root-finding algorithms
│   ├── include/
│   │   ├── RootHunter.hpp
│   │   ├── Bisection.hpp
│   │   ├── NewtonRaphson.hpp
│   │   └── FixedPoint.hpp
│   ├── src/
│   ├── utils/
│   └── main.cpp
│
├── random_number_generation/      # Standalone C++ RNG module
│   ├── include/
│   │   ├── RandomNumberGenerator.hpp
│   │   └── LCG.hpp
│   ├── src/
│   │   ├── RandomNumberGenerator.cpp
│   │   └── LCG.cpp
│   ├── tests/
│   │   └── test_rng.cpp
│   └── Makefile
│
├── Complex_class_assignment/      # Complex number class
│   ├── complexClass_header.hpp
│   ├── complexClass.cpp
│   └── main.cpp
│
├── Extended_in_python/            # Python port (PyNumerics)
│   ├── main.py                    # Interactive CLI entry point
│   ├── pyproject.toml
│   ├── pynumerics/
│   │   ├── cli_*.py              # Interactive feature menus
│   │   ├── differentiation/
│   │   ├── integration/           # Numerical integration package
│   │   ├── interpolation/
│   │   ├── rng/                   # RandomNumberGenerator and LCG
│   │   ├── roots/
│   │   └── solvers/
│   ├── tests/                    # Python test suite
│   └── README.md
│
├── Miscellaneous/                 # Numerical explorations
│   ├── factorial_limits.cpp
│   └── geometric_series_sum.cpp
│
├── Assigment_1/ / assignment1_graphs/ # Coursework and plots
├── books_followed/                # Reference material
├── sem_3_notebooks/               # Coursework notebooks and data
├── Using_octave_results/          # Reference outputs
│
├── numcomp.hpp                    # Unified header for core C++ modules
├── Makefile                       # Unified build system
├── LICENSE
└── README.md
```

The Python package also includes `pynumerics/rng/` for the LCG implementation and
`pynumerics/cli_rng.py` for its interactive interface. The C++ equivalent is in
`matrix_class/include/{RandomNumberGenerator.hpp,LCG.hpp}` and
`matrix_class/src/{RandomNumberGenerator.cpp,LCG.cpp}`.

---

## Getting Started

### Prerequisites

- C++ compiler with C++11 support (GCC, Clang, or MSVC)
- GNU Make
- NVIDIA CUDA Toolkit *(only for GPU builds)*

### Build and Run

**Matrix Operations Library**

```bash
cd matrix_class
make cpu
./matrix_program
```

The matrix menu includes matrix operations, direct and iterative solvers,
property checks, Gershgorin analysis, interpolation, differentiation, and LCG
random-number generation.

**Root-Finding Methods**

```bash
cd root_finding_methods
g++ -std=c++11 -o rootHunter main.cpp src/*.cpp
./rootHunter
```

**Complex Number Class**

```bash
cd Complex_class_assignment
g++ -std=c++11 -o complex_op main.cpp complexClass.cpp
./complex_op
```

**Build the reusable C++ library and example**

From the repository root:

```bash
make all
make example
./examples/example_usage
```

**Clean build artifacts**

```bash
cd matrix_class
make clean
```

---

## Matrix Operations Library

The interactive program offers 31 operations plus exit:

```
 1. Add (A + B)                    17. Adjoint
 2. Subtract (A - B)               18. Check if Square
 3. Multiply (A * B)               19. Check if Symmetric
 4. Determinant                    20. Check if Identity
 5. Gaussian elimination (pivot)   21. Check if Null
 6. Gaussian elimination           22. Check if Diagonal
 7. LU — Doolittle                 23. Check diagonal dominance
 8. LU — Crout                     24. Make diagonally dominant
 9. LU — Cholesky                  25. Check equality (A == B)
10. Gauss-Jacobi                   26. Gershgorin eigenvalue analysis
11. Gauss-Seidel                   27. Lagrange interpolation
12. Transpose                      28. Least-squares line fit
13. Scalar multiply                29. Least-squares parabola fit
14. Inverse                        30. Numerical differentiation
15. Minor matrix                   31. Random number generation (LCG)
16. Cofactor                       32. Exit
```

Matrices can be entered manually via console or loaded from a space-separated text file.

### Programmatic Usage

```cpp
#include "include/Matrix.hpp"
#include "include/GaussianElimination.hpp"
#include "include/LUDecomposition.hpp"

// Create and load matrices
Matrix A(3, 3);
A.readFromConsole();

Matrix B("data.txt");         // from file

// Arithmetic (operator overloading)
Matrix C = A + B;
Matrix D = A * B;
Matrix S = A * 2.5;

// Unary operations
Matrix T   = A.transpose();
double det = A.determinant();
Matrix inv = A.inverse();
Matrix adj = A.adjoint();

// Solve a linear system Ax = b
GaussianElimination ge(3, 3);
double b[] = {1.0, 2.0, 3.0};
double *x = ge.solveWithPivoting(b, 3);

// Cholesky for symmetric positive-definite systems
Cholesky ch(3, 3);
SolverResult result = ch.solve(b, 3);
double *x2 = result.x;       // caller owns the solution vector
delete[] x2;
```

---

## Linear System Solvers

All solvers inherit from `SystemOfLinearEquationSolver` and expose a uniform interface:

```cpp
SolverResult solve(double *b, int n, int maxIter = 10000,
                   double tol = 1e-10);
```

| Solver | Algorithm | When to Use |
|:--|:--|:--|
| `GaussianElimination` | Row reduction with/without pivoting | General dense systems |
| `Doolittle` | LU decomposition, L has unit diagonal | Multiple right-hand sides |
| `Crout` | LU decomposition, U has unit diagonal | Multiple right-hand sides |
| `Cholesky` | LL^T decomposition | Symmetric positive-definite matrices |
| `GaussJacobi` | Jacobi iterative method | Diagonally dominant / sparse systems |
| `GaussSeidel` | Gauss-Seidel iterative method | Diagonally dominant / sparse systems |

---

## Root-Finding Methods

All methods inherit from `RootHunter` and implement `input()` and `solve()`:

```cpp
Bisection b(1e-6);         // set tolerance
b.input();                  // prompts for interval / initial guess
b.solve();                  // run algorithm

cout << "Root: "       << b.getRoot()       << endl;
cout << "Iterations: " << b.getIterations() << endl;
```

| Method | Convergence | Requires |
|:--|:--|:--|
| `Bisection` | Linear | Bracketing interval [a, b] with sign change |
| `NewtonRaphson` | Quadratic | Initial guess, derivative f'(x) |
| `FixedPoint` | Linear | Transformation g(x) such that x = g(x) |

---

## Numerical Integration

All methods inherit from `Integration` (which inherits `Matrix`) and implement `integrate()`:

```cpp
#include "numcomp.hpp"
#include <cmath>

// define integrand and its exact integral
double f_exp(double x)              { return exp(x); }
double F_exp(double a, double b)    { return exp(b) - exp(a); }

double f_gauss(double x)            { return exp(-x * x); }
double F_gauss(double a, double b)  { return sqrt(M_PI)/2.0 * (erf(b) - erf(a)); }

// create method objects
TrapezoidalRule trap;
Simpsons13 s13;
Simpsons38 s38;

// register functions
trap.addFunction("e^x",      f_exp,   F_exp);
trap.addFunction("e^(-x^2)", f_gauss, F_gauss);

// set sub-interval counts
int n[] = {6, 12, 24, 48, 96};
trap.setSubIntervals(n, 5);

// compute over [0, 1]
trap.computeAll(0.0, 1.0);
trap.display();
trap.saveResults("output_trapezoidal.txt");

// or use a single call:
double result = s13.integrate(f_exp, 0.0, 1.0, 100);
// result ≈ 1.71828 (e − 1)
```

| Method | Formula | Order | Constraint |
|:--|:--|:--|:--|
| `TrapezoidalRule` | (h/2)[f(x₀) + 2Σf(xᵢ) + f(xₙ)] | O(h²) | n ≥ 1 |
| `Simpsons13` | (h/3)[f(x₀) + 4f(x₁) + 2f(x₂) + …] | O(h⁴) | n must be even |
| `Simpsons38` | (3h/8)[f(x₀) + 3f(x₁) + 3f(x₂) + 2f(x₃) + …] | O(h⁴) | n must be multiple of 3 |

### Built-in Functions

The module ships with 8 hardcoded test functions:

| Function | f(x) | Exact ∫ₐᵇ f(x)dx |
|:---------|:------|:------------------|
| e^x | `exp(x)` | e^b − e^a |
| sin(x) | `sin(x)` | −cos(b) + cos(a) |
| x² | `x*x` | b³/3 − a³/3 |
| x³−2x+1 | `x³−2x+1` | x⁴/4 − x² + x |
| e^(-x²) | `exp(-x*x)` | √π/2 · (erf(b) − erf(a)) |
| 1/(1+x²) | `1/(1+x*x)` | atan(b) − atan(a) |
| ln(x) | `log(x)` | b·ln(b) − b − (a·ln(a) − a) |
| 1/x | `1/x` | ln(b) − ln(a) |

---

## Random Number Generation

The C++ and Python implementations provide the same linear congruential recurrence:

$$X_{n+1} = (aX_n + c) \bmod m, \qquad U_n = X_n / m$$

The default parameters are `a = 1103515245`, `c = 12345`, and `m = 2^31`.
Both implementations support configurable seeds, generation of normalized values
or raw integer states, reset-to-seed behavior, and saving samples to a text file.

The Python implementation additionally provides sequence and histogram plots:

```python
from pynumerics.rng.lcg import LCG

rng = LCG(seed=42)
samples = rng.generate(1000)
rng.save_results("random_samples.txt", samples)
# rng.plot_sequence(samples)
# rng.plot_histogram(samples)
```

For the C++ version, the reusable classes are `RandomNumberGenerator` and `LCG`.
The C++ implementation is in `random_number_generation/`; its menu integration
is kept in `matrix_class/app/Menu.cpp` without making RNG a matrix subclass or
matrix module component.

Both implementations reset to their initial seed, so the same seed and
parameters reproduce the same sequence. The class relationship is independent
of `Matrix`:

```
RandomNumberGenerator
└── LCG
```

---

## Complex Number Class

```cpp
#include "complexClass_header.hpp"

Complex a(3.0, 4.0);       // 3 + 4i
Complex b(1.0, -2.0);      // 1 - 2i

Complex sum  = a + b;       // operator overloading
Complex diff = a - b;
Complex prod = a * b;
Complex quot = a / b;

Complex conj = a.conjugate();
float   norm = a.Norm();    // sqrt(3^2 + 4^2) = 5
```

---

## Class Hierarchy

```
Matrix
├── I/O: readFromConsole, readFromFile, saveToFile, display
├── Arithmetic: +, -, *, scalar *, transpose
├── Properties: determinant, inverse, adjoint, cofactor, minorMatrix, isSymmetric
│
├── SystemOfLinearEquationSolver   [abstract — solve() = 0]
│   ├── GaussianElimination
│   ├── Doolittle
│   ├── Crout
│   ├── Cholesky
│   ├── GaussJacobi
│   └── GaussSeidel
├── Differentiation                [abstract — computeDerivative() = 0]
│   ├── ForwardDifference
│   ├── BackwardDifference
│   ├── CentralDifference
│   └── RichardsonExtrapolation
└── Integration                    [abstract — integrate() = 0]
    ├── TrapezoidalRule
    ├── Simpsons13
    └── Simpsons38

Interpolation                    [independent; composes Matrix]
├── Lagrange
├── NewtonDividedDifference
├── LeastSquareLine
└── LeastSquareParabola


RootHunter   [abstract — input() = 0, solve() = 0]
├── Bisection
├── NewtonRaphson
└── FixedPoint

EigenSolver / GershgorinAnalyzer [independent eigenvalue analysis]

RandomNumberGenerator             [abstract — nextInt() = 0]
└── LCG

Complex                           [independent complex arithmetic class]
```

---

## API Reference

### Matrix

| Method | Description |
|:--|:--|
| `Matrix()` | Default constructor |
| `Matrix(int r, int c)` | Create r x c zero matrix |
| `Matrix(string filename)` | Load from file |
| `Matrix(const Matrix &other)` | Copy constructor |
| `readFromConsole()` | Read matrix from stdin |
| `readFromFile(string filename)` | Read from text file |
| `saveToFile(string filename)` | Write matrix to file |
| `inputMatrix(string label)` | Static — interactive input (manual or file) |
| `+`, `-`, `*` | Matrix-matrix arithmetic |
| `*(double scalar)` | Scalar multiplication |
| `transpose()` | Return transposed matrix |
| `determinant()` | Compute determinant |
| `minorMatrix(int r, int c)` | Sub-matrix with row r and col c removed |
| `cofactor(int r, int c)` | Signed minor |
| `adjoint()` | Adjugate matrix |
| `inverse()` | Inverse via adjoint and determinant |
| `isSymmetric()` | Check if A equals A^T |
| `display()` | Print to stdout |
| `getRowPointer(int i)` | Raw row pointer (for CUDA transfers) |

### SystemOfLinearEquationSolver

| Method | Description |
|:--|:--|
| `solve(double *b, int n, int maxIter = 10000, double tol = 1e-10)` | Solve Ax = b and return a `SolverResult` |

### Integration

| Method | Description |
|:--|:--|
| `Integration()` | Default constructor |
| `addFunction(name, f, F)` | Register an integrand with its exact integral |
| `setSubIntervals(n[], count)` | Set sub-interval counts to evaluate at |
| `integrate(f, a, b, n)` | **Pure virtual** — compute ∫ₐᵇ f(x)dx with n sub-intervals |
| `getMethodName()` | **Pure virtual** — return method name |
| `computeAll(a, b)` | Compute all registered functions × all n values |
| `display()` | Print formatted results table |
| `saveResults(filename)` | Save results to file |
| `getNumFunctions()` | Number of registered functions |
| `getNumN()` | Number of sub-interval counts |
| `getFunction(i)` | Get i-th registered function entry |
| `getN(i)` | Get i-th sub-interval count |

### RootHunter

| Method | Description |
|:--|:--|
| `RootHunter(double tol)` | Set convergence tolerance |
| `input()` | Prompt for method-specific parameters |
| `solve()` | Execute the algorithm |
| `getRoot()` | Retrieve computed root |
| `getIterations()` | Retrieve iteration count |

### RandomNumberGenerator and LCG

| Method | Description |
|:--|:--|
| `nextInt()` | Generate the next raw integer state; implemented by `LCG` |
| `nextUniform()` | Return the next state normalized to `[0, 1)` |
| `generate(int n)` | Generate `n` normalized values |
| `generateInts(int n)` | Generate `n` raw integer states |
| `reset()` | Restore the initial seed state |
| `display(samples)` | Print generated values in a formatted table |
| `saveResults(filename, samples)` | Save generated values to a file |
| `LCG(seed, a, c, m)` | Construct an LCG with optional parameters |
| `getState()`, `getA()`, `getC()`, `getM()` | Read LCG state and parameters |

---

## GPU Acceleration

The matrix library supports an optional CUDA backend for GPU-accelerated operations.

```bash
cd matrix_class
make gpu                      # compile with CUDA support
./matrix_program_gpu
```

This compiles with `-DUSE_CUDA` and links the kernels in `cuda/src/`. GPU code paths are selected automatically for operations that benefit from parallelism.

**Requirements:** NVIDIA GPU with compute capability >= 5.0 and the CUDA Toolkit.

---

## Testing

The C++ modules provide separate verification targets:

```bash
cd matrix_class
make verify       # build the iterative-solver verification program
make cpu          # build the matrix menu, including its RNG integration

cd ../random_number_generation
make run          # build and run the standalone C++ RNG tests
```

The Python package uses `pytest`:

```bash
cd Extended_in_python
python3 -m pytest -q
```

The current Python suite contains 336 passing tests. Plot smoke tests may emit
non-fatal warnings in headless environments because no interactive display is available.

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

Copyright (c) 2025 Aditya Gowari
