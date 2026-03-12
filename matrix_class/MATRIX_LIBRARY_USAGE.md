# How to Share and Use This Library

This guide explains how someone else can download your library, build it, and use it in their own C++ projects.

---

## Table of Contents

1. [Sharing the Library (How to Give It to Someone)](#1-sharing-the-library)
2. [What the Receiver Gets](#2-what-the-receiver-gets)
3. [Quick Start — Build and Run the Menu Program](#3-quick-start--build-and-run-the-menu-program)
4. [Using the Library in Your Own Code](#4-using-the-library-in-your-own-code)
5. [What to Include and Why](#5-what-to-include-and-why)
6. [Compiling Your Own Program — Manual Way](#6-compiling-your-own-program--manual-way)
7. [Compiling Your Own Program — Makefile Way](#7-compiling-your-own-program--makefile-way)
8. [Complete Example Programs](#8-complete-example-programs)
9. [Common Mistakes and Fixes](#9-common-mistakes-and-fixes)
10. [File-by-File Reference — What Each File Does](#10-file-by-file-reference)

---

## 1. Sharing the Library

### Option A: GitHub (recommended)

Push to GitHub, then tell them:
```bash
git clone https://github.com/your-username/matrix_class.git
cd matrix_class
make cpu
./matrix_program
```

### Option B: ZIP file

Zip the folder and send it:
```bash
zip -r matrix_library.zip matrix_class/
```

They unzip and run:
```bash
unzip matrix_library.zip
cd matrix_class
make cpu
./matrix_program
```

### Option C: Just the library files (no menu program)

If someone only wants to use the library in their own code, they need these files:

```
MINIMUM files needed:
├── include/                          ← ALL header files (required)
│   ├── Matrix.hpp
│   ├── MatrixException.hpp
│   ├── SystemOfLinearEquationSolver.hpp
│   ├── GaussianElimination.hpp
│   ├── LUDecomposition.hpp
│   └── GaussJacobi.hpp
├── src/                              ← ALL source files (required)
│   ├── Matrix.cpp
│   ├── MatrixException.cpp
│   ├── SystemOfLinearEquationSolver.cpp
│   ├── GaussianElimination.cpp
│   ├── LUDecomposition.cpp
│   ├── Doolittle.cpp
│   ├── Crout.cpp
│   ├── Cholesky.cpp
│   ├── GaussJacobi.cpp
│   └── MatrixOperations.cpp
└── utils/                            ← OPTIONAL (only for I/O helpers)
    ├── Input.hpp / Input.cpp
    └── Display.hpp / Display.cpp
```

They do NOT need: `main.cpp`, `cuda/`, `test_cases/`, `49/`, `225/`, `Makefile`, any `.o` files, or the documentation files.

---

## 2. What the Receiver Gets

When someone downloads or clones your project, here is every file and what it does:

```
matrix_class/
│
├── main.cpp                    ← THE menu-driven program (they can run this or ignore it)
│
├── include/                    ← HEADER FILES (class declarations)
│   ├── Matrix.hpp                  base Matrix class: data storage, arithmetic, display
│   ├── MatrixException.hpp         custom exception class with meme error messages
│   ├── SystemOfLinearEquationSolver.hpp   abstract base: has virtual solve()
│   ├── GaussianElimination.hpp     GE solver (with/without pivoting)
│   ├── LUDecomposition.hpp         LU base + Doolittle + Crout + Cholesky classes
│   └── GaussJacobi.hpp            iterative solver
│
├── src/                        ← SOURCE FILES (function implementations)
│   ├── Matrix.cpp                  constructors, add/sub/mul, determinant, I/O
│   ├── MatrixOperations.cpp        transpose, inverse, minor, cofactor, adjoint
│   ├── MatrixException.cpp         exception constructor + printError()
│   ├── SystemOfLinearEquationSolver.cpp   base class constructors
│   ├── GaussianElimination.cpp     GE with/without pivoting
│   ├── LUDecomposition.cpp         LU base constructors
│   ├── Doolittle.cpp               Doolittle decomposition + L*U=A verification
│   ├── Crout.cpp                   Crout decomposition + verification
│   ├── Cholesky.cpp                Cholesky decomposition + verification
│   └── GaussJacobi.cpp            Jacobi iteration
│
├── utils/                      ← UTILITY FUNCTIONS (optional helpers)
│   ├── Input.hpp / Input.cpp       read matrices from console or file
│   └── Display.hpp / Display.cpp   display solutions, save to file
│
├── cuda/                       ← GPU BACKEND (optional, needs NVIDIA GPU)
│   ├── include/
│   │   ├── gpu_backend.cuh         GPU function declarations
│   │   └── gpu_dispatch.hpp        BackendDispatcher class
│   └── src/
│       ├── gpu_kernels.cu          CUDA kernels
│       └── gpu_dispatch.cu         CPU/GPU decision logic
│
├── Makefile                    ← BUILD SCRIPT (type "make cpu" to build)
├── 49/ , 225/                  ← TEST DATA (49×49 and 225×225 systems)
├── test_cases/                 ← TEST GENERATOR (SageMath script)
├── examples/                   ← EXAMPLE PROGRAMS (for learning how to use the library)
│
└── DOCUMENTATION:
    ├── README.md               ← original readme
    ├── README_FULL.md          ← complete library reference
    ├── ALGORITHMS.md           ← how each algorithm works with examples
    ├── CUDA_README.md          ← GPU backend deep dive
    ├── UNDER_THE_HOOD.md       ← compiler/linker/loader internals
    ├── LIBRARY_USAGE.md        ← THIS FILE — how to use in your own code
    └── LICENSE                 ← MIT license
```

---

## 3. Quick Start — Build and Run the Menu Program

```bash
cd matrix_class
make cpu
./matrix_program
```

That's it. The menu program lets you try everything interactively.

---

## 4. Using the Library in Your Own Code

Say you want to write your OWN program that uses the Matrix library. Here's how.

### Step 1: Create your file

Create a file called `my_program.cpp` **inside the `matrix_class/` folder**:

```cpp
// my_program.cpp
#include "include/Matrix.hpp"
#include <iostream>
using namespace std;

int main() {
    // create a 3x3 matrix
    Matrix A(3, 3);
    A.setData(0, 0, 1);  A.setData(0, 1, 2);  A.setData(0, 2, 3);
    A.setData(1, 0, 4);  A.setData(1, 1, 5);  A.setData(1, 2, 6);
    A.setData(2, 0, 7);  A.setData(2, 1, 8);  A.setData(2, 2, 9);

    cout << "Matrix A:" << endl;
    A.display();

    cout << "\nTranspose:" << endl;
    Matrix T = A.transpose();
    T.display();

    cout << "\nDeterminant = " << A.determinant() << endl;

    return 0;
}
```

### Step 2: Compile it

You need to compile your file AND all the library source files, then link them:

```bash
g++ -std=c++11 -o my_program \
    my_program.cpp \
    src/Matrix.cpp \
    src/MatrixException.cpp \
    src/MatrixOperations.cpp
```

**Why those 3 files?** Because your code uses `Matrix`, which is implemented in `Matrix.cpp`. `Matrix.cpp` uses `MatrixException`, which is in `MatrixException.cpp`. `transpose()` is in `MatrixOperations.cpp`.

### Step 3: Run it

```bash
./my_program
```

Output:
```
Matrix A:
1.000000  2.000000  3.000000
4.000000  5.000000  6.000000
7.000000  8.000000  9.000000

Transpose:
1.000000  4.000000  7.000000
2.000000  5.000000  8.000000
3.000000  6.000000  9.000000

Determinant = 0
```

---

## 5. What to Include and Why

### Rule: Include the header for what you use

| If you want to use... | Include this header | Also compile these .cpp files |
|---|---|---|
| `Matrix` (add/sub/mul/det/display) | `include/Matrix.hpp` | `src/Matrix.cpp`, `src/MatrixException.cpp` |
| `transpose`, `inverse`, `adjoint`... | `include/Matrix.hpp` | + `src/MatrixOperations.cpp` |
| `GaussianElimination` | `include/GaussianElimination.hpp` | + `src/GaussianElimination.cpp`, `src/SystemOfLinearEquationSolver.cpp` |
| `Doolittle`, `Crout`, `Cholesky` | `include/LUDecomposition.hpp` | + `src/LUDecomposition.cpp`, `src/Doolittle.cpp` (or `Crout.cpp` or `Cholesky.cpp`), `src/SystemOfLinearEquationSolver.cpp` |
| `GaussJacobi` | `include/GaussJacobi.hpp` | + `src/GaussJacobi.cpp`, `src/SystemOfLinearEquationSolver.cpp` |
| `getMatrixInput`, `getSystemInput` | `utils/Input.hpp` | + `utils/Input.cpp` |
| `displaySolution`, `solveLU` | `utils/Display.hpp` | + `utils/Display.cpp` |

### Why you need to compile the .cpp files too

The `.hpp` files only contain **declarations** (telling the compiler "these functions exist"):
```cpp
// Matrix.hpp — declaration only
class Matrix {
    Matrix add(Matrix other);  // "this function exists" — but WHERE is the code?
};
```

The actual code is in the `.cpp` files:
```cpp
// Matrix.cpp — implementation
Matrix Matrix::add(Matrix other) {
    // ... 20 lines of actual code ...
}
```

If you only include the `.hpp` but don't compile the `.cpp`, you get:
```
undefined reference to `Matrix::add(Matrix)'
```
This means: "I know the function exists (from the header) but I can't find the machine code for it (because you didn't compile the .cpp)."

---

## 6. Compiling Your Own Program — Manual Way

### Just Matrix operations (no solvers):

```bash
g++ -std=c++11 -o my_program \
    my_program.cpp \
    src/Matrix.cpp \
    src/MatrixException.cpp \
    src/MatrixOperations.cpp
```

### Matrix + Gaussian Elimination:

```bash
g++ -std=c++11 -o my_program \
    my_program.cpp \
    src/Matrix.cpp \
    src/MatrixException.cpp \
    src/MatrixOperations.cpp \
    src/SystemOfLinearEquationSolver.cpp \
    src/GaussianElimination.cpp
```

### Everything (all solvers):

```bash
g++ -std=c++11 -o my_program \
    my_program.cpp \
    src/Matrix.cpp \
    src/MatrixException.cpp \
    src/MatrixOperations.cpp \
    src/SystemOfLinearEquationSolver.cpp \
    src/GaussianElimination.cpp \
    src/LUDecomposition.cpp \
    src/Doolittle.cpp \
    src/Crout.cpp \
    src/Cholesky.cpp \
    src/GaussJacobi.cpp
```

### With the I/O utilities too:

```bash
g++ -std=c++11 -o my_program \
    my_program.cpp \
    src/Matrix.cpp \
    src/MatrixException.cpp \
    src/MatrixOperations.cpp \
    src/SystemOfLinearEquationSolver.cpp \
    src/GaussianElimination.cpp \
    src/LUDecomposition.cpp \
    src/Doolittle.cpp \
    src/Crout.cpp \
    src/Cholesky.cpp \
    src/GaussJacobi.cpp \
    utils/Input.cpp \
    utils/Display.cpp
```

---

## 7. Compiling Your Own Program — Makefile Way

Instead of typing that long command every time, create a `Makefile` for your project.

### Simple Makefile (put this in the `matrix_class/` folder as `Makefile.user`):

```makefile
# Makefile.user — for building YOUR program against the matrix library

CXX = g++
CXXFLAGS = -std=c++11 -Wall

# your source file
USER_SRC = my_program.cpp

# library source files (include ALL that you need)
LIB_SRCS = src/Matrix.cpp \
           src/MatrixException.cpp \
           src/MatrixOperations.cpp \
           src/SystemOfLinearEquationSolver.cpp \
           src/GaussianElimination.cpp \
           src/LUDecomposition.cpp \
           src/Doolittle.cpp \
           src/Crout.cpp \
           src/Cholesky.cpp \
           src/GaussJacobi.cpp

# output binary name
TARGET = my_program

# build
$(TARGET): $(USER_SRC) $(LIB_SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(USER_SRC) $(LIB_SRCS)

# clean
clean:
	rm -f $(TARGET)
```

Then build with:
```bash
make -f Makefile.user
./my_program
```

---

## 8. Complete Example Programs

### Example 1: Basic Matrix Operations

```cpp
// example_basic.cpp — basic matrix operations demo
#include "include/Matrix.hpp"
#include <iostream>
using namespace std;

int main() {
    // === CREATE MATRICES ===
    Matrix A(2, 2);
    A.setData(0, 0, 1);  A.setData(0, 1, 2);
    A.setData(1, 0, 3);  A.setData(1, 1, 4);

    Matrix B(2, 2);
    B.setData(0, 0, 5);  B.setData(0, 1, 6);
    B.setData(1, 0, 7);  B.setData(1, 1, 8);

    // === ADDITION ===
    cout << "A + B:" << endl;
    Matrix C = A + B;
    C.display();
    // Output: 6 8 / 10 12

    // === SUBTRACTION ===
    cout << "\nA - B:" << endl;
    Matrix D = A - B;
    D.display();
    // Output: -4 -4 / -4 -4

    // === MULTIPLICATION ===
    cout << "\nA * B:" << endl;
    Matrix E = A * B;
    E.display();
    // Output: 19 22 / 43 50

    // === SCALAR MULTIPLY ===
    cout << "\nA * 3:" << endl;
    Matrix F = A * 3.0;
    F.display();
    // Output: 3 6 / 9 12

    // === TRANSPOSE ===
    cout << "\nTranspose of A:" << endl;
    Matrix T = A.transpose();
    T.display();
    // Output: 1 3 / 2 4

    // === DETERMINANT ===
    cout << "\ndet(A) = " << A.determinant() << endl;
    // Output: -2

    // === INVERSE ===
    cout << "\nInverse of A:" << endl;
    Matrix inv = A.inverse();
    inv.display();
    // Output: -2 1 / 1.5 -0.5

    return 0;
}
```

**Compile:**
```bash
g++ -std=c++11 -o example_basic example_basic.cpp \
    src/Matrix.cpp src/MatrixException.cpp src/MatrixOperations.cpp
```

---

### Example 2: Solving a System with Gaussian Elimination

```cpp
// example_gauss.cpp — solve Ax = b using Gaussian Elimination
#include "include/GaussianElimination.hpp"
#include <iostream>
using namespace std;

int main() {
    // System:
    //  2x + y - z  =  8
    // -3x - y + 2z = -11
    // -2x + y + 2z = -3

    GaussianElimination ge;

    // set up the 3x3 coefficient matrix A
    ge.setRows(3);
    ge.setCols(3);
    // allocate and fill data manually
    double **data = new double*[3];
    for (int i = 0; i < 3; i++)
        data[i] = new double[3];

    data[0][0] = 2;  data[0][1] = 1;  data[0][2] = -1;
    data[1][0] = -3; data[1][1] = -1; data[1][2] = 2;
    data[2][0] = -2; data[2][1] = 1;  data[2][2] = 2;

    // use readFromConsole or readFromFile instead for real use
    // but for this example we'll use the internal copy method
    Matrix temp(3, 3);
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            temp.setData(i, j, data[i][j]);
    ge.copyFrom(temp);

    // RHS vector b
    double b[] = {8, -11, -3};

    // solve!
    double *x = ge.solve(b, 3);

    // print solution
    cout << "Solution:" << endl;
    for (int i = 0; i < 3; i++)
        cout << "x[" << i << "] = " << x[i] << endl;
    // Expected: x = 2, y = 3, z = -1

    // cleanup
    delete[] x;
    for (int i = 0; i < 3; i++)
        delete[] data[i];
    delete[] data;

    return 0;
}
```

**Compile:**
```bash
g++ -std=c++11 -o example_gauss example_gauss.cpp \
    src/Matrix.cpp src/MatrixException.cpp src/MatrixOperations.cpp \
    src/SystemOfLinearEquationSolver.cpp src/GaussianElimination.cpp
```

---

### Example 3: Solving with Doolittle LU (with verification)

```cpp
// example_lu.cpp — solve Ax = b using LU Doolittle with verification
#include "include/LUDecomposition.hpp"
#include <iostream>
using namespace std;

int main() {
    // same system as above but using LU
    Doolittle solver;

    Matrix temp(3, 3);
    temp.setData(0, 0, 2);  temp.setData(0, 1, 1);  temp.setData(0, 2, -1);
    temp.setData(1, 0, -3); temp.setData(1, 1, -1); temp.setData(1, 2, 2);
    temp.setData(2, 0, -2); temp.setData(2, 1, 1);  temp.setData(2, 2, 2);
    solver.copyFrom(temp);

    double b[] = {8, -11, -3};

    // this will print "Doolittle LU verification PASSED ✅" automatically
    double *x = solver.solve(b, 3);

    cout << "Solution:" << endl;
    for (int i = 0; i < 3; i++)
        cout << "x[" << i << "] = " << x[i] << endl;

    delete[] x;
    return 0;
}
```

**Compile:**
```bash
g++ -std=c++11 -o example_lu example_lu.cpp \
    src/Matrix.cpp src/MatrixException.cpp src/MatrixOperations.cpp \
    src/SystemOfLinearEquationSolver.cpp src/LUDecomposition.cpp src/Doolittle.cpp
```

---

### Example 4: Loading from File (like the 49×49 test)

```cpp
// example_file.cpp — load a system from files and solve
#include "include/LUDecomposition.hpp"
#include "utils/Input.hpp"
#include "utils/Display.hpp"
#include <iostream>
using namespace std;

int main() {
    Doolittle solver;
    double *b = NULL;
    int n = 0;

    // load from files (same as menu option 3)
    getSystemInput(solver, b, n);

    // solve
    double *x = solver.solve(b, n);

    // display
    displaySolution(x, n);

    // save
    saveSolution(x, n, "my_solution.txt");

    // cleanup
    delete[] x;
    delete[] b;

    return 0;
}
```

**Compile (needs everything including utils):**
```bash
g++ -std=c++11 -o example_file example_file.cpp \
    src/Matrix.cpp src/MatrixException.cpp src/MatrixOperations.cpp \
    src/SystemOfLinearEquationSolver.cpp src/LUDecomposition.cpp src/Doolittle.cpp \
    utils/Input.cpp utils/Display.cpp
```

---

### Example 5: Error Handling

```cpp
// example_errors.cpp — showing how errors work
#include "include/Matrix.hpp"
#include "include/LUDecomposition.hpp"
#include <iostream>
using namespace std;

int main() {
    // --- Error 1: dimension mismatch ---
    try {
        Matrix A(2, 3);
        Matrix B(4, 5);
        Matrix C = A + B;    // BOOM — dimensions don't match
    } catch (MatrixException &e) {
        e.printError();
        // prints: "can't add these matrices... they don't even match bro"
    }

    // --- Error 2: singular inverse ---
    try {
        Matrix A(2, 2);
        A.setData(0, 0, 1); A.setData(0, 1, 2);
        A.setData(1, 0, 2); A.setData(1, 1, 4);  // det = 0!
        Matrix inv = A.inverse();
    } catch (MatrixException &e) {
        e.printError();
        // prints: "matrix is singular... inverse doesn't exist, just like my social life"
    }

    // --- Error 3: Cholesky on non-symmetric ---
    try {
        Cholesky ch;
        Matrix temp(2, 2);
        temp.setData(0, 0, 1); temp.setData(0, 1, 2);
        temp.setData(1, 0, 3); temp.setData(1, 1, 4);  // NOT symmetric
        ch.copyFrom(temp);

        double b[] = {1, 2};
        double *x = ch.solve(b, 2);
    } catch (MatrixException &e) {
        e.printError();
        // prints: "matrix is not symmetric..."
    }

    cout << "\nAll errors handled gracefully!" << endl;
    return 0;
}
```

---

## 9. Common Mistakes and Fixes

### Mistake 1: "undefined reference to Matrix::transpose()"

**Cause:** You forgot to compile `MatrixOperations.cpp`.

**Fix:** Add `src/MatrixOperations.cpp` to your compile command:
```bash
g++ ... src/MatrixOperations.cpp ...
```

### Mistake 2: "undefined reference to MatrixException::MatrixException..."

**Cause:** You forgot to compile `MatrixException.cpp`.

**Fix:** Always include `src/MatrixException.cpp` — almost everything needs it.

### Mistake 3: "undefined reference to SystemOfLinearEquationSolver::..."

**Cause:** You're using a solver but forgot `SystemOfLinearEquationSolver.cpp`.

**Fix:** Any solver (GE, LU, GJ) needs:
```bash
src/SystemOfLinearEquationSolver.cpp
```

### Mistake 4: "No such file or directory: include/Matrix.hpp"

**Cause:** You're running the compile command from the wrong directory.

**Fix:** `cd` into the `matrix_class/` folder first:
```bash
cd matrix_class
g++ -std=c++11 -o my_program my_program.cpp src/Matrix.cpp ...
```

### Mistake 5: "multiple definition of Matrix::add()"

**Cause:** You included a `.cpp` file with `#include` instead of the `.hpp`:
```cpp
#include "src/Matrix.cpp"    // WRONG — never include .cpp files!
#include "include/Matrix.hpp" // CORRECT
```

### Mistake 6: Segfault when using solver

**Cause:** You forgot to set up the matrix data before calling `solve()`.

**Fix:** Either `readFromFile()` or `copyFrom()` before solving:
```cpp
Doolittle d;
// d.solve(b, n);          ← CRASH! data is NULL
d.readFromFile("matrix.txt"); // or d.copyFrom(someMatrix);
double *x = d.solve(b, n);   // now it works
```

---

## 10. File-by-File Reference

### Header files (what they declare)

| File | Classes / Functions Declared |
|---|---|
| `Matrix.hpp` | `Matrix` — constructors, add, subtract, multiply, operator+/-/*, determinant, transpose, inverse, minorMatrix, cofactor, adjoint, isSymmetric, display, readFromFile, saveToFile, setData, getData |
| `MatrixException.hpp` | `MatrixException` — constructor(string), printError() |
| `SystemOfLinearEquationSolver.hpp` | `SystemOfLinearEquationSolver` — abstract class, pure virtual solve() |
| `GaussianElimination.hpp` | `GaussianElimination` — solve(), solveWithPivoting(), solveWithoutPivoting() |
| `LUDecomposition.hpp` | `LUDecomposition` (abstract), `Doolittle`, `Crout`, `Cholesky` — each has solve() |
| `GaussJacobi.hpp` | `GaussJacobi` — solve() with max iterations and tolerance |
| `utils/Input.hpp` | `getMatrixInput()`, `getSystemInput()` — console/file I/O |
| `utils/Display.hpp` | `displaySolution()`, `saveSolution()`, `solveGaussian()`, `solveLU()`, `solveIterative()` |

### Source files (what they implement)

| File | Functions Implemented |
|---|---|
| `src/Matrix.cpp` | All Matrix constructors, destructor, operator=, add/subtract/multiply, operator+/-/*(Matrix), determinant, isSymmetric, readFromConsole, readFromFile, saveToFile, display, getRowPointer, copyFrom |
| `src/MatrixOperations.cpp` | operator*(double scalar), transpose(), minorMatrix(), cofactor(), adjoint(), inverse() |
| `src/MatrixException.cpp` | MatrixException constructor, printError() |
| `src/SystemOfLinearEquationSolver.cpp` | SLE constructors |
| `src/GaussianElimination.cpp` | solve(), solveWithPivoting(), solveWithoutPivoting() |
| `src/LUDecomposition.cpp` | LU base constructors |
| `src/Doolittle.cpp` | Doolittle::solve() with L*U=A verification |
| `src/Crout.cpp` | Crout::solve() with L*U=A verification |
| `src/Cholesky.cpp` | Cholesky::solve() with L*L^T=A verification + symmetry check |
| `src/GaussJacobi.cpp` | GaussJacobi::solve() with convergence check |
| `utils/Input.cpp` | All I/O input functions |
| `utils/Display.cpp` | All display/save/workflow functions |

### Dependency chain (what needs what)

```
If you use...          You MUST compile...
─────────────────────────────────────────────────
Matrix                 Matrix.cpp, MatrixException.cpp
 + operations          + MatrixOperations.cpp
 + GE                  + SystemOfLinearEquationSolver.cpp, GaussianElimination.cpp
 + Doolittle           + SystemOfLinearEquationSolver.cpp, LUDecomposition.cpp, Doolittle.cpp
 + Crout               + SystemOfLinearEquationSolver.cpp, LUDecomposition.cpp, Crout.cpp
 + Cholesky            + SystemOfLinearEquationSolver.cpp, LUDecomposition.cpp, Cholesky.cpp
 + GaussJacobi         + SystemOfLinearEquationSolver.cpp, GaussJacobi.cpp
 + file I/O            + utils/Input.cpp
 + display/save        + utils/Display.cpp
```

---

## TL;DR for Someone Who Just Wants to Use It

```bash
# 1. get the library
git clone <url>
cd matrix_class

# 2. write your code
cat > my_code.cpp << 'EOF'
#include "include/Matrix.hpp"
#include <iostream>
using namespace std;
int main() {
    Matrix A(2, 2);
    A.setData(0,0,1); A.setData(0,1,2);
    A.setData(1,0,3); A.setData(1,1,4);
    A.display();
    cout << "det = " << A.determinant() << endl;
    return 0;
}
EOF

# 3. compile
g++ -std=c++11 -o my_code my_code.cpp src/Matrix.cpp src/MatrixException.cpp src/MatrixOperations.cpp

# 4. run
./my_code
```

Done. That's it. Three commands.
