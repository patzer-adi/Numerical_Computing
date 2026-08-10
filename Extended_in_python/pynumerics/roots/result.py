"""
RootResult dataclass.

Encapsulates the result of a root-finding operation, including the
root value, iteration count, convergence status, and final error.
"""

from dataclasses import dataclass


@dataclass
class RootResult:
    """The result of a root-finding operation.

    Attributes:
        root: The approximate root value.
        iterations: Number of iterations taken.
        converged: Whether the solver converged within tolerance.
        error: Final absolute difference between iterations.
        interval: The final bracketing interval (for bisection), or None.
    """
    root: float = 0.0
    iterations: int = 0
    converged: bool = False
    error: float = 0.0
    interval: tuple[float, float] | None = None
