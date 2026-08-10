import sys
from pynumerics.matrix import Matrix
from pynumerics.solvers import GaussianElimination, Doolittle, Crout, Cholesky, GaussJacobi, GaussSeidel
from pynumerics.exceptions import MatrixException

def run_solvers_cli() -> None:
    print("\n" + "=" * 44)
    print("  PyNumerics — Linear System Solvers")
    print("=" * 44)

    A = None
    b = None

    while True:
        print("\n╔══════════════════════════════════════════╗")
        print("║          Linear Solvers Menu             ║")
        print("╠══════════════════════════════════════════╣")
        print("║  1. Enter Matrix A & Vector b (Console)  ║")
        print("║  2. Load Matrix A & Vector b (File)      ║")
        print("║  3. Solve using Gaussian Elimination     ║")
        print("║  4. Solve using LU (Doolittle)           ║")
        print("║  5. Solve using LU (Crout)               ║")
        print("║  6. Solve using LU (Cholesky)            ║")
        print("║  7. Solve using Gauss-Jacobi             ║")
        print("║  8. Solve using Gauss-Seidel             ║")
        print("║  9. Return to Main Menu                  ║")
        print("╚══════════════════════════════════════════╝")

        try:
            choice = input("\n  Enter choice (1-9): ").strip()
        except (EOFError, KeyboardInterrupt):
            break

        if choice == "1":
            try:
                n = int(input("  Enter system size N: "))
                A = Matrix(n, n)
                A.read_from_console()
                print(f"  Enter vector b ({n} elements):")
                b_input = input("  b: ").split()
                if len(b_input) != n:
                    print(f"  ⚠ Expected {n} values.")
                    continue
                b = [float(x) for x in b_input]
            except ValueError:
                print("  ⚠ Invalid numeric input.")

        elif choice == "2":
            file_A = input("  Enter filename for Matrix A: ").strip()
            file_b = input("  Enter filename for Vector b: ").strip()
            try:
                A = Matrix.from_file(file_A)
                b_mat = Matrix.from_file(file_b)
                if b_mat.cols == 1:
                    b = [b_mat[i, 0] for i in range(b_mat.rows)]
                elif b_mat.rows == 1:
                    b = [b_mat[0, j] for j in range(b_mat.cols)]
                else:
                    print("  ⚠ Vector b file must be 1D (1 column or 1 row).")
                    b = None
                if b is not None and len(b) != A.rows:
                    print("  ⚠ Dimension mismatch between A and b.")
                    b = None
            except MatrixException as e:
                print(f"  ⚠ Error loading files: {e}")

        elif choice in ["3", "4", "5", "6", "7", "8"]:
            if A is None or b is None:
                print("  ⚠ Please input A and b first.")
                continue

            try:
                if choice == "3":
                    solver = GaussianElimination(A)
                    res = solver.solve(b)
                elif choice == "4":
                    solver = Doolittle(A)
                    solver.decompose()
                    res = solver.solve(b)
                elif choice == "5":
                    solver = Crout(A)
                    solver.decompose()
                    res = solver.solve(b)
                elif choice == "6":
                    solver = Cholesky(A)
                    solver.decompose()
                    res = solver.solve(b)
                elif choice == "7":
                    solver = GaussJacobi(A)
                    res = solver.solve(b)
                elif choice == "8":
                    solver = GaussSeidel(A)
                    res = solver.solve(b)

                print("\n  ✅ Solution found:")
                for i, val in enumerate(res.x):
                    print(f"    x_{i} = {val:.6f}")
                
            except MatrixException as e:
                print(f"  ⚠ Solver Error: {e}")

        elif choice == "9":
            break
        else:
            print("  ⚠ Invalid choice.")

