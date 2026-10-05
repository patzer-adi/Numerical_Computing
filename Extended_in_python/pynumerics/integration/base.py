"""
Numerical Integration base class.

Inherits from Matrix — the inherited ``_data`` stores the results table,
matching the C++ design (``class Integration : public Matrix``).

Hierarchy (mirrors C++):
    Matrix
     └── Integration (abstract base)
           ├── TrapezoidalRule
           ├── Simpsons13
           └── Simpsons38
"""

from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Callable
import math

from pynumerics.matrix import Matrix


@dataclass
class IntegrationFunctionEntry:
    """A registered integrand together with its known exact integral."""
    name: str
    f: Callable[[float], float]
    F: Callable[[float, float], float]   # F(a, b) -> exact integral


@dataclass
class IntegrationResult:
    """One row of the integration results table."""
    function_name: str
    n: int
    h: float
    exact: float
    approx: float
    error: float
    relative_error: float


# ═══════════════════════════════════════════════
# Built-in functions (hardcoded, ship with the module)
# ═══════════════════════════════════════════════

BUILTIN_FUNCTIONS: list[tuple[str, Callable, Callable]] = [
    ("e^x",         lambda x: math.exp(x),        lambda a, b: math.exp(b) - math.exp(a)),
    ("sin(x)",      lambda x: math.sin(x),        lambda a, b: -math.cos(b) + math.cos(a)),
    ("x^2",         lambda x: x**2,               lambda a, b: (b**3 - a**3) / 3),
    ("x^3-2x+1",   lambda x: x**3 - 2*x + 1,     lambda a, b: (b**4/4 - b**2 + b) - (a**4/4 - a**2 + a)),
    ("e^(-x^2)",    lambda x: math.exp(-x**2),     lambda a, b: math.sqrt(math.pi)/2 * (math.erf(b) - math.erf(a))),
    ("1/(1+x^2)",   lambda x: 1/(1+x**2),          lambda a, b: math.atan(b) - math.atan(a)),
    ("ln(x)",       lambda x: math.log(x),         lambda a, b: (b*math.log(b) - b) - (a*math.log(a) - a)),
    ("1/x",         lambda x: 1/x,                 lambda a, b: math.log(b) - math.log(a)),
]


# Number of columns in the results table (stored in Matrix._data)
# Col 0: func_index  Col 1: n  Col 2: h  Col 3: exact
# Col 4: approx      Col 5: error        Col 6: relative_error
_NUM_COLS = 7


class Integration(Matrix, ABC):
    """Abstract base class for numerical integration methods.

    Inherits from Matrix — the inherited ``_data`` stores the results table,
    each row = one (function, n) pair.

    Follows the same pattern as the C++ ``Integration : public Matrix`` and
    the Python ``SystemOfLinearEquationSolver(Matrix, ABC)``.

    Derived classes override:
        integrate(f, a, b, n) — the quadrature formula
        get_method_name()     — human-readable method name
    """

    def __init__(self) -> None:
        super().__init__(0, 0)               # empty matrix initially
        self._functions: list[IntegrationFunctionEntry] = []
        self._n_values: list[int] = []
        self._results: list[IntegrationResult] = []
        self._a: float | None = None         # last-used lower bound
        self._b: float | None = None         # last-used upper bound

    # ── registration ──────────────────────────────────────────────

    def add_function(
        self,
        name: str,
        f: Callable[[float], float],
        F: Callable[[float, float], float],
    ) -> None:
        """Register an integrand with its known exact integral."""
        self._functions.append(IntegrationFunctionEntry(name=name, f=f, F=F))

    def add_builtin_functions(self) -> None:
        """Register all built-in functions."""
        for name, f, F in BUILTIN_FUNCTIONS:
            self.add_function(name, f, F)

    def set_sub_intervals(self, n_values: list[int]) -> None:
        """Set the sub-interval counts to evaluate at."""
        self._n_values = list(n_values)

    # ── pure-virtual interface ────────────────────────────────────

    @abstractmethod
    def integrate(
        self, f: Callable[[float], float], a: float, b: float, n: int
    ) -> float:
        """Compute the approximate integral of *f* over [a, b] with *n* sub-intervals."""
        pass

    @abstractmethod
    def get_method_name(self) -> str:
        """Return a human-readable name for this method."""
        pass

    # ── compute all ───────────────────────────────────────────────

    def compute_all(self, a: float, b: float) -> list[IntegrationResult]:
        """Evaluate every registered function at every sub-interval count over [a, b].

        Populates the inherited Matrix ``_data`` with the results table
        and returns a list of ``IntegrationResult`` objects.

        Matrix layout: (numFunctions × numN) rows × 7 columns
        Col 0: func_index  Col 1: n  Col 2: h  Col 3: exact
        Col 4: approx      Col 5: error        Col 6: relative_error

        Raises:
            ValueError: If no functions or sub-interval counts have been set,
                        or if a >= b.
        """
        if not self._functions:
            raise ValueError("no functions registered... add some first")
        if not self._n_values:
            raise ValueError("no sub-interval counts set... call set_sub_intervals() first")
        if a >= b:
            raise ValueError("invalid integration bounds: a must be less than b")

        self._a = a
        self._b = b

        total_rows = len(self._functions) * len(self._n_values)

        # Resize the inherited Matrix storage
        self.rows = total_rows
        self.cols = _NUM_COLS
        self._data = [[0.0] * _NUM_COLS for _ in range(total_rows)]

        # Also populate the convenience list
        self._results.clear()

        row = 0
        for fi, entry in enumerate(self._functions):
            exact = entry.F(a, b)
            for n in self._n_values:
                h = (b - a) / n
                approx = self.integrate(entry.f, a, b, n)
                error = abs(exact - approx)
                rel_error = error / abs(exact) if abs(exact) > 1e-15 else 0.0

                # Store in Matrix._data (mirrors C++ Integration::computeAll)
                self._data[row][0] = float(fi)
                self._data[row][1] = float(n)
                self._data[row][2] = h
                self._data[row][3] = exact
                self._data[row][4] = approx
                self._data[row][5] = error
                self._data[row][6] = rel_error

                self._results.append(
                    IntegrationResult(
                        function_name=entry.name,
                        n=n,
                        h=h,
                        exact=exact,
                        approx=approx,
                        error=error,
                        relative_error=rel_error,
                    )
                )
                row += 1

        return list(self._results)

    # ── display ───────────────────────────────────────────────────

    def display(self) -> None:
        """Print a formatted results table to stdout."""
        if not self._results:
            print("No results computed yet... call compute_all() first")
            return

        title = f"=== {self.get_method_name()} ==="
        print(f"\n{title}")
        header = (
            f"{'Function':<18}"
            f"{'n':>8}"
            f"{'h':>14}"
            f"{'Exact':>16}"
            f"{'Approx':>16}"
            f"{'Error':>16}"
            f"{'Rel Err':>12}"
        )
        print(header)
        print("-" * len(header))

        for r in self._results:
            print(
                f"{r.function_name:<18}"
                f"{r.n:>8d}"
                f"{r.h:>14.6e}"
                f"{r.exact:>16.6e}"
                f"{r.approx:>16.6e}"
                f"{r.error:>16.6e}"
                f"{r.relative_error:>11.4%}"
            )
        print()

    # ── save to file ──────────────────────────────────────────────

    def save_results(self, filename: str) -> None:
        """Write results to *filename* in scientific notation."""
        if not self._results:
            raise ValueError("no results to save... call compute_all() first")

        with open(filename, "w") as fout:
            fout.write(f"# {self.get_method_name()}\n")
            fout.write(
                f"{'Function':<18}"
                f"{'n':>8}"
                f"{'h':>14}"
                f"{'Exact':>16}"
                f"{'Approx':>16}"
                f"{'Error':>16}"
                f"{'Rel Err':>12}\n"
            )
            for r in self._results:
                fout.write(
                    f"{r.function_name:<18}"
                    f"{r.n:>8d}"
                    f"{r.h:>14.8e}"
                    f"{r.exact:>16.8e}"
                    f"{r.approx:>16.8e}"
                    f"{r.error:>16.8e}"
                    f"{r.relative_error:>11.4%}\n"
                )
        print(f"Results saved to {filename}")

    # ── plotting ──────────────────────────────────────────────────

    def plot_function(
        self,
        func_index: int,
        a: float,
        b: float,
        n: int | None = None,
        num_samples: int = 200,
        title: str | None = None,
        save_path: str | None = None,
    ) -> None:
        """Plot the function being integrated over [a, b].

        Shows the function curve, shaded area under the curve,
        interval boundaries, and optionally the grid points.

        Args:
            func_index: Index of the registered function to plot.
            a: Lower bound of integration.
            b: Upper bound of integration.
            n: If given, show the n+1 grid/sample points.
            num_samples: Number of points for the smooth curve.
            title: Plot title (default: auto-generated).
            save_path: If given, save the figure to this path.
        """
        import matplotlib.pyplot as plt
        import numpy as np

        entry = self._functions[func_index]

        x_curve = np.linspace(a, b, num_samples)
        y_curve = [entry.f(xi) for xi in x_curve]

        fig, ax = plt.subplots(figsize=(10, 6))

        # shaded area (the integral)
        ax.fill_between(x_curve, y_curve, alpha=0.15, color='#2196F3')

        # function curve
        ax.plot(x_curve, y_curve, '-', linewidth=2,
                color='#2196F3', label=f'f(x) = {entry.name}')

        # interval boundaries
        ax.axvline(a, color='#666666', linestyle='--', linewidth=1, alpha=0.6)
        ax.axvline(b, color='#666666', linestyle='--', linewidth=1, alpha=0.6)

        # grid points
        if n is not None and n >= 1:
            h = (b - a) / n
            x_grid = [a + i * h for i in range(n + 1)]
            y_grid = [entry.f(xi) for xi in x_grid]
            ax.scatter(x_grid, y_grid, color='#F44336', zorder=5, s=40,
                       edgecolors='white', linewidths=1.2, label=f'Grid points (n={n})')

        # annotate exact integral value
        exact = entry.F(a, b)
        ax.annotate(f'∫ = {exact:.6f}', xy=(0.02, 0.95),
                    xycoords='axes fraction', fontsize=11,
                    bbox=dict(boxstyle='round,pad=0.3', facecolor='white',
                              edgecolor='#cccccc', alpha=0.9))

        if title is None:
            title = f"{self.get_method_name()} — f(x) = {entry.name}"
        ax.set_title(title, fontsize=14, fontweight='bold')
        ax.set_xlabel("x", fontsize=12)
        ax.set_ylabel("f(x)", fontsize=12)
        ax.legend(fontsize=10)
        ax.grid(True, alpha=0.3)
        fig.tight_layout()

        if save_path:
            fig.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"  Plot saved to {save_path}")

        plt.show()

    def plot_approximation(
        self,
        func_index: int,
        a: float,
        b: float,
        n: int,
        num_samples: int = 200,
        title: str | None = None,
        save_path: str | None = None,
    ) -> None:
        """Plot the function and the method's piecewise approximation.

        The base class draws the function curve and grid points.
        Each subclass overrides ``_draw_approximation()`` to draw the
        method-specific shapes (trapezoids, parabolas, cubics).

        Args:
            func_index: Index of the registered function to plot.
            a: Lower bound of integration.
            b: Upper bound of integration.
            n: Number of sub-intervals.
            num_samples: Number of points for the smooth curve.
            title: Plot title (default: auto-generated).
            save_path: If given, save the figure to this path.
        """
        import matplotlib.pyplot as plt
        import numpy as np

        entry = self._functions[func_index]
        h = (b - a) / n

        x_curve = np.linspace(a, b, num_samples)
        y_curve = [entry.f(xi) for xi in x_curve]

        x_grid = [a + i * h for i in range(n + 1)]
        y_grid = [entry.f(xi) for xi in x_grid]

        fig, ax = plt.subplots(figsize=(10, 6))

        # function curve
        ax.plot(x_curve, y_curve, '-', linewidth=2,
                color='#2196F3', label=f'f(x) = {entry.name}')

        # subclass draws its approximation shapes
        self._draw_approximation(ax, entry.f, a, b, n, x_grid, y_grid)

        # grid points (on top)
        ax.scatter(x_grid, y_grid, color='#F44336', zorder=5, s=40,
                   edgecolors='white', linewidths=1.2, label=f'Grid points (n={n})')

        # annotate approximate value
        approx = self.integrate(entry.f, a, b, n)
        exact = entry.F(a, b)
        ax.annotate(
            f'Approx = {approx:.6f}\nExact   = {exact:.6f}\nError   = {abs(exact - approx):.2e}',
            xy=(0.02, 0.82), xycoords='axes fraction', fontsize=10,
            bbox=dict(boxstyle='round,pad=0.3', facecolor='white',
                      edgecolor='#cccccc', alpha=0.9),
            family='monospace'
        )

        if title is None:
            title = f"{self.get_method_name()} — n = {n}"
        ax.set_title(title, fontsize=14, fontweight='bold')
        ax.set_xlabel("x", fontsize=12)
        ax.set_ylabel("f(x)", fontsize=12)
        ax.legend(fontsize=10)
        ax.grid(True, alpha=0.3)
        fig.tight_layout()

        if save_path:
            fig.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"  Plot saved to {save_path}")

        plt.show()

    def _draw_approximation(self, ax, f, a, b, n, x_grid, y_grid) -> None:
        """Draw the method-specific approximation shapes on *ax*.

        Override in each subclass to draw trapezoids (TrapezoidalRule),
        parabolas (Simpsons13), or cubics (Simpsons38).

        Default: draws nothing (base class fallback).
        """
        pass

    def plot_convergence(
        self,
        func_index: int,
        a: float,
        b: float,
        title: str | None = None,
        save_path: str | None = None,
    ) -> None:
        """Plot error convergence on a log-log scale.

        Uses the n values from ``set_sub_intervals()`` and the results
        from ``compute_all()``.  Plots log₁₀(error) vs log₁₀(n) and
        overlays a reference slope line for the expected convergence order.

        Args:
            func_index: Index of the registered function.
            a: Lower bound (used if results not yet computed).
            b: Upper bound.
            title: Plot title.
            save_path: If given, save the figure to this path.
        """
        import matplotlib.pyplot as plt
        import numpy as np

        entry = self._functions[func_index]

        # Gather errors for this function across all n values
        n_vals = []
        errors = []
        for r in self._results:
            if r.function_name == entry.name and r.error > 0:
                n_vals.append(r.n)
                errors.append(r.error)

        if len(n_vals) < 2:
            print("Need at least 2 data points for convergence plot.")
            return

        log_n = np.log10(n_vals)
        log_err = np.log10(errors)

        fig, ax = plt.subplots(figsize=(10, 6))

        # actual convergence data
        ax.plot(log_n, log_err, 'o-', linewidth=2, markersize=6,
                color='#2196F3', label=f'{self.get_method_name()}')

        # reference slope lines
        # estimate the actual slope from the data
        if len(log_n) >= 2:
            slope = (log_err[-1] - log_err[0]) / (log_n[-1] - log_n[0])
            midx = (log_n[0] + log_n[-1]) / 2
            midy = (log_err[0] + log_err[-1]) / 2

            # O(h²) reference (slope -2 on log(error) vs log(n))
            ref_y2 = midy + (-2) * (log_n - midx)
            ax.plot(log_n, ref_y2, '--', linewidth=1, color='#4CAF50',
                    alpha=0.7, label='O(h²) reference')

            # O(h⁴) reference (slope -4 on log(error) vs log(n))
            ref_y4 = midy + (-4) * (log_n - midx)
            ax.plot(log_n, ref_y4, '--', linewidth=1, color='#FF9800',
                    alpha=0.7, label='O(h⁴) reference')

            ax.annotate(f'Measured slope ≈ {slope:.2f}',
                        xy=(0.02, 0.05), xycoords='axes fraction', fontsize=11,
                        bbox=dict(boxstyle='round,pad=0.3', facecolor='white',
                                  edgecolor='#cccccc', alpha=0.9))

        if title is None:
            title = f"Convergence — {self.get_method_name()} — f(x) = {entry.name}"
        ax.set_title(title, fontsize=14, fontweight='bold')
        ax.set_xlabel("log₁₀(n)", fontsize=12)
        ax.set_ylabel("log₁₀(error)", fontsize=12)
        ax.legend(fontsize=10)
        ax.grid(True, alpha=0.3)
        fig.tight_layout()

        if save_path:
            fig.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"  Plot saved to {save_path}")

        plt.show()

    # ── getters ───────────────────────────────────────────────────

    @property
    def num_functions(self) -> int:
        return len(self._functions)

    @property
    def num_n(self) -> int:
        return len(self._n_values)

    @property
    def results(self) -> list[IntegrationResult]:
        return list(self._results)

    def get_function(self, i: int) -> IntegrationFunctionEntry:
        return self._functions[i]

    def get_n(self, i: int) -> int:
        return self._n_values[i]
