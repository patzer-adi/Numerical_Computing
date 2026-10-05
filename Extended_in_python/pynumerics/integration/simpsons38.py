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

    def _draw_approximation(self, ax, f, a, b, n, x_grid, y_grid) -> None:
        """Draw cubic approximation for each group of 3 sub-intervals.

        Simpson's 3/8 fits a cubic through 4 consecutive points
        (x_{3k}, x_{3k+1}, x_{3k+2}, x_{3k+3}) for each panel.
        """
        import numpy as np

        for k in range(0, n, 3):
            # four points for this panel
            x0, x1, x2, x3 = x_grid[k], x_grid[k + 1], x_grid[k + 2], x_grid[k + 3]
            y0, y1, y2, y3 = y_grid[k], y_grid[k + 1], y_grid[k + 2], y_grid[k + 3]

            # fit cubic through 4 points via Lagrange
            xs = np.linspace(x0, x3, 50)

            ys = (y0 * (xs - x1) * (xs - x2) * (xs - x3)
                      / ((x0 - x1) * (x0 - x2) * (x0 - x3))
                + y1 * (xs - x0) * (xs - x2) * (xs - x3)
                      / ((x1 - x0) * (x1 - x2) * (x1 - x3))
                + y2 * (xs - x0) * (xs - x1) * (xs - x3)
                      / ((x2 - x0) * (x2 - x1) * (x2 - x3))
                + y3 * (xs - x0) * (xs - x1) * (xs - x2)
                      / ((x3 - x0) * (x3 - x1) * (x3 - x2)))

            ax.fill_between(xs, ys, alpha=0.2, color='#FF9800')
            label = "Simpson's 3/8 approx." if k == 0 else None
            ax.plot(xs, ys, '-', linewidth=1.5, color='#FF9800', label=label)
