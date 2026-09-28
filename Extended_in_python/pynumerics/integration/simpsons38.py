"""
Simpson's 3/8 Rule.

∫ₐᵇ f(x)dx ≈ (3h/8) [f(x₀) + 3f(x₁) + 3f(x₂) + 2f(x₃) + 3f(x₄) + ... + f(xₙ)]
where h = (b - a) / n

Composite Simpson's 3/8 rule.
Truncation error: O(h⁴)
Constraint: n must be a MULTIPLE OF 3 (n ≥ 3)
"""

from typing import Callable
from pynumerics.integration.base import Integration


class Simpsons38(Integration):
    """Composite Simpson's 3/8 rule approximation."""

    def integrate(
        self, f: Callable[[float], float], a: float, b: float, n: int
    ) -> float:
        """(3h/8) [f(x₀) + 3f(x₁) + 3f(x₂) + 2f(x₃) + ... + f(xₙ)]"""
        if n < 3 or n % 3 != 0:
            raise ValueError("Simpson's 3/8 rule requires n to be a multiple of 3 (n >= 3)")

        h = (b - a) / n
        result = f(a) + f(b)

        for i in range(1, n):
            xi = a + i * h
            if i % 3 == 0:
                result += 2.0 * f(xi)     # every 3rd interior point: coefficient 2
            else:
                result += 3.0 * f(xi)     # other interior points: coefficient 3

        return (3.0 * h / 8.0) * result

    def get_method_name(self) -> str:
        return "Simpson's 3/8 Rule"
