"""
CLI helpers for PyNumerics numerical integration module.

Provides an interactive menu for:
- Function selection (built-in functions)
- Integration bound and sub-interval setup
- Method selection (Trapezoidal, Simpson's 1/3, Simpson's 3/8)
- Result table display
- Function, approximation, and convergence plotting
"""

from __future__ import annotations

from pynumerics.integration.base import Integration, BUILTIN_FUNCTIONS
from pynumerics.integration.trapezoidal import TrapezoidalRule
from pynumerics.integration.simpsons13 import Simpsons13
from pynumerics.integration.simpsons38 import Simpsons38


def _select_functions(method: Integration) -> None:
    """Interactive function selection menu."""
    print("\n  Available functions:")
    for i, (name, _, _) in enumerate(BUILTIN_FUNCTIONS, 1):
        print(f"    {i}. {name}")
    print(f"    {len(BUILTIN_FUNCTIONS) + 1}. All of the above")

    try:
        choice = input("\n  Select function(s) (number, or comma-separated): ").strip()

        if choice == str(len(BUILTIN_FUNCTIONS) + 1):
            method.add_builtin_functions()
            print(f"  ✓ Registered all {len(BUILTIN_FUNCTIONS)} built-in functions.")
            return

        indices = [int(c.strip()) - 1 for c in choice.split(",")]
        for idx in indices:
            if 0 <= idx < len(BUILTIN_FUNCTIONS):
                name, f, F = BUILTIN_FUNCTIONS[idx]
                method.add_function(name, f, F)
                print(f"  ✓ Registered {name}")
            else:
                print(f"  ⚠ Invalid index: {idx + 1}")

    except ValueError:
        print("  ⚠ Invalid input.")


def _set_bounds() -> tuple[float, float] | None:
    """Read integration bounds from console."""
    try:
        a = float(input("  Enter lower bound a: "))
        b = float(input("  Enter upper bound b: "))
        if a >= b:
            print("  ⚠ a must be less than b.")
            return None
        print(f"  ✓ Integration bounds: [{a}, {b}]")
        return a, b
    except ValueError:
        print("  ⚠ Invalid numeric input.")
        return None


def _set_sub_intervals(method: Integration) -> None:
    """Read sub-interval counts from console."""
    try:
        raw = input("  Enter sub-interval counts (space-separated, e.g. 6 12 24 48): ").strip()
        n_values = [int(v) for v in raw.split()]
        if not n_values:
            print("  ⚠ No values entered.")
            return
        method.set_sub_intervals(n_values)
        print(f"  ✓ Sub-interval counts: {n_values}")
    except ValueError:
        print("  ⚠ Invalid numeric input.")


def _select_plot_function(method: Integration) -> int | None:
    """Let the user choose which registered function to plot."""
    if method.num_functions == 0:
        print("  ⚠ No functions registered yet.")
        return None
    if method.num_functions == 1:
        return 0

    print("\n  Registered functions:")
    for i in range(method.num_functions):
        print(f"    {i + 1}. {method.get_function(i).name}")
    try:
        idx = int(input("  Select function (number): ")) - 1
        if 0 <= idx < method.num_functions:
            return idx
        print("  ⚠ Invalid selection.")
        return None
    except ValueError:
        print("  ⚠ Invalid input.")
        return None


def run_integration_cli() -> None:
    """Run the interactive numerical integration calculator."""
    print("\n" + "=" * 44)
    print("  PyNumerics — Numerical Integration")
    print("=" * 44)

    method: Integration | None = None
    a: float | None = None
    b: float | None = None

    while True:
        print("\n╔══════════════════════════════════════════╗")
        print("║        Numerical Integration             ║")
        print("╠══════════════════════════════════════════╣")
        print("║  Setup:                                  ║")
        print("║  1. Select function(s)                   ║")
        print("║  2. Set integration bounds [a, b]        ║")
        print("║  3. Set sub-interval counts              ║")
        print("║                                          ║")
        print("║  Method:                                 ║")
        print("║  4. Trapezoidal Rule                     ║")
        print("║  5. Simpson's 1/3 Rule                   ║")
        print("║  6. Simpson's 3/8 Rule                   ║")
        print("║                                          ║")
        print("║  Output:                                 ║")
        print("║  7. Display result table                 ║")
        print("║  8. Save results to file                 ║")
        print("║  9. Plot function                        ║")
        print("║ 10. Plot approximation                   ║")
        print("║ 11. Plot convergence                     ║")
        print("║                                          ║")
        print("║  0. Return to Main Menu                  ║")
        print("╚══════════════════════════════════════════╝")

        try:
            choice = input("\n  Enter choice (0-11): ").strip()
        except (EOFError, KeyboardInterrupt):
            break

        # ── 1. Select functions ──────────────────────────────────
        if choice == "1":
            if method is None:
                print("  (No method selected yet — functions will carry over when you choose one.)")
                # create a temporary to hold the functions, transfer later
                temp = TrapezoidalRule()
                _select_functions(temp)
                # store for later transfer
                method = temp
            else:
                _select_functions(method)

        # ── 2. Set bounds ────────────────────────────────────────
        elif choice == "2":
            result = _set_bounds()
            if result is not None:
                a, b = result

        # ── 3. Set sub-intervals ─────────────────────────────────
        elif choice == "3":
            if method is None:
                print("  ⚠ Select functions first (option 1), then set sub-intervals.")
                continue
            _set_sub_intervals(method)

        # ── 4/5/6. Select method and compute ─────────────────────
        elif choice in ["4", "5", "6"]:
            if a is None or b is None:
                print("  ⚠ Set integration bounds first (option 2).")
                continue

            # transfer existing registrations if switching methods
            old_functions = method._functions if method is not None else []
            old_n_values = method._n_values if method is not None else []

            if choice == "4":
                method = TrapezoidalRule()
            elif choice == "5":
                method = Simpsons13()
            elif choice == "6":
                method = Simpsons38()

            # restore registrations
            for entry in old_functions:
                method.add_function(entry.name, entry.f, entry.F)
            if old_n_values:
                method.set_sub_intervals(old_n_values)

            if method.num_functions == 0:
                print("  ⚠ No functions registered yet. Use option 1 first.")
                continue
            if method.num_n == 0:
                print("  ⚠ No sub-interval counts set. Use option 3 first.")
                continue

            try:
                method.compute_all(a, b)
                print(f"\n  ✅ {method.get_method_name()} — computed for "
                      f"{method.num_functions} functions × {method.num_n} sub-interval counts.")
            except ValueError as e:
                print(f"  ⚠ Error: {e}")

        # ── 7. Display results ───────────────────────────────────
        elif choice == "7":
            if method is None or not method.results:
                print("  ⚠ No results to display. Compute first (options 4-6).")
                continue
            method.display()

        # ── 8. Save results ──────────────────────────────────────
        elif choice == "8":
            if method is None or not method.results:
                print("  ⚠ No results to save. Compute first (options 4-6).")
                continue
            filename = input("  Enter filename: ").strip()
            if filename:
                try:
                    method.save_results(filename)
                except ValueError as e:
                    print(f"  ⚠ Error: {e}")

        # ── 9. Plot function ─────────────────────────────────────
        elif choice == "9":
            if method is None or method.num_functions == 0:
                print("  ⚠ No functions registered. Use option 1 first.")
                continue
            if a is None or b is None:
                print("  ⚠ Set bounds first (option 2).")
                continue

            idx = _select_plot_function(method)
            if idx is not None:
                try:
                    n_str = input("  Show grid points? Enter n or press Enter to skip: ").strip()
                    n_plot = int(n_str) if n_str else None
                    save_str = input("  Save to file? Enter path or press Enter to skip: ").strip()
                    save_path = save_str if save_str else None
                    method.plot_function(idx, a, b, n=n_plot, save_path=save_path)
                except Exception as e:
                    print(f"  ⚠ Error: {e}")

        # ── 10. Plot approximation ───────────────────────────────
        elif choice == "10":
            if method is None or method.num_functions == 0:
                print("  ⚠ No functions registered. Use option 1 first.")
                continue
            if a is None or b is None:
                print("  ⚠ Set bounds first (option 2).")
                continue

            idx = _select_plot_function(method)
            if idx is not None:
                try:
                    n_str = input("  Enter n (sub-intervals): ").strip()
                    n_val = int(n_str)
                    save_str = input("  Save to file? Enter path or press Enter to skip: ").strip()
                    save_path = save_str if save_str else None
                    method.plot_approximation(idx, a, b, n_val, save_path=save_path)
                except ValueError:
                    print("  ⚠ Invalid n value.")
                except Exception as e:
                    print(f"  ⚠ Error: {e}")

        # ── 11. Plot convergence ─────────────────────────────────
        elif choice == "11":
            if method is None or not method.results:
                print("  ⚠ Compute results first (options 4-6).")
                continue
            if a is None or b is None:
                print("  ⚠ Set bounds first.")
                continue

            idx = _select_plot_function(method)
            if idx is not None:
                try:
                    save_str = input("  Save to file? Enter path or press Enter to skip: ").strip()
                    save_path = save_str if save_str else None
                    method.plot_convergence(idx, a, b, save_path=save_path)
                except Exception as e:
                    print(f"  ⚠ Error: {e}")

        # ── 0. Exit ──────────────────────────────────────────────
        elif choice == "0":
            break
        else:
            print("  ⚠ Invalid choice.")
