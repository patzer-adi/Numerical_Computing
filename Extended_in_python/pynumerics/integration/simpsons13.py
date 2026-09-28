"""
Simpson's 1/3 Rule.

∫ₐᵇ f(x)dx ≈ (h/3) [f(x₀) + 4f(x₁) + 2f(x₂) + 4f(x₃) + ... + f(xₙ)]
where h = (b - a) / n

Composite Simpson's 1/3 rule.
Truncation error: O(h⁴)
Constraint: n must be EVEN (n ≥ 2)
"""

from typing import Callable
from pynumerics.integration.base import Integration


class Simpsons13(Integration):
    """Composite Simpson's 1/3 rule approximation."""

    def integrate(
        self, f: Callable[[float], float], a: float, b: float, n: int
    ) -> float:
        """(h/3) [f(x₀) + 4f(x₁) + 2f(x₂) + ... + f(xₙ)]"""
        if n < 2 or n % 2 != 0:
            raise ValueError("Simpson's 1/3 rule requires n to be even (n >= 2)")

        h = (b - a) / n
        result = f(a) + f(b)

        for i in range(1, n):
            xi = a + i * h
            if i % 2 == 0:
                result += 2.0 * f(xi)     # even index: coefficient 2
            else:
                result += 4.0 * f(xi)     # odd index: coefficient 4

        return (h / 3.0) * result

    def get_method_name(self) -> str:
        return "Simpson's 1/3 Rule"
