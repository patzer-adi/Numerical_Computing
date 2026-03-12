# Matrix Class Internals — Under the Hood

A deep dive into how the `Matrix` class uses **friend functions**, **operator overloading**, and **matrix property checks**. This document explains *why* each technique is used, *what* it does, and *how* it works internally.

---

## Table of Contents

- [1. Friend Functions](#1-friend-functions)
  - [1.1 What Are Friend Functions?](#11-what-are-friend-functions)
  - [1.2 operator\<\< (Insertion / Output)](#12-operator-insertion--output)
  - [1.3 operator\>\> (Extraction / Input)](#13-operator-extraction--input)
  - [1.4 Why Friend Instead of Member?](#14-why-friend-instead-of-member)
- [2. Operator Overloading](#2-operator-overloading)
  - [2.1 What Is Operator Overloading?](#21-what-is-operator-overloading)
  - [2.2 Arithmetic Operators (+, -, \*)](#22-arithmetic-operators----)
  - [2.3 Scalar Multiplication (Matrix \* double)](#23-scalar-multiplication-matrix--double)
  - [2.4 Element Access Operator — operator()](#24-element-access-operator--operator)
  - [2.5 Equality Operator — operator==](#25-equality-operator--operator)
  - [2.6 Assignment Operator — operator=](#26-assignment-operator--operator)
  - [2.7 Why Overload Operators?](#27-why-overload-operators)
- [3. Matrix Property Checks](#3-matrix-property-checks)
  - [3.1 isSquare()](#31-issquare)
  - [3.2 isSymmetric()](#32-issymmetric)
  - [3.3 isIdentity()](#33-isidentity)
  - [3.4 isNull()](#34-isnull)
  - [3.5 isDiagonal()](#35-isdiagonal)
  - [3.6 isDiagonallyDominant()](#36-isdiagonallydominant)
  - [3.7 makeDiagonallyDominant()](#37-makediagonallydominant)
- [4. Summary Table](#4-summary-table)

---

## 1. Friend Functions

### 1.1 What Are Friend Functions?

In C++, class members (like `data`, `rows`, `cols`) are `protected` or `private` — outside code can't touch them. A **friend function** is a special non-member function that the class explicitly grants access to its private/protected data. You declare it inside the class with the `friend` keyword:

```cpp
class Matrix {
protected:
    double **data;
    int rows, cols;
public:
    friend ostream &operator<<(ostream &out, const Matrix &m);
    friend istream &operator>>(istream &in, Matrix &m);
};
```

The function itself is **not** a member of `Matrix` — it lives outside the class. But because it's declared `friend`, it can read and write `m.data`, `m.rows`, `m.cols` directly.

---

### 1.2 operator<< (Insertion / Output)

**Purpose:** Let you do `cout << myMatrix;` instead of `myMatrix.display();`

**Declaration (in Matrix.hpp):**
```cpp
friend ostream &operator<<(ostream &out, const Matrix &m);
```

**Implementation (in MatrixOperations.cpp):**
```cpp
ostream &operator<<(ostream &out, const Matrix &m) {
    out << fixed << setprecision(6);
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++)
            out << setw(14) << m.data[i][j];
        out << endl;
    }
    out << endl;
    return out;
}
```

**How it works step by step:**

1. Takes a reference to an `ostream` (like `cout`) and a `const Matrix &` (the matrix to print)
2. Sets formatting — `fixed` decimal notation, 6 decimal places
3. Loops through every row and column of `m.data[][]`
4. Prints each element right-aligned in a 14-character-wide column (`setw(14)`)
5. Returns the `ostream` reference — this allows **chaining**: `cout << A << B << endl;`

**Why `const Matrix &m`?** Printing shouldn't modify the matrix. The `const` enforces that the `<<` operator is read-only.

**Why return `ostream &`?** So you can chain multiple `<<` calls in one statement. Without the return, `cout << A << B` wouldn't work.

---

### 1.3 operator>> (Extraction / Input)

**Purpose:** Let you do `cin >> myMatrix;` to read a matrix from standard input.

**Declaration:**
```cpp
friend istream &operator>>(istream &in, Matrix &m);
```

**Implementation:**
```cpp
istream &operator>>(istream &in, Matrix &m) {
    // clean up old data if any
    if (m.data != nullptr) {
        for (int i = 0; i < m.rows; i++)
            delete[] m.data[i];
        delete[] m.data;
    }

    cout << "Enter rows: ";
    in >> m.rows;
    cout << "Enter cols: ";
    in >> m.cols;

    m.data = new double *[m.rows];
    for (int i = 0; i < m.rows; i++)
        m.data[i] = new double[m.cols];

    cout << "Enter matrix elements row by row:" << endl;
    for (int i = 0; i < m.rows; i++) {
        cout << "Row " << i + 1 << ": ";
        for (int j = 0; j < m.cols; j++)
            in >> m.data[i][j];
    }
    return in;
}
```

**How it works:**

1. **Memory cleanup** — if `m` already holds data (from a previous read), free it to avoid memory leaks
2. **Read dimensions** — asks user for `rows` and `cols`, stores into `m.rows`, `m.cols`
3. **Allocate memory** — creates a `rows × cols` 2D array via `new double*[rows]` + `new double[cols]` for each row
4. **Read elements** — loops through each row, reading `cols` numbers from `cin` per row
5. **Return `istream &`** — enables chaining: `cin >> A >> B;`

**Why `Matrix &m` (no `const`)?** Because `>>` modifies the matrix — it writes data into it.

---

### 1.4 Why Friend Instead of Member?

Stream operators (`<<` and `>>`) **must** be friend functions, not member functions. Here's why:

| Approach | Syntax | Problem |
|---|---|---|
| Member function | `A.operator<<(cout)` → `A << cout` | The matrix would be on the **left**, cout on the right — backwards! |
| Friend function | `operator<<(cout, A)` → `cout << A` | The stream is on the left, matrix on the right — correct! |

The left operand of `<<` is `cout` (an `ostream`), which is defined by the C++ standard library. We can't add a member function to `ostream`. So we write a standalone function and make it a `friend` so it can access `data`, `rows`, and `cols`.

---

## 2. Operator Overloading

### 2.1 What Is Operator Overloading?

Operator overloading lets you use built-in operators (`+`, `-`, `*`, `()`, `==`, `=`) with your own classes. Instead of writing:

```cpp
Matrix C = A.add(B);
```

You can write:

```cpp
Matrix C = A + B;
```

Both do the same thing — the `+` operator calls the `add()` method internally. The purpose is to make the code read like math.

---

### 2.2 Arithmetic Operators (+, -, *)

**Declarations:**
```cpp
Matrix operator+(const Matrix &other);
Matrix operator-(const Matrix &other);
Matrix operator*(const Matrix &other);
```

**How `operator+` works:**
```cpp
Matrix Matrix::operator+(const Matrix &other) {
    Matrix temp(other);       // make a copy (because add() takes by value)
    return add(temp);         // delegate to the add() method
}
```

1. The `+` operator is a **wrapper** around the existing `add()` function
2. It creates a temporary copy of `other` (because `add()` takes its argument by value, but `operator+` receives a `const` reference)
3. Calls `add(temp)` which:
   - Checks dimensions match (`rows == other.rows && cols == other.cols`)
   - Creates a `result` matrix of the same size
   - Loops through every `(i, j)`, computing `result[i][j] = this->data[i][j] + other.data[i][j]`
   - Returns `result`

**The same pattern applies to `-` and `*`:**
- `operator-` → calls `subtract()` → element-wise `data[i][j] - other.data[i][j]`
- `operator*` → calls `multiply()` → standard row-by-column: `result[i][j] = Σ data[i][k] * other[k][j]`

**Dimension checks are enforced:**
- Addition/Subtraction: both matrices must have identical dimensions
- Multiplication: `this->cols` must equal `other.rows` (result is `this->rows × other.cols`)

---

### 2.3 Scalar Multiplication (Matrix * double)

**Declaration:**
```cpp
Matrix operator*(double scalar);
```

**Implementation:**
```cpp
Matrix Matrix::operator*(double scalar) {
    Matrix result(rows, cols);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.data[i][j] = data[i][j] * scalar;
    return result;
}
```

**How it works:**

1. Creates a new matrix `result` with the same dimensions
2. Multiplies every single element by the scalar value
3. Returns the new matrix

**Usage:** `Matrix S = A * 2.5;` — multiplies every element of A by 2.5.

> **Note:** This only works with the matrix on the left: `A * 2.5`. For `2.5 * A`, you would need a non-member function (not currently implemented).

---

### 2.4 Element Access Operator — operator()

**Declaration:**
```cpp
double &operator()(int i, int j);
```

**Implementation:**
```cpp
double &Matrix::operator()(int i, int j) {
    if (i < 0 || i >= rows || j < 0 || j >= cols)
        throw MatrixException(
            "bruh you went out of bounds with ()... matrix ain't that big");
    return data[i][j];
}
```

**What it does:**

Instead of `A.getData(i, j)` and `A.setData(i, j, val)`, you can write:

```cpp
double x = A(0, 1);    // read element at row 0, col 1
A(2, 3) = 7.5;         // write directly to row 2, col 3
```

**How it works:**

1. Checks bounds — throws an exception if the index is out of range
2. Returns a **reference** (`double &`) to `data[i][j]`
3. Because it returns a reference, you can both **read** and **write** through it

**Why `double &` (reference)?** If it returned a plain `double` (copy), `A(2, 3) = 7.5` would assign 7.5 to a temporary copy that immediately gets destroyed — the actual matrix wouldn't change. Returning a reference means you're writing directly to the matrix's internal storage.

---

### 2.5 Equality Operator — operator==

**Declaration:**
```cpp
bool operator==(const Matrix &other);
```

**Implementation:**
```cpp
bool Matrix::operator==(const Matrix &other) {
    if (rows != other.rows || cols != other.cols)
        return false;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fabs(data[i][j] - other.data[i][j]) > 1e-10)
                return false;
        }
    }
    return true;
}
```

**How it works:**

1. **Check dimensions** — if the matrices have different sizes, they can't be equal
2. **Compare every element** — but NOT with `==` on doubles! Instead, checks if `|a - b| > 1e-10`
3. If any element pair differs by more than the tolerance, return `false`
4. If all elements match, return `true`

**Why the tolerance (1e-10)?**

Floating-point arithmetic is imprecise. Something like:

```
0.1 + 0.2 = 0.30000000000000004  (not exactly 0.3)
```

Using `==` directly on doubles would give wrong results for matrices that are "equal" in every practical sense. The tolerance `1e-10` (0.0000000001) catches these rounding artifacts while still detecting genuinely different values.

---

### 2.6 Assignment Operator — operator=

**Declaration:**
```cpp
Matrix &operator=(const Matrix &other);
```

**Implementation:**
```cpp
Matrix &Matrix::operator=(const Matrix &other) {
    if (this == &other)
        return *this;                  // self-assignment check

    // free old memory
    if (data != nullptr) {
        for (int i = 0; i < rows; i++)
            delete[] data[i];
        delete[] data;
    }

    // deep copy
    rows = other.rows;
    cols = other.cols;
    data = new double *[rows];
    for (int i = 0; i < rows; i++) {
        data[i] = new double[cols];
        for (int j = 0; j < cols; j++)
            data[i][j] = other.data[i][j];
    }
    return *this;
}
```

**How it works:**

1. **Self-assignment guard** — `if (this == &other)` catches `A = A;` which would otherwise free the data before reading it (crash!)
2. **Free existing memory** — the left-hand matrix already has allocated arrays; free them to prevent memory leaks
3. **Deep copy** — allocate new arrays matching `other`'s dimensions, then copy every element value
4. **Return `*this`** — enables chaining: `A = B = C;`

**Why deep copy?** If we just copied the `data` pointer (`this->data = other.data`), both matrices would point to the **same** memory. Modifying one would silently modify the other. Worse, when one gets destroyed, its destructor frees the memory — the other matrix now has a dangling pointer. Deep copying creates independent memory for each matrix.

---

### 2.7 Why Overload Operators?

| Without overloading | With overloading |
|---|---|
| `Matrix C = A.add(B);` | `Matrix C = A + B;` |
| `Matrix D = A.multiply(B);` | `Matrix D = A * B;` |
| `double x = A.getData(1, 2);` | `double x = A(1, 2);` |
| `A.setData(1, 2, 5.0);` | `A(1, 2) = 5.0;` |
| `A.display();` | `cout << A;` |
| `A.readFromConsole();` | `cin >> A;` |

Operator overloading makes the code read like actual mathematics. When someone writes `C = A + B`, it's immediately clear what's happening — no need to memorize method names. This is especially valuable in a numerical computing library where the audience thinks in terms of mathematical notation.

---

## 3. Matrix Property Checks

### 3.1 isSquare()

**What it checks:** Is the number of rows equal to the number of columns?

```cpp
bool Matrix::isSquare() {
    return rows == cols;
}
```

**The math:**
A matrix is **square** if it's n × n (same number of rows and columns).

```
✅ Square:        ❌ Not square:
[1 2 3]           [1 2 3]
[4 5 6]           [4 5 6]
[7 8 9]
(3×3)             (2×3)
```

**Why it matters:** Many operations only work on square matrices — determinant, inverse, eigenvalues, LU decomposition, checking symmetry, checking diagonal dominance. This check is often the first line of defense in those functions.

---

### 3.2 isSymmetric()

**What it checks:** Does `A[i][j] == A[j][i]` for every pair (i, j)?

```cpp
bool Matrix::isSymmetric() {
    if (rows != cols)
        return false;
    for (int i = 0; i < rows; i++) {
        for (int j = i + 1; j < cols; j++) {
            if (fabs(data[i][j] - data[j][i]) > 1e-10)
                return false;
        }
    }
    return true;
}
```

**How it works:**

1. **Must be square first** — a non-square matrix can't be symmetric
2. **Only check above the diagonal** (`j = i + 1`) — comparing `A[i][j]` with `A[j][i]` automatically covers both the upper and lower triangle. No need to check both directions.
3. **Floating-point tolerance** — uses `fabs(a - b) > 1e-10` instead of `a != b`

**The math:**

A matrix A is symmetric if A = Aᵀ (it equals its own transpose).

```
✅ Symmetric:     ❌ Not symmetric:
[1 2 3]           [1 2 3]
[2 5 4]           [4 5 6]
[3 4 9]           [7 8 9]
```

**Why it matters:** Cholesky decomposition only works on symmetric positive-definite matrices. Gauss-Jacobi converges faster on symmetric systems. Many physics/engineering problems naturally produce symmetric matrices.

---

### 3.3 isIdentity()

**What it checks:** Are all diagonal elements 1, and all off-diagonal elements 0?

```cpp
bool Matrix::isIdentity() {
    if (rows != cols)
        return false;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (i == j) {
                if (fabs(data[i][j] - 1.0) > 1e-10)
                    return false;   // diagonal must be 1
            } else {
                if (fabs(data[i][j]) > 1e-10)
                    return false;   // off-diagonal must be 0
            }
        }
    }
    return true;
}
```

**How it works:**

1. Must be square
2. For every element `(i, j)`:
   - If `i == j` (on the diagonal): check if the value is close to 1.0
   - If `i != j` (off-diagonal): check if the value is close to 0.0
3. Uses the δ function (Kronecker delta) concept: `I[i][j] = 1 if i==j, 0 otherwise`

**The identity matrix:**

```
[1 0 0]
[0 1 0]
[0 0 1]
```

**Why it matters:** The identity matrix is the multiplicative identity — `A × I = I × A = A`. It's used to verify inverse computations: if `A × A⁻¹ = I`, the inverse is correct.

---

### 3.4 isNull()

**What it checks:** Are all elements zero?

```cpp
bool Matrix::isNull() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (fabs(data[i][j]) > 1e-10)
                return false;
        }
    }
    return true;
}
```

**How it works:**

1. Does **not** require square — any matrix can be null
2. Scans every element, checks if its absolute value is ≤ tolerance
3. Returns `false` as soon as any non-zero element is found

**The null (zero) matrix:**

```
[0 0 0]
[0 0 0]
```

**Why it matters:** The null matrix is the additive identity — `A + O = A`. It's also a singularity indicator: a zero matrix has determinant 0 and no inverse.

---

### 3.5 isDiagonal()

**What it checks:** Are all off-diagonal elements zero? (Diagonal elements can be anything.)

```cpp
bool Matrix::isDiagonal() {
    if (rows != cols)
        return false;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (i != j && fabs(data[i][j]) > 1e-10)
                return false;
        }
    }
    return true;
}
```

**How it works:**

1. Must be square
2. For every off-diagonal element (where `i != j`): check if it's zero
3. Diagonal elements are ignored — they can be any value

**A diagonal matrix:**

```
[5 0 0]
[0 3 0]
[0 0 7]
```

**Why it matters:** Diagonal matrices are trivially invertible (just invert each diagonal element), and their determinant is just the product of diagonal elements. They also guarantee diagonal dominance. Many numerical methods produce or require diagonal-heavy matrices.

---

### 3.6 isDiagonallyDominant()

**What it checks:** For every row, is the diagonal element's absolute value ≥ the sum of all other elements' absolute values in that row?

```cpp
bool Matrix::isDiagonallyDominant() {
    if (rows != cols)
        return false;
    for (int i = 0; i < rows; i++) {
        double diagVal = fabs(data[i][i]);
        double offDiagSum = 0.0;
        for (int j = 0; j < cols; j++) {
            if (j != i)
                offDiagSum += fabs(data[i][j]);
        }
        if (diagVal < offDiagSum)
            return false;
    }
    return true;
}
```

**How it works, row by row:**

1. For row `i`, grab the diagonal element: `|A[i][i]|`
2. Sum up **all other** elements in that row: `|A[i][0]| + |A[i][1]| + ... + |A[i][n-1]|` (skipping `j == i`)
3. Check: is `|A[i][i]| ≥ sum`?
4. If **any** row fails, the matrix is not diagonally dominant

**The math:**

A matrix is diagonally dominant if for every row i:

```
|a[i][i]| ≥ Σ |a[i][j]|   for all j ≠ i
```

**Example:**

```
✅ Diagonally dominant:        ❌ Not diagonally dominant:
[10  1  2]                      [1  5  2]
[ 3  8  1]                      [3  2  6]
[ 1  2  7]                      [7  1  3]

Row 0: |10| ≥ |1|+|2| = 3 ✅    Row 0: |1| < |5|+|2| = 7 ❌
Row 1: |8|  ≥ |3|+|1| = 4 ✅
Row 2: |7|  ≥ |1|+|2| = 3 ✅
```

**Why it matters:** Diagonal dominance **guarantees convergence** for iterative solvers like Gauss-Jacobi and Gauss-Seidel. Without it, these methods might oscillate forever and never find a solution. It also improves numerical stability for direct solvers.

---

### 3.7 makeDiagonallyDominant()

**What it does:** Tries to rearrange the rows of the matrix so that diagonal dominance is achieved.

```cpp
Matrix Matrix::makeDiagonallyDominant() {
    if (rows != cols)
        throw MatrixException(
            "can only make square matrices diagonally dominant... come on");

    int n = rows;
    Matrix result(*this);   // work on a copy

    for (int col = 0; col < n; col++) {
        // find the row (from col onward) with the largest value in this column
        int bestRow = col;
        double bestVal = fabs(result.data[col][col]);

        for (int row = col; row < n; row++) {
            if (fabs(result.data[row][col]) > bestVal) {
                bestVal = fabs(result.data[row][col]);
                bestRow = row;
            }
        }

        // swap rows
        if (bestRow != col) {
            double *temp = result.data[col];
            result.data[col] = result.data[bestRow];
            result.data[bestRow] = temp;
        }
    }

    // verify if it worked
    if (result.isDiagonallyDominant())
        cout << "successfully made the matrix diagonally dominant!" << endl;
    else
        cout << "tried my best but this matrix can't be made diagonally "
                "dominant just by swapping rows... sorry bro" << endl;

    return result;
}
```

**How the algorithm works:**

1. **Copy the matrix** — don't modify the original
2. **For each column `col` (0, 1, ..., n-1):**
   - Look at rows from `col` downward (not already placed rows)
   - Find which row has the **largest absolute value** in column `col`
   - Swap that row into position `col`
3. **Verify** — after all swaps, call `isDiagonallyDominant()` to check if we succeeded
4. **Return the result** — whether it achieved dominance or not

**Why only row swaps?** Row swapping doesn't change the mathematical system (for Ax = b, it's equivalent to reordering the equations). The values themselves don't change, only their positions. This is the simplest and safest approach.

**Limitation:** Not every matrix **can** be made diagonally dominant by row swaps alone. If no arrangement of rows produces dominance, the algorithm reports failure honestly. Some matrices fundamentally lack the property.

**Example:**

```
Before:                After swapping:
[1  5  2]              [7  1  3]    ← moved from row 2
[3  2  6]              [3  2  6]    ← stays
[7  1  3]              [1  5  2]    ← moved from row 0

Row 0: |7| ≥ |1|+|3| = 4 ✅
Row 1: |2| < |3|+|6| = 9 ❌  → still not diagonally dominant
```

---

## 4. Summary Table

### Friend Functions

| Function | Purpose | Why Friend? |
|---|---|---|
| `operator<<` | Print matrix with `cout << A` | Stream must be left operand — can't be a member of `Matrix` |
| `operator>>` | Read matrix with `cin >> A` | Stream must be left operand — can't be a member of `Matrix` |

### Operator Overloading

| Operator | Method | What It Does | Returns |
|---|---|---|---|
| `A + B` | `operator+` | Element-wise addition | New `Matrix` |
| `A - B` | `operator-` | Element-wise subtraction | New `Matrix` |
| `A * B` | `operator*(Matrix)` | Row-by-column multiplication | New `Matrix` |
| `A * 2.5` | `operator*(double)` | Multiply every element by scalar | New `Matrix` |
| `A(i, j)` | `operator()` | Access/modify element at (i, j) | `double &` (reference) |
| `A == B` | `operator==` | Compare all elements (with tolerance) | `bool` |
| `A = B` | `operator=` | Deep copy all data | `Matrix &` (self) |

### Property Checks

| Method | Question Answered | Requires Square? | Complexity |
|---|---|---|---|
| `isSquare()` | rows == cols? | No | O(1) |
| `isSymmetric()` | A == Aᵀ? | Yes | O(n²) |
| `isIdentity()` | Diagonal = 1, rest = 0? | Yes | O(n²) |
| `isNull()` | All elements = 0? | No | O(n×m) |
| `isDiagonal()` | Off-diagonal = 0? | Yes | O(n²) |
| `isDiagonallyDominant()` | \|diag\| ≥ Σ\|off-diag\| per row? | Yes | O(n²) |
| `makeDiagonallyDominant()` | Rearrange rows for dominance | Yes | O(n²) |

---

**Author:** Aditya Gowari  
**Part of:** Numerical Computing Matrix Library  
**License:** MIT
