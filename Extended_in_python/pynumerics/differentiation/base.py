"""
Numerical Differentiation base class.

ABC for all finite-difference methods.  Uses COMPOSITION — holds registered
functions and step sizes, does NOT inherit Matrix.

Hierarchy (mirrors C++):
    Differentiation (abstract base)
      ├── ForwardDifference
      ├── BackwardDifference
      ├── CentralDifference
      └── RichardsonExtrapolation
"""

from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from typing import Callable


@dataclass
class FunctionEntry:
    """A registered function together with its known exact derivative."""
    name: str
    f: Callable[[float], float]
    df: Callable[[float], float]


@dataclass
class DiffResult:
    """One row of the differentiation results table."""
    function_name: str
    h: float
    exact: float
    approx: float
    error: float


class Differentiation(ABC):
    """Abstract base class for numerical differentiation methods.

    Uses COMPOSITION — holds a list of registered functions and step sizes.

    Derived classes override:
        compute_derivative(f, x, h) — the finite-difference formula
        get_method_name()           — human-readable method name
    """

    def __init__(self) -> None:
        self._functions: list[FunctionEntry] = []
        self._h_values: list[float] = []
        self._results: list[DiffResult] = []

    # ── registration ──────────────────────────────────────────────

    def add_function(
        self,
        name: str,
        f: Callable[[float], float],
        df: Callable[[float], float],
    ) -> None:
        """Register a function with its known exact derivative."""
        self._functions.append(FunctionEntry(name=name, f=f, df=df))

    def set_step_sizes(self, h_values: list[float]) -> None:
        """Set the step sizes to evaluate at."""
        self._h_values = list(h_values)

    # ── pure-virtual interface ────────────────────────────────────

    @abstractmethod
    def compute_derivative(
        self, f: Callable[[float], float], x: float, h: float
    ) -> float:
        """Compute the approximate derivative of *f* at *x* with step *h*."""
        pass

    @abstractmethod
    def get_method_name(self) -> str:
        """Return a human-readable name for this method."""
        pass

    # ── compute all ───────────────────────────────────────────────

    def compute_all(self, x0: float) -> list[DiffResult]:
        """Evaluate every registered function at every step size.

        Populates and returns the internal results list.

        Raises:
            ValueError: If no functions or step sizes have been set.
        """
        if not self._functions:
            raise ValueError("no functions registered... add some first")
        if not self._h_values:
            raise ValueError("no step sizes set... call set_step_sizes() first")

        self._results.clear()
        for entry in self._functions:
            exact = entry.df(x0)
            for h in self._h_values:
                approx = self.compute_derivative(entry.f, x0, h)
                error = abs(exact - approx)
                self._results.append(
                    DiffResult(
                        function_name=entry.name,
                        h=h,
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
            f"{'h':>14}"
            f"{'Exact':>16}"
            f"{'Approx':>16}"
            f"{'Error':>16}"
        )
        print(header)
        print("-" * len(header))

        for r in self._results:
            print(
                f"{r.function_name:<18}"
                f"{r.h:>14.6e}"
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
                f"{'h':>14}"
                f"{'Exact':>16}"
                f"{'Approx':>16}"
                f"{'Error':>16}\n"
            )
            for r in self._results:
                fout.write(
                    f"{r.function_name:<18}"
                    f"{r.h:>14.8e}"
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
    def num_h(self) -> int:
        return len(self._h_values)

    @property
    def results(self) -> list[DiffResult]:
        return list(self._results)

    def get_function(self, i: int) -> FunctionEntry:
        return self._functions[i]

    def get_h(self, i: int) -> float:
        return self._h_values[i]
