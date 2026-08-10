"""
Least Squares Line Fit (y = a + bx).

Uses normal equations solved via Matrix inverse.
"""

from pynumerics.interpolation.base import Interpolation, FitResult
from pynumerics.matrix import Matrix
from pynumerics.exceptions import MatrixException


class LeastSquareLine(Interpolation):
    """Least squares linear regression: y = a + bx."""

    def __init__(self, x_data: list[float], y_data: list[float]):
        super().__init__(x_data, y_data)
        self._coeff_a = 0.0
        self._coeff_b = 0.0
        self._fitted = False

    def fit(self) -> None:
        """Compute coefficients using normal equations."""
        if self._fitted:
            return

        if self.num_points < 2:
            raise MatrixException(
                "least squares line needs at least 2 data points"
            )

        n = self.num_points
        sum_x = sum(self._x)
        sum_y = sum(self._y)
        sum_x2 = sum(xi ** 2 for xi in self._x)
        sum_xy = sum(self._x[i] * self._y[i] for i in range(n))

        # Build 2x2 normal equations matrix
        A = Matrix.from_list([
            [float(n), sum_x],
            [sum_x, sum_x2]
        ])

        rhs = Matrix.from_list([
            [sum_y],
            [sum_xy]
        ])

        A_inv = A.inverse()
        coeffs = A_inv * rhs

        self._coeff_a = coeffs[0, 0]
        self._coeff_b = coeffs[1, 0]
        self._fitted = True

    def evaluate(self, x: float) -> float:
        if not self._fitted:
            self.fit()
        return self._coeff_a + self._coeff_b * x

    @property
    def a(self) -> float:
        if not self._fitted:
            self.fit()
        return self._coeff_a

    @property
    def b(self) -> float:
        if not self._fitted:
            self.fit()
        return self._coeff_b

    def get_fit_result(self) -> FitResult:
        if not self._fitted:
            self.fit()
        base = self.compute_errors()
        base.coefficients = {"a": self._coeff_a, "b": self._coeff_b}
        return base
