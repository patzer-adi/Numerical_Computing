# How to Use the Numerical Computing Library (`libnumcomp`)

This guide explains how to download, build, link, and use the unified numerical computing library in your own C++ projects. Works on **macOS** and **Linux** — the Makefile auto-detects your OS and builds the correct library format (`.dylib` on macOS, `.so` on Linux).

---

## Table of Contents

1. [What You Get](#1-what-you-get)
2. [Requirements](#2-requirements)
3. [Quick Start — Build Everything in 2 Commands](#3-quick-start)
4. [Three Ways to Use the Library](#4-three-ways-to-use-the-library)
   - [Way 1: Link Against the Dynamic Library (.dylib)](#way-1-dynamic-library-recommended)
   - [Way 2: Link Against the Static Library (.a)](#way-2-static-library)
   - [Way 3: Compile Source Files Directly](#way-3-compile-sources-directly)
5. [The Unified Header — numcomp.hpp](#5-the-unified-header)
6. [What to Include and Why](#6-what-to-include-and-why)
7. [Complete Example Programs](#7-complete-example-programs)
   - [Example 1: Matrix Operations](#example-1-matrix-operations)
   - [Example 2: Solving Ax = b](#example-2-solving-ax--b)
   - [Example 3: Complex Arithmetic](#example-3-complex-arithmetic)
   - [Example 4: Root-Finding](#example-4-root-finding)
   - [Example 5: Property Checks & Diagonal Dominance](#example-5-property-checks)
   - [Example 6: File I/O & Stream Operators](#example-6-file-io)
   - [Example 7: Error Handling](#example-7-error-handling)
8. [Installing System-Wide](#8-installing-system-wide)
9. [Sharing the Library](#9-sharing-the-library)
10. [Common Mistakes and Fixes](#10-common-mistakes-and-fixes)
11. [Full API Quick Reference](#11-full-api-quick-reference)

---

## 1. What You Get

The `libnumcomp` library compiles **19 source files** from 3 modules into a single linkable library:

| Module | Classes | What It Does |
|---|---|---|
| **Matrix** | `Matrix`, `GaussianElimination`, `Doolittle`, `Crout`, `Cholesky`, `GaussJacobi` | Matrix arithmetic, linear algebra, system-of-equation solvers |
| **Root-Finding** | `Bisection`, `NewtonRaphson`, `FixedPoint` | Numerical root-finding algorithms |
| **Complex** | `Complex` | Complex number arithmetic with operator overloading |

**Output files after building:**

| File | Size | Description |
|---|---|---|
| `lib/libnumcomp.dylib` | ~180 KB | Dynamic shared library (**macOS**) |
| `lib/libnumcomp.so` | ~180 KB | Dynamic shared library (**Linux**) |
| `lib/libnumcomp.a` | ~534 KB | Static archive (**both platforms**) |

> Only one of `.dylib` / `.so` is built — whichever matches your OS.

---

## 2. Requirements

- **C++ compiler** with C++11 support — GCC (`g++`) or Clang (`clang++`)
- **GNU Make**

**macOS:**
```bash
xcode-select --install    # installs clang++ and make
```

**Linux (Ubuntu/Debian):**
```bash
sudo apt install build-essential    # installs g++ and make
```

**Linux (Fedora/RHEL):**
```bash
sudo dnf groupinstall "Development Tools"
```

That's it. No external libraries, no package managers, no Python, no Java.

**Check if you're ready:**
```bash
g++ --version    # should print version info
make --version   # should print GNU Make info
```

---

## 3. Quick Start

```bash
# 1. Get the code
git clone <repo-url> Numerical_Computing
cd Numerical_Computing

# 2. Build the library (auto-detects macOS/Linux)
make all

# 3. Build and run the example
make example
./examples/example_usage
```

That's it — three commands. You now have `lib/libnumcomp.dylib` and `lib/libnumcomp.a` ready to link against.

---

## 4. Three Ways to Use the Library

### Way 1: Dynamic Library (recommended)

Build the shared library once (`.dylib` on macOS, `.so` on Linux), then link any program against it:

```bash
# Step 1: Build the library (only need to do this once)
make dylib    # produces .dylib on macOS, .so on Linux

# Step 2: Write your code (my_app.cpp)
# Step 3: Compile and link
g++ -std=c++11 -I. my_app.cpp -Llib -lnumcomp -o my_app

# Step 4: Run (library must be findable at runtime)
./my_app
```

**When to use:** Your main use case. Fast compile times (only your code recompiles), small binary, easy to update the library without recompiling your app.

**Runtime note:** The shared library must be findable when you run the program. The Makefile embeds an rpath for the example, but for your own programs, either:
- Run from the `Numerical_Computing/` directory, or
- Set the library path:
  - **macOS:** `export DYLD_LIBRARY_PATH=./lib:$DYLD_LIBRARY_PATH`
  - **Linux:** `export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH`
- Install system-wide with `sudo make install` (runs `ldconfig` on Linux)

---

### Way 2: Static Library

The entire library gets baked into your executable — no `.dylib`/`.so` needed at runtime:

```bash
# Build
make static

# Link
g++ -std=c++11 -I. my_app.cpp lib/libnumcomp.a -o my_app

# Run anywhere — no shared library files needed
./my_app
```

**When to use:** When you want a self-contained binary that works without the library files present. Larger binary, but zero runtime dependencies. Works identically on macOS and Linux.

---

### Way 3: Compile Sources Directly

No library build step needed — just compile your code alongside the source files:

```bash
g++ -std=c++11 \
    -Imatrix_class -Iroot_finding_methods -IComplex_class_assignment \
    my_app.cpp \
    matrix_class/src/Matrix.cpp \
    matrix_class/src/MatrixOperations.cpp \
    matrix_class/src/MatrixException.cpp \
    -o my_app
```

**When to use:** Quick one-off programs, or when you only need a few classes and don't want to build the full library. Just include the `.cpp` files you actually use.

---

## 5. The Unified Header

Instead of remembering which headers to include, just use:

```cpp
#include "numcomp.hpp"    // includes EVERYTHING
```

This pulls in all classes from all 3 modules. It's defined at the repository root:

```cpp
// numcomp.hpp includes:
// Matrix, MatrixException, GaussianElimination, Doolittle, Crout, Cholesky, GaussJacobi
// RootHunter, Bisection, NewtonRaphson, FixedPoint
// Complex
```

Or include only what you need:

```cpp
#include "matrix_class/include/Matrix.hpp"                    // Matrix only
#include "matrix_class/include/GaussianElimination.hpp"       // + GE solver
#include "matrix_class/include/LUDecomposition.hpp"           // + Doolittle/Crout/Cholesky
#include "matrix_class/include/GaussJacobi.hpp"               // + iterative solver
#include "root_finding_methods/include/Bisection.hpp"         // root-finding
#include "Complex_class_assignment/complexClass_header.hpp"   // complex numbers
```

---

## 6. What to Include and Why

### When linking against the library (.dylib or .a)

You just need the headers — the `.cpp` implementations are already compiled into the library:

```cpp
#include "numcomp.hpp"    // that's all you need
```

Compile with: `g++ -std=c++11 -I. my_app.cpp -Llib -lnumcomp -o my_app`

### When compiling sources directly (Way 3)

You need both the headers AND the corresponding `.cpp` files:

| If you use... | Include this header | Also compile these `.cpp` files |
|---|---|---|
| `Matrix` (basic ops) | `matrix_class/include/Matrix.hpp` | `Matrix.cpp`, `MatrixException.cpp` |
| `transpose`, `inverse`, `adjoint` | same | + `MatrixOperations.cpp` |
| `GaussianElimination` | `matrix_class/include/GaussianElimination.hpp` | + `SystemOfLinearEquationSolver.cpp`, `GaussianElimination.cpp` |
| `Doolittle` / `Crout` / `Cholesky` | `matrix_class/include/LUDecomposition.hpp` | + `LUDecomposition.cpp`, `Doolittle.cpp` (etc.) |
| `GaussJacobi` | `matrix_class/include/GaussJacobi.hpp` | + `GaussJacobi.cpp` |
| `Bisection` / `NewtonRaphson` / etc. | `root_finding_methods/include/Bisection.hpp` | All root-finding `.cpp` files |
| `Complex` | `Complex_class_assignment/complexClass_header.hpp` | `complexClass.cpp` |

---

## 7. Complete Example Programs

### Example 1: Matrix Operations

```cpp
// matrix_demo.cpp
#include "numcomp.hpp"
#include <iostream>
using namespace std;

int main() {
    // Create matrices using operator() for element access
    Matrix A(3, 3);
    A(0,0) = 1;  A(0,1) = 2;  A(0,2) = 3;
    A(1,0) = 4;  A(1,1) = 5;  A(1,2) = 6;
    A(2,0) = 7;  A(2,1) = 8;  A(2,2) = 10;

    // Print using stream operator (friend function)
    cout << "Matrix A:" << endl << A;

    // Arithmetic via operator overloading
    Matrix B = A.transpose();
    Matrix C = A + B;              // addition
    Matrix D = A - B;              // subtraction
    Matrix E = A * B;              // matrix multiplication
    Matrix S = A * 2.5;            // scalar multiplication

    cout << "A + A^T:" << endl << C;
    cout << "A * 2.5:" << endl << S;

    // Determinant and inverse
    double det = A.determinant();
    cout << "det(A) = " << det << endl;

    Matrix inv = A.inverse();
    cout << "Inverse:" << endl << inv;

    // Verify: A * A⁻¹ should be identity
    Matrix I = A * inv;
    cout << "is A * A⁻¹ identity? " << (I.isIdentity() ? "YES" : "NO") << endl;

    // Equality check (floating-point tolerant)
    Matrix A_copy = A;
    cout << "A == copy(A)? " << (A == A_copy ? "YES" : "NO") << endl;

    return 0;
}
```

**Compile:**
```bash
g++ -std=c++11 -I. matrix_demo.cpp -Llib -lnumcomp -o matrix_demo
./matrix_demo
```

---

### Example 2: Solving Ax = b

```cpp
// solver_demo.cpp
#include "numcomp.hpp"
#include <iostream>
using namespace std;

int main() {
    // System: 2x + y - z = 8,  -3x - y + 2z = -11,  -2x + y + 2z = -3

    // --- Method 1: Gaussian Elimination with pivoting ---
    GaussianElimination ge;
    Matrix A(3, 3);
    A(0,0)=2; A(0,1)=1; A(0,2)=-1;
    A(1,0)=-3; A(1,1)=-1; A(1,2)=2;
    A(2,0)=-2; A(2,1)=1; A(2,2)=2;
    ge.copyFrom(A);

    double b[] = {8, -11, -3};
    double *x = ge.solveWithPivoting(b, 3);

    cout << "--- Gaussian Elimination ---" << endl;
    for (int i = 0; i < 3; i++)
        cout << "x[" << i << "] = " << x[i] << endl;
    // Expected: x=2, y=3, z=-1
    delete[] x;

    // --- Method 2: Doolittle LU ---
    Doolittle d;
    d.copyFrom(A);
    double b2[] = {8, -11, -3};
    x = d.solve(b2, 3);  // prints verification automatically

    cout << "\n--- Doolittle LU ---" << endl;
    for (int i = 0; i < 3; i++)
        cout << "x[" << i << "] = " << x[i] << endl;
    delete[] x;

    // --- Method 3: Load from file ---
    Doolittle solver;
    solver.readFromFile("matrix_class/49/49l.txt");  // 49×49 system
    // load b from file, solve, etc.

    return 0;
}
```

---

### Example 3: Complex Arithmetic

```cpp
// complex_demo.cpp
#include "numcomp.hpp"
#include <iostream>
using namespace std;

int main() {
    Complex a(3.0, 4.0);    // 3 + 4i
    Complex b(1.0, -2.0);   // 1 - 2i

    cout << "a = "; a.display();
    cout << "b = "; b.display();

    // Arithmetic (both methods and operators work)
    Complex sum  = a + b;     //  4 + 2i
    Complex diff = a - b;     //  2 + 6i
    Complex prod = a * b;     // 11 - 2i
    Complex quot = a / b;     // -1 + 2i
    Complex conj = a.conjugate();  // 3 - 4i
    float   norm = a.Norm();       // 5.0

    cout << "\na + b = "; sum.display();
    cout << "a - b = "; diff.display();
    cout << "a * b = "; prod.display();
    cout << "a / b = "; quot.display();
    cout << "conj(a) = "; conj.display();
    cout << "|a| = " << norm << endl;

    return 0;
}
```

---

### Example 4: Root-Finding

```cpp
// root_demo.cpp
#include "numcomp.hpp"
#include <iostream>
using namespace std;

int main() {
    // Note: f(x) = x³ - 2x - 5 is hardcoded in the library
    // To change the function, edit root_finding_methods/src/RootHunter.cpp

    // --- Bisection Method ---
    Bisection bis(1e-6);
    bis.input();    // prompts: start value, step size, max steps
    bis.solve();
    cout << "Bisection root: " << bis.getRoot()
         << " (iterations: " << bis.getIterations() << ")" << endl;

    // --- Newton-Raphson ---
    NewtonRaphson nr(1e-6);
    nr.input();     // prompts: initial guess
    nr.solve();
    cout << "Newton-Raphson root: " << nr.getRoot()
         << " (iterations: " << nr.getIterations() << ")" << endl;

    // --- Fixed Point ---
    FixedPoint fp(1e-6);
    fp.input();     // prompts: initial guess
    fp.solve();
    cout << "Fixed Point root: " << fp.getRoot()
         << " (iterations: " << fp.getIterations() << ")" << endl;

    return 0;
}
```

> **Important:** The target function `f(x)`, derivative `df(x)`, and transformation `g(x)` are **hardcoded** in `root_finding_methods/src/RootHunter.cpp`. To solve a different equation, edit those functions and rebuild with `make dylib`.

---

### Example 5: Property Checks

```cpp
// property_demo.cpp
#include "numcomp.hpp"
#include <iostream>
using namespace std;

int main() {
    Matrix A(3, 3);
    A(0,0)=10; A(0,1)=1; A(0,2)=2;
    A(1,0)=3;  A(1,1)=8; A(1,2)=1;
    A(2,0)=1;  A(2,1)=2; A(2,2)=7;

    cout << "Matrix A:" << endl << A;

    // Check all properties
    cout << "isSquare?              " << (A.isSquare() ? "YES" : "NO") << endl;
    cout << "isSymmetric?           " << (A.isSymmetric() ? "YES" : "NO") << endl;
    cout << "isIdentity?            " << (A.isIdentity() ? "YES" : "NO") << endl;
    cout << "isNull?                " << (A.isNull() ? "YES" : "NO") << endl;
    cout << "isDiagonal?            " << (A.isDiagonal() ? "YES" : "NO") << endl;
    cout << "isDiagonallyDominant?  " << (A.isDiagonallyDominant() ? "YES" : "NO") << endl;

    // Try to make a matrix diagonally dominant
    Matrix B(3, 3);
    B(0,0)=1; B(0,1)=5; B(0,2)=2;
    B(1,0)=3; B(1,1)=2; B(1,2)=6;
    B(2,0)=7; B(2,1)=1; B(2,2)=3;

    cout << "\nOriginal B:" << endl << B;
    Matrix D = B.makeDiagonallyDominant();  // rearranges rows
    cout << "After makeDiagonallyDominant:" << endl << D;

    return 0;
}
```

---

### Example 6: File I/O

```cpp
// file_demo.cpp
#include "numcomp.hpp"
#include <iostream>
using namespace std;

int main() {
    // Read a matrix from file
    Matrix A("matrix_class/49/49l.txt");
    cout << "Loaded " << A.getRows() << "x" << A.getCols() << " matrix" << endl;

    // Also works via stream operator
    Matrix B;
    cin >> B;    // prompts: rows, cols, then elements row by row
    cout << "\nYou entered:" << endl << B;

    // Save to file
    A.saveToFile("output_matrix.txt");

    return 0;
}
```

---

### Example 7: Error Handling

```cpp
// error_demo.cpp
#include "numcomp.hpp"
#include <iostream>
using namespace std;

int main() {
    // All errors are caught via MatrixException — the program won't crash

    // Dimension mismatch
    try {
        Matrix A(2, 3);
        Matrix B(4, 5);
        Matrix C = A + B;    // BOOM
    } catch (MatrixException &e) {
        e.printError();
        // "can't add these matrices... they don't even match bro"
    }

    // Singular inverse
    try {
        Matrix A(2, 2);
        A(0,0)=1; A(0,1)=2; A(1,0)=2; A(1,1)=4;  // det = 0
        Matrix inv = A.inverse();
    } catch (MatrixException &e) {
        e.printError();
        // "matrix is singular (det = 0)... inverse doesn't exist"
    }

    // Out of bounds
    try {
        Matrix A(2, 2);
        double x = A(5, 5);  // doesn't exist
    } catch (MatrixException &e) {
        e.printError();
        // "bruh you went out of bounds with ()..."
    }

    cout << "\nAll errors handled — program kept running!" << endl;
    return 0;
}
```

---

## 8. Installing System-Wide

To use the library from anywhere on your system (without `-I` and `-L` flags):

```bash
# Install
sudo make install

# Linux only — update the linker cache:
sudo ldconfig

# Now compile from anywhere:
g++ -std=c++11 -I/usr/local/include/numcomp my_app.cpp -lnumcomp -o my_app
```

Headers are installed to:
```
/usr/local/include/numcomp/
├── matrix/          ← Matrix.hpp, GaussianElimination.hpp, etc.
├── rootfinding/     ← RootHunter.hpp, Bisection.hpp, etc.
├── complex/         ← complexClass_header.hpp
└── numcomp.hpp      ← unified header
```

To remove:
```bash
sudo make uninstall
```

---

## 9. Sharing the Library

### Option A: GitHub (recommended)

```bash
git clone <your-repo-url>
cd Numerical_Computing
make all
make example
./examples/example_usage
```

### Option B: ZIP the whole thing

```bash
zip -r numerical_computing_library.zip Numerical_Computing/
```

They unzip, run `make all`, done.

### Option C: Pre-built library only

Ship just the built library + headers. They need:

```
MINIMUM files someone needs to use the library:
├── lib/
│   └── libnumcomp.dylib (or .a)
├── matrix_class/include/       ← all .hpp files
├── root_finding_methods/include/
├── Complex_class_assignment/complexClass_header.hpp
└── numcomp.hpp
```

---

## 10. Common Mistakes and Fixes

### "undefined reference to Matrix::transpose()"

**Cause:** Not linking against the library.

**Fix:**
```bash
# Add -Llib -lnumcomp
g++ -std=c++11 -I. my_app.cpp -Llib -lnumcomp -o my_app
```

### "dyld: Library not loaded" (macOS) or "cannot open shared object file" (Linux)

**Cause:** OS can't find the shared library at runtime.

**Fix (pick one):**
```bash
# Option 1: Set the library path
# macOS:
export DYLD_LIBRARY_PATH=$(pwd)/lib:$DYLD_LIBRARY_PATH
# Linux:
export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH

./my_app

# Option 2: Install system-wide
sudo make install
# Linux only: sudo ldconfig
./my_app

# Option 3: Use the static library instead (no runtime dependency, works on both platforms)
g++ -std=c++11 -I. my_app.cpp lib/libnumcomp.a -o my_app
```

### "No such file or directory: numcomp.hpp"

**Cause:** Compiling from the wrong directory.

**Fix:** Either `cd` into `Numerical_Computing/` first, or use `-I/path/to/Numerical_Computing`:
```bash
g++ -std=c++11 -I/path/to/Numerical_Computing my_app.cpp -L/path/to/Numerical_Computing/lib -lnumcomp -o my_app
```

### "multiple definition of ..."

**Cause:** You `#include`d a `.cpp` file instead of a `.hpp` file.

**Fix:**
```cpp
#include "numcomp.hpp"                    // ✅ CORRECT
// #include "matrix_class/src/Matrix.cpp" // ❌ NEVER do this
```

### Segfault when calling solve()

**Cause:** You didn't load data into the solver before calling `solve()`.

**Fix:** Always `readFromFile()` or `copyFrom()` first:
```cpp
Doolittle d;
// d.solve(b, n);              // ❌ crash — data is NULL
d.readFromFile("data.txt");    // ✅ load data first
double *x = d.solve(b, n);     // ✅ now it works
```

---

## 11. Full API Quick Reference

### Matrix

```cpp
// Construction
Matrix()                        // empty
Matrix(int r, int c)           // r×c zeros
Matrix(string filename)        // from file
Matrix(const Matrix &other)    // copy

// Element access
A(i, j)                        // read/write via operator()
A.getData(i, j)                // read
A.setData(i, j, val)           // write

// Arithmetic
A + B, A - B, A * B            // operator overloading
A * 2.5                        // scalar multiply

// Linear algebra
A.transpose()
A.determinant()
A.inverse()
A.adjoint()
A.cofactor(r, c)
A.minorMatrix(r, c)

// Properties
A.isSquare()
A.isSymmetric()
A.isIdentity()
A.isNull()
A.isDiagonal()
A.isDiagonallyDominant()
A.makeDiagonallyDominant()

// I/O
cout << A                      // stream output
cin >> A                       // stream input
A.readFromFile("file.txt")
A.saveToFile("file.txt")
A.display()

// Comparison
A == B                         // with floating-point tolerance

// Assignment
A = B                          // deep copy
```

### Solvers

```cpp
// All solvers share this interface:
double* solve(double *b, int n);

// GaussianElimination also has:
double* solveWithPivoting(double *b, int n);
double* solveWithoutPivoting(double *b, int n);
```

### Root-Finding

```cpp
// All methods share this interface:
void input();                  // interactive parameter input
void solve();                  // run the algorithm
double getRoot();
int getIterations();
```

### Complex

```cpp
Complex(float r, float i)
c1 + c2, c1 - c2, c1 * c2, c1 / c2
c.conjugate()
c.Norm()
c.getReal(), c.getImag()
c.display()
```

---

## TL;DR

```bash
# Build
cd Numerical_Computing
make all

# Use in your code
cat > my_app.cpp << 'EOF'
#include "numcomp.hpp"
#include <iostream>
using namespace std;
int main() {
    Matrix A(2, 2);
    A(0,0)=1; A(0,1)=2; A(1,0)=3; A(1,1)=4;
    cout << A;
    cout << "det = " << A.determinant() << endl;
    Complex c(3, 4);
    cout << "|c| = " << c.Norm() << endl;
    return 0;
}
EOF

# Compile and run
g++ -std=c++11 -I. my_app.cpp -Llib -lnumcomp -o my_app
./my_app
```

Four commands, you're done.

---

**License:** MIT © 2025-2026 Aditya Gowari
