# Numerical Computing Library — `libnumcomp`

A unified C++ shared library combining **matrix algebra**, **linear system solvers**, **root-finding algorithms**, and **complex number arithmetic** into a single linkable package.

**Author:** Aditya Gowari  
**License:** MIT  
**Platform:** macOS (`.dylib`), Linux (`.so`)

---

## What's Inside

The library bundles **3 modules** from the Numerical Computing repository:

| Module | Classes | Source Files |
|---|---|---|
| **Matrix** | `Matrix`, `GaussianElimination`, `Doolittle`, `Crout`, `Cholesky`, `GaussJacobi` | 12 files |
| **Root-Finding** | `RootHunter`, `Bisection`, `NewtonRaphson`, `FixedPoint` | 6 files |
| **Complex** | `Complex` | 1 file |

**Total:** 19 source files → 161 exported symbols → 180 KB dynamic library

---

## Quick Start

### Build

```bash
cd Numerical_Computing

# Dynamic library (macOS .dylib)
make dylib

# Static library (.a)
make static

# Both
make all
```

Output:
```
lib/libnumcomp.dylib    ← dynamic shared library (180 KB)
lib/libnumcomp.a        ← static archive (534 KB)
```

### Link Your Own Program

```bash
# Using the dynamic library
g++ -std=c++11 -I. my_app.cpp -Llib -lnumcomp -o my_app

# Using the static library
g++ -std=c++11 -I. my_app.cpp lib/libnumcomp.a -o my_app
```

### Build & Run the Example

```bash
make example
./examples/example_usage
```

---

## Unified Header

Include everything with a single line:

```cpp
#include "numcomp.hpp"
```

This pulls in all classes from all 3 modules. Or include individual modules:

```cpp
// Matrix only
#include "matrix_class/include/Matrix.hpp"
#include "matrix_class/include/GaussianElimination.hpp"
#include "matrix_class/include/LUDecomposition.hpp"      // Doolittle, Crout, Cholesky
#include "matrix_class/include/GaussJacobi.hpp"

// Root-finding only
#include "root_finding_methods/include/Bisection.hpp"
#include "root_finding_methods/include/NewtonRaphson.hpp"
#include "root_finding_methods/include/FixedPoint.hpp"

// Complex only
#include "Complex_class_assignment/complexClass_header.hpp"
```

---

## Class Hierarchy

```
Matrix                                      ← base class (storage + operations)
│   data[][], rows, cols
│   +, -, *, scalar *, transpose, determinant
│   inverse, adjoint, cofactor, minorMatrix
│   isSquare, isSymmetric, isIdentity, isNull
│   isDiagonal, isDiagonallyDominant, makeDiagonallyDominant
│   operator<<, operator>>, operator(), operator==, operator=
│
└── SystemOfLinearEquationSolver            ← abstract (solve() = 0)
      ├── GaussianElimination               ← with/without pivoting
      ├── LUDecomposition                   ← abstract
      │     ├── Doolittle
      │     ├── Crout
      │     └── Cholesky
      └── GaussJacobi                       ← iterative (diag. dominant)


RootHunter                                  ← abstract (input(), solve() = 0)
├── Bisection                               ← bracketing method
├── NewtonRaphson                           ← quadratic convergence
└── FixedPoint                              ← x = g(x) iteration


Complex                                     ← standalone class
    +, -, *, /, conjugate, Norm
    operator+, operator-, operator*, operator/
```

---

## API Reference

### Matrix Class

| Method | Description |
|---|---|
| `Matrix()` | Default constructor (empty) |
| `Matrix(int r, int c)` | Create r×c zero matrix |
| `Matrix(string filename)` | Load from file |
| `Matrix(const Matrix &other)` | Deep copy constructor |
| `getRows()`, `getCols()` | Dimensions |
| `getData(i, j)`, `setData(i, j, val)` | Element access (bounds-checked) |
| `operator()(i, j)` | Element access — `A(1,2)` (returns reference) |
| `operator+`, `operator-`, `operator*` | Matrix arithmetic |
| `operator*(double)` | Scalar multiplication |
| `operator==` | Equality (with floating-point tolerance 1e-10) |
| `operator=` | Deep copy assignment |
| `operator<<` | Stream output — `cout << A` (friend function) |
| `operator>>` | Stream input — `cin >> A` (friend function) |
| `transpose()` | Return transposed matrix |
| `determinant()` | Gaussian elimination method |
| `inverse()` | Via adjoint/determinant |
| `adjoint()` | Transpose of cofactor matrix |
| `cofactor(r, c)` | Signed minor determinant |
| `minorMatrix(r, c)` | Remove row r, col c |
| `isSquare()` | rows == cols? |
| `isSymmetric()` | A == Aᵀ? |
| `isIdentity()` | Diagonal=1, rest=0? |
| `isNull()` | All zeros? |
| `isDiagonal()` | Off-diagonal=0? |
| `isDiagonallyDominant()` | \|diag\| ≥ Σ\|off-diag\| per row? |
| `makeDiagonallyDominant()` | Row-swap attempt to achieve dominance |
| `readFromConsole()` | Interactive input |
| `readFromFile(string)` | File input (auto-detect format) |
| `saveToFile(string)` | Write to file |
| `display()` | Print to stdout |

### Linear System Solvers

All solvers inherit from `Matrix` and expose:

```cpp
double* solve(double *b, int n);  // returns solution vector x
```

| Solver | Algorithm | Best For |
|---|---|---|
| `GaussianElimination` | Row reduction ± pivoting | General dense systems |
| `Doolittle` | LU, L has unit diagonal | Multiple right-hand sides |
| `Crout` | LU, U has unit diagonal | Multiple right-hand sides |
| `Cholesky` | LL^T decomposition | Symmetric positive-definite |
| `GaussJacobi` | Iterative (tol 1e-10, max 10000) | Diagonally dominant / sparse |

Additional methods on `GaussianElimination`:
```cpp
double* solveWithPivoting(double *b, int n);
double* solveWithoutPivoting(double *b, int n);
```

### Root-Finding Methods

All methods inherit from `RootHunter`:

```cpp
void input();           // interactive parameter input
void solve();           // run the algorithm
double getRoot();       // get result
int getIterations();    // get iteration count
```

| Method | Convergence | Requires |
|---|---|---|
| `Bisection` | Linear | Bracketing interval with sign change |
| `NewtonRaphson` | Quadratic | Initial guess, derivative f'(x) |
| `FixedPoint` | Linear | Transformation g(x) such that x = g(x) |

> **Note:** The target function `f(x)`, derivative `df(x)`, and fixed-point transform `g(x)` are currently hardcoded in the source. To change the function, modify `root_finding_methods/src/RootHunter.cpp` and rebuild.

### Complex Class

```cpp
Complex()                    // default (0 + 0i)
Complex(float r, float i)   // parameterized
```

| Method | Description |
|---|---|
| `add()`, `operator+` | Complex addition |
| `subtract()`, `operator-` | Complex subtraction |
| `multiply()`, `operator*` | Complex multiplication |
| `divide()`, `operator/` | Complex division |
| `conjugate()` | a - bi |
| `Norm()` | √(a² + b²) |
| `getReal()`, `getImag()` | Accessors |
| `input()` | Interactive input |
| `display()` | Print to stdout |

---

## Usage Examples

### Matrix Operations

```cpp
#include "numcomp.hpp"
#include <iostream>
using namespace std;

int main() {
    // Create and fill a matrix
    Matrix A(3, 3);
    A(0,0) = 6;  A(0,1) = 1;  A(0,2) = 1;
    A(1,0) = 4;  A(1,1) = -2; A(1,2) = 5;
    A(2,0) = 2;  A(2,1) = 8;  A(2,2) = 7;

    // Print using stream operator
    cout << "Matrix A:" << endl << A;

    // Arithmetic
    Matrix B = A.transpose();
    Matrix C = A + B;           // A + Aᵀ (symmetric)
    Matrix S = A * 3.0;         // scalar multiply

    // Properties
    cout << "det(A) = " << A.determinant() << endl;
    cout << "Symmetric? " << (C.isSymmetric() ? "yes" : "no") << endl;

    // Inverse
    Matrix inv = A.inverse();
    Matrix I = A * inv;         // should be identity
    cout << "A * A⁻¹ is identity? " << (I.isIdentity() ? "yes" : "no") << endl;

    return 0;
}
```

### Solving Ax = b (Doolittle LU)

```cpp
#include "numcomp.hpp"

int main() {
    Doolittle solver;
    solver.readFromFile("matrix_class/49/49l.txt");

    double *b = new double[49];
    // ... load b from file ...

    double *x = solver.solve(b, 49);

    for (int i = 0; i < 49; i++)
        cout << "x[" << i << "] = " << x[i] << endl;

    delete[] x;
    delete[] b;
    return 0;
}
```

### Complex Arithmetic

```cpp
#include "numcomp.hpp"
#include <iostream>

int main() {
    Complex a(3.0, 4.0);    // 3 + 4i
    Complex b(1.0, -2.0);   // 1 - 2i

    Complex sum  = a + b;
    Complex prod = a * b;
    Complex conj = a.conjugate();
    float   norm = a.Norm();  // 5.0

    sum.display();   // 4 + 2i
    prod.display();  // 11 - 2i
    return 0;
}
```

### Compiling Examples

```bash
# Against the dynamic library
g++ -std=c++11 -I. my_app.cpp -Llib -lnumcomp -o my_app
./my_app

# Against the static library (no .dylib needed at runtime)
g++ -std=c++11 -I. my_app.cpp lib/libnumcomp.a -o my_app
./my_app
```

---

## Makefile Targets

| Target | What It Does |
|---|---|
| `make dylib` | Build `lib/libnumcomp.dylib` |
| `make static` | Build `lib/libnumcomp.a` |
| `make all` | Build both dylib + static |
| `make example` | Build & compile example program |
| `make install` | Copy library + headers to `/usr/local` |
| `make uninstall` | Remove from `/usr/local` |
| `make clean` | Remove all build artifacts |
| `make info` | Print build configuration |

---

## Install System-Wide

```bash
sudo make install
```

This copies:
- `libnumcomp.dylib` and `libnumcomp.a` → `/usr/local/lib/`
- All headers → `/usr/local/include/numcomp/`

After installing, compile anywhere without `-I` and `-L` flags:

```bash
g++ -std=c++11 -I/usr/local/include/numcomp my_app.cpp -lnumcomp -o my_app
```

To remove:
```bash
sudo make uninstall
```

---

## Source Files Included

### Matrix Module (12 files)

| File | Contents |
|---|---|
| `matrix_class/src/Matrix.cpp` | Constructors, I/O, add/sub/mul, determinant |
| `matrix_class/src/MatrixOperations.cpp` | Transpose, inverse, cofactor, adjoint, stream ops, property checks |
| `matrix_class/src/MatrixException.cpp` | Custom exception class |
| `matrix_class/src/SystemOfLinearEquationSolver.cpp` | Abstract SLE base |
| `matrix_class/src/GaussianElimination.cpp` | GE with/without pivoting |
| `matrix_class/src/LUDecomposition.cpp` | LU base constructors |
| `matrix_class/src/Doolittle.cpp` | Doolittle LU + verification |
| `matrix_class/src/Crout.cpp` | Crout LU + verification |
| `matrix_class/src/Cholesky.cpp` | Cholesky LL^T + verification |
| `matrix_class/src/GaussJacobi.cpp` | Jacobi iterative solver |
| `matrix_class/utils/Input.cpp` | Matrix & system input helpers |
| `matrix_class/utils/Display.cpp` | Result display & file save |

### Root-Finding Module (6 files)

| File | Contents |
|---|---|
| `root_finding_methods/src/RootHunter.cpp` | Base class + f(x), g(x), df(x) definitions |
| `root_finding_methods/src/Biscetion.cpp` | Bisection method |
| `root_finding_methods/src/NewtonRaphson.cpp` | Newton-Raphson method |
| `root_finding_methods/src/FixedPointer.cpp` | Fixed-point iteration |
| `root_finding_methods/utils/DisplayUtils.cpp` | Result display helper |
| `root_finding_methods/utils/InputUtils.cpp` | Tolerance input helper |

### Complex Module (1 file)

| File | Contents |
|---|---|
| `Complex_class_assignment/complexClass.cpp` | Complex number class implementation |

---

## Build Requirements

- **C++ compiler** with C++11 support (GCC, Clang, or MSVC)
- **GNU Make**
- macOS (produces `.dylib`) or Linux (produces `.so`)

No external dependencies required.

---

## License

MIT License © 2025-2026 Aditya Gowari. See [LICENSE](LICENSE).
