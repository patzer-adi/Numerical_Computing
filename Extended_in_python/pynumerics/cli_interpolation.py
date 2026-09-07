"""
CLI helpers for PyNumerics interpolation and curve fitting module.

Provides an interactive menu for:
- Data input (console, file, built-in functions)
- Interpolation method selection
- Point evaluation
- Plotting
"""

from __future__ import annotations

from pynumerics.interpolation.lagrange import Lagrange
from pynumerics.interpolation.newton_divided_difference import NewtonDividedDifference
from pynumerics.interpolation.least_square_line import LeastSquareLine
from pynumerics.interpolation.least_square_parabola import LeastSquareParabola
from pynumerics.interpolation.test_functions import FUNCTIONS, generate_data
from pynumerics.exceptions import MatrixException


def _read_data_console() -> tuple[list[float], list[float]] | None:
    """Read x and y data points from console."""
    try:
        print("\n  Enter x values (space-separated):")
        x_input = input("  x: ").strip().split()
        print("  Enter y values (space-separated):")
        y_input = input("  y: ").strip().split()

        x_data = [float(v) for v in x_input]
        y_data = [float(v) for v in y_input]

        if len(x_data) != len(y_data):
            print(f"  ⚠ x has {len(x_data)} values, y has {len(y_data)} — must match.")
            return None

        if len(x_data) < 2:
            print("  ⚠ Need at least 2 data points.")
            return None

        print(f"\n  ✓ Loaded {len(x_data)} data points from console.")
        return x_data, y_data

    except ValueError:
        print("  ⚠ Invalid numeric input.")
        return None


def _read_data_file() -> tuple[list[float], list[float]] | None:
    """Read x and y data from a file.

    Supports formats:
      - Two rows: first row = x values, second row = y values
      - Two columns: each line has 'x y' or 'x,y'
    """
    filename = input("  Enter filename: ").strip()
    try:
        with open(filename, 'r') as f:
            lines = [line.strip() for line in f if line.strip() and not line.startswith('#')]

        if len(lines) < 2:
            print("  ⚠ File must have at least 2 lines of data.")
            return None

        # try two-row format first: row 1 = x, row 2 = y
        first_line_vals = lines[0].replace(',', ' ').split()
        second_line_vals = lines[1].replace(',', ' ').split()

        if len(first_line_vals) > 2 and len(first_line_vals) == len(second_line_vals):
            # two-row format
            x_data = [float(v) for v in first_line_vals]
            y_data = [float(v) for v in second_line_vals]
        else:
            # two-column format: each line has x y
            x_data = []
            y_data = []
            for line in lines:
                parts = line.replace(',', ' ').split()
                if len(parts) >= 2:
                    x_data.append(float(parts[0]))
                    y_data.append(float(parts[1]))

        if len(x_data) != len(y_data) or len(x_data) < 2:
            print("  ⚠ Could not parse valid data from file.")
            return None

        print(f"\n  ✓ Loaded {len(x_data)} data points from '{filename}'.")
        return x_data, y_data

    except FileNotFoundError:
        print(f"  ⚠ File '{filename}' not found.")
        return None
    except ValueError:
        print("  ⚠ File contains non-numeric data.")
        return None


def _generate_from_function() -> tuple[list[float], list[float], str | None]:
    """Generate data from a built-in function.

    Returns:
        (x_data, y_data, func_name) or (None, None, None) on error.
    """
    print("\n  Available functions:")
    func_names = list(FUNCTIONS.keys())
    for i, name in enumerate(func_names, 1):
        print(f"    {i}. {name}")

    try:
        choice = int(input("\n  Select function (number): ")) - 1
        if choice < 0 or choice >= len(func_names):
            print("  ⚠ Invalid selection.")
            return None, None, None

        func_name = func_names[choice]
        func_callable, default_start, default_end = FUNCTIONS[func_name]

        start_str = input(f"  Start x [{default_start}]: ").strip()
        end_str = input(f"  End x [{default_end}]: ").strip()
        n_str = input("  Number of points [5]: ").strip()

        x_start = float(start_str) if start_str else default_start
        x_end = float(end_str) if end_str else default_end
        n_points = int(n_str) if n_str else 5

        x_data, y_data = generate_data(func_callable, x_start, x_end, n_points)

        print(f"\n  ✓ Generated {n_points} points from {func_name} "
              f"over [{x_start}, {x_end}].")
        return x_data, y_data, func_name

    except (ValueError, IndexError):
        print("  ⚠ Invalid input.")
        return None, None, None


def run_interpolation_cli() -> None:
    """Run the interactive interpolation & curve fitting calculator."""
    print("\n" + "=" * 44)
    print("  PyNumerics — Interpolation & Curve Fitting")
    print("=" * 44)

    x_data: list[float] | None = None
    y_data: list[float] | None = None
    current_interp = None
    source_func_name: str | None = None  # track if data came from a built-in function

    while True:
        print("\n╔══════════════════════════════════════════╗")
        print("║       Interpolation & Curve Fitting      ║")
        print("╠══════════════════════════════════════════╣")
        print("║  Data Input:                             ║")
        print("║  1. Enter data points (Console)          ║")
        print("║  2. Load data from file                  ║")
        print("║  3. Generate from built-in function      ║")
        print("║                                          ║")
        print("║  Interpolation:                          ║")
        print("║  4. Lagrange Interpolation               ║")
        print("║  5. Newton Divided Difference            ║")
        print("║  6. Least Squares Line Fit               ║")
        print("║  7. Least Squares Parabola Fit           ║")
        print("║                                          ║")
        print("║  Evaluate & Plot:                        ║")
        print("║  8. Evaluate at a point                  ║")
        print("║  9. Plot current result                  ║")
        print("║                                          ║")
        print("║  0. Return to Main Menu                  ║")
        print("╚══════════════════════════════════════════╝")

        try:
            choice = input("\n  Enter choice (0-9): ").strip()
        except (EOFError, KeyboardInterrupt):
            break

        if choice == "1":
            result = _read_data_console()
            if result is not None:
                x_data, y_data = result
                current_interp = None
                source_func_name = None

        elif choice == "2":
            result = _read_data_file()
            if result is not None:
                x_data, y_data = result
                current_interp = None
                source_func_name = None

        elif choice == "3":
            xd, yd, fname = _generate_from_function()
            if xd is not None:
                x_data, y_data = xd, yd
                current_interp = None
                source_func_name = fname

        elif choice in ["4", "5", "6", "7"]:
            if x_data is None or y_data is None:
                print("  ⚠ Please load data first (options 1-3).")
                continue

            try:
                if choice == "4":
                    current_interp = Lagrange(x_data, y_data)
                    print("  ✓ Lagrange interpolation ready.")

                elif choice == "5":
                    current_interp = NewtonDividedDifference(x_data, y_data)
                    current_interp.print_table()
                    print("  ✓ Newton Divided Difference interpolation ready.")

                elif choice == "6":
                    current_interp = LeastSquareLine(x_data, y_data)
                    current_interp.fit()
                    result = current_interp.get_fit_result()
                    print(f"\n  Best fit line: y = {current_interp.a:.6f} + {current_interp.b:.6f}x")
                    print(f"  SSE = {result.sse:.6f}  MSE = {result.mse:.6f}  RMSE = {result.rmse:.6f}")

                elif choice == "7":
                    current_interp = LeastSquareParabola(x_data, y_data)
                    current_interp.fit()
                    result = current_interp.get_fit_result()
                    print(f"\n  Best fit parabola: y = {current_interp.a:.6f} "
                          f"+ {current_interp.b:.6f}x + {current_interp.c:.6f}x²")
                    print(f"  SSE = {result.sse:.6f}  MSE = {result.mse:.6f}  RMSE = {result.rmse:.6f}")

            except MatrixException as e:
                print(f"  ⚠ Error: {e}")

        elif choice == "8":
            if current_interp is None:
                print("  ⚠ Please select an interpolation method first (options 4-7).")
                continue

            try:
                query_x = float(input("  Enter x value to evaluate: "))
                result = current_interp.evaluate(query_x)
                print(f"\n  P({query_x:.6f}) = {result:.6f}")
            except ValueError:
                print("  ⚠ Invalid numeric input.")
            except MatrixException as e:
                print(f"  ⚠ Error: {e}")

        elif choice == "9":
            if current_interp is None:
                print("  ⚠ Please select an interpolation method first (options 4-7).")
                continue

            try:
                # if data came from a built-in function, offer comparison overlay
                func = None
                func_label = None
                if source_func_name is not None and source_func_name in FUNCTIONS:
                    show_actual = input(
                        f"  Overlay actual {source_func_name}? (y/n) [y]: "
                    ).strip().lower()
                    if show_actual != "n":
                        func = FUNCTIONS[source_func_name][0]
                        func_label = f"Actual: {source_func_name}"

                save_str = input("  Save to file? Enter path or press Enter to skip: ").strip()
                save_path = save_str if save_str else None

                current_interp.plot(func=func, func_label=func_label, save_path=save_path)

            except Exception as e:
                print(f"  ⚠ Plotting error: {e}")

        elif choice == "0":
            break
        else:
            print("  ⚠ Invalid choice.")
