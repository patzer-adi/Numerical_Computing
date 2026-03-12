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
| **SLE Solvers** | Gaussian Elimination (±pivoting), Doolittle LU, Crout LU, Cholesky LU, Gauss-Jacobi |
| **CUDA GPU** | GPU-accelerated add, subtract, multiply, scalar mul, transpose, LU factorization |
| **I/O** | Console input, file input (auto-detect format), solution save to file |

---

## Project Structure

```
matrix_class/
├── main.cpp                          ← menu-driven program
├── include/                          ← header files
│   ├── Matrix.hpp                        base Matrix class
│   ├── MatrixException.hpp               custom exception class
│   ├── SystemOfLinearEquationSolver.hpp   abstract SLE base class
│   ├── GaussianElimination.hpp           GE solver
│   ├── LUDecomposition.hpp               LU abstract + Doolittle/Crout/Cholesky
│   └── GaussJacobi.hpp                  iterative solver
├── src/                              ← implementations
│   ├── Matrix.cpp                        constructors, add/sub/mul, det, I/O
│   ├── MatrixOperations.cpp              transpose, inverse, minor, cofactor, adjoint
│   ├── MatrixException.cpp               (header-only, kept for future use)
│   ├── SystemOfLinearEquationSolver.cpp   base constructors
│   ├── GaussianElimination.cpp           GE with/without pivoting
│   ├── LUDecomposition.cpp               LU base constructors
│   ├── Doolittle.cpp                     Doolittle decomposition + verification
│   ├── Crout.cpp                         Crout decomposition + verification
│   ├── Cholesky.cpp                      Cholesky decomposition + verification
│   └── GaussJacobi.cpp                  Jacobi iteration
├── cuda/                             ← GPU backend
│   ├── gpu_backend.cuh                   GPU function declarations
│   ├── gpu_kernels.cu                    CUDA kernels (add, sub, mul, scalar, transpose, LU)
│   ├── gpu_dispatch.hpp                  BackendDispatcher class
│   └── gpu_dispatch.cu                   runtime CPU/GPU decision logic
├── utils/                            ← helper I/O functions
│   ├── Input.hpp / Input.cpp             matrix & system input
│   └── Display.hpp / Display.cpp         result display & file save
├── 49/                               ← 49×49 test system
├── 225/                              ← 225×225 test system
├── test_cases/                       ← SageMath generator scripts
├── Makefile                          ← build rules (cpu / gpu targets)
├── README.md                         ← original readme
├── README_FULL.md                    ← this file
├── ALGORITHMS.md                     ← algorithm explanations with examples
└── LICENSE                           ← MIT license
```

---

## Class Hierarchy

```
Matrix                                    ← base class (data storage + operations)
│   data[][],  rows, cols
│   add, subtract, multiply, determinant
│   transpose, inverse, minor, cofactor, adjoint
│   operator+, operator-, operator*, operator*(scalar)
│   display, readFromFile, saveToFile
│
└── SystemOfLinearEquationSolver          ← abstract SLE base (inherits Matrix)
    │   pure virtual: solve(double *b, int n)
    │
    ├── GaussianElimination               ← solveWithPivoting / solveWithoutPivoting
    │
    ├── LUDecomposition                   ← abstract base for LU methods
    │   ├── Doolittle                         L has 1s on diagonal, U upper triangular
    │   ├── Crout                             U has 1s on diagonal, L lower triangular
    │   └── Cholesky                          A = L*L^T (symmetric positive definite only)
    │
    └── GaussJacobi                       ← iterative method (max 10000 iter, tol 1e-10)
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
| `getRows()`, `getCols()` | Get dimensions |
| `getData(i, j)`, `setData(i, j, val)` | Element access |
| `readFromConsole()` | Manual input |
| `readFromFile(string)` | File input (auto-detect format) |
| `saveToFile(string)` | Write matrix to file |
| `display()` | Print matrix to console |
| `add(Matrix)`, `operator+` | Matrix addition |
| `subtract(Matrix)`, `operator-` | Matrix subtraction |
| `multiply(Matrix)`, `operator*(Matrix)` | Matrix multiplication |
| `operator*(double)` | Scalar multiplication |
| `transpose()` | Return transposed matrix |
| `determinant()` | Compute determinant (Gaussian elimination method) |
| `minorMatrix(r, c)` | Remove row r, col c → (n-1)×(n-1) matrix |
| `cofactor(r, c)` | (-1)^(r+c) × det(minor(r,c)) |
| `adjoint()` | Transpose of cofactor matrix |
| `inverse()` | adj(A) / det(A) |
| `isSymmetric()` | Check if A[i][j] == A[j][i] for all i,j |
| `copyFrom(Matrix&)` | Deep copy from another matrix |
| `operator=` | Assignment operator (deep copy) |

### Solver Classes

| Method | Description |
|---|---|
| `GaussianElimination::solve(b, n)` | Default: uses pivoting |
| `GaussianElimination::solveWithPivoting(b, n)` | Forward elimination + back substitution |
| `GaussianElimination::solveWithoutPivoting(b, n)` | No pivoting (less stable) |
| `Doolittle::solve(b, n)` | A = LU, L unit diagonal, verifies L*U=A |
| `Crout::solve(b, n)` | A = LU, U unit diagonal, verifies L*U=A |
| `Cholesky::solve(b, n)` | A = LL^T, symmetric check, verifies L*L^T=A |
| `GaussJacobi::solve(b, n)` | Iterative (max 10000 iter, tol 1e-10) |

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

double *x = solver.solve(b, 49);
// prints: "Doolittle LU verification PASSED ✅"

for (int i = 0; i < 49; i++)
    cout << "x[" << i << "] = " << x[i] << endl;

delete[] x;
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
- Constructors allocate, destructors free
- Copy constructor and `operator=` perform deep copies
- Solver functions allocate working copies (original matrix preserved)
- Caller must `delete[]` returned solution vectors

---

## License

MIT License © 2025-2026 Aditya Gowari. See [LICENSE](LICENSE).
