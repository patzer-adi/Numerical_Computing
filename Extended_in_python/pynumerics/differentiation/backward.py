"""
Backward Difference.

f'(x) ≈ (f(x) - f(x-h)) / h

First-order backward finite difference.
Truncation error: O(h)
"""

from typing import Callable
from pynumerics.differentiation.base import Differentiation


class BackwardDifference(Differentiation):
    """Backward finite-difference approximation."""

    def compute_derivative(
        self, f: Callable[[float], float], x: float, h: float
    ) -> float:
        """(f(x) - f(x-h)) / h"""
        return (f(x) - f(x - h)) / h

    def get_method_name(self) -> str:
        return "Backward Difference"
