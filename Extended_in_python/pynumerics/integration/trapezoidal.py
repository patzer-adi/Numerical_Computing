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

    def _draw_approximation(self, ax, f, a, b, n, x_grid, y_grid) -> None:
        """Draw trapezoids between consecutive grid points."""
        for i in range(n):
            # each trapezoid is a polygon: bottom-left, top-left, top-right, bottom-right
            xs = [x_grid[i], x_grid[i], x_grid[i + 1], x_grid[i + 1]]
            ys = [0, y_grid[i], y_grid[i + 1], 0]
            ax.fill(xs, ys, alpha=0.2, color='#FF9800', edgecolor='#FF9800',
                    linewidth=1)

        # piecewise-linear approximation line
        ax.plot(x_grid, y_grid, '-', linewidth=1.5, color='#FF9800',
                label='Trapezoidal approx.')
