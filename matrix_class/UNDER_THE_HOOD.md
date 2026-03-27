# Under the Hood — How the Library Actually Works

This document goes **4 levels deep** into how this library delivers functionality to the end user. From the moment you write `#include "Matrix.hpp"` to the moment your program prints a result — every step explained.

---

## Table of Contents

1. [The Big Picture](#1-the-big-picture)
2. [Phase 1: Preprocessor — What #include Actually Does](#2-phase-1-preprocessor--what-include-actually-does)
3. [Phase 2: Compilation — Turning .cpp into .o](#3-phase-2-compilation--turning-cpp-into-o)
4. [Phase 3: Linking — Stitching Object Files Together](#4-phase-3-linking--stitching-object-files-together)
5. [Phase 4: Loading — OS Puts Your Program in Memory](#5-phase-4-loading--os-puts-your-program-in-memory)
6. [How One #include Gives You Everything](#6-how-one-include-gives-you-everything)
7. [The Symbol Table — How the Linker Finds Functions](#7-the-symbol-table--how-the-linker-finds-functions)
8. [Function Call Mechanics — What Happens at Runtime](#8-function-call-mechanics--what-happens-at-runtime)
9. [Object File Anatomy](#9-object-file-anatomy)
10. [The Makefile — Orchestrating Everything](#10-the-makefile--orchestrating-everything)
11. [Memory Layout of a Matrix Object](#11-memory-layout-of-a-matrix-object)
12. [Inheritance — How SLE Solvers Access Matrix Data](#12-inheritance--how-sle-solvers-access-matrix-data)
13. [Virtual Functions — How solve() Dispatches](#13-virtual-functions--how-solve-dispatches)
14. [Complete Execution Trace — Start to Finish](#14-complete-execution-trace--start-to-finish)

---

## 1. The Big Picture

When you compile and run everything, there are **four phases**:

```
Source Files (.cpp, .hpp)
    │
    ▼
┌──────────────┐
│ PREPROCESSOR │  Handles #include, #define, #ifdef
└──────┬───────┘
       │ Expanded source (one big text file per .cpp)
       ▼
┌──────────────┐
│   COMPILER   │  Turns C++ into machine code
└──────┬───────┘
       │ Object files (.o) — machine code + unresolved symbols
       ▼
┌──────────────┐
│    LINKER    │  Connects all object files, resolves symbols
└──────┬───────┘
       │ Executable (matrix_program) — complete, ready to run
       ▼
┌──────────────┐
│    LOADER    │  OS loads executable into RAM, starts it
└──────┬───────┘
       │
       ▼
    Running program!
```

---

## 2. Phase 1: Preprocessor — What #include Actually Does

When you write:
```cpp
#include "include/Matrix.hpp"
```

The preprocessor **literally copy-pastes the entire contents of `Matrix.hpp`** into your file, replacing the `#include` line.

### What actually happens to `main.cpp`:

Before preprocessing:
```cpp
// main.cpp
#include "include/Matrix.hpp"
#include <iostream>

int main() {
    Matrix A(3, 3);
    // ...
}
```

After preprocessing (what the compiler actually sees):
```cpp
// === pasted from include/MatrixException.hpp ===
#include <string>      // ← this also gets expanded!
class MatrixException {
    std::string message;
public:
    MatrixException(std::string msg);
    void printError();
};

```cpp
// === pasted from include/Matrix.hpp ===
class Matrix {
protected:
    double **data;
    int rows, cols;
public:
    Matrix();
    Matrix(int r, int c);
    Matrix(const Matrix &other);          // copy constructor
    Matrix(Matrix &&other) noexcept;      // move constructor (Rule of 5)
    Matrix add(const Matrix &other) const;
    Matrix subtract(const Matrix &other) const;
    Matrix multiply(const Matrix &other) const;
    Matrix operator+(const Matrix &other) const;
    // ... all 30+ method declarations
    void display() const;
};

// === pasted from <iostream> (thousands of lines from the standard library) ===
// ... namespace std { class ostream { ... }; extern ostream cout; ... }

// === your actual code ===
int main() {
    Matrix A(3, 3);
    // ...
}
```

**Key insight:** The compiler NEVER sees your original file. It sees this **massive expanded version**. For a typical file including `<iostream>`, the expanded version is **20,000+ lines** — most of which are standard library headers.

### How #include guards work

`Matrix.hpp` starts with:
```cpp
#ifndef MATRIX_HPP
#define MATRIX_HPP
// ... class declaration ...
#endif
```

If two files both `#include "Matrix.hpp"`, the preprocessor would try to paste it twice. But the second time, `MATRIX_HPP` is already defined, so `#ifndef MATRIX_HPP` is false, and the entire file is skipped. This prevents **duplicate class definitions**.

### How the chain works

```
main.cpp
 │ #include "include/GaussianElimination.hpp"
 │
 └──→ GaussianElimination.hpp
       │ #include "SystemOfLinearEquationSolver.hpp"
       │
       └──→ SystemOfLinearEquationSolver.hpp
             │ #include "Matrix.hpp"
             │
             └──→ Matrix.hpp
                   │ #include "MatrixException.hpp"
                   │ #include <cmath>
                   │ #include <fstream>
                   │ #include <iostream>
                   └──→ (standard library headers)
```

So just `#include "GaussianElimination.hpp"` automatically pulls in `SystemOfLinearEquationSolver.hpp`, which pulls in `Matrix.hpp`, which pulls in `MatrixException.hpp` and all the standard library headers. **One include → everything.**

---

## 3. Phase 2: Compilation — Turning .cpp into .o

The compiler takes each `.cpp` file independently and produces an **object file** (`.o`).

```
g++ -c main.cpp -o main.o
g++ -c src/Matrix.cpp -o src/Matrix.o
g++ -c src/Doolittle.cpp -o src/Doolittle.o
// ... one command per .cpp file
```

### What the compiler does:

1. **Lexing:** Breaks source into tokens: `int`, `main`, `(`, `)`, `{`, etc.
2. **Parsing:** Builds an Abstract Syntax Tree (AST) — the program's logical structure
3. **Semantic Analysis:** Type checking, overload resolution, template instantiation
4. **Optimization:** Dead code elimination, inlining, loop unrolling (with `-O2`)
5. **Code Generation:** Produces machine instructions (x86_64 assembly)
6. **Assembly:** Converts assembly to binary machine code → `.o` file

### What's in an object file?

An `.o` file contains:
- **Machine code** (the actual CPU instructions)
- **Symbol table** (list of functions it defines and functions it needs)
- **Relocation entries** (addresses that need to be filled in by the linker)

**Crucial point:** Each `.cpp` is compiled **independently**. When `main.cpp` calls `A.add(B)`, the compiler doesn't know where `Matrix::add()` is implemented. It just writes a **placeholder** address and adds an entry to the symbol table saying "I need `Matrix::add()` from somewhere."

### Example: what main.o contains

```
main.o:
  DEFINED symbols (functions I provide):
    main                    at offset 0x0000

  UNDEFINED symbols (functions I need from other .o files):
    Matrix::Matrix(int, int)         ← from Matrix.o
    Matrix::operator+(Matrix const&) ← from Matrix.o
    Matrix::display()                ← from Matrix.o
    Matrix::~Matrix()                ← from Matrix.o
    getMatrixInput(Matrix&)          ← from Input.o
    std::cout                        ← from libstdc++
    std::endl                        ← from libstdc++

  Machine code:
    0x0000: push rbp          ← function prologue
    0x0001: mov rbp, rsp
    ...
    0x002A: call ????????      ← Matrix::Matrix(3,3) — address unknown!
    ...
    0x0055: call ????????      ← Matrix::operator+() — address unknown!
    ...
```

Those `????????` are **relocation entries**. The linker will fill them in.

---

## 4. Phase 3: Linking — Stitching Object Files Together

The linker takes ALL `.o` files and connects them into one executable.

```
g++ -o matrix_program main.o src/Matrix.o src/MatrixException.o \
    src/GaussianElimination.o src/LUDecomposition.o \
    src/Doolittle.o src/Crout.o src/Cholesky.o \
    src/GaussJacobi.o src/MatrixOperations.o \
    utils/Input.o utils/Display.o
```

### What the linker does:

**Step 1: Collect all symbols**
```
From main.o:           DEFINES: main
                       NEEDS:   Matrix::Matrix(int,int), Matrix::add(), ...

From src/Matrix.o:     DEFINES: Matrix::Matrix(), Matrix::Matrix(int,int),
                                Matrix::add(), Matrix::subtract(),
                                Matrix::multiply(), Matrix::determinant(),
                                Matrix::operator+(), Matrix::operator-(),
                                Matrix::operator*(), Matrix::isSymmetric(),
                                Matrix::display(), Matrix::~Matrix(), ...
                       NEEDS:   MatrixException::MatrixException(string), ...

From src/Doolittle.o:  DEFINES: Doolittle::solve()
                       NEEDS:   MatrixException::MatrixException(string), ...

From src/MatrixOperations.o: DEFINES: Matrix::transpose(), Matrix::inverse(),
                                      Matrix::cofactor(), Matrix::adjoint(),
                                      Matrix::minorMatrix(), Matrix::operator*(double)
                             NEEDS:   Matrix::determinant(), ...
```

**Step 2: Resolution** — Match every UNDEFINED symbol with a DEFINED one:
```
main.o NEEDS Matrix::Matrix(int,int)
  → FOUND in src/Matrix.o ✅

main.o NEEDS Matrix::operator+(Matrix const&)
  → FOUND in src/Matrix.o ✅

src/Matrix.o NEEDS MatrixException::MatrixException(string)
  → FOUND in src/MatrixException.o ✅

main.o NEEDS getMatrixInput(Matrix&)
  → FOUND in utils/Input.o ✅
```

If any symbol is not found → **linker error**:
```
undefined reference to `Matrix::transpose()'
```
This means you forgot to add `MatrixOperations.o` to the link command (or forgot to implement the function).

**Step 3: Relocation** — Fill in all the `????????` addresses:
```
main.o, offset 0x002A:  call ????????
  → Matrix::Matrix(int,int) is at address 0x00401200 in the final executable
  → Patch: call 0x00401200
```

**Step 4: Output** — Write the final executable with:
- All machine code sections merged
- All addresses resolved
- Program entry point set to `main()`
- Header describing memory layout for the OS loader

---

## 5. Phase 4: Loading — OS Puts Your Program in Memory

When you type `./matrix_program`, the **OS loader** does:

**Step 1: Read the executable header**
```
ELF Header (Linux) or Mach-O Header (macOS):
  Type: Executable
  Entry point: 0x00401000 (where main() starts)
  Segments:
    .text  at 0x00401000, size 0x20000  (machine code)
    .data  at 0x00601000, size 0x1000   (global variables)
    .bss   at 0x00602000, size 0x500    (uninitialized globals)
```

**Step 2: Allocate virtual memory**

The OS creates a **virtual address space** for your process:
```
┌──────────────────────────┐  High addresses
│         Stack            │  ← local variables, function call frames
│           ↓              │     grows downward
│                          │
│     (unmapped gap)       │
│                          │
│           ↑              │
│         Heap             │  ← new/malloc allocations (your double** data)
│                          │     grows upward
├──────────────────────────┤
│    .bss (zeroed data)    │  ← uninitialized globals
├──────────────────────────┤
│    .data (init data)     │  ← initialized globals (cout, cerr)
├──────────────────────────┤
│    .text (code)          │  ← all your compiled functions
│    main()      at 0x1000 │
│    Matrix::add at 0x1200 │
│    Doolittle:: at 0x3400 │
└──────────────────────────┘  Low addresses
```

**Step 3: Load dynamic libraries**

`libstdc++.so` (C++ standard library) is a **shared library** — it's not in your executable. The loader maps it into your address space:
```
Your executable:  call _ZSt4cout    ← mangled name for std::cout
Dynamic linker:   _ZSt4cout is at 0x7fff12340000 in libstdc++.so
                  → patches your call target
```

**Step 4: Jump to entry point**

The OS sets the instruction pointer to your `main()` function, and your program starts running.

---

## 6. How One #include Gives You Everything

The end user sees:
```cpp
#include "include/Matrix.hpp"

int main() {
    Matrix A(3, 3);            // works!
    GaussianElimination ge;    // works! (wait... how?)
    Doolittle d;               // works! (how??)
}
```

**But `Matrix.hpp` doesn't include `GaussianElimination.hpp`!** So how does this work?

**Answer: It doesn't.** To use `GaussianElimination`, the user MUST include it:
```cpp
#include "include/Matrix.hpp"                    // for Matrix
#include "include/GaussianElimination.hpp"       // for GaussianElimination
#include "include/LUDecomposition.hpp"           // for Doolittle, Crout, Cholesky
#include "include/GaussJacobi.hpp"               // for GaussJacobi
```

### Why we don't make Matrix.hpp include everything

If `Matrix.hpp` included all solver headers, then **every .cpp file that includes Matrix.hpp would have to recompile whenever any solver changes**. By keeping includes minimal, changing `Doolittle.cpp` only recompiles `Doolittle.o` — not everything.

### The include dependency graph

```
MatrixException.hpp          ← no dependencies
       ↑
Matrix.hpp                   ← includes MatrixException.hpp
       ↑
SolverResult.hpp             ← standalone struct
       ↑
SystemOfLinearEquationSolver.hpp  ← includes Matrix.hpp, SolverResult.hpp
       ↑
       ├── GaussianElimination.hpp
       ├── LUDecomposition.hpp
       │       ↑
       │       ├── Doolittle   (declared inside LUDecomposition.hpp)
       │       ├── Crout       (declared inside LUDecomposition.hpp)
       │       └── Cholesky    (declared inside LUDecomposition.hpp)
       ├── GaussJacobi.hpp
       └── GaussSeidel.hpp
```

**Including `LUDecomposition.hpp` automatically gives you:**
- `Matrix` class (via the chain)
- `MatrixException` class
- `SystemOfLinearEquationSolver` base class
- `LUDecomposition` class
- `Doolittle`, `Crout`, `Cholesky` classes

That's 6 classes from one `#include`.

### What the user's minimal program looks like

```cpp
#include "include/LUDecomposition.hpp"
#include "utils/Input.hpp"
#include "utils/Display.hpp"

int main() {
    Doolittle solver;
    // ... use solver
}
```

The compiler only needs to see the class **declarations** (from the `.hpp` files). The actual **implementations** (from the `.cpp` files) are in the `.o` files that get linked in.

---

## 7. The Symbol Table — How the Linker Finds Functions

Every `.o` file has a **symbol table** — a list of names (mangled) that this file either **defines** or **needs**.

### C++ Name Mangling

The compiler transforms human-readable names into unique mangled names:

```
Matrix::Matrix(int, int)     →  _ZN6MatrixC1Eii
Matrix::add(Matrix)          →  _ZN6Matrix3addES_
Matrix::operator+(const Matrix&)  →  _ZN6MatrixplERKS_
Matrix::determinant()        →  _ZN6Matrix11determinantEv
Doolittle::solve(double*, int)    →  _ZN9Doolittle5solveEPdi
```

**Why?** C++ allows overloading (same name, different parameters). Mangling encodes the parameter types into the name, so the linker can distinguish `Matrix::operator*(const Matrix&)` from `Matrix::operator*(double)`.

### Viewing the symbol table

You can see what's in an object file:
```bash
nm src/Matrix.o | head -20
```

Output:
```
0000000000000000 T _ZN6MatrixC1Ev             ← Matrix() constructor (DEFINED, Text section)
0000000000000080 T _ZN6MatrixC1Eii            ← Matrix(int,int) constructor
0000000000000200 T _ZN6Matrix3addES_          ← Matrix::add()
                 U _ZN15MatrixExceptionC1ENSt..  ← MatrixException() (UNDEFINED — needs)
                 U _Znwm                       ← operator new (from standard library)
```

- `T` = defined in this file (Text/code section)
- `U` = undefined (needed from another file)
- `D` = data section (global variables)

### How the linker resolves

```
Phase 1: Read all .o files, collect all symbols
  src/Matrix.o        DEFINES: _ZN6MatrixC1Ev, _ZN6MatrixC1Eii, _ZN6Matrix3addES_, ...
  src/Doolittle.o     DEFINES: _ZN9Doolittle5solveEPdi
  src/MatrixOps.o     DEFINES: _ZN6Matrix9transposeEv, _ZN6Matrix7inverseEv, ...
  main.o              DEFINES: main
  
Phase 2: For each UNDEFINED symbol, search all DEFINED symbols
  main.o NEEDS _ZN6MatrixC1Eii  → found in src/Matrix.o ✅
  main.o NEEDS _ZN6Matrix3addES_ → found in src/Matrix.o ✅
  src/Matrix.o NEEDS _ZN15MatrixExceptionC1E... → found in src/MatrixException.o ✅
  
Phase 3: Any UNDEFINED left? → linker error
```

---

## 8. Function Call Mechanics — What Happens at Runtime

When your code calls `A.add(B)`, here's what the CPU actually does:

### Step 1: Prepare the call

```assembly
; A is at address [rbp-48], B is at address [rbp-96]

lea rdi, [rbp-48]      ; rdi = pointer to A (the "this" pointer)
lea rsi, [rbp-96]      ; rsi = pointer to B (the argument)
call _ZN6Matrix3addES_  ; jump to Matrix::add()
```

In C++, `A.add(B)` is actually `Matrix::add(&A, B)`. The object `A` is passed as a hidden **this** pointer in register `rdi`.

### Step 2: Function prologue

```assembly
_ZN6Matrix3addES_:        ; Matrix::add() starts here
    push rbp               ; save caller's base pointer
    mov rbp, rsp           ; set up our own stack frame
    sub rsp, 64            ; allocate space for local variables
```

### Step 3: Execute the function

The function body runs — reads `A.data[i][j]` and `B.data[i][j]`, adds them, stores in `result.data[i][j]`.

### Step 4: Return

```assembly
    mov rax, [rbp-8]       ; return value (pointer to result Matrix)
    leave                   ; restore rbp
    ret                     ; jump back to caller
```

### The call stack during a solve

When you call `Doolittle::solve()`, the stack looks like:

```
┌─────────────────────────────────┐
│ main()                          │  your menu loop
│   choice = 7                    │
│   Doolittle d                   │  solver object on stack
├─────────────────────────────────┤
│ solveLU(d)                      │  utils/Display.cpp
│   calls d.solve(b, n)           │
├─────────────────────────────────┤
│ Doolittle::solve(b, n)          │  src/Doolittle.cpp
│   L[n][n], U[n][n]              │  allocated on heap (via new)
│   calls forward/back sub        │
├─────────────────────────────────┤
│ back substitution loop          │  inside solve()
│   x[n-1] = y[n-1] / U[n-1][n-1]│
└─────────────────────────────────┘  ← stack pointer (top of stack)
```

Each function call **pushes** a new frame. When the function returns, it **pops** its frame. The stack grows downward in memory.

---

## 9. Object File Anatomy

Let's look inside `src/Matrix.o` in detail:

```
ELF Object File Layout:
┌────────────────────────────┐
│  ELF Header                │  magic bytes, architecture (x86_64), endianness
├────────────────────────────┤
│  .text section             │  machine code for all functions
│    Matrix::Matrix()    0x0 │
│    Matrix::Matrix(r,c) 0x80│
│    Matrix::~Matrix()   0x120│
│    Matrix::add()       0x200│
│    Matrix::subtract()  0x400│
│    Matrix::multiply()  0x600│
│    Matrix::display()   0xA00│
│    Matrix::determinant()    │
│    Matrix::operator+()      │
│    Matrix::isSymmetric()    │
├────────────────────────────┤
│  .rodata section           │  read-only data (string literals)
│    "can't add these..."    │  error messages
│    "%.6f "                 │  format strings
├────────────────────────────┤
│  .symtab section           │  symbol table
│    _ZN6MatrixC1Ev   T 0x0  │  Matrix() defined at offset 0
│    _ZN6MatrixC1Eii  T 0x80 │  Matrix(int,int) defined at 0x80
│    _Znwm            U ---- │  operator new, undefined
│    _ZdlPv           U ---- │  operator delete, undefined
├────────────────────────────┤
│  .rela.text section        │  relocation entries
│    offset 0x45: needs _Znwm│  (memory allocation call)
│    offset 0x88: needs _ZN15│  (MatrixException constructor)
├────────────────────────────┤
│  Section Header Table      │  describes all sections
└────────────────────────────┘
```

### How functions span multiple .o files

```
Matrix::add()         → in src/Matrix.o
Matrix::transpose()   → in src/MatrixOperations.o
Matrix::solve()       → DOES NOT EXIST (Matrix has no solve)
Doolittle::solve()    → in src/Doolittle.o
```

The class `Matrix` has its methods split across **two .o files** (`Matrix.o` and `MatrixOperations.o`). This is perfectly legal in C++ — the compiler only needs the class **declaration** (from the `.hpp`), and the **definitions** can be in any `.cpp` file. The linker glues them together.

---

## 10. The Makefile — Orchestrating Everything

The Makefile automates the build process. Let's trace what `make cpu` does:

```makefile
CXX = g++
CXXFLAGS = -std=c++11 -Wall

SRCS = main.cpp src/Matrix.cpp src/MatrixException.cpp \
       src/SystemOfLinearEquationSolver.cpp \
       src/GaussianElimination.cpp src/LUDecomposition.cpp \
       src/Doolittle.cpp src/Crout.cpp src/Cholesky.cpp \
       src/GaussJacobi.cpp src/GaussSeidel.cpp \
       src/MatrixOperations.cpp \
       utils/Input.cpp utils/Display.cpp \
       app/Menu.cpp

OBJS = $(SRCS:.cpp=.o)    # Replace .cpp with .o in all paths
TARGET = matrix_program

cpu: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
```

### What `make cpu` does step by step:

```
1. Goal: build "matrix_program"
2. Needs: main.o, src/Matrix.o, src/MatrixException.o, ... (13 .o files)
3. For each .o, check if its .cpp is newer:

   main.cpp newer than main.o? YES → compile
      g++ -std=c++11 -Wall -c main.cpp -o main.o

   src/Matrix.cpp newer than src/Matrix.o? YES → compile
      g++ -std=c++11 -Wall -c src/Matrix.cpp -o src/Matrix.o

   ... (repeat for all 13 files)

4. All .o files ready → link:
      g++ -std=c++11 -Wall -o matrix_program main.o src/Matrix.o \
          src/MatrixException.o ... utils/Display.o
```

### Incremental builds

If you only change `Doolittle.cpp`:
```
make cpu
  main.cpp newer than main.o? NO → skip
  src/Matrix.cpp newer? NO → skip
  src/Doolittle.cpp newer than src/Doolittle.o? YES → recompile ONLY this one
  ... (skip rest)
  Link step: always runs (because at least one .o changed)
```

Only 1 file recompiled + 1 link step, instead of 13 compiles. This is why we split into many files!

---

## 11. Memory Layout of a Matrix Object

When you create `Matrix A(3, 3)`, here's what's in memory:

```
Stack:
┌──────────────────────┐
│   A.rows = 3         │  4 bytes (int)
│   A.cols = 3         │  4 bytes (int)
│   A.data = 0x7F8000  │  8 bytes (pointer to heap)
└──────────────────────┘

Heap (dynamically allocated):
0x7F8000: ┌─────────────────────┐
          │ data[0] = 0x7F8100  │  pointer to row 0
          │ data[1] = 0x7F8118  │  pointer to row 1
          │ data[2] = 0x7F8130  │  pointer to row 2
          └─────────────────────┘

0x7F8100: ┌──────────────────────────────┐
          │ data[0][0]  data[0][1]  data[0][2] │  3 doubles = 24 bytes
          └──────────────────────────────┘

0x7F8118: ┌──────────────────────────────┐
          │ data[1][0]  data[1][1]  data[1][2] │  3 doubles = 24 bytes
          └──────────────────────────────┘

0x7F8130: ┌──────────────────────────────┐
          │ data[2][0]  data[2][1]  data[2][2] │  3 doubles = 24 bytes
          └──────────────────────────────┘
```

Total memory: 12 bytes (stack) + 24 bytes (row pointers) + 72 bytes (data) = **108 bytes** for a 3×3 matrix.

For a 225×225 matrix: 12 + 1800 + 405,000 = **~407 KB**.

### Constructor code

```cpp
Matrix::Matrix(int r, int c) {
    rows = r;
    cols = c;
    data = new double*[rows];        // allocate array of row pointers
    for (int i = 0; i < rows; i++) {
        data[i] = new double[cols];  // allocate each row
        for (int j = 0; j < cols; j++)
            data[i][j] = 0.0;       // zero out
    }
}
```

### Destructor — freeing in reverse

```cpp
Matrix::~Matrix() {
    if (data != NULL) {
        for (int i = 0; i < rows; i++)
            delete[] data[i];     // free each row first
        delete[] data;            // then free the pointer array
    }
}
```

**Order matters!** If you `delete[] data` first, you lose the pointers to the rows → memory leak.

---

## 12. Inheritance — How SLE Solvers Access Matrix Data

```cpp
class Matrix {
protected:           // ← accessible by derived classes
    double **data;
    int rows, cols;
};

class SystemOfLinearEquationSolver : public Matrix {
    // inherits data, rows, cols
    virtual SolverResult solve(double *b, int n,
                               int maxIter = 10000,
                               double tol = 1e-10) = 0;
};

class Doolittle : public LUDecomposition {
    // inherits data, rows, cols (from Matrix, through SLE, through LU)
    SolverResult solve(double *b, int n,
                       int maxIter = 10000,
                       double tol = 1e-10) override;
};
```

### Memory layout of a Doolittle object

```
┌──────────────────────────┐
│ vptr (virtual table ptr) │  8 bytes — points to Doolittle's vtable
│ rows = 49                │  4 bytes (from Matrix)
│ cols = 49                │  4 bytes (from Matrix)
│ data = 0x7F8000          │  8 bytes (from Matrix)
└──────────────────────────┘
```

When `Doolittle::solve()` accesses `data[i][j]`, it's accessing **the same `data` member** that Matrix defines. Inheritance means the Doolittle object **IS** a Matrix — it has all the same fields.

### The inheritance chain

```
Doolittle d;

d.readFromFile("49l.txt");   // calls Matrix::readFromFile()
d.display();                 // calls Matrix::display()
d.solve(b, 49);              // calls Doolittle::solve()
d.determinant();             // calls Matrix::determinant()
```

All of these work on the **same `data[][]`** inside the `d` object.

---

## 13. Virtual Functions — How solve() Dispatches

When you have a `SystemOfLinearEquationSolver*` pointer, how does C++ know which `solve()` to call?

```cpp
SystemOfLinearEquationSolver *solver;

solver = new Doolittle();
solver->solve(b, n);         // calls Doolittle::solve() — how?

solver = new Crout();
solver->solve(b, n);         // calls Crout::solve() — different function!
```

### The vtable (virtual function table)

Each class with virtual functions has a **vtable** — an array of function pointers:

```
Doolittle's vtable:
┌──────────────────────────────────┐
│ [0] → Doolittle::solve()         │  address: 0x00403400
│ [1] → Doolittle::~Doolittle()    │  address: 0x00403600
└──────────────────────────────────┘

Crout's vtable:
┌──────────────────────────────────┐
│ [0] → Crout::solve()             │  address: 0x00404200
│ [1] → Crout::~Crout()            │  address: 0x00404400
└──────────────────────────────────┘
```

Every object has a hidden **vptr** (virtual pointer) that points to its class's vtable.

### Runtime dispatch

```
solver->solve(b, n);

Assembly:
    mov rax, [solver]          ; load vptr from the object
    mov rax, [rax]             ; load first entry from vtable (= solve address)
    call rax                   ; call whatever function is there
```

If `solver` points to a Doolittle → vptr → Doolittle's vtable → `Doolittle::solve()`
If `solver` points to a Crout → vptr → Crout's vtable → `Crout::solve()`

**This is determined at runtime**, not compile time. That's why it's called **dynamic dispatch**.

---

## 14. Complete Execution Trace — Start to Finish

Let's trace exactly what happens when the user runs Doolittle LU on a 49×49 system:

```
$ ./matrix_program

1. OS LOADER:
   - Read Mach-O header from "matrix_program"
   - Map .text segment to virtual memory (all functions)
   - Map .data segment (global variables)
   - Load libstdc++.dylib (C++ standard library)
   - Load libSystem.dylib (macOS system calls)
   - Set instruction pointer to main()

2. PROGRAM STARTS:
   main() → prints menu
   User enters: 7 (LU - Doolittle)

3. OBJECT CREATION:
   Doolittle d;
   - Stack: allocate 24 bytes for d (vptr + rows + cols + data)
   - d.vptr = &Doolittle_vtable
   - d.rows = 0, d.cols = 0, d.data = NULL
   - Constructor calls: Doolittle() → LUDecomposition() → SLE() → Matrix()
     (chain of constructors, base class first)

4. CALL solveLU(d):
   utils/Display.cpp → calls getSystemInput(d, b, n)

5. FILE READING:
   utils/Input.cpp → getSystemInput():
   - User chooses option 3 (separate files)
   - Load "49/49l.txt":
     - Open file, read "49 50" header
     - d.rows = 49, d.cols = 49  (strip last col because it's augmented format)
     - d.data = new double*[49]
     - For each row: d.data[i] = new double[49], read 49 values
     - Total heap allocations: 1 (pointer array) + 49 (rows) = 50 new[] calls
     - Total data: 49 × 49 × 8 bytes = 19,208 bytes on heap
   - Load "49/49r.txt":
     - b = new double[49], read 49 values

6. VIRTUAL DISPATCH:
   d.solve(b, 49)
   - compiler generates: call [d.vptr[0]]
   - d.vptr → Doolittle_vtable → Doolittle::solve at address 0x00403400
   - jumps to Doolittle::solve()

7. DOOLITTLE::SOLVE() RUNS:
   src/Doolittle.cpp:
   a) Allocate L and U:
      - L = new double*[49], each row = new double[49]  → 50 allocations
      - U = new double*[49], each row = new double[49]  → 50 allocations
      - Initialize: L diagonal = 1.0, rest = 0.0
      - Initialize: U = all zeros
      - Total new memory: ~38 KB

   b) Decomposition (O(n³) = 117,649 operations):
      for k = 0 to 48:
          compute U row k (access data[k][j], L[k][s], U[s][j])
          compute L col k (access data[i][k], L[i][s], U[s][k])

   c) LU VERIFICATION:
      Compute L*U, compare with data[][]
      max error = 2.84e-14
      prints "Doolittle LU verification PASSED ✅"

   d) Forward substitution (Ly = b):
      y = new double[49]
      for i = 0 to 48:
          y[i] = (b[i] - sum(L[i][j]*y[j])) / L[i][i]

   e) Back substitution (Ux = y):
      x = new double[49]
      for i = 48 down to 0:
          x[i] = (y[i] - sum(U[i][j]*x[j])) / U[i][i]

   f) Cleanup:
      delete[] each row of L, delete[] L
      delete[] each row of U, delete[] U
      delete[] y

   g) Return x (caller must delete[])

8. DISPLAY RESULT:
   utils/Display.cpp → displaySolution(x, 49)
   Prints x[0] through x[48]

9. SAVE TO FILE:
   User chooses yes → saveSolution(x, 49, "solution.txt")
   Opens file, writes values, closes file

10. CLEANUP:
    delete[] x    (the solution vector)
    delete[] b    (the RHS vector)
    d.~Doolittle()  → ~LUDecomposition() → ~SLE() → ~Matrix()
       → delete[] each row of data, delete[] data

11. RETURN TO MENU:
    main() loops back, prints menu again
    User enters 17 → "bye bye!" → return 0

12. PROGRAM EXIT:
    OS reclaims ALL memory (stack, heap, mapped libraries)
    Process terminated
```

---

## Summary — Key Takeaways

| Concept | What It Does |
|---|---|
| **Preprocessor** | Copy-pastes header files into .cpp files |
| **#include guards** | Prevent duplicate class definitions |
| **Compiler** | Turns each .cpp into machine code (.o) independently |
| **Object file (.o)** | Machine code + symbol table + relocation entries |
| **Symbol table** | List of "I define X" and "I need Y" for each .o |
| **Name mangling** | Encodes function signatures into unique names |
| **Linker** | Matches all "I need Y" with "I define Y" across all .o files |
| **Relocation** | Fills in function addresses that were unknown at compile time |
| **Loader** | OS maps executable into virtual memory, resolves shared libraries |
| **vtable** | Array of function pointers for virtual dispatch |
| **vptr** | Hidden pointer in each object, points to its class's vtable |
| **Virtual dispatch** | At runtime, reads vptr → vtable → correct function address |
| **Stack** | Local variables, function frames (LIFO, grows down) |
| **Heap** | Dynamic allocations (new/delete), your matrix data lives here |
| **Inheritance** | Derived class contains all base class fields + its own |
| **Makefile** | Automates compile → link, only rebuilds changed files |
