"""
Forward Difference.

f'(x) ≈ (f(x+h) - f(x)) / h

First-order forward finite difference.
Truncation error: O(h)
"""

from typing import Callable
from pynumerics.differentiation.base import Differentiation


class ForwardDifference(Differentiation):
    """Forward finite-difference approximation."""

    def compute_derivative(
        self, f: Callable[[float], float], x: float, h: float
    ) -> float:
        """(f(x+h) - f(x)) / h"""
        return (f(x + h) - f(x)) / h

    def get_method_name(self) -> str:
        return "Forward Difference"
