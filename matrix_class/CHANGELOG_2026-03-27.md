# 📝 CHANGELOG — March 27, 2026

## Architectural Refactoring: From "Working Project" → "Library-Grade Code"

This document explains **every change** we made, **why** we made it, and **what it means** — written so even a complete beginner can follow along.

---

## 🔵 Change 1: Rule of 5 (Move Semantics)

### What was there before?
We had **Rule of 3**: destructor, copy constructor, copy assignment operator.

### What did we add?
**Move constructor** and **move assignment operator**.

### What does that mean?

When you write:

```cpp
Matrix result = A * B;
```

**Before:** The result of `A * B` gets **deep copied** into `result` — every single element is copied one by one. For a 225×225 matrix, that's 50,625 copies!

**After:** The result just **steals the pointer** from the temporary. No copying. Same result, way faster.

### How does it work?

```cpp
// Move constructor — steal the guts, leave other empty
Matrix::Matrix(Matrix &&other) noexcept {
  rows = other.rows;
  cols = other.cols;
  data = other.data;       // STEAL the pointer

  other.data = nullptr;    // leave other empty (so its destructor doesn't delete OUR data)
  other.rows = 0;
  other.cols = 0;
}
```

The `&&` means "this is a temporary that's about to die anyway." Since it's dying, we can steal its data instead of copying it.

### Files changed
- `include/Matrix.hpp` — declared move constructor + move assignment
- `src/Matrix.cpp` — implemented them

---

## 🔵 Change 2: Const-Correctness

### What was there before?
Functions like `getRows()`, `isSquare()`, `determinant()` were **not** marked `const`.

### What did we change?
Added `const` to every function that **doesn't modify the matrix**.

```cpp
// Before
int getRows();
bool isSquare();

// After
int getRows() const;
bool isSquare() const;
```

### Why does this matter?

**1. Safety:** If you accidentally try to modify data inside a `const` function, the compiler catches it.

**2. Usability:** Without `const`, you can't call `getRows()` on a const Matrix:

```cpp
void printSize(const Matrix& m) {
  cout << m.getRows();  // ERROR without const!
}
```

**3. Professionalism:** Every real library (Eigen, Boost, STL) uses `const` everywhere. It's the #1 sign of professional C++ code.

### Functions marked const
`getRows`, `getCols`, `getData`, `isSquare`, `isSymmetric`, `isIdentity`, `isNull`, `isDiagonal`, `isDiagonallyDominant`, `determinant`, `transpose`, `minorMatrix`, `cofactor`, `adjoint`, `inverse`, `display`, `operator==`, `operator()` (const overload), `add`, `subtract`, `multiply`, `operator+`, `operator-`, `operator*`

### Files changed
- `include/Matrix.hpp`
- `src/Matrix.cpp`
- `src/MatrixOperations.cpp`

---

## 🔵 Change 3: Pass by const-reference (not by value)

### What was there before?
```cpp
Matrix add(Matrix other);        // copies the entire matrix!
Matrix multiply(Matrix other);   // copies again!
```

### What did we change?
```cpp
Matrix add(const Matrix &other) const;     // NO copy!
Matrix multiply(const Matrix &other) const; // NO copy!
```

### Why?

When you pass `Matrix other` (by value), C++ makes a **complete deep copy** of the matrix before the function even starts. For a 225×225 matrix, that's 50,625 unnecessary copies.

With `const Matrix &other`, we pass a **reference** (just a pointer), and `const` promises we won't modify it.

### Files changed
- `include/Matrix.hpp` — changed signatures
- `src/Matrix.cpp` — changed `add()`, `subtract()`, `multiply()`, `operator+`, `operator-`, `operator*`

---

## 🔵 Change 4: SolverResult Struct

### What was there before?
Solvers returned just `double*`:
```cpp
double* solve(double* b, int n);
```

### What did we change?
Solvers now return a `SolverResult`:
```cpp
struct SolverResult {
  double *x;       // solution vector
  int n;           // size
  int iterations;  // how many iterations (0 for direct methods)
  bool converged;  // did it converge?
  double error;    // max residual
};
```

### Why?

A raw `double*` tells you **nothing**:
- Did it converge? 🤷
- How many iterations? 🤷
- What was the error? 🤷

With `SolverResult`, you get **everything** in one clean package.

### Files changed
- `include/SolverResult.hpp` — **NEW** struct definition
- `include/SystemOfLinearEquationSolver.hpp` — changed `solve()` return type
- All solver headers and implementations

---

## 🔵 Change 5: No `cout` in Core Library

### What was there before?
Solvers printed messages directly:
```cpp
// Inside GaussSeidel::solve()
cout << "Gauss-Seidel converged in " << iter << " iterations" << endl;
```

### What did we change?
**Removed ALL `cout` from solvers.** Solvers now return data silently. The **UI layer** (`Display.cpp`, `Menu.cpp`) handles all printing.

### Why?

Imagine you want to:
- Use this library in a **GUI app** → `cout` goes nowhere
- Use it from **Python** → `cout` clutters the terminal
- Run **automated tests** → `cout` makes output unparseable

A library should be **silent**. It does the math, returns the result. The **caller** decides what to print.

### What prints now?
The `Display.cpp` layer prints:
```
matrix is NOT diagonally dominant... trying to fix it by swapping rows
Gauss-Seidel converged in 116 iterations
```

Same messages, but now they come from the **UI layer**, not the solver.

### Files changed
- `src/GaussJacobi.cpp` — removed all `cout`
- `src/GaussSeidel.cpp` — removed all `cout`
- `src/Doolittle.cpp` — removed LU verification `cout`
- `src/Crout.cpp` — removed LU verification `cout`
- `src/Cholesky.cpp` — removed Cholesky verification `cout`
- `utils/Display.cpp` — prints solver status messages

---

## 🔵 Change 6: Configurable maxIter and tol

### What was there before?
```cpp
int maxIter = 10000;  // hardcoded inside the solver
double tol = 1e-10;   // hardcoded inside the solver
```

### What did we change?
```cpp
SolverResult solve(double *b, int n, int maxIter = 10000, double tol = 1e-10);
```

### Why?

Different problems need different settings:
- Quick test? Use `maxIter = 100`
- High precision? Use `tol = 1e-15`
- Large system? Use `maxIter = 50000`

The defaults are the same as before, so **nothing breaks** — but now you **can** customize if needed.

### Files changed
- `include/SystemOfLinearEquationSolver.hpp`
- All solver headers and implementations

---

## 🔵 Change 7: Don't Mutate the Original Matrix

### What was there before?
```cpp
// Inside GaussSeidel::solve()
makeDiagDominant(data, b, n);  // this MODIFIES data!
```

### What did we change?
```cpp
// COPY matrix data and b so we don't mutate the original
double **A = new double *[n];
for (int i = 0; i < n; i++) { ... copy data[i] into A[i] ... }
makeDiagDominant(A, rhs, n);  // modify the COPY
```

### Why?

If you solve twice:
```cpp
solver.solve(b1);  // modifies data to be diagonally dominant
solver.solve(b2);  // uses the MODIFIED data! Wrong!
```

By working on copies, the original matrix is **always preserved**.

### Files changed
- `src/GaussJacobi.cpp`
- `src/GaussSeidel.cpp`

---

## 🔵 Change 8: Menu Separation (App Layer)

### What was there before?
`main.cpp` was **359 lines** with all menu display, switch/case, and handler logic mixed in.

### What did we change?
Created a separate **app layer**:

```
app/Menu.hpp  — declares runMenu()
app/Menu.cpp  — ALL menu + handler logic (284 lines)
```

And `main.cpp` became:
```cpp
#include "app/Menu.hpp"

int main() {
  runMenu();
  return 0;
}
```

**6 lines. That's it.**

### Why?

This separates your project into 3 clean layers:

| Layer | What it does | What it does NOT do |
|-------|-------------|-------------------|
| **Core** (Matrix, Solvers) | Math and logic | No user interaction |
| **Utils** (Input, Display) | Read/write I/O | No menus or decisions |
| **App** (Menu) | Menu, routing, decisions | No math |

### Benefits
- **Reusability:** The core library can be used in GUI apps, Python bindings, web servers, etc.
- **Testability:** You can unit test solvers without dealing with menu prompts.
- **Maintainability:** Adding a new solver just needs one new handler function.

### Files changed
- `app/Menu.hpp` — **NEW**
- `app/Menu.cpp` — **NEW** (all menu logic moved here)
- `main.cpp` — reduced from 359 lines to 6 lines

---

## 🔵 Change 9: Unified solveIterative (Polymorphism)

### What was there before?
Two separate overloaded functions:
```cpp
void solveIterative(GaussJacobi &solver);
void solveIterative(GaussSeidel &solver);
```
Both had **identical** code.

### What did we change?
One polymorphic function:
```cpp
void solveIterative(SystemOfLinearEquationSolver &solver, const string &methodName);
```

### Why?

Both functions did the exact same thing. The only difference was the type name. By using the **base class reference**, one function works for **any** solver — Jacobi, Seidel, or any future iterative solver you add.

This is **polymorphism** — one of the core ideas of object-oriented programming.

### Files changed
- `utils/Display.hpp` — single function declaration
- `utils/Display.cpp` — single implementation

---

## 🔵 Change 10: Makefile Update

### What changed?
Added `app/Menu.cpp` to the build sources.

### Files changed
- `Makefile`

---

## ✅ Verification Results

| Test | Result |
|------|--------|
| Build (zero warnings) | ✅ |
| GE 49×49 | ✅ Correct |
| Gauss-Seidel 49×49 | ✅ Converged 116 iter, matches GE |
| GE 225×225 | ✅ Correct |
| Gauss-Seidel 225×225 | ✅ Converged 429 iter, matches GE |
| All menu options | ✅ Working |

---

## 📁 New Project Structure

```
matrix_class/
├── main.cpp                         ← 6 lines (just calls runMenu)
├── Makefile
│
├── include/                         ← CORE LIBRARY (no cin/cout)
│   ├── Matrix.hpp
│   ├── SolverResult.hpp             ← NEW
│   ├── SystemOfLinearEquationSolver.hpp
│   ├── GaussianElimination.hpp
│   ├── LUDecomposition.hpp
│   ├── GaussJacobi.hpp
│   └── GaussSeidel.hpp
│
├── src/                             ← CORE IMPLEMENTATIONS (no cin/cout)
│   ├── Matrix.cpp
│   ├── MatrixOperations.cpp
│   ├── GaussianElimination.cpp
│   ├── GaussJacobi.cpp
│   ├── GaussSeidel.cpp
│   ├── Doolittle.cpp
│   ├── Crout.cpp
│   └── Cholesky.cpp
│
├── utils/                           ← I/O HELPERS
│   ├── Input.hpp / Input.cpp
│   └── Display.hpp / Display.cpp
│
└── app/                             ← APPLICATION LAYER (menu + routing)
    ├── Menu.hpp                     ← NEW
    └── Menu.cpp                     ← NEW
```
