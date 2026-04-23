# Matrix Operations Program — Complete Technical Reference

A menu-driven C++ numerical computing library built from scratch with **zero external dependencies**. Implements matrix operations, linear system solvers, eigenvalue analysis, interpolation, and curve fitting — all using OOP principles and the Matrix class as the foundation.

---

## Table of Contents

1. [Project Architecture](#project-architecture)
2. [C++ Concepts Used](#c-concepts-used)
3. [Class Hierarchy](#class-hierarchy)
4. [Numerical Methods Implemented](#numerical-methods-implemented)
5. [How to Build and Run](#how-to-build-and-run)
6. [Detailed README Index](#detailed-readme-index)

---

## Project Architecture

### Three-Layer Design

```
┌─────────────────────────────────────────────────────┐
│  APP LAYER (app/)                                   │
│  Menu.cpp — menu display, routing, handler functions│
│  This is the ONLY layer that knows about the menu.  │
└────────────────────┬────────────────────────────────┘
                     │ calls
┌────────────────────▼────────────────────────────────┐
│  UTILS LAYER (utils/)                               │
│  Input.cpp  — reading matrices from console/files   │
│  Display.cpp — printing results, solver workflows   │
│  Raw I/O only. No menu logic. No math.              │
└────────────────────┬────────────────────────────────┘
                     │ calls
┌────────────────────▼────────────────────────────────┐
│  CORE LIBRARY (include/ + src/)                     │
│  Matrix, Solvers, Interpolation, EigenSolver        │
│  Pure computation. NO cin/cout. NO user interaction.│
└─────────────────────────────────────────────────────┘
```

### Directory Structure

```
matrix_class/
├── main.cpp                    ← entry point (6 lines — just calls runMenu())
├── Makefile                    ← build system (CPU + GPU targets)
├── README.md                   ← this file
│
├── include/                    ← headers (class declarations)
│   ├── Matrix.hpp                  base class (Rule of 5, const-correct)
│   ├── MatrixException.hpp         custom exception class
│   ├── SolverResult.hpp            struct returned by all solvers
│   ├── SystemOfLinearEquationSolver.hpp   abstract solver base
│   ├── GaussianElimination.hpp     GE (with/without pivoting)
│   ├── LUDecomposition.hpp         LU base + Doolittle/Crout/Cholesky
│   ├── GaussJacobi.hpp             iterative solver
│   ├── GaussSeidel.hpp             iterative solver
│   ├── EigenSolver.hpp             abstract eigen base + GershgorinDisc struct
│   ├── GershgorinAnalyzer.hpp      Gershgorin circle theorem
│   ├── Interpolation.hpp           abstract interpolation base
│   ├── Lagrange.hpp                Lagrange polynomial
│   ├── LeastSquareLine.hpp         least squares line fit
│   └── LeastSquareParabola.hpp     least squares parabola fit
│
├── src/                        ← implementations
│   ├── Matrix.cpp                  constructors, I/O, add/sub/mul, determinant
│   ├── MatrixOperations.cpp        transpose, inverse, property checks
│   ├── MatrixException.cpp         (empty — header-only class)
│   ├── SystemOfLinearEquationSolver.cpp   base + diagonal dominance helpers
│   ├── GaussianElimination.cpp     forward elim + back substitution
│   ├── LUDecomposition.cpp         base class constructors
│   ├── Doolittle.cpp               Doolittle LU decomposition
│   ├── Crout.cpp                   Crout LU decomposition
│   ├── Cholesky.cpp                Cholesky LU decomposition
│   ├── GaussJacobi.cpp             Gauss-Jacobi iterative solver
│   ├── GaussSeidel.cpp             Gauss-Seidel iterative solver
│   ├── EigenSolver.cpp             base class constructor
│   ├── GershgorinAnalyzer.cpp      disc computation + analysis
│   ├── Interpolation.cpp           data loading, interpolate overloads
│   ├── Lagrange.cpp                Lagrange formula
│   ├── LeastSquareLine.cpp         normal equations (2×2) + error analysis
│   └── LeastSquareParabola.cpp     normal equations (3×3) + error analysis
│
├── utils/                      ← I/O helpers
│   ├── Input.hpp / Input.cpp       matrix/system input from console or files
│   └── Display.hpp / Display.cpp   result display, solver workflows
│
├── app/                        ← application layer
│   ├── Menu.hpp / Menu.cpp         menu loop + handler functions
│
├── cuda/                       ← GPU acceleration (optional)
│   ├── include/                    GPU function declarations
│   └── src/                        CUDA kernels
│
├── examples/                   ← example data files
├── Readme/                     ← detailed documentation (18 files)
├── 49/ , 225/                  ← test data (49×49, 225×225 systems)
└── output/                     ← saved solution outputs
```

---

## C++ Concepts Used

### 1. Classes and Objects

Every component is a class. The `Matrix` class encapsulates a 2D dynamic array (`double**`) with its dimensions (`rows`, `cols`), hiding raw pointer manipulation behind clean methods.

```cpp
class Matrix {
protected:
  double **data;    // 2D heap array
  int rows, cols;
public:
  void setData(int i, int j, double val);
  double getData(int i, int j) const;
  void display() const;
  // ...
};
```

**Where used:** Every single file in the project.

### 2. Constructors — Five Types

| Type | Signature | Purpose |
|------|-----------|---------|
| Default | `Matrix()` | Creates empty matrix (data=nullptr, rows=cols=0) |
| Parameterized | `Matrix(int r, int c)` | Allocates r×c, fills with zeros |
| Copy | `Matrix(const Matrix &other)` | Deep copy of another matrix |
| Move | `Matrix(Matrix &&other) noexcept` | Steals resources, leaves other empty |
| File | `Matrix(string filename)` | Reads matrix from file |

```cpp
Matrix A;                  // default
Matrix B(3, 3);            // parameterized
Matrix C(B);               // copy
Matrix D(std::move(C));    // move — C is now empty
Matrix E("data.txt");      // file
```

**Constructor Overloading:** Same class name `Matrix()`, different parameter lists. The compiler picks the right one based on arguments passed.

### 3. Rule of 5

Since `Matrix` manages raw heap memory (`new double*[]`), it must implement all five special members to prevent memory leaks and dangling pointers:

| Member | What It Does |
|--------|-------------|
| **Destructor** `~Matrix()` | `delete[]` every row, then `delete[] data` |
| **Copy Constructor** `Matrix(const Matrix&)` | Allocates new memory, copies every element |
| **Copy Assignment** `operator=(const Matrix&)` | Frees old memory, then deep copies |
| **Move Constructor** `Matrix(Matrix&&)` | Steals `data` pointer, sets other to nullptr |
| **Move Assignment** `operator=(Matrix&&)` | Frees old, steals from other |

**Why move semantics?** When returning a Matrix from a function (`return result;`), the move constructor avoids copying the entire 2D array — it just transfers the pointer.

### 4. Operator Overloading

| Operator | Type | Signature | Usage |
|----------|------|-----------|-------|
| `+` | Binary arithmetic | `Matrix operator+(const Matrix&) const` | `C = A + B` |
| `-` | Binary arithmetic | `Matrix operator-(const Matrix&) const` | `C = A - B` |
| `*` | Matrix multiply | `Matrix operator*(const Matrix&) const` | `C = A * B` |
| `*` | Scalar multiply | `Matrix operator*(double) const` | `C = A * 2.5` |
| `==` | Equality | `bool operator==(const Matrix&) const` | `if (A == B)` |
| `()` | Element access | `double& operator()(int, int)` | `A(i,j) = 5.0` |
| `()` | Const access | `double operator()(int, int) const` | `val = A(i,j)` |
| `=` | Copy assign | `Matrix& operator=(const Matrix&)` | `A = B` |
| `=` | Move assign | `Matrix& operator=(Matrix&&)` | `A = std::move(B)` |
| `<<` | Stream insert | `friend ostream& operator<<(...)` | `cout << A` |
| `>>` | Stream extract | `friend istream& operator>>(...)` | `cin >> A` |

**Friend functions:** `<<` and `>>` are declared `friend` because the left operand is `ostream`/`istream`, not `Matrix`. A member function's first operand is always `this`.

### 5. Function Overloading

Same function name, different parameter types:

```cpp
// In Interpolation — same name, different first parameter type
void interpolate(int samples, bool saveToFile, string filename);     // full range
void interpolate(double queryX, bool saveToFile, string filename);   // single point

// In Matrix — operator* overloaded for different right operands
Matrix operator*(const Matrix &other) const;    // matrix × matrix
Matrix operator*(double scalar) const;          // matrix × scalar
```

The compiler resolves which to call based on argument types at the call site.

### 6. Inheritance

Three inheritance hierarchies in the project:

**Hierarchy 1: Matrix → Solvers (IS-A relationship)**
```
Matrix
  └── SystemOfLinearEquationSolver (abstract)
        ├── GaussianElimination
        └── LUDecomposition (abstract)
              ├── Doolittle
              ├── Crout
              └── Cholesky
        ├── GaussJacobi
        └── GaussSeidel
```

A solver **IS-A** matrix — the coefficient matrix A is stored in `this->data`. The solver operates directly on its own inherited data.

**Hierarchy 2: Interpolation → Methods (IS-A relationship via abstraction)**
```
Interpolation (abstract, uses Matrix via composition)
  ├── Lagrange
  ├── LeastSquareLine
  └── LeastSquareParabola
```

**Hierarchy 3: EigenSolver → Analyzers (IS-A with composition)**
```
EigenSolver (abstract, holds const Matrix& reference)
  └── GershgorinAnalyzer
```

### 7. Abstract Base Classes and Pure Virtual Functions

A class with at least one **pure virtual function** (`= 0`) cannot be instantiated — it's abstract. Derived classes MUST override it.

```cpp
// In SystemOfLinearEquationSolver:
virtual SolverResult solve(double *b, int n, int maxIter, double tol) = 0;

// In Interpolation:
virtual double evaluate(double x) const = 0;

// In EigenSolver:
virtual void solve() = 0;
virtual void printAnalysis() const = 0;
```

**Why?** Forces every derived class to implement the core algorithm while sharing common infrastructure (data storage, I/O, validation).

### 8. Polymorphism

Virtual functions enable runtime dispatch — calling the right method based on the actual object type:

```cpp
// solveInterpolation takes any Interpolation& — works with Lagrange,
// LeastSquareLine, LeastSquareParabola, or any future method
void solveInterpolation(Interpolation &interp, const string &methodName) {
  interp.loadData(xMat, yMat);      // base class method
  interp.printFitInfo();             // virtual — dispatches to derived
  interp.interpolate(samples, ...);  // calls evaluate() — virtual dispatch
}

// In Menu.cpp — same workflow, different objects:
Lagrange lag;
solveInterpolation(lag, "Lagrange");

LeastSquareLine lsl;
solveInterpolation(lsl, "Least Squares Line");
```

The `solveInterpolation` function doesn't know or care which specific method it's using. It calls `evaluate()`, and polymorphism routes to the correct implementation.

### 9. Composition vs Inheritance

| Pattern | Relationship | Used By | Why |
|---------|-------------|---------|-----|
| **Inheritance** | IS-A | Solvers inherit Matrix | A solver IS a matrix — operates on `this->data` directly |
| **Composition** | HAS-A | Interpolation holds Matrix members | Interpolation HAS data points stored in Matrix objects |
| **Composition** | HAS-A (reference) | EigenSolver holds `const Matrix&` | Analyzer USES a matrix but doesn't own or modify it |

**Key design decision:** Interpolation uses composition because an interpolation is NOT a matrix — it doesn't have a determinant, inverse, or dimensions. It USES matrices to store (x,y) data and to solve normal equations.

### 10. const Correctness

Three levels of const used throughout:

```cpp
// 1. const member functions — promise not to modify the object
double getData(int i, int j) const;
bool isSquare() const;
double evaluate(double x) const;    // interpolation evaluation

// 2. const parameters — promise not to modify the argument
Matrix add(const Matrix &other) const;
void loadData(const Matrix &x, const Matrix &y);

// 3. const references — efficient pass-by-reference without copy risk
EigenSolver(const Matrix &m);     // stores a const reference
```

### 11. mutable Keyword

Used in least squares classes for lazy initialization:

```cpp
class LeastSquareLine : public Interpolation {
protected:
  mutable double coeffA, coeffB;   // cache — not part of logical state
  mutable bool fitted;
  void fit() const;                // modifies mutable members only
public:
  double evaluate(double x) const override;  // calls fit() on first use
};
```

`evaluate()` is `const` (semantically correct — evaluation doesn't change the object). But the first call needs to compute coefficients. `mutable` allows this cache to be filled from a `const` method — the standard C++ pattern for lazy initialization.

### 12. Encapsulation and Access Specifiers

```cpp
class Matrix {
protected:      // accessible by this class AND derived classes
  double **data;
  int rows, cols;

public:         // accessible by everyone
  int getRows() const;
  double getData(int i, int j) const;   // bounds-checked access
  void setData(int i, int j, double val);
};
```

`data` is `protected` so that solver classes (which inherit Matrix) can access it directly for performance. External code uses `getData()`/`setData()` which include bounds checking.

### 13. Static Methods and Members

```cpp
// In Matrix — static factory method (doesn't need an object to call)
static Matrix inputMatrix(string label);

// In SystemOfLinearEquationSolver — shared utility for all solvers
static bool makeDiagDominant(double **A, double *b, int n);
static bool checkDiagDominant(double **A, int n);
```

Static methods belong to the class, not to any specific object. `makeDiagDominant` is shared by both Jacobi and Seidel without duplication.

### 14. Structs vs Classes

```cpp
// SolverResult — plain data container, all fields public
struct SolverResult {
  double *x;
  int n, iterations;
  bool converged;
  double error;
  bool dominanceAchieved;
  double **L, **U;     // LU decomposition matrices
  int luSize;
  void freeLU();       // cleanup helper
};

// GershgorinDisc — simple data holder
struct GershgorinDisc {
  int row;
  double center, radius, low, high;
};
```

`struct` is used when the type is a simple data container with no invariants to protect. `class` is used when the type has complex state that needs encapsulation.

### 15. Exception Handling

Custom exception class with descriptive messages:

```cpp
class MatrixException {
  string message;
public:
  MatrixException(string msg) { message = msg; }
  void printError() { cout << "Error: " << message << endl; }
};

// Throwing:
throw MatrixException("matrix is singular... inverse doesn't exist");

// Catching (in Menu.cpp — catches ALL operations):
try {
  switch (choice) { /* ... */ }
} catch (MatrixException &e) {
  e.printError();    // prints error, returns to menu — never crashes
}
```

Every menu operation is wrapped in try-catch. The program **never crashes** — errors print a message and return to the menu.

### 16. Dynamic Memory Management

All memory uses raw `new`/`delete` (no smart pointers, no STL containers in core):

```cpp
// Allocation (in Matrix constructor):
data = new double*[rows];
for (int i = 0; i < rows; i++)
  data[i] = new double[cols];

// Deallocation (in destructor):
for (int i = 0; i < rows; i++)
  delete[] data[i];
delete[] data;

// Solver results — caller must free:
double *x = ge.solveWithPivoting(b, n);
// ... use x ...
delete[] x;
delete[] b;
```

### 17. File I/O

Two directions, with auto-format detection:

```cpp
// READING — auto-detects header vs no-header format:
void Matrix::readFromFile(string filename);
// Handles: "3 3\n1 2 3\n..." (with header) or "1 2 3\n4 5 6\n..." (no header)

// WRITING — gnuplot-compatible output:
void Matrix::saveToFile(string filename);

// File constructor — one-line matrix creation from file:
Matrix A("data.txt");
```

### 18. Header Guards

Every header file uses include guards to prevent double-inclusion:

```cpp
#ifndef MATRIX_HPP
#define MATRIX_HPP
// ... class declaration ...
#endif
```

### 19. Virtual Destructors

Base classes with virtual functions have virtual destructors:

```cpp
virtual ~Matrix();
virtual ~Interpolation() = default;
virtual ~EigenSolver();
virtual ~SystemOfLinearEquationSolver() {}
```

This ensures proper cleanup when deleting through a base pointer — the derived class destructor runs first, then the base.

### 20. Default Function Arguments

```cpp
virtual SolverResult solve(double *b, int n,
                           int maxIter = 10000,    // default if not specified
                           double tol = 1e-10) = 0;
```

Callers can write `solver.solve(b, n)` without specifying iteration limits.

---

## Class Hierarchy

### Complete Hierarchy Diagram

```
Matrix (base class)
│  Rule of 5, operator overloading, file I/O
│  double** data, int rows, int cols
│
├── SystemOfLinearEquationSolver (abstract, inherits Matrix)
│   │  pure virtual: solve(b, n) → SolverResult
│   │  static helpers: makeDiagDominant(), checkDiagDominant()
│   │
│   ├── GaussianElimination
│   │     solveWithPivoting(), solveWithoutPivoting()
│   │
│   ├── LUDecomposition (abstract)
│   │   ├── Doolittle    (L has unit diagonal)
│   │   ├── Crout        (U has unit diagonal)
│   │   └── Cholesky     (symmetric positive definite, A = LLᵀ)
│   │
│   ├── GaussJacobi      (iterative, uses old values)
│   └── GaussSeidel      (iterative, uses newest values)

Interpolation (abstract, uses Matrix via COMPOSITION)
│  Matrix xData (1×n), Matrix yData (1×n)
│  pure virtual: evaluate(x) → double
│  virtual: printFitInfo(), hasErrorAnalysis(), printErrorTable()
│
├── Lagrange              (exact polynomial interpolation)
├── LeastSquareLine       (y = a + bx, normal equations via Matrix)
└── LeastSquareParabola   (y = a + bx + cx², normal equations via Matrix)

EigenSolver (abstract, holds const Matrix& via COMPOSITION)
│  pure virtual: solve(), printAnalysis()
│
└── GershgorinAnalyzer    (Gershgorin Circle Theorem)
```

---

## Numerical Methods Implemented

### Matrix Operations (Menu 1–4, 12–25)

| Operation | Method | Menu |
|-----------|--------|------|
| Addition | Element-wise A[i][j] + B[i][j] | 1 |
| Subtraction | Element-wise A[i][j] - B[i][j] | 2 |
| Multiplication | Row × Column dot product | 3 |
| Determinant | Gaussian elimination to upper triangular | 4 |
| Transpose | Swap rows and columns | 12 |
| Scalar Multiply | Every element × scalar | 13 |
| Inverse | adj(A) / det(A) | 14 |
| Minor Matrix | Remove row r, col c | 15 |
| Cofactor | (-1)^(r+c) × det(minor) | 16 |
| Adjoint | Transpose of cofactor matrix | 17 |
| Property Checks | Square, symmetric, identity, null, diagonal, diag. dominant | 18–24 |
| Equality | Element-wise comparison with tolerance | 25 |

### Linear System Solvers (Menu 5–11)

| Method | Type | Key Feature |
|--------|------|-------------|
| Gaussian Elimination (pivoting) | Direct | Partial pivoting for stability |
| Gaussian Elimination (no pivoting) | Direct | For comparison — less stable |
| Doolittle LU | Direct | L has unit diagonal |
| Crout LU | Direct | U has unit diagonal |
| Cholesky LU | Direct | For symmetric positive definite (A = LLᵀ) |
| Gauss-Jacobi | Iterative | Uses old values only, auto diag. dominance fix |
| Gauss-Seidel | Iterative | Uses newest values, faster convergence |

### Eigenvalue Analysis (Menu 26)

| Method | What It Does |
|--------|-------------|
| Gershgorin Circle Theorem | Bounds eigenvalues using disc regions |

### Interpolation & Curve Fitting (Menu 27–29)

| Method | Equation | Fit Type | Min Points |
|--------|----------|----------|------------|
| Lagrange | Degree n-1 polynomial | Exact (through all points) | 2 |
| Least Squares Line | y = a + bx | Approximate (minimizes SSE) | 2 |
| Least Squares Parabola | y = a + bx + cx² | Approximate (minimizes SSE) | 3 |

---

## How to Build and Run

### Build

```bash
cd matrix_class
make clean && make cpu
```

### Run

```bash
./matrix_program
```

### Menu

```
=== Menu ===
1.  Add (A + B)              12. Transpose              23. Check Diag. Dominant
2.  Subtract (A - B)         13. Scalar Multiply         24. Make Diag. Dominant
3.  Multiply (A * B)         14. Inverse                 25. Check Equality
4.  Determinant              15. Minor Matrix            26. Gershgorin Analysis
5.  GE (with pivoting)       16. Cofactor                27. Lagrange Interpolation
6.  GE (without pivoting)    17. Adjoint                 28. Least Squares Line
7.  LU - Doolittle           18. Check Square            29. Least Squares Parabola
8.  LU - Crout               19. Check Symmetric         30. Exit
9.  LU - Cholesky            20. Check Identity
10. Gauss-Jacobi             21. Check Null
11. Gauss-Seidel             22. Check Diagonal
```

---

## Detailed README Index

All detailed documentation is in the `Readme/` directory:

| File | Contents |
|------|----------|
| `ALGORITHMS.md` | Step-by-step algorithm explanations for all solvers |
| `BUILD.md` | Detailed build instructions and compiler flags |
| `INTERPOLATION_README.md` | Interpolation module: Lagrange + composition design |
| `LEAST_SQUARES_README.md` | Least squares: algorithms, Matrix usage, error analysis |
| `MATRIX_CLASS_INTERNALS.md` | Deep dive into Matrix class internals |
| `MATRIX_CLASS_README.md` | Matrix class overview and capabilities |
| `MATRIX_FILES.md` | File format documentation |
| `OOP_AND_CPP_DEEP_DIVE.md` | OOP concepts and C++ design patterns explained |
| `UNDER_THE_HOOD.md` | How each algorithm works internally |
| `LIBRARY_DEEP_DIVE.md` | Library architecture deep dive |
| `LIBRARY_BUILD.md` | Building as a shared/static library |
| `LIBRARY_USAGE.md` | Using the library in other projects |
| `CUDA_README.md` | GPU acceleration documentation |
| `TEST_CASES_README.md` | Test case descriptions |
| `CHANGELOG_2026-03-27.md` | Major refactoring changelog |
| `README_FULL.md` | Extended README with all options explained |
