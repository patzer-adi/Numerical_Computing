"""
Numerical Integration base class.

ABC for all quadrature methods.  Uses COMPOSITION — holds registered
functions and sub-interval counts, does NOT inherit Matrix.

Hierarchy (mirrors C++):
    Integration (abstract base)
      ├── TrapezoidalRule
      ├── Simpsons13
      └── Simpsons38
"""

from abc import ABC, abstractmethod
from dataclasses import dataclass
from typing import Callable
import math


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
    exact: float
    approx: float
    error: float


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


class Integration(ABC):
    """Abstract base class for numerical integration methods.

    Uses COMPOSITION — holds a list of registered integrands and sub-interval counts.

    Derived classes override:
        integrate(f, a, b, n) — the quadrature formula
        get_method_name()     — human-readable method name
    """

    def __init__(self) -> None:
        self._functions: list[IntegrationFunctionEntry] = []
        self._n_values: list[int] = []
        self._results: list[IntegrationResult] = []

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

        Populates and returns the internal results list.

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

        self._results.clear()
        for entry in self._functions:
            exact = entry.F(a, b)
            for n in self._n_values:
                approx = self.integrate(entry.f, a, b, n)
                error = abs(exact - approx)
                self._results.append(
                    IntegrationResult(
                        function_name=entry.name,
                        n=n,
                        exact=exact,
                        approx=approx,
                        error=error,
                    )
                )
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
            f"{'Exact':>16}"
            f"{'Approx':>16}"
            f"{'Error':>16}"
        )
        print(header)
        print("-" * len(header))

        for r in self._results:
            print(
                f"{r.function_name:<18}"
                f"{r.n:>8d}"
                f"{r.exact:>16.6e}"
                f"{r.approx:>16.6e}"
                f"{r.error:>16.6e}"
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
                f"{'Exact':>16}"
                f"{'Approx':>16}"
                f"{'Error':>16}\n"
            )
            for r in self._results:
                fout.write(
                    f"{r.function_name:<18}"
                    f"{r.n:>8d}"
                    f"{r.exact:>16.8e}"
                    f"{r.approx:>16.8e}"
                    f"{r.error:>16.8e}\n"
                )
        print(f"Results saved to {filename}")

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
