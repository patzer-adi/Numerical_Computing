"""
Central Difference.

f'(x) ≈ (f(x+h) - f(x-h)) / (2h)

Second-order central finite difference.
Truncation error: O(h²)
"""

from typing import Callable
from pynumerics.differentiation.base import Differentiation


class CentralDifference(Differentiation):
    """Central finite-difference approximation."""

    def compute_derivative(
        self, f: Callable[[float], float], x: float, h: float
    ) -> float:
        """(f(x+h) - f(x-h)) / (2h)"""
        return (f(x + h) - f(x - h)) / (2.0 * h)

    def get_method_name(self) -> str:
        return "Central Difference"
