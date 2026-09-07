"""
Lagrange Interpolation.

P(x) = sum_{i=0}^{n-1} y_i * L_i(x)
L_i(x) = product_{j!=i} (x - x_j) / (x_i - x_j)
"""

from pynumerics.interpolation.base import Interpolation
from pynumerics.exceptions import MatrixException


class Lagrange(Interpolation):
    """Lagrange polynomial interpolation."""

    def evaluate(self, x: float) -> float:
        """Evaluate the Lagrange polynomial at x."""
        # Check for duplicate x values
        for i in range(self.num_points):
            for j in range(i + 1, self.num_points):
                if abs(self._x[i] - self._x[j]) < 1e-10:
                    raise MatrixException(
                        "duplicate x values found... Lagrange can't handle that "
                        "(division by zero)"
                    )

        result = 0.0
        for i in range(self.num_points):
            xi = self._x[i]
            yi = self._y[i]

            # Compute i-th Lagrange basis polynomial L_i(x)
            Li = 1.0
            for j in range(self.num_points):
                if j != i:
                    Li *= (x - self._x[j]) / (xi - self._x[j])
            result += yi * Li

        return result

    def plot(self, num_samples: int = 200, title: str | None = None,
             show_points: bool = True, save_path: str | None = None,
             func=None, func_label: str | None = None) -> None:
        """Plot with Lagrange-specific title."""
        if title is None:
            title = "Lagrange Interpolation"
        super().plot(
            num_samples=num_samples, title=title,
            show_points=show_points, save_path=save_path,
            func=func, func_label=func_label,
        )

