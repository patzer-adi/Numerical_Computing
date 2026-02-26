# Understanding the Matrix Data Files 📂

This explains the test matrix files in the `49/` and `225/` directories — what they are, how they're structured, how to use them, and where students commonly make mistakes.

---

## What Are These Files?

These are **systems of linear equations** in the form **Ax = b**, stored as text files:

| System | Matrix A (left) | Vector b (right) | Size |
|--------|----------------|-------------------|------|
| 49×49  | `49/49l.txt`   | `49/49r.txt`      | 49 unknowns |
| 225×225| `225/225left.txt`| `225/225right.txt`| 225 unknowns |

The goal is to **solve for x** — find the 49 (or 225) unknown values that satisfy all equations simultaneously.

---

## Why "Left" and "Right"?

From the equation **Ax = b**:

```
[    A    ] [x] = [b]
 ↑ LEFT        ↑ RIGHT
 (matrix)      (vector)
```

- **Left file** = The coefficient matrix **A** (the left-hand side)
- **Right file** = The RHS vector **b** (the right-hand side)

This naming comes from how they appear in the equation. Your professor or textbook might also call them the **coefficient matrix** and the **constant vector**.

---

## File Format Details

### Left Files (Coefficient Matrix A)

```
49 50           ← header line: "49 rows, 50 columns"
-3.800e+002  7.166e+001  -1.848e+001 ...   ← row 1 (49 values)
 3.603e+001 -2.360e+002   2.336e+001 ...   ← row 2
...                                         ← 49 rows total
```

**Important quirk:** The header says `49 50` (not `49 49`). This means the file was originally exported as a 49×50 matrix — it may include an extra column. The `Input.cpp` code handles this by checking:
- If the first line has exactly 2 numbers and the remaining rows are wider → it's a dimension header
- The reader then uses `rows` and `cols` from the header

For the 225 file, the header says `225 226`.

> **What's that extra column?** It could be an augmented format `[A|b]` where the last column IS the RHS vector. But since you have separate right-hand side files, you typically **load A from the left file** and **b from the right file** (option 3 in the menu). The program will figure out the correct dimensions.

### Right Files (RHS Vector b)

```
-4.8924480e+002    ← b[0]
-1.7704529e+002    ← b[1]
-1.5400009e+002    ← b[2]
...                ← one value per line
```

Much simpler — just one number per line, one for each equation.

### Number Format

All values use **scientific notation** (also called exponential notation):

```
-3.8004877e+002  means  -3.8004877 × 10² = -380.04877
 1.0704565e+003  means   1.0704565 × 10³ =  1070.4565
 0.0000000e+000  means   0.0
```

C++ reads this automatically with `cin >>` or `ifstream >>` — no special parsing needed.

---

## What Kind of Matrices Are These?

These are **sparse, banded, diagonally-dominant matrices** — typical of numerical methods in engineering and science. Key characteristics:

### Sparsity (mostly zeros)
Look at any row of the 49×49 matrix: most entries are `0.0000000e+000`. Only ~14 entries per row are nonzero. The matrix is **~85% zeros**.

### Band Structure
The nonzero entries cluster around the diagonal and appear at regular offsets (every 7 positions for the 49×49, every 15 for the 225×225). This is the hallmark of a **stencil-based discretization**.

### Block Structure
- **49 = 7 × 7** → a 7×7 grid of unknowns
- **225 = 15 × 15** → a 15×15 grid of unknowns

The matrices come from discretizing a **2D problem** (like a plate bending equation or biharmonic equation) on a regular grid. Each grid point interacts with its neighbors, producing the banded pattern.

### Diagonal Dominance
The diagonal entries are large and negative (e.g., `-380`, `-3009`), which means Gaussian elimination works well on these systems — especially with pivoting.

---

## How to Load and Solve These in the Program

### Option 1: Load A and b separately (Recommended)

```
Enter choice: 5  (Gaussian elimination with pivoting)

How do you want to input the system Ax = b?
3. Load A from left file and b from right file

Enter matrix (left) file: 49/49l.txt
Enter RHS (right) file: 49/49r.txt
```

### Option 2: Save the solution

```
Save solution to file? (1=yes, 0=no): 1
Enter output filename: solution_49.txt
```

### Interpreting the Solution

The output `x[0], x[1], ..., x[48]` are the **values at each grid point**. For the 49×49 system (7×7 grid), you can reshape them into a 7×7 grid:

```
x[0]  x[1]  x[2]  x[3]  x[4]  x[5]  x[6]
x[7]  x[8]  x[9]  x[10] x[11] x[12] x[13]
...
x[42] x[43] x[44] x[45] x[46] x[47] x[48]
```

These values represent the physical quantity (e.g., deflection, temperature) at each point on the grid.

---

## How to Verify Your Solution

After finding **x**, compute **Ax** and compare with **b**. The residual should be tiny:

```
residual = ||Ax - b|| ≈ 0  (should be ~1e-8 or smaller)
```

In code:
```cpp
// After solving, verify
for (int i = 0; i < n; i++) {
    double Ax_i = 0;
    for (int j = 0; j < n; j++)
        Ax_i += A[i][j] * x[j];
    double error = fabs(Ax_i - b[i]);
    // error should be very small for each row
}
```

---

## Common Mistakes Students Make

### 1. Confusing "49 50" header with actual matrix dimensions
The first line `49 50` is NOT row 1 of the matrix — it's a header saying "49 rows, 50 columns." If you read it as data, you'll have a 50×50 matrix with wrong values. The `Input.cpp` code handles this, but if you're writing your own reader, watch out.

### 2. Mismatched A and b sizes
If A is 49×49 but you load a b with 48 or 50 entries, the solve will either crash or give garbage. Always check `A.rows == length(b)`.

### 3. Not using pivoting
These matrices work fine with pivoting. Without pivoting, you might encounter a zero (or near-zero) pivot and get `inf` or `nan` in your solution. **Always use partial pivoting** (option 5) unless you have a specific reason not to.

### 4. File path issues
If you run the program from the wrong directory, it can't find `49/49l.txt`. Always run from the `matrix_class/` directory:
```bash
cd matrix_class
./matrix_program
```

### 5. Forgetting the file is space-separated, not comma-separated
The values are separated by spaces. If you ever convert these files or create your own, make sure to use spaces (not commas, tabs, or semicolons).

### 6. Precision loss with `float` vs `double`
These files have 7-8 significant digits. Using `float` (7 digits) will lose precision. The program uses `double` (15-16 digits) — don't change this.

### 7. Index confusion (0-based vs 1-based)
The program uses **0-based indexing** (`x[0]` through `x[48]`). Your textbook or professor might use 1-based indexing (`x₁` through `x₄₉`). Don't mix them up.

### 8. Trying to print a 225×225 matrix to the terminal
It's 225 columns wide. It'll wrap horribly and be unreadable. Use file output instead, or only print the solution vector.

---

## Creating Your Own Test Files

If you want to test with a smaller system, create files in the same format:

**my_matrix.txt** (3×3 system):
```
2  1  0
1  3  1
0  1  2
```

**my_rhs.txt**:
```
1
2
3
```

Then load them:
```
Enter matrix (left) file: my_matrix.txt
Enter RHS (right) file: my_rhs.txt
```

---

## Quick Reference

| Detail | 49×49 System | 225×225 System |
|--------|-------------|---------------|
| Grid | 7×7 | 15×15 |
| Matrix file | `49/49l.txt` | `225/225left.txt` |
| RHS file | `49/49r.txt` | `225/225right.txt` |
| Header line | `49 50` | `225 226` |
| Nonzeros per row | ~14 | ~30 |
| Matrix file size | ~38 KB | ~810 KB |
| RHS file size | ~763 B | ~3.5 KB |
| Use pivoting? | Yes | Yes |
