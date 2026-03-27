# Matrix Operations Library — Full Reference

A C++ matrix computation library with **CPU** and **CUDA GPU** backends. Provides a clean `Matrix` class API with operations including arithmetic, linear algebra, and system-of-equations solvers. At runtime, the library detects GPU availability and dispatches operations to the fastest backend automatically.

**Author:** Aditya Gowari  
**License:** MIT  

---

## Features

| Category | Operations |
|---|---|
| **Arithmetic** | Addition, Subtraction, Multiplication, Scalar Multiplication |
| **Unary** | Transpose, Determinant, Inverse, Minor Matrix, Cofactor, Adjoint |
| **SLE Solvers** | Gaussian Elimination (±pivoting), Doolittle LU, Crout LU, Cholesky LU, Gauss-Jacobi, Gauss-Seidel |
| **CUDA GPU** | GPU-accelerated add, subtract, multiply, scalar mul, transpose, LU factorization |
| **I/O** | Console input, file input (auto-detect format), solution save to file |
| **Architecture** | Rule of 5, const-correctness, SolverResult return type, 3-layer separation (Core/Utils/App) |

---

## Project Structure

```
matrix_class/
├── main.cpp                          ← entry point (6 lines, calls runMenu())
├── app/                              ← application layer (menu + routing)
│   ├── Menu.hpp                          runMenu() declaration
│   └── Menu.cpp                          handler functions + menu loop
├── include/                          ← core library headers (NO cin/cout)
│   ├── Matrix.hpp                        base Matrix class (Rule of 5, const-correct)
│   ├── SolverResult.hpp                  struct returned by all solvers
│   ├── MatrixException.hpp               custom exception class
│   ├── SystemOfLinearEquationSolver.hpp   abstract SLE base class
│   ├── GaussianElimination.hpp           GE solver
│   ├── LUDecomposition.hpp               LU abstract + Doolittle/Crout/Cholesky
│   ├── GaussJacobi.hpp                  Jacobi iterative solver
│   └── GaussSeidel.hpp                  Gauss-Seidel iterative solver
├── src/                              ← core implementations (NO cin/cout)
│   ├── Matrix.cpp                        constructors, add/sub/mul, det, I/O
│   ├── MatrixOperations.cpp              transpose, inverse, minor, cofactor, adjoint
│   ├── MatrixException.cpp               (header-only, kept for future use)
│   ├── SystemOfLinearEquationSolver.cpp   base constructors
│   ├── GaussianElimination.cpp           GE with/without pivoting
│   ├── LUDecomposition.cpp               LU base constructors
│   ├── Doolittle.cpp                     Doolittle decomposition + verification
│   ├── Crout.cpp                         Crout decomposition + verification
│   ├── Cholesky.cpp                      Cholesky decomposition + verification
│   ├── GaussJacobi.cpp                  Jacobi iteration
│   └── GaussSeidel.cpp                  Gauss-Seidel iteration
├── cuda/                             ← GPU backend
│   ├── gpu_backend.cuh                   GPU function declarations
│   ├── gpu_kernels.cu                    CUDA kernels
│   ├── gpu_dispatch.hpp                  BackendDispatcher class
│   └── gpu_dispatch.cu                   runtime CPU/GPU decision logic
├── utils/                            ← I/O helpers (no menus, no decisions)
│   ├── Input.hpp / Input.cpp             matrix & system input
│   └── Display.hpp / Display.cpp         result display, solver status, file save
├── 49/                               ← 49×49 test system
├── 225/                              ← 225×225 test system
├── test_cases/                       ← SageMath generator scripts
├── Makefile                          ← build rules (cpu / gpu targets)
├── README.md                         ← original readme
├── README_FULL.md                    ← this file
├── ALGORITHMS.md                     ← algorithm explanations with examples
├── CHANGELOG_2026-03-27.md           ← detailed refactoring changelog
└── LICENSE                           ← MIT license
```

---

## Class Hierarchy

```
Matrix                                    ← base class (Rule of 5, const-correct)
│   data[][],  rows, cols
│   add, subtract, multiply, determinant (all const)
│   transpose, inverse, minor, cofactor, adjoint (all const)
│   operator+, -, *(Matrix), *(scalar) (all const)
│   display, readFromFile, saveToFile
│   move constructor, move assignment (Rule of 5)
│
└── SystemOfLinearEquationSolver          ← abstract SLE base (inherits Matrix)
    │   pure virtual: solve(b, n, maxIter, tol) → SolverResult
    │
    ├── GaussianElimination               ← solveWithPivoting / solveWithoutPivoting
    │      solve() → SolverResult (converged=true, iterations=0)
    │
    ├── LUDecomposition                   ← abstract base for LU methods
    │   ├── Doolittle → SolverResult (error = max|L*U-A|)
    │   ├── Crout → SolverResult (error = max|L*U-A|)
    │   └── Cholesky → SolverResult (error = max|L*L^T-A|)
    │
    ├── GaussJacobi                       ← iterative method (configurable maxIter/tol)
    │      solve() → SolverResult {x, iterations, converged, error}
    │
    └── GaussSeidel                       ← iterative method (faster than Jacobi)
           solve() → SolverResult {x, iterations, converged, error}
```

---

## Building

### CPU-Only (default)
```bash
cd matrix_class
make clean && make cpu
./matrix_program
```

### With CUDA GPU Support
Requires `nvcc` and NVIDIA GPU with compute capability ≥ 5.0:
```bash
make clean && make gpu
./matrix_program_gpu
```

> **Note:** macOS does not support NVIDIA CUDA natively. The CUDA code will compile and run on Linux/Windows machines with NVIDIA GPUs. On Mac, use the CPU-only build.

---

## CUDA GPU Backend

The library automatically detects whether a CUDA GPU is available at runtime via `BackendDispatcher`:

| Operation | GPU Threshold | Notes |
|---|---|---|
| Matrix Addition | n ≥ 64 | Element-wise parallel |
| Matrix Subtraction | n ≥ 64 | Element-wise parallel |
| Matrix Multiplication | n ≥ 64 | Tiled kernel (16×16 tiles) |
| Scalar Multiplication | n ≥ 64 | Element-wise parallel |
| Transpose | n ≥ 64 | Index remapping |
| LU Factorization | n ≥ 128 | Parallelized row/col elimination |
| Gauss-Jacobi | CPU only | Too much branching for GPU |
| Gaussian Elimination | CPU only | Pivoting requires serial logic |

If no GPU is found or the matrix is too small, the CPU backend runs automatically.

---

## Function Reference

### Matrix Class

| Method | Description |
|---|---|
| `Matrix()` | Default constructor (empty matrix) |
| `Matrix(int r, int c)` | Create r×c matrix filled with zeros |
| `Matrix(string filename)` | Construct from file |
| `Matrix(const Matrix &other)` | Copy constructor (deep copy) |
| `Matrix(Matrix &&other) noexcept` | Move constructor (steals pointer, no copy) |
| `getRows() const`, `getCols() const` | Get dimensions |
| `getData(i, j) const`, `setData(i, j, val)` | Element access |
| `readFromConsole()` | Manual input |
| `readFromFile(string)` | File input (auto-detect format) |
| `saveToFile(string)` | Write matrix to file |
| `display() const` | Print matrix to console |
| `add(const Matrix&) const`, `operator+` | Matrix addition |
| `subtract(const Matrix&) const`, `operator-` | Matrix subtraction |
| `multiply(const Matrix&) const`, `operator*(Matrix)` | Matrix multiplication |
| `operator*(double) const` | Scalar multiplication |
| `transpose() const` | Return transposed matrix |
| `determinant() const` | Compute determinant |
| `minorMatrix(r, c) const` | Remove row r, col c |
| `cofactor(r, c) const` | (-1)^(r+c) × det(minor(r,c)) |
| `adjoint() const` | Transpose of cofactor matrix |
| `inverse() const` | adj(A) / det(A) |
| `isSquare() const`, `isSymmetric() const` | Matrix property checks |
| `isIdentity() const`, `isNull() const`, `isDiagonal() const` | Matrix property checks |
| `isDiagonallyDominant() const` | Check diagonal dominance |
| `makeDiagonallyDominant()` | Rearrange rows for dominance |
| `operator==(const Matrix&) const` | Equality check (with fp tolerance) |
| `operator=(const Matrix&)` | Copy assignment (deep copy) |
| `operator=(Matrix&&) noexcept` | Move assignment (steal pointer) |
| `copyFrom(const Matrix&)` | Deep copy from another matrix |

### Solver Classes

| Method | Description |
|---|---|
| `GaussianElimination::solve(b, n)` | Returns `SolverResult` (uses pivoting) |
| `GaussianElimination::solveWithPivoting(b, n)` | Returns `double*` |
| `GaussianElimination::solveWithoutPivoting(b, n)` | Returns `double*` (less stable) |
| `Doolittle::solve(b, n)` | Returns `SolverResult` (error = max\|L*U-A\|) |
| `Crout::solve(b, n)` | Returns `SolverResult` (error = max\|L*U-A\|) |
| `Cholesky::solve(b, n)` | Returns `SolverResult` (error = max\|L*L^T-A\|) |
| `GaussJacobi::solve(b, n, maxIter, tol)` | Returns `SolverResult` {x, iterations, converged, error} |
| `GaussSeidel::solve(b, n, maxIter, tol)` | Returns `SolverResult` {x, iterations, converged, error} |

---

## Usage Examples

### Basic Operations
```cpp
#include "include/Matrix.hpp"

Matrix A(3, 3);
A.setData(0, 0, 1); A.setData(0, 1, 2); A.setData(0, 2, 3);
A.setData(1, 0, 4); A.setData(1, 1, 5); A.setData(1, 2, 6);
A.setData(2, 0, 7); A.setData(2, 1, 8); A.setData(2, 2, 9);

Matrix B = A.transpose();    // 3x3 transpose
Matrix C = A * 2.5;          // scalar multiply
double det = A.determinant(); // determinant
Matrix inv = A.inverse();    // inverse (throws if singular)
```

### Solving Ax = b (Doolittle LU)
```cpp
#include "include/LUDecomposition.hpp"

Doolittle solver;
solver.readFromFile("49/49l.txt");  // load 49×49 matrix

double *b = new double[49];
// ... load b from file ...

SolverResult res = solver.solve(b, 49);
// res.x = solution, res.error = LU verification error

if (res.error < 1e-6)
    cout << "LU verification PASSED" << endl;

for (int i = 0; i < res.n; i++)
    cout << "x[" << i << "] = " << res.x[i] << endl;

delete[] res.x;
delete[] b;
```

### Using the Menu Program
```bash
./matrix_program

=== Menu ===
1.  Add (A + B)          11. Transpose
2.  Subtract (A - B)     12. Scalar Multiply
3.  Multiply (A * B)     13. Inverse
4.  Determinant           14. Minor Matrix
5.  GE (with pivoting)   15. Cofactor
6.  GE (without pivoting) 16. Adjoint
7.  LU - Doolittle       17. Exit
8.  LU - Crout
9.  LU - Cholesky
10. Gauss-Jacobi
```

---

## Test Data

| File | Size | Description |
|---|---|---|
| `49/49l.txt` | 49×49 | Coefficient matrix A |
| `49/49r.txt` | 49×1 | RHS vector b |
| `225/225left.txt` | 225×225 | Coefficient matrix A |
| `225/225right.txt` | 225×1 | RHS vector b |
| `solution_49.txt` | 49 values | Known correct solution for 49×49 |
| `solution_right.txt` | 225 values | Known correct solution for 225×225 |

### Verified Results
- **49×49 GE with pivoting:** solution matches `solution_49.txt` exactly
- **49×49 Doolittle:** LU verification PASSED (max error: 2.84e-14)
- **49×49 Crout:** LU verification PASSED (max error: 2.84e-14)
- **225×225 GE with pivoting:** solution matches `solution_right.txt` exactly

---

## Error Handling

All errors are caught via `MatrixException`. The program never crashes — errors print a message and return to the menu.

| Error | Cause |
|---|---|
| `can't add these matrices... they don't even match bro` | Dimension mismatch |
| `matrix multiplication dimensions don't work...` | A.cols ≠ B.rows |
| `matrix is singular (det = 0)... inverse doesn't exist` | Singular matrix |
| `matrix is not symmetric... Cholesky only works on symmetric positive definite matrices` | Cholesky on non-symmetric |
| `zero pivot in Doolittle...` | Singular or near-singular matrix |
| `Gauss-Jacobi did NOT converge...` | Non-convergent system |

---

## Memory Management

- All matrices use `double**` (2D dynamic arrays)
- Constructors allocate, destructors free (Rule of 3)
- Move constructor and move assignment steal pointers (Rule of 5)
- Copy constructor and `operator=` perform deep copies
- Solver functions work on **copies** (original matrix never modified)
- Caller must `delete[]` the `SolverResult.x` vector

---

## License

MIT License © 2025-2026 Aditya Gowari. See [LICENSE](LICENSE).
