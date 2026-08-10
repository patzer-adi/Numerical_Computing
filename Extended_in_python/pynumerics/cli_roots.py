"""
CLI helpers for PyNumerics root finding module.
"""

from __future__ import annotations
import math

from pynumerics.roots import Bisection, NewtonRaphson, FixedPoint

def run_roots_cli() -> None:
    """Run the interactive root finding calculator."""
    print("\n" + "=" * 44)
    print("  PyNumerics — Root Finding Methods")
    print("=" * 44)

    # Some sample functions
    def f1(x: float) -> float: return x**2 - 4.0
    def df1(x: float) -> float: return 2.0 * x
    def g1(x: float) -> float: return 4.0 / x if x != 0 else float('inf')

    def f2(x: float) -> float: return math.cos(x) - x
    def df2(x: float) -> float: return -math.sin(x) - 1.0
    def g2(x: float) -> float: return math.cos(x)

    func_choice = 1

    while True:
        print("\n╔══════════════════════════════════════════╗")
        print("║          Root Finding Menu               ║")
        print("╠══════════════════════════════════════════╣")
        print("║  Select Function:                        ║")
        print("║  1. f(x) = x² - 4                        ║")
        print("║  2. f(x) = cos(x) - x                    ║")
        print("║                                          ║")
        print("║  Solve using:                            ║")
        print("║  3. Bisection Method                     ║")
        print("║  4. Newton-Raphson Method                ║")
        print("║  5. Fixed-Point Iteration                ║")
        print("║                                          ║")
        print("║  9. Return to Main Menu                  ║")
        print("╚══════════════════════════════════════════╝")

        try:
            choice = input("\n  Enter choice (1-9): ").strip()
        except (EOFError, KeyboardInterrupt):
            break

        if choice in ["1", "2"]:
            func_choice = int(choice)
            print(f"  ✓ Selected function {func_choice}")

        elif choice == "3":
            try:
                a = float(input("  Enter interval start (a): "))
                b = float(input("  Enter interval end (b): "))
                solver = Bisection(f1 if func_choice == 1 else f2)
                res = solver.solve(a, b)
                print(f"\n  ✓ Bisection Result:")
                print(f"    Root = {res.root:.6f} (Iterations: {res.iterations}, Error: {res.error:.2e})")
            except Exception as e:
                print(f"  ⚠ Error: {e}")

        elif choice == "4":
            try:
                x0 = float(input("  Enter initial guess (x0): "))
                f = f1 if func_choice == 1 else f2
                df = df1 if func_choice == 1 else df2
                solver = NewtonRaphson(f, df)
                res = solver.solve(x0)
                print(f"\n  ✓ Newton-Raphson Result:")
                print(f"    Root = {res.root:.6f} (Iterations: {res.iterations}, Error: {res.error:.2e})")
            except Exception as e:
                print(f"  ⚠ Error: {e}")

        elif choice == "5":
            try:
                x0 = float(input("  Enter initial guess (x0): "))
                g = g1 if func_choice == 1 else g2
                solver = FixedPoint(g)
                res = solver.solve(x0)
                if res.converged:
                    print(f"\n  ✓ Fixed-Point Result:")
                    print(f"    Root = {res.root:.6f} (Iterations: {res.iterations}, Error: {res.error:.2e})")
                else:
                    print(f"\n  ⚠ Failed to converge. Last x: {res.root:.6f}")
            except Exception as e:
                print(f"  ⚠ Error: {e}")

        elif choice == "9":
            break
        else:
            print("  ⚠ Invalid choice.")
