"""
Trapezoidal Rule.

∫ₐᵇ f(x)dx ≈ (h/2) [f(x₀) + 2·Σf(xᵢ) + f(xₙ)]
where h = (b - a) / n

Composite trapezoidal rule.
Truncation error: O(h²)
Constraint: n ≥ 1
"""

from typing import Callable
from pynumerics.integration.base import Integration


class TrapezoidalRule(Integration):
    """Composite trapezoidal rule approximation."""

    def integrate(
        self, f: Callable[[float], float], a: float, b: float, n: int
    ) -> float:
        """(h/2) [f(x₀) + 2·Σf(xᵢ) + f(xₙ)]"""
        if n < 1:
            raise ValueError("trapezoidal rule requires n >= 1")

        h = (b - a) / n
        result = f(a) + f(b)

        for i in range(1, n):
            xi = a + i * h
            result += 2.0 * f(xi)

        return (h / 2.0) * result

    def get_method_name(self) -> str:
        return "Trapezoidal Rule"
