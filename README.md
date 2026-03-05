<![CDATA[<div align="center">

# 🔢 Numerical Computing

**A modular C++ library for numerical methods — matrix algebra, linear system solvers, root-finding algorithms, and more.**

[![Language](https://img.shields.io/badge/Language-C%2B%2B11-blue.svg)](https://isocpp.org/)
[![Build](https://img.shields.io/badge/Build-Make-brightgreen.svg)](#building)
[![CUDA](https://img.shields.io/badge/GPU-CUDA%20Optional-76b900.svg)](#gpu-acceleration)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

</div>

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Building](#building)
- [Usage](#usage)
  - [Matrix Operations](#1-matrix-operations)
  - [Solving Linear Systems](#2-solving-linear-systems)
  - [Root-Finding Methods](#3-root-finding-methods)
  - [Complex Number Arithmetic](#4-complex-number-arithmetic)
- [Class Hierarchy](#class-hierarchy)
- [API Reference](#api-reference)
- [GPU Acceleration](#gpu-acceleration)
- [License](#license)

---

## Overview

This repository is a collection of numerical computing implementations written in C++11. It covers the core topics of a numerical methods course — from matrix algebra and systems of linear equations to root-finding algorithms and floating-point analysis — all organized into clean, modular, object-oriented code.

The flagship component is a full-featured **Matrix class** with an inheritance-based hierarchy of linear equation solvers and optional **CUDA GPU acceleration**.

---

## Features

| Category | Details |
|---|---|
| **Matrix Algebra** | Addition, subtraction, multiplication, scalar multiply, transpose, determinant, inverse, adjoint, cofactor, minor matrix |
| **Direct Solvers** | Gaussian Elimination (with/without partial pivoting) |
| **LU Decomposition** | Doolittle, Crout, and Cholesky (symmetric positive-definite) |
| **Iterative Solvers** | Gauss-Jacobi |
| **Root Finding** | Bisection, Newton-Raphson, Fixed-Point Iteration |
| **Complex Numbers** | Arithmetic (+, −, ×, ÷), conjugate, norm, operator overloading |
| **I/O** | Console input, file input/output (auto-format detection), solution saving |
| **GPU** | Optional CUDA backend for parallelizable operations |
| **Error Handling** | Custom `MatrixException` class with descriptive messages |

---

## Project Structure

```
Numerical_Computing/
│
├── matrix_class/               # Core matrix library
│   ├── include/                # Header files
│   │   ├── Matrix.hpp                          # Matrix base class
│   │   ├── MatrixException.hpp                 # Custom exception class
│   │   ├── SystemOfLinearEquationSolver.hpp     # Abstract solver base
│   │   ├── GaussianElimination.hpp             # Gaussian elimination solver
│   │   ├── LUDecomposition.hpp                 # LU variants (Doolittle, Crout, Cholesky)
│   │   └── GaussJacobi.hpp                     # Jacobi iterative solver
│   │
│   ├── src/                    # Implementation files
│   │   ├── Matrix.cpp              # Matrix core + I/O
│   │   ├── MatrixOperations.cpp    # Arithmetic operators
│   │   ├── GaussianElimination.cpp
│   │   ├── Doolittle.cpp
│   │   ├── Crout.cpp
│   │   ├── Cholesky.cpp
│   │   ├── GaussJacobi.cpp
│   │   └── ...
│   │
│   ├── utils/                  # Shared I/O utilities
│   │   ├── Input.hpp / Input.cpp       # Matrix input helpers
│   │   └── Display.hpp / Display.cpp   # Solution display & save helpers
│   │
│   ├── cuda/                   # CUDA GPU kernels (optional)
│   ├── examples/               # Example usage programs
│   ├── test_cases/             # Pre-built test matrices
│   ├── Makefile                # Build system (cpu / gpu targets)
│   └── main.cpp                # Interactive menu-driven program
│
├── root_finding_methods/       # Root-finding algorithms
│   ├── include/
│   │   ├── RootHunter.hpp          # Abstract root-finder base
│   │   ├── Bisection.hpp
│   │   ├── NewtonRaphson.hpp
│   │   └── FixedPoint.hpp
│   ├── src/                        # Implementations
│   ├── utils/                      # I/O utilities
│   └── main.cpp                    # Interactive selector
│
├── Complex_class_assignment/   # Complex number class
│   ├── complexClass_header.hpp
│   ├── complexClass.cpp
│   └── main.cpp
│
├── Miscellaneous/              # Numerical explorations
│   ├── factorial_limits.cpp        # Factorial overflow across data types
│   └── geometric_series_sum.cpp    # Geometric series convergence analysis
│
├── Assigment_1/                # Assignment graphs & plots
├── assignment1_graphs/         # Additional graph outputs
├── books_followed/             # Reference textbooks
│
├── LICENSE                     # MIT License
└── README.md                   # ← you are here
```

---

## Building

### Prerequisites

- **C++ compiler** with C++11 support (GCC, Clang, MSVC)
- **GNU Make**
- *(Optional)* NVIDIA CUDA Toolkit for GPU builds

### Matrix Library

```bash
cd matrix_class

# CPU-only build (default)
make cpu

# GPU build (requires CUDA toolkit)
make gpu

# Run the program
./matrix_program        # CPU version
./matrix_program_gpu    # GPU version

# Clean build artifacts
make clean
```

### Root-Finding Methods

```bash
cd root_finding_methods
g++ -std=c++11 -o rootHunter main.cpp src/*.cpp
./rootHunter
```

### Complex Number Class

```bash
cd Complex_class_assignment
g++ -std=c++11 -o complex_op main.cpp complexClass.cpp
./complex_op
```

---

## Usage

### 1. Matrix Operations

The interactive menu provides 17 operations:

```
=== Menu ===
1.  Add (A + B)              10. Gauss-Jacobi (iterative)
2.  Subtract (A - B)         11. Transpose
3.  Multiply (A * B)         12. Scalar Multiply
4.  Determinant              13. Inverse
5.  Gauss Elim. (pivoting)   14. Minor Matrix
6.  Gauss Elim. (no pivot)   15. Cofactor
7.  LU - Doolittle           16. Adjoint
8.  LU - Crout               17. Exit
9.  LU - Cholesky
```

**Input options** — for each operation you can enter a matrix:
- **Manually** via console (you enter rows, cols, then the values)
- **From a file** (space/tab-separated text file, auto-detected format)

#### Programmatic Example

```cpp
#include "include/Matrix.hpp"
#include "include/GaussianElimination.hpp"
#include "include/LUDecomposition.hpp"

// Create matrices
Matrix A(3, 3);
A.readFromConsole();

Matrix B("data.txt");     // load from file

Matrix C = A + B;         // operator overloading
Matrix T = A.transpose();
double det = A.determinant();
Matrix inv = A.inverse();

// Solve Ax = b using Gaussian Elimination
GaussianElimination ge(3, 3);
// ... populate ge with coefficients ...
double b[] = {1.0, 2.0, 3.0};
double *x = ge.solveWithPivoting(b, 3);

// Solve using Cholesky (symmetric positive-definite)
Cholesky ch(3, 3);
double *x2 = ch.solve(b, 3);
```

### 2. Solving Linear Systems

All solvers inherit from `SystemOfLinearEquationSolver` and implement `solve(double *b, int n)`:

| Solver | Method | Best For |
|---|---|---|
| `GaussianElimination` | Row reduction with/without pivoting | General dense systems |
| `Doolittle` | LU decomposition (L has unit diagonal) | Multiple right-hand sides |
| `Crout` | LU decomposition (U has unit diagonal) | Multiple right-hand sides |
| `Cholesky` | LLᵀ decomposition | Symmetric positive-definite systems |
| `GaussJacobi` | Iterative (Jacobi) method | Diagonally dominant, sparse systems |

### 3. Root-Finding Methods

```
Select method:
1. Bisection
2. Newton Raphson
3. Fixed Point iteration
```

All methods inherit from `RootHunter` and provide `input()` and `solve()`:

```cpp
Bisection b(1e-6);    // tolerance
b.input();             // prompts for interval
b.solve();
cout << "Root: " << b.getRoot() << endl;
cout << "Iterations: " << b.getIterations() << endl;
```

### 4. Complex Number Arithmetic

```cpp
#include "complexClass_header.hpp"

Complex a(3.0, 4.0);   // 3 + 4i
Complex b(1.0, -2.0);  // 1 - 2i

Complex sum  = a + b;
Complex diff = a - b;
Complex prod = a * b;
Complex quot = a / b;

Complex conj = a.conjugate();
float   norm = a.Norm();     // |a| = sqrt(3² + 4²) = 5
```

---

## Class Hierarchy

```
Matrix
  ├── getRows(), getCols(), getData(), setData()
  ├── readFromConsole(), readFromFile(), saveToFile()
  ├── add(), subtract(), multiply(), operator+, operator-, operator*, operator*(scalar)
  ├── transpose(), determinant(), inverse()
  ├── minorMatrix(), cofactor(), adjoint()
  ├── isSymmetric(), display()
  │
  └── SystemOfLinearEquationSolver  (abstract: solve() = 0)
        ├── GaussianElimination
        │     ├── solve()
        │     ├── solveWithPivoting()
        │     └── solveWithoutPivoting()
        │
        ├── LUDecomposition  (abstract: solve() = 0)
        │     ├── Doolittle   → solve()
        │     ├── Crout       → solve()
        │     └── Cholesky    → solve()
        │
        └── GaussJacobi
              └── solve()


RootHunter  (abstract: input() = 0, solve() = 0)
  ├── Bisection       → findInterval(), input(), solve()
  ├── NewtonRaphson   → input(), solve()
  └── FixedPoint      → input(), solve()
```

---

## API Reference

### Matrix Class

| Method | Description |
|---|---|
| `Matrix()` | Default constructor |
| `Matrix(int r, int c)` | Create an r × c zero matrix |
| `Matrix(string filename)` | Load matrix from file |
| `Matrix(const Matrix &other)` | Copy constructor |
| `readFromConsole()` | Read dimensions and values from stdin |
| `readFromFile(string filename)` | Read from a text file |
| `saveToFile(string filename)` | Write matrix to file |
| `static inputMatrix(string label)` | Interactive input (manual or file) |
| `operator+`, `operator-`, `operator*` | Matrix arithmetic |
| `operator*(double scalar)` | Scalar multiplication |
| `transpose()` | Returns the transpose |
| `determinant()` | Computes determinant (recursive cofactor expansion) |
| `minorMatrix(int r, int c)` | Sub-matrix with row r and col c removed |
| `cofactor(int r, int c)` | Cofactor at (r, c) |
| `adjoint()` | Adjugate (transpose of cofactor matrix) |
| `inverse()` | Matrix inverse via adjoint/determinant |
| `isSymmetric()` | Returns `true` if A = Aᵀ |
| `display()` | Pretty-print to stdout |
| `getRowPointer(int i)` | Raw pointer for CUDA transfers |

### Solver Classes

Each solver exposes `double* solve(double *b, int n)` — given the right-hand-side vector **b** of length **n**, it returns the solution vector **x**.

### RootHunter Classes

| Method | Description |
|---|---|
| `RootHunter(double tol)` | Set convergence tolerance |
| `input()` | Prompt the user for method-specific parameters |
| `solve()` | Run the algorithm |
| `getRoot()` | Return the computed root |
| `getIterations()` | Return the number of iterations used |

---

## GPU Acceleration

The library optionally supports **CUDA** for parallelizable matrix operations. Build with:

```bash
make gpu
```

This compiles with the `-DUSE_CUDA` flag and links the CUDA kernels in `cuda/src/`. At runtime, GPU-accelerated code paths are selected automatically for operations that benefit from parallelism (e.g., large matrix multiplications).

**Requirements:** NVIDIA GPU with compute capability ≥ 5.0 and the CUDA Toolkit installed.

---

## License

This project is licensed under the **MIT License** — see the [LICENSE](LICENSE) file for details.

Copyright © 2025 Aditya Gowari

---

<div align="center">
  <sub>Built with ❤️ for numerical computing enthusiasts.</sub>
</div>
]]>
