"""
Richardson Extrapolation.

R = (4·D(h/2) - D(h)) / 3
where D(h) = (f(x+h) - f(x-h)) / (2h)  (central difference)

Combines two central differences to cancel the leading error term.
Truncation error: O(h⁴)
"""

from typing import Callable
from pynumerics.differentiation.base import Differentiation


class RichardsonExtrapolation(Differentiation):
    """Richardson extrapolation for numerical differentiation."""

    def compute_derivative(
        self, f: Callable[[float], float], x: float, h: float
    ) -> float:
        """(4·D(h/2) - D(h)) / 3"""
        # central difference at step h
        d_h = (f(x + h) - f(x - h)) / (2.0 * h)

        # central difference at step h/2
        h2 = h / 2.0
        d_h2 = (f(x + h2) - f(x - h2)) / (2.0 * h2)

        # Richardson combination
        return (4.0 * d_h2 - d_h) / 3.0

    def get_method_name(self) -> str:
        return "Richardson Extrapolation"
