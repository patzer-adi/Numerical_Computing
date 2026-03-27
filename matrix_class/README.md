# Matrix Operations Program

## What This Program Does

This is a menu-based C++ program for working with matrices. It can:

- **Add, subtract, multiply** matrices of any size
- **Calculate determinants** of square matrices
- **Solve systems of linear equations** (Ax = b) using:
  - Gaussian elimination (with/without pivoting)
  - LU Decomposition (Doolittle, Crout, Cholesky)
  - Gauss-Jacobi iteration
  - Gauss-Seidel iteration (with automatic diagonal dominance fix)
- **Matrix properties**: transpose, inverse, cofactor, adjoint, minor
- **Check properties**: square, symmetric, identity, null, diagonal, diagonally dominant
- **Read matrices from files** or let you type them in manually
- **Save solutions** to a file
- **GPU acceleration** with CUDA (optional)

Everything runs in a loop — pick an option, do the operation, go back to the menu.

---

## Compiling

```bash
make clean && make
```

Or manually:
```bash
g++ -std=c++11 -Wall -o matrix_program \
    main.cpp \
    src/Matrix.cpp \
    src/MatrixException.cpp \
    src/SystemOfLinearEquationSolver.cpp \
    src/GaussianElimination.cpp \
    src/LUDecomposition.cpp \
    src/Doolittle.cpp \
    src/Crout.cpp \
    src/Cholesky.cpp \
    src/GaussJacobi.cpp \
    src/GaussSeidel.cpp \
    src/MatrixOperations.cpp \
    utils/Input.cpp \
    utils/Display.cpp \
    app/Menu.cpp
```

This compiles everything into one executable called `matrix_program`.

Or, if you've filled in the Makefile:
```bash
make          # or make cpu
make verify   # builds the verification test suite
```

---

## Running

```bash
./matrix_program
```

**Important:** Run this from the `matrix_class/` directory. If you're in a different directory, file paths like `49/49l.txt` won't work.

---

## The Main Menu

When you start the program, you see:

```
=== Matrix Operations Program ===

=== Menu ===
1.  Add (A + B)
2.  Subtract (A - B)
3.  Multiply (A * B)
4.  Determinant
5.  Gaussian elimination (with pivoting)
6.  Gaussian elimination (without pivoting)
7.  LU - Doolittle
8.  LU - Crout
9.  LU - Cholesky
10. Gauss-Jacobi (iterative)
11. Gauss-Seidel (iterative)
12. Transpose
13. Scalar Multiply
14. Inverse
15. Minor Matrix
16. Cofactor
17. Adjoint
18. Check if Square
19. Check if Symmetric
20. Check if Identity
21. Check if Null
22. Check if Diagonal
23. Check if Diagonally Dominant
24. Make Diagonally Dominant
25. Check Equality (A == B)
26. Exit
Enter choice:
```

Type a number and press Enter.

After each operation finishes, the menu appears again. This continues until you choose 26 (Exit).

---

## How Input Works

Every operation needs matrix data. The program always asks:

```
How do you want to enter the matrix?
1. Enter manually
2. Load from file
Enter choice:
```

### Manual Entry

If you choose 1, you type in the size and then each row:

```
Enter rows: 3
Enter cols: 3
Enter matrix elements row by row:
Row 1: 1 2 3
Row 2: 4 5 6
Row 3: 7 8 9
```

- Type all values for one row on the same line, separated by spaces
- Press Enter after each row
- You must enter exactly `rows × cols` numbers

### File Input

If you choose 2, you give a filename:

```
Enter filename: 49/49l.txt
Loaded 49x49 matrix from 49/49l.txt
```

The program reads the file and figures out the dimensions automatically. It handles two formats:

1. **With header:** First line has `rows cols`, remaining lines have the data
2. **Without header:** Every line is data, dimensions come from counting rows and columns

---

## Each Option in Detail

### Option 1: Addition

**What it does:** Takes matrices A and B, computes A + B.

**Requirements:** A and B must have the **same dimensions** (same rows and same cols).

**Flow:**
```
1. Program asks for Matrix A (manual or file)
2. Program asks for Matrix B (manual or file)
3. Computes A + B element by element
4. Prints the result
```

**Example with 2×2 matrices:**

```
--- Matrix A ---
Enter rows: 2  Enter cols: 2
Row 1: 1 2
Row 2: 3 4

--- Matrix B ---
Enter rows: 2  Enter cols: 2
Row 1: 5 6
Row 2: 7 8

Result (A + B):
      6.000000      8.000000
     10.000000     12.000000
```

**What happens inside:** For each position (i,j), it simply does `result[i][j] = A[i][j] + B[i][j]`.

**Error case:** If A is 2×3 and B is 3×2, you get:
```
Error: can't add these matrices... they don't even match bro 😂
```

### Option 2: Subtraction

Exactly like addition, but computes A - B. Same dimension requirement.

### Option 3: Multiplication

**What it does:** Takes matrices A and B, computes A × B.

**Requirements:** A's column count must equal B's row count. If A is m×n and B is n×p, the result is m×p.

**Flow:**
```
1. Program asks for Matrix A
2. Program asks for Matrix B
3. Checks that A.cols == B.rows
4. Computes A × B using the standard formula
5. Prints the result
```

**Inside:** For each entry in the result:
```
result[i][j] = sum of A[i][k] * B[k][j]  for all k
```

This is the standard row-times-column multiplication.

**Example:**
```
A = [1 2]    B = [5 6]    Result = [1×5+2×7  1×6+2×8] = [19 22]
    [3 4]        [7 8]             [3×5+4×7  3×6+4×8]   [43 50]
```

### Option 4: Determinant

**What it does:** Takes one square matrix, returns a single number (the determinant).

**Requirements:** The matrix must be square (rows == cols).

**Flow:**
```
1. Program asks for one matrix
2. Checks it's square
3. Uses Gaussian elimination (with pivoting) to reduce to upper triangular form
4. Multiplies the diagonal entries to get the determinant
5. Prints the result
```

**How the algorithm works:**

1. Make a copy of the matrix (so we don't destroy the original)
2. For each column k:
   - Find the row below (or at) k with the largest value in column k (pivoting)
   - Swap that row with row k (if needed — and flip the sign of det)
   - Eliminate all entries below the diagonal in column k
3. The determinant = product of all diagonal entries × sign flips from swapping

**Example:**
```
Enter rows: 3   Enter cols: 3
Row 1: 6 1 1
Row 2: 4 -2 5
Row 3: 2 8 7

Determinant = -306.000000
```

**Special case:** If the matrix is singular (det = 0), it means the rows are linearly dependent — no unique solution exists for Ax = b.

### Option 5: Gaussian Elimination WITH Pivoting

**What it does:** Solves the system Ax = b for x.

**This is the most important option in the program.** It's what you use for the 49×49 and 225×225 systems.

**Flow:**
```
1. Program asks how to input the system:
   - Manual (type A and b)
   - Augmented file [A|b] in one file
   - Left file (A) + Right file (b) separately

2. Loads the data into A and b

3. Forward Elimination (with partial pivoting):
   - For each column k from 0 to n-1:
     a. Look at all entries from row k downward in column k
     b. Find the one with the LARGEST absolute value → call it the pivot
     c. Swap row k with the pivot row (so the biggest value is on the diagonal)
     d. For each row i below k:
        - Compute factor = A[i][k] / A[k][k]
        - Subtract factor × (row k) from row i
        - This makes A[i][k] = 0
     e. Apply the same operation to b

4. Back Substitution:
   - Now A is upper triangular (all zeros below diagonal)
   - Solve from the bottom up:
     x[n-1] = b[n-1] / A[n-1][n-1]
     x[n-2] = (b[n-2] - A[n-2][n-1]*x[n-1]) / A[n-2][n-2]
     ...and so on upward

5. Print solution
6. Optionally save to file
```

**Why pivoting matters:**

Without pivoting, if `A[k][k]` is zero or very small, dividing by it gives infinity or huge errors. Pivoting swaps in the largest available value, keeping the math stable.

**Example with files:**
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

### Option 6: Gaussian Elimination WITHOUT Pivoting

Same as option 5, but **skips the pivoting step**. It just uses whatever value is on the diagonal, even if it's zero.

- If the diagonal value is zero → the program throws an error
- If the diagonal value is very small → the solution will be inaccurate
- **Only use this if your professor specifically asks for it**, to compare accuracy with vs without pivoting

### Options 7, 8, 9: LU Decomposition

**All implemented!** Doolittle, Crout, and Cholesky LU decomposition methods.

Each decomposes A into L×U (or L×L^T for Cholesky), then solves via forward/back substitution. The solver returns a `SolverResult` containing the solution, iteration count, and verification error (max |L*U - A|).

### Options 10-11: Iterative Solvers

**Option 10 (Gauss-Jacobi):** Solves Ax = b using the Jacobi iterative method. Each x[i] is updated using values from the previous iteration only. Automatically checks and tries to fix diagonal dominance.

**Option 11 (Gauss-Seidel):** Solves Ax = b using the Gauss-Seidel method. More efficient than Jacobi because it uses updated values as soon as they're computed. Also automatically checks and fixes diagonal dominance.

**Both methods:**
- Return a `SolverResult` with `{x, iterations, converged, error}`
- Accept configurable `maxIter` and `tol` (defaults: 10000, 1e-10)
- Work on copies of the matrix (original is never modified)
- Print status messages (converged/diverged) from the UI layer, not from the solver itself

**Important:** Both methods require the matrix to be diagonally dominant (or have spectral radius < 1) for convergence.

### Options 12-25: Matrix Operations & Property Checks

Transpose, scalar multiply, inverse, minor, cofactor, adjoint, and various property checks (square, symmetric, identity, null, diagonal, diagonally dominant, equality).

### Option 26: Exit

Prints `bye bye!` and quits.

---

## Error Handling

The program **never crashes**. Every operation is wrapped in a try-catch:

```cpp
try {
    // do the operation
} catch (MatrixException &e) {
    e.printError();  // prints the error, goes back to the menu
}
```

**Types of errors you might see:**

| Error | When it happens |
|-------|----------------|
| `can't add these matrices... they don't even match bro 😂` | Addition/subtraction with different-sized matrices |
| `matrix multiplication dimensions don't work... go back to math class 📐` | A.cols ≠ B.rows |
| `determinant only works on square matrices... you knew that right? 🤔` | Non-square matrix for det |
| `matrix is singular... it has no solution, just like my love life 💔` | Zero pivot during GE |
| `can't open file 'xyz'... did you spell it right? 🤦` | File not found |
| `zero pivot found without pivoting... shoulda used pivoting bro 😬` | Zero on diagonal in option 6 |

After any error, the program goes right back to the menu — you don't need to restart.

---

## How the Code is Organized

### Project Structure

```
matrix_class/
├── main.cpp                 ← entry point (just calls runMenu(), 6 lines)
├── app/                     ← application layer (menu + routing)
│   ├── Menu.hpp                 runMenu() declaration
│   └── Menu.cpp                 all menu logic + handler functions
├── include/                 ← core library (NO cin/cout, pure logic)
│   ├── Matrix.hpp               base Matrix class (Rule of 5, const-correct)
│   ├── SolverResult.hpp         return type for all solvers
│   ├── MatrixException.hpp      error message class
│   ├── SystemOfLinearEquationSolver.hpp  abstract base for solvers
│   ├── GaussianElimination.hpp  GE class declaration
│   ├── LUDecomposition.hpp      LU + Doolittle/Crout/Cholesky declarations
│   ├── GaussJacobi.hpp          Gauss-Jacobi iterative solver
│   └── GaussSeidel.hpp          Gauss-Seidel iterative solver
├── src/                     ← core implementations (NO cin/cout)
│   ├── Matrix.cpp               constructors, add/sub/mul, det, display
│   ├── MatrixOperations.cpp     transpose, inverse, property checks
│   ├── MatrixException.cpp      (empty — everything is in the header)
│   ├── SystemOfLinearEquationSolver.cpp  base class + consolidated makeDiagDominant
│   ├── GaussianElimination.cpp  solveWithPivoting / solveWithoutPivoting
│   ├── LUDecomposition.cpp      base class constructors
│   ├── Doolittle.cpp            Doolittle LU decomposition
│   ├── Crout.cpp                Crout LU decomposition
│   ├── Cholesky.cpp             Cholesky LU decomposition
│   ├── GaussJacobi.cpp          Gauss-Jacobi iterative solver
│   └── GaussSeidel.cpp          Gauss-Seidel with diagonal dominance fix
├── cuda/                    ← GPU-accelerated implementations
│   ├── include/gpu_backend.cuh  GPU function declarations
│   └── src/gpu_kernels.cu       GPU kernels (Jacobi, Seidel, etc.)
├── utils/                   ← I/O helpers (read/write only, no menus)
│   ├── Input.hpp / Input.cpp    reading matrices from console or files
│   └── Display.hpp / Display.cpp printing results, solver status, saving
├── 49/                      ← test data (49×49 system)
├── 225/                     ← test data (225×225 system)
├── output/                  ← saved solution outputs
├── Makefile                 ← build for CPU-only version (+ verify target)
├── Makefile.lib             ← build as shared/static library
├── verify.cpp               ← automated verification test suite
├── README.md                ← this file
├── ALGORITHMS.md            ← algorithm explanations
└── CHANGELOG_2026-03-27.md  ← detailed refactoring changelog
```

### Class Hierarchy

```
Matrix                                ← base class (Rule of 5, const-correct)
│  stores: data[][], rows, cols
│  can do:  add, subtract, multiply, determinant, display (all const)
│  can do:  transpose, inverse, cofactor, adjoint, minor (all const)
│  checks:  isSquare, isSymmetric, isIdentity, isNull, isDiagonal (all const)
│  checks:  isDiagonallyDominant, makeDiagonallyDominant
│  move:    move constructor, move assignment (Rule of 5)
│
└── SystemOfLinearEquationSolver       ← inherits from Matrix
    │    abstract base: solve(b, n, maxIter, tol) → SolverResult
    │    shared: makeDiagDominant() → bool (consolidated helper)
    │    shared: checkDiagDominant() → bool
    │
    ├── GaussianElimination            ← direct solver
    │      solve() → SolverResult (converged=true, iterations=0)
    │      solveWithPivoting(b, n) → double*
    │      solveWithoutPivoting(b, n) → double*
    │
    ├── LUDecomposition                ← abstract base for LU methods
    │   ├── Doolittle → SolverResult (error = max|L*U-A|)
    │   ├── Crout → SolverResult (error = max|L*U-A|)
    │   └── Cholesky → SolverResult (error = max|L*L^T-A|)
    │
    ├── GaussJacobi                    ← iterative (two-array, old values only)
    │      solve() → SolverResult {x, iterations, converged, error, dominanceAchieved}
    │
    └── GaussSeidel                    ← iterative (in-place, newest values)
           solve() → SolverResult {x, iterations, converged, error, dominanceAchieved}
```

**Why inheritance?** All these algorithms need a matrix to work on. Instead of copying the matrix storage code into every class, they all inherit from `Matrix`. This means `GaussianElimination` can use `data[i][j]`, `rows`, `cols`, `setData()`, etc. — all from the parent class.

### How Data Flows Through the Program

```
User Input → main.cpp → getMatrixInput() → creates Matrix/GE object
                              ↓
             calls add/subtract/multiply/solve on the object
                              ↓
             prints result via display() or displaySolution()
                              ↓
             optionally saves via writeSolutionToFile()
                              ↓
             catches any MatrixException → prints error
                              ↓
             loops back to menu
```

### Memory Management

The matrices use raw `double**` pointers (2D arrays on the heap):

- **Constructor** allocates memory: `data = new double*[rows]`, then each `data[i] = new double[cols]`
- **Destructor** frees it: loops through deleting each `data[i]`, then `delete[] data`
- **Copy constructor** makes a deep copy (new memory, copies every element)
- **Move constructor** steals the pointer (no copy, just takes ownership — Rule of 5)
- **Move assignment** same as move constructor but for assignment
- **copyFrom()** delegates to operator= (kept for backward compatibility)

Solvers work on **copies** of the matrix data (original matrix is never modified). Caller must `delete[]` the solution vector from `SolverResult.x`.

---

## What's Implemented vs What's Left

| Feature | Status | File |
|---------|--------|------|
| Matrix storage & constructors | ✅ | `Matrix.cpp` |
| Add / Subtract / Multiply | ✅ | `Matrix.cpp` |
| Operator overloading (+, -, *, ==, <<, >>) | ✅ | `Matrix.cpp` |
| Determinant | ✅ | `Matrix.cpp` |
| Transpose / Inverse / Cofactor / Adjoint | ✅ | `MatrixOperations.cpp` |
| Property checks (square, symmetric, etc.) | ✅ | `MatrixOperations.cpp` |
| Diagonal dominance check & fix | ✅ | `MatrixOperations.cpp` |
| GE with pivoting | ✅ | `GaussianElimination.cpp` |
| GE without pivoting | ✅ | `GaussianElimination.cpp` |
| Doolittle LU | ✅ | `Doolittle.cpp` |
| Crout LU | ✅ | `Crout.cpp` |
| Cholesky LU | ✅ | `Cholesky.cpp` |
| Gauss-Jacobi (iterative) | ✅ | `GaussJacobi.cpp` |
| Gauss-Seidel (iterative, with diag. dominance fix) | ✅ | `GaussSeidel.cpp` |
| GPU kernels (Jacobi, Seidel, operations) | ✅ | `gpu_kernels.cu` |
| Console input | ✅ | `Input.cpp` |
| File input (matrix + RHS + augmented) | ✅ | `Input.cpp` |
| Solution display & file save | ✅ | `Display.cpp` |
| Error handling | ✅ | `MatrixException.hpp` |
