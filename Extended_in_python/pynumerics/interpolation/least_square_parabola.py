"""
Least Squares Parabola Fit (y = a + bx + cx²).

Uses normal equations solved via Matrix inverse.
"""

from pynumerics.interpolation.base import Interpolation, FitResult
from pynumerics.matrix import Matrix
from pynumerics.exceptions import MatrixException


class LeastSquareParabola(Interpolation):
    """Least squares quadratic regression: y = a + bx + cx²."""

    def __init__(self, x_data: list[float], y_data: list[float]):
        super().__init__(x_data, y_data)
        self._coeff_a = 0.0
        self._coeff_b = 0.0
        self._coeff_c = 0.0
        self._fitted = False

    def fit(self) -> None:
        """Compute coefficients using normal equations."""
        if self._fitted:
            return

        if self.num_points < 3:
            raise MatrixException(
                "least squares parabola needs at least 3 data points"
            )

        n = self.num_points
        sum_x = sum(self._x)
        sum_y = sum(self._y)
        sum_x2 = sum(xi ** 2 for xi in self._x)
        sum_x3 = sum(xi ** 3 for xi in self._x)
        sum_x4 = sum(xi ** 4 for xi in self._x)
        sum_xy = sum(self._x[i] * self._y[i] for i in range(n))
        sum_x2y = sum(self._x[i] ** 2 * self._y[i] for i in range(n))

        A = Matrix.from_list([
            [float(n), sum_x, sum_x2],
            [sum_x, sum_x2, sum_x3],
            [sum_x2, sum_x3, sum_x4]
        ])

        rhs = Matrix.from_list([
            [sum_y],
            [sum_xy],
            [sum_x2y]
        ])

        A_inv = A.inverse()
        coeffs = A_inv * rhs

        self._coeff_a = coeffs[0, 0]
        self._coeff_b = coeffs[1, 0]
        self._coeff_c = coeffs[2, 0]
        self._fitted = True

    def evaluate(self, x: float) -> float:
        if not self._fitted:
            self.fit()
        return self._coeff_a + self._coeff_b * x + self._coeff_c * x * x

    @property
    def a(self) -> float:
        if not self._fitted: self.fit()
        return self._coeff_a

    @property
    def b(self) -> float:
        if not self._fitted: self.fit()
        return self._coeff_b

    @property
    def c(self) -> float:
        if not self._fitted: self.fit()
        return self._coeff_c

    def get_fit_result(self) -> FitResult:
        if not self._fitted:
            self.fit()
        base = self.compute_errors()
        base.coefficients = {"a": self._coeff_a, "b": self._coeff_b, "c": self._coeff_c}
        return base
