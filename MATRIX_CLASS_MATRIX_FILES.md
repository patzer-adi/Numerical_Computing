# Understanding the Matrix Data Files

## The Big Picture

In numerical computing, we often need to solve systems of linear equations. These look like:

```
2x + 3y = 8
4x + 1y = 6
```

When you have 2 equations it's easy. But what about **49 equations with 49 unknowns**? Or **225 equations with 225 unknowns**? You can't do that by hand — that's where this program comes in.

We write the system in matrix form:

```
A · x = b

where:
  A = the matrix of coefficients (the numbers multiplying the unknowns)
  x = the vector of unknowns (what we want to find)
  b = the vector of constants (the right-hand side values)
```

For the small example above:

```
A = [2  3]      x = [x]      b = [8]
    [4  1]          [y]          [6]
```

The files in the `49/` and `225/` folders store **A** and **b** for much larger systems.

---

## What's in Each Folder

### The 49×49 System (`49/` folder)

| File | What it stores | Size |
|------|---------------|------|
| `49l.txt` | The 49×49 coefficient matrix **A** | 49 rows, each with 49 numbers |
| `49r.txt` | The RHS vector **b** | 49 numbers, one per line |

### The 225×225 System (`225/` folder)

| File | What it stores | Size |
|------|---------------|------|
| `225left.txt` | The 225×225 coefficient matrix **A** | 225 rows, each with 225 numbers |
| `225right.txt` | The RHS vector **b** | 225 numbers, one per line |

---

## Why "Left" and "Right"?

Look at the equation again:

```
A    ·    x    =    b
↑                   ↑
LEFT side           RIGHT side
of the equation     of the equation
```

- **Left file** = matrix **A** — the coefficients on the **left** side of the equals sign
- **Right file** = vector **b** — the constants on the **right** side of the equals sign

That's it. The naming literally comes from which side of the `=` sign they sit on.

Some professors call them:
- **Coefficient matrix** and **load vector**
- **Stiffness matrix** and **force vector** (in engineering)
- **LHS** and **RHS**

They all mean the same thing: A and b.

---

## File Format — Explained Line by Line

### Left File (49l.txt)

Open `49l.txt` and you'll see:

```
49 50
-3.8004877e+002  7.1664431e+001  -1.8485281e+001  ...  0.0000000e+000
 3.6037916e+001 -2.3602439e+002   2.3361444e+001  ...  0.0000000e+000
...
```

**Line 1: `49 50`** — This is a **header line**. It tells you the matrix has 49 rows. The "50" is because the file was exported with one extra column (possibly including b as the last column in augmented form). Our program reads this header and knows to expect 49 rows of data after it.

**Lines 2 onwards** — Each line is one row of the matrix. Row 1 has 49 numbers (or 50, with the extra column that the program handles). Row 2 has 49 numbers. And so on, for 49 rows.

**The numbers themselves** — They're in scientific notation:

```
-3.8004877e+002  means  -3.8004877 × 10^2  =  -380.04877
 7.1664431e+001  means   7.1664431 × 10^1  =   71.664431
 0.0000000e+000  means   0.0               =   0.0
-1.7704529e+002  means  -1.7704529 × 10^2  =  -177.04529
```

The `e+002` part means "move the decimal point 2 places right." The `e+000` means "don't move it" (so the number is just what you see before the e).

C++ reads scientific notation automatically — `cin >> val` or `fin >> val` parses it correctly.

### Right File (49r.txt)

```
-4.8924480e+002
-1.7704529e+002
-1.5400009e+002
-1.2534672e+002
...
```

Much simpler — just **one number per line**, 49 lines total. Each number is one element of vector b.

`b[0] = -489.24480`, `b[1] = -177.04529`, etc.

### Spaces vs Commas vs Tabs

The left files use **spaces** to separate values. Not commas, not tabs, not semicolons. This is important if you ever create your own files — use spaces.

---

## Where Do These Matrices Come From?

These are **stiffness matrices** from a numerical method called **finite differences** (or possibly finite elements). Here's the idea:

1. You have a physical problem — maybe a plate being bent, or heat spreading across a surface
2. You lay a **grid** over the surface
3. At each grid point, you write an equation relating that point to its neighbors
4. You end up with one equation per grid point

For the 49×49 system:
- The grid is **7 × 7** (7 rows × 7 columns = 49 points)
- Each grid point gives one equation → 49 equations
- Each equation has 49 unknowns (one per grid point) → 49×49 matrix

For the 225×225 system:
- The grid is **15 × 15** (15 × 15 = 225 points)
- Finer grid = more accuracy = bigger matrix

### Why are most entries zero?

Each grid point only talks to its **nearby neighbors** (not to points far away). So most entries in each row are zero. If point #25 only interacts with ~14 neighbors, then row 25 has only ~14 nonzero entries and ~35 zeros. This makes the matrix **sparse** (mostly zeros).

### Why is the diagonal so large?

The diagonal entry `A[i][i]` represents the "self-interaction" of point i. It's always the largest entry in its row (in absolute value). This property is called **diagonal dominance** — it means Gaussian elimination will work well on this matrix.

### The repeating block pattern

If you look carefully, you'll see a block structure. In the 49×49 matrix, there are 7×7 = 49 entries arranged in 7 blocks of 7. This comes from the 2D grid — one block per row of grid points, and the blocks interact diagonally (because grid rows are neighbors of each other).

---

## How to Compute the Solution

### Step 1: Load the data

In the program, choose option 5 (Gaussian elimination with pivoting):

```
Enter choice: 5

How do you want to input the system Ax = b?
3. Load A from left file and b from right file

Enter matrix (left) file: 49/49l.txt
Enter RHS (right) file: 49/49r.txt
Loaded 49x49 system
```

### Step 2: The program solves it

Behind the scenes, Gaussian elimination does:

1. **Forward elimination** — converts the matrix to upper triangular form (zeros below the diagonal)
2. **Pivoting** — swaps rows to put the largest value on the diagonal at each step (prevents dividing by tiny numbers)
3. **Back substitution** — solves from the bottom equation up, plugging answers back in

### Step 3: Read the solution

```
--- Solution ---
x[0] = ...
x[1] = ...
x[2] = ...
...
x[48] = ...
```

Each `x[i]` is the value at one grid point. For the 7×7 grid, you can picture it as:

```
Row 0: x[0]  x[1]  x[2]  x[3]  x[4]  x[5]  x[6]
Row 1: x[7]  x[8]  x[9]  x[10] x[11] x[12] x[13]
Row 2: x[14] x[15] x[16] x[17] x[18] x[19] x[20]
Row 3: x[21] x[22] x[23] x[24] x[25] x[26] x[27]
Row 4: x[28] x[29] x[30] x[31] x[32] x[33] x[34]
Row 5: x[35] x[36] x[37] x[38] x[39] x[40] x[41]
Row 6: x[42] x[43] x[44] x[45] x[46] x[47] x[48]
```

If this is a plate bending problem, these values are the **deflection** at each point. If it's a heat problem, they're the **temperature** at each point.

### Step 4: Save to file

```
Save solution to file? (1=yes, 0=no): 1
Enter output filename: solution_49.txt
Solution written to solution_49.txt
```

---

## How to Verify Your Answer

After solving, you should check: does `A × x` actually equal `b`?

The **residual** is the difference: `r = b - Ax`. If your solution is correct, every entry of r should be nearly zero (maybe ~1e-10 due to floating point rounding).

You can check this by hand for small systems, or write a quick loop:

```cpp
for (int i = 0; i < n; i++) {
    double Ax_i = 0;
    for (int j = 0; j < n; j++)
        Ax_i += A[i][j] * x[j];
    cout << "residual[" << i << "] = " << fabs(b[i] - Ax_i) << endl;
}
```

If the residuals are small (< 1e-6), your answer is good. If they're large, something went wrong.

---

## Common Mistakes and How to Avoid Them

### 1. Wrong working directory
**Problem:** `can't open file '49/49l.txt'`
**Fix:** Make sure you run the program from the `matrix_class/` folder:
```bash
cd matrix_class
./matrix_program
```

### 2. Confusing the header line with data
**Problem:** The first line of `49l.txt` is `49 50` — if your code reads this as the first row of data, everything shifts by one row and the solution is garbage.
**Fix:** Our `Input.cpp` handles this automatically. If you write your own reader, check if the first line has only 2 numbers — if so, it's a header.

### 3. Swapping left and right files
**Problem:** You accidentally give the RHS file as the matrix or vice versa.
**Fix:** The left/matrix file is always the BIGGER file. `49l.txt` is 38 KB. `49r.txt` is only 763 bytes.

### 4. Not using pivoting
**Problem:** You use option 6 (without pivoting) and get `nan` or `inf` in the solution.
**Why:** Without pivoting, if a zero (or near-zero) value lands on the diagonal, the program divides by it → infinity.
**Fix:** Always use option 5 (with pivoting) unless your professor specifically asks for without.

### 5. Using float instead of double
**Problem:** You changed the code to use `float` and the solution is inaccurate.
**Why:** `float` has only ~7 digits of precision. These matrices have values like `-3.8004877e+002` (8 significant digits). With `float`, you lose the last digit at every step, and after 49 steps of elimination, the error accumulates.
**Fix:** Always use `double` (15-16 digits of precision).

### 6. Index confusion (0-based vs 1-based)
**Problem:** Your textbook says "solve for x₁ through x₄₉" but the program prints `x[0]` through `x[48]`.
**Why:** C++ arrays start at 0.
**Fix:** Just remember: `x[0]` in the program = x₁ in the textbook.

### 7. Trying to print the whole 225×225 matrix
**Problem:** You call `display()` on a 225×225 matrix and the terminal becomes unreadable.
**Fix:** Only display small matrices (< 10×10). For large systems, just print the solution vector, or save to a file.

### 8. Mismatched dimensions
**Problem:** You load a 49×49 matrix but a b vector with 50 entries (or 48).
**Fix:** The program checks this and throws an error. But if you're creating your own files, always make sure `rows of A == length of b`.

---

## Quick Reference

| Detail | 49×49 System | 225×225 System |
|--------|-------------|---------------|
| Grid size | 7 × 7 | 15 × 15 |
| Number of unknowns | 49 | 225 |
| Matrix file | `49/49l.txt` | `225/225left.txt` |
| RHS file | `49/49r.txt` | `225/225right.txt` |
| Header in matrix file | `49 50` | `225 226` |
| Nonzeros per row | ~14 | ~30 |
| Matrix file size | ~38 KB | ~810 KB |
| RHS file size | ~763 B | ~3.5 KB |
| Recommended solver | Option 5 (pivoting) | Option 5 (pivoting) |
