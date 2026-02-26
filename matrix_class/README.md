# Matrix Operations Program 🧮

A C++ matrix library built with inheritance and operator overloading. Supports basic matrix arithmetic, determinants, Gaussian elimination (with/without pivoting), and has skeleton LU decomposition methods ready for you to implement.

## Project Structure

```
matrix_class/
├── main.cpp                       # menu-driven driver program
├── include/                       # header files
│   ├── Matrix.hpp                 # base Matrix class
│   ├── MatrixException.hpp        # custom exception class (inline)
│   ├── GaussianElimination.hpp    # GE class (inherits Matrix)
│   └── LUDecomposition.hpp        # LU base + Doolittle/Crout/Cholesky classes
├── src/                           # implementations
│   ├── Matrix.cpp                 # constructors, add/sub/mul, determinant, display
│   ├── MatrixException.cpp        # (empty — exception is header-only)
│   ├── GaussianElimination.cpp    # solve with/without pivoting
│   ├── LUDecomposition.cpp        # base class constructors
│   ├── Doolittle.cpp              # TODO skeleton
│   ├── Crout.cpp                  # TODO skeleton
│   └── Cholesky.cpp               # TODO skeleton
├── utils/                         # I/O helpers
│   ├── Input.hpp / Input.cpp      # console & file input for matrices/systems
│   └── Display.hpp / Display.cpp  # display & write solution to file
├── 49/                            # test data: 49×49 system
│   ├── 49l.txt                    # coefficient matrix (left)
│   └── 49r.txt                    # RHS vector (right)
└── 225/                           # test data: 225×225 system
    ├── 225left.txt                # coefficient matrix
    └── 225right.txt               # RHS vector
```

---

## How to Compile

### Manual (one command)

```bash
g++ -std=c++11 -Wall -o matrix_program \
    main.cpp \
    src/Matrix.cpp \
    src/MatrixException.cpp \
    src/GaussianElimination.cpp \
    src/LUDecomposition.cpp \
    src/Doolittle.cpp \
    src/Crout.cpp \
    src/Cholesky.cpp \
    utils/Input.cpp \
    utils/Display.cpp
```

### Using Make (after you fill in the Makefile)

```bash
make          # builds the program
make clean    # removes object files and the binary
```

---

## How to Run

```bash
./matrix_program
```

The program shows an interactive menu:

```
=== Menu ===
1. Add two matrices
2. Subtract two matrices
3. Multiply two matrices
4. Calculate determinant
5. Gaussian elimination with pivoting
6. Gaussian elimination without pivoting
7. LU Decomposition (Doolittle)    ← not implemented yet
8. LU Decomposition (Crout)        ← not implemented yet
9. LU Decomposition (Cholesky)     ← not implemented yet
10. Exit
Enter choice:
```

For each operation, you can enter matrices **manually** (element by element) or **load from a file**.

---

## Expected Output Examples

### Addition (Option 1) — Manual Input

```
Enter choice: 1

--- Matrix A ---
How do you want to enter the matrix?
1. Enter manually
2. Load from file
Enter choice: 1
Enter rows: 2
Enter cols: 2
Enter matrix elements row by row:
Row 1: 1 2
Row 2: 3 4

--- Matrix B ---
...same flow, enter: 5 6 / 7 8...

Result (A + B):
      6.000000     8.000000
     10.000000    12.000000
```

### Determinant (Option 4)

```
Enter choice: 4
--- Enter square matrix ---
Enter rows: 3   Enter cols: 3
Row 1: 6 1 1
Row 2: 4 -2 5
Row 3: 2 8 7

Determinant = -306.000000
```

### Gaussian Elimination (Option 5) — From Files

```
Enter choice: 5
How do you want to input the system Ax = b?
3. Load A from left file and b from right file
Enter matrix (left) file: 49/49l.txt
Enter RHS (right) file: 49/49r.txt
Loaded 49x49 system

--- Solution ---
x[0] = ...
x[1] = ...
...
x[48] = ...

Save solution to file? (1=yes, 0=no): 1
Enter output filename: solution_49.txt
Solution written to solution_49.txt
```

---

## What's Implemented

| Feature                            | Status |
|------------------------------------|--------|
| Matrix addition (`+`)             | ✅     |
| Matrix subtraction (`-`)          | ✅     |
| Matrix multiplication (`*`)       | ✅     |
| Determinant (via row reduction)    | ✅     |
| Gaussian elimination WITH pivoting | ✅     |
| Gaussian elimination WITHOUT pivoting | ✅  |
| LU Decomposition — Doolittle      | 🚧 skeleton |
| LU Decomposition — Crout          | 🚧 skeleton |
| LU Decomposition — Cholesky       | 🚧 skeleton |
| File I/O (matrix & RHS loading)    | ✅     |
| Save solution to file             | ✅     |

---

## Implementation Ideas for LU Decomposition

All three methods decompose the matrix **A** into a product of two triangular matrices, then solve by forward + back substitution.

### Doolittle (L has 1s on diagonal)

```
For each row i and column j:
  if i <= j:  U[i][j] = A[i][j] - sum(L[i][k]*U[k][j] for k=0..i-1)
  if i > j:   L[i][j] = (A[i][j] - sum(L[i][k]*U[k][j] for k=0..j-1)) / U[j][j]
  L[i][i] = 1   (unit diagonal)
```

### Crout (U has 1s on diagonal)

```
For each column j and row i:
  if i >= j:  L[i][j] = A[i][j] - sum(L[i][k]*U[k][j] for k=0..j-1)
  if i < j:   U[i][j] = (A[i][j] - sum(L[i][k]*U[k][j] for k=0..i-1)) / L[i][i]
  U[j][j] = 1   (unit diagonal)
```

### Cholesky (A = L * L^T, requires symmetric positive definite matrix)

```
For each column j:
  L[j][j] = sqrt(A[j][j] - sum(L[j][k]^2 for k=0..j-1))
  For each row i > j:
    L[i][j] = (A[i][j] - sum(L[i][k]*L[j][k] for k=0..j-1)) / L[j][j]
```

### Solving After Decomposition

Once you have **L** and **U**:

1. **Forward substitution**: Solve `Ly = b` for `y`
2. **Back substitution**: Solve `Ux = y` for `x`

```cpp
// forward: solve Ly = b
for (int i = 0; i < n; i++) {
    y[i] = b[i];
    for (int j = 0; j < i; j++)
        y[i] -= L[i][j] * y[j];
    y[i] /= L[i][i]; // skip this line for Doolittle (L diagonal is 1)
}

// backward: solve Ux = y
for (int i = n-1; i >= 0; i--) {
    x[i] = y[i];
    for (int j = i+1; j < n; j++)
        x[i] -= U[i][j] * x[j];
    x[i] /= U[i][i]; // skip this line for Crout (U diagonal is 1)
}
```

---

## Class Hierarchy

```
Matrix                      ← base class (data, rows, cols, add/sub/mul/det)
├── GaussianElimination     ← solveWithPivoting / solveWithoutPivoting
└── LUDecomposition         ← abstract base (virtual solve = 0)
    ├── Doolittle
    ├── Crout
    └── Cholesky
```

All child classes **inherit** `data`, `rows`, `cols` from `Matrix`, so you can use `data[i][j]` directly inside your `solve()` methods.

---

## Input File Format

- **Matrix files** (e.g. `49l.txt`): space-separated values, one row per line
- **RHS files** (e.g. `49r.txt`): one value per line
- The program auto-detects if a file has a `rows cols` header line or just raw data

---

## Notes

- All values are `double` precision
- The program uses **partial pivoting** in the Gaussian elimination (option 5) for numerical stability
- Exceptions use meme-based messages 💀 because why not
- The `matrix_program` binary is generated in the same directory
