# Numerical Computing

A modular C++ library for numerical methods — matrix algebra, linear system solvers, root-finding algorithms, numerical differentiation, numerical integration, and complex number arithmetic. Built with clean OOP design, optional CUDA GPU acceleration, and a menu-driven interactive interface.

---

## Table of Contents

- [Features](#features)
- [Project Structure](#project-structure)
- [Getting Started](#getting-started)
- [Matrix Operations Library](#matrix-operations-library)
- [Linear System Solvers](#linear-system-solvers)
- [Root-Finding Methods](#root-finding-methods)
- [Numerical Integration](#numerical-integration)
- [Complex Number Class](#complex-number-class)
- [Class Hierarchy](#class-hierarchy)
- [API Reference](#api-reference)
- [GPU Acceleration](#gpu-acceleration)
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
> Gauss-Jacobi method for diagonally dominant systems.

**Root Finding**
> Bisection, Newton-Raphson, and Fixed-Point Iteration with configurable tolerance.

**Numerical Integration**
> Trapezoidal Rule, Simpson's 1/3 Rule, and Simpson's 3/8 Rule with 8 built-in test functions including e^x, sin(x), x², e^(-x²), 1/(1+x²), ln(x), and 1/x.

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
│   │   ├── GaussJacobi.hpp
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
│   ├── examples/                  # Example programs
│   ├── test_cases/                # Pre-built test matrices
│   ├── Makefile
│   └── main.cpp
│
├── examples/                      # Standalone example programs
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
├── Complex_class_assignment/      # Complex number class
│   ├── complexClass_header.hpp
│   ├── complexClass.cpp
│   └── main.cpp
│
├── Extended_in_python/            # Python port (PyNumerics)
│   ├── pynumerics/
│   │   ├── differentiation/
│   │   ├── integration/           # Numerical integration package
│   │   ├── interpolation/
│   │   ├── roots/
│   │   └── solvers/
│   └── tests/
│
├── Miscellaneous/                 # Numerical explorations
│   ├── factorial_limits.cpp
│   └── geometric_series_sum.cpp
│
├── numcomp.hpp                    # Unified header (includes all modules)
├── Makefile                       # Unified build system
├── LICENSE
└── README.md
```

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
make cpu                  # CPU-only build (default)
./matrix_program          # launch interactive menu
```

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

**Clean build artifacts**

```bash
cd matrix_class
make clean
```

---

## Matrix Operations Library

The interactive program offers a full menu of 17 operations:

```
 1.  Add (A + B)               10. Gauss-Jacobi (iterative)
 2.  Subtract (A - B)          11. Transpose
 3.  Multiply (A * B)          12. Scalar Multiply
 4.  Determinant               13. Inverse
 5.  Gauss Elim. (pivoting)    14. Minor Matrix
 6.  Gauss Elim. (no pivot)    15. Cofactor
 7.  LU — Doolittle            16. Adjoint
 8.  LU — Crout                17. Exit
 9.  LU — Cholesky
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
double *x2 = ch.solve(b, 3);
```

---

## Linear System Solvers

All solvers inherit from `SystemOfLinearEquationSolver` and expose a uniform interface:

```cpp
double* solve(double *b, int n);
```

| Solver | Algorithm | When to Use |
|:--|:--|:--|
| `GaussianElimination` | Row reduction with/without pivoting | General dense systems |
| `Doolittle` | LU decomposition, L has unit diagonal | Multiple right-hand sides |
| `Crout` | LU decomposition, U has unit diagonal | Multiple right-hand sides |
| `Cholesky` | LL^T decomposition | Symmetric positive-definite matrices |
| `GaussJacobi` | Jacobi iterative method | Diagonally dominant / sparse systems |

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
│     │
│     ├── GaussianElimination
│     │     ├── solve
│     │     ├── solveWithPivoting
│     │     └── solveWithoutPivoting
│     │
│     ├── LUDecomposition            [abstract — solve() = 0]
│     │     ├── Doolittle
│     │     ├── Crout
│     │     └── Cholesky
│     │
│     └── GaussJacobi
│
├── Differentiation                  [abstract — computeDerivative() = 0]
│     ├── ForwardDifference
│     ├── BackwardDifference
│     ├── CentralDifference
│     └── RichardsonExtrapolation
│
└── Integration                      [abstract — integrate() = 0]
      ├── TrapezoidalRule
      ├── Simpsons13
      └── Simpsons38


RootHunter   [abstract — input() = 0, solve() = 0]
├── Bisection
├── NewtonRaphson
└── FixedPoint
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
| `solve(double *b, int n)` | Solve Ax = b, return solution vector x |

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

---

## GPU Acceleration

The matrix library supports an optional CUDA backend for GPU-accelerated operations.

```bash
make gpu                      # compile with CUDA support
./matrix_program_gpu          # run GPU-enabled binary
```

This compiles with `-DUSE_CUDA` and links the kernels in `cuda/src/`. GPU code paths are selected automatically for operations that benefit from parallelism.

**Requirements:** NVIDIA GPU with compute capability >= 5.0 and the CUDA Toolkit.

---

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

Copyright (c) 2025 Aditya Gowari
