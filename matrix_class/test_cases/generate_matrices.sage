#!/usr/bin/env sage
"""
Test Matrix Generator for the Matrix Operations Program
========================================================
Generates random matrices and systems of equations (Ax = b)
in the format expected by our C++ program.

Usage:
    sage generate_matrices.sage

Or run interactively:
    sage
    sage: load("generate_matrices.sage")
"""

import os
import sys

def generate_random_matrix(n, m=None, value_range=(-100, 100)):
    """Generate a random n×m matrix with entries in value_range."""
    if m is None:
        m = n
    low, high = value_range
    A = matrix(RDF, n, m, [RDF.random_element(low, high) for _ in range(n * m)])
    return A


def generate_diagonally_dominant(n, value_range=(-10, 10)):
    """
    Generate an n×n diagonally dominant matrix.
    These are guaranteed to be non-singular and work well
    with Gaussian elimination even without pivoting.
    """
    low, high = value_range
    A = matrix(RDF, n, n, [RDF.random_element(low, high) for _ in range(n * n)])

    # make it diagonally dominant: |A[i][i]| > sum of |A[i][j]| for j ≠ i
    for i in range(n):
        row_sum = sum(abs(A[i, j]) for j in range(n) if j != i)
        # make diagonal at least row_sum + random extra
        sign = 1 if RDF.random_element() > 0 else -1
        A[i, i] = sign * (row_sum + RDF.random_element(1, 10))

    return A


def generate_symmetric_positive_definite(n):
    """
    Generate an n×n symmetric positive definite matrix.
    This is needed for Cholesky decomposition.
    Method: A = M^T * M + n*I (guarantees positive definiteness).
    """
    M = matrix(RDF, n, n, [RDF.random_element(-10, 10) for _ in range(n * n)])
    A = M.transpose() * M + n * identity_matrix(RDF, n)
    return A


def generate_rhs_vector(n, value_range=(-100, 100)):
    """Generate a random right-hand side vector b of length n."""
    low, high = value_range
    b = vector(RDF, [RDF.random_element(low, high) for _ in range(n)])
    return b


def save_matrix(A, filename, include_header=True):
    """
    Save matrix A to a file.
    If include_header=True, writes "rows cols" as the first line.
    """
    n, m = A.nrows(), A.ncols()
    with open(filename, 'w') as f:
        if include_header:
            f.write(f"{n} {m}\n")
        for i in range(n):
            row_str = "  ".join(f"{A[i,j]:.7e}" for j in range(m))
            f.write(row_str + "\n")
    print(f"  Saved matrix ({n}x{m}) to {filename}")


def save_vector(b, filename):
    """Save vector b to a file, one value per line."""
    with open(filename, 'w') as f:
        for val in b:
            f.write(f"{val:.7e}\n")
    print(f"  Saved vector (length {len(b)}) to {filename}")


def save_augmented(A, b, filename, include_header=True):
    """
    Save matrix A and vector b as augmented matrix [A|b].
    This is one file with n rows and n+1 columns.
    """
    n = A.nrows()
    m = A.ncols() + 1  # extra column for b
    with open(filename, 'w') as f:
        if include_header:
            f.write(f"{n} {m}\n")
        for i in range(n):
            row_str = "  ".join(f"{A[i,j]:.7e}" for j in range(A.ncols()))
            row_str += f"  {b[i]:.7e}"
            f.write(row_str + "\n")
    print(f"  Saved augmented matrix ({n}x{m}) to {filename}")


def generate_test_system(n, name, folder=".", matrix_type="random", augmented=False):
    """
    Generate a complete test system and save files.

    Args:
        n: size of the system (n equations, n unknowns)
        name: base name for files (e.g., "test1")
        folder: output directory
        matrix_type: "random", "diagonal_dominant", or "spd"
        augmented: if True, save as one [A|b] file; if False, save as separate A and b files
    """
    os.makedirs(folder, exist_ok=True)

    print(f"\nGenerating {n}x{n} {matrix_type} system '{name}'...")

    # generate matrix A
    if matrix_type == "diagonal_dominant":
        A = generate_diagonally_dominant(n)
    elif matrix_type == "spd":
        A = generate_symmetric_positive_definite(n)
    else:
        A = generate_random_matrix(n)

    # generate RHS vector b
    b = generate_rhs_vector(n)

    if augmented:
        # single file: [A|b]
        save_augmented(A, b, os.path.join(folder, f"{name}_augmented.txt"))
    else:
        # two files: A (left) and b (right)
        save_matrix(A, os.path.join(folder, f"{name}_left.txt"))
        save_vector(b, os.path.join(folder, f"{name}_right.txt"))

    # also compute the true solution for reference
    try:
        x = A.solve_right(b)
        save_vector(x, os.path.join(folder, f"{name}_true_solution.txt"))
        print(f"  True solution computed and saved (use this to check your answer)")
    except Exception as e:
        print(f"  Warning: could not compute true solution ({e})")

    print(f"  Done!")


def interactive_mode():
    """Run the generator interactively, asking the user what they want."""

    print("=" * 50)
    print("  Matrix Test Case Generator")
    print("=" * 50)
    print()

    while True:
        print("What do you want to generate?")
        print("1. Small test (3x3 to 10x10)")
        print("2. Medium test (20x20 to 50x50)")
        print("3. Large test (100x100+)")
        print("4. Custom size")
        print("5. Generate a batch of all sizes")
        print("6. Quit")
        print()

        choice = input("Enter choice: ").strip()

        if choice == "1":
            n = int(input("Exact size (3-10): ").strip())
        elif choice == "2":
            n = int(input("Exact size (20-50): ").strip())
        elif choice == "3":
            n = int(input("Exact size (100+): ").strip())
        elif choice == "4":
            n = int(input("Enter n: ").strip())
        elif choice == "5":
            generate_batch()
            continue
        elif choice == "6":
            print("Bye!")
            return
        else:
            print("Invalid choice")
            continue

        print()
        print("Matrix type?")
        print("1. Random (may be singular — good for testing error handling)")
        print("2. Diagonally dominant (guaranteed non-singular — always solvable)")
        print("3. Symmetric positive definite (needed for Cholesky)")
        mt_choice = input("Enter choice: ").strip()
        mt = {"1": "random", "2": "diagonal_dominant", "3": "spd"}.get(mt_choice, "random")

        print()
        print("File format?")
        print("1. Separate files: left (A) and right (b)")
        print("2. Single augmented file [A|b]")
        print("3. Both")
        fmt_choice = input("Enter choice: ").strip()

        name = input("Test name (e.g., 'test1'): ").strip() or f"test_{n}x{n}"

        if fmt_choice == "1":
            generate_test_system(n, name, folder=".", matrix_type=mt, augmented=False)
        elif fmt_choice == "2":
            generate_test_system(n, name, folder=".", matrix_type=mt, augmented=True)
        else:
            generate_test_system(n, name + "_sep", folder=".", matrix_type=mt, augmented=False)
            generate_test_system(n, name + "_aug", folder=".", matrix_type=mt, augmented=True)

        print()


def generate_batch():
    """Generate a standard set of test cases at various sizes."""

    print("\nGenerating standard test batch...")

    sizes = [3, 5, 10, 25, 50]

    for n in sizes:
        # diagonally dominant, separate files (best for Gaussian elimination)
        generate_test_system(n, f"dd_{n}x{n}", folder=".",
                             matrix_type="diagonal_dominant", augmented=False)

        # SPD, separate files (needed for Cholesky)
        generate_test_system(n, f"spd_{n}x{n}", folder=".",
                             matrix_type="spd", augmented=False)

        # random, augmented (tests augmented file reading)
        generate_test_system(n, f"rand_{n}x{n}", folder=".",
                             matrix_type="random", augmented=True)

    print(f"\nGenerated {len(sizes) * 3} test cases!")
    print("Files saved in current directory")


# === MAIN ===
if __name__ == "__main__" or True:
    interactive_mode()
