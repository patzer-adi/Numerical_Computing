"""
Newton's Divided Difference Interpolation.

P(x) = f[x₀] + f[x₀,x₁](x−x₀) + f[x₀,x₁,x₂](x−x₀)(x−x₁) + ⋯

The divided difference table is built recursively:
  f[xᵢ]           = yᵢ
  f[xᵢ, xᵢ₊₁]    = (f[xᵢ₊₁] − f[xᵢ]) / (xᵢ₊₁ − xᵢ)
  f[xᵢ, …, xᵢ₊ₖ] = (f[xᵢ₊₁,…,xᵢ₊ₖ] − f[xᵢ,…,xᵢ₊ₖ₋₁]) / (xᵢ₊ₖ − xᵢ)
"""

from pynumerics.interpolation.base import Interpolation
from pynumerics.exceptions import MatrixException


class NewtonDividedDifference(Interpolation):
    """Newton's Divided Difference polynomial interpolation."""

    def __init__(self, x_data: list[float], y_data: list[float]):
        super().__init__(x_data, y_data)
        self._dd_table: list[list[float]] | None = None
        self._coefficients: list[float] | None = None

    def _build_table(self) -> None:
        """Build the divided difference table (lazy, computed once)."""
        if self._dd_table is not None:
            return

        if self.num_points < 2:
            raise MatrixException(
                "Newton divided difference needs at least 2 data points"
            )

        # check for duplicate x values
        for i in range(self.num_points):
            for j in range(i + 1, self.num_points):
                if abs(self._x[i] - self._x[j]) < 1e-10:
                    raise MatrixException(
                        "duplicate x values found... Newton divided difference "
                        "can't handle that (division by zero)"
                    )

        n = self.num_points

        # build n×n table (lower-triangular)
        # table[i][j] = f[xᵢ₋ⱼ, ..., xᵢ]
        table = [[0.0] * n for _ in range(n)]

        # column 0: zeroth-order divided differences = y values
        for i in range(n):
            table[i][0] = self._y[i]

        # fill columns 1 to n-1
        for j in range(1, n):
            for i in range(j, n):
                num = table[i][j - 1] - table[i - 1][j - 1]
                den = self._x[i] - self._x[i - j]
                table[i][j] = num / den

        self._dd_table = table

        # extract diagonal coefficients: table[k][k] = f[x₀, ..., xₖ]
        self._coefficients = [table[k][k] for k in range(n)]

    def evaluate(self, x: float) -> float:
        """Evaluate the Newton polynomial at x using Horner's nested form.

        P(x) = f[x₀] + (x−x₀)(f[x₀,x₁] + (x−x₁)(f[x₀,x₁,x₂] + ...))

        Working from inside out:
            result = coeff[n-1]
            for i = n-2 down to 0:
                result = result * (x - x[i]) + coeff[i]
        """
        self._build_table()

        n = self.num_points
        coeffs = self._coefficients

        # Horner's form
        result = coeffs[n - 1]
        for i in range(n - 2, -1, -1):
            result = result * (x - self._x[i]) + coeffs[i]

        return result

    def get_table(self) -> list[list[float]]:
        """Return the full divided difference table."""
        self._build_table()
        return self._dd_table

    def get_coefficients(self) -> list[float]:
        """Return the diagonal coefficients [f[x₀], f[x₀,x₁], ...]."""
        self._build_table()
        return list(self._coefficients)

    def print_table(self) -> None:
        """Print the divided difference table in a formatted way."""
        self._build_table()

        n = self.num_points
        table = self._dd_table

        print("\n--- Newton Divided Difference Table ---")

        # header row
        header = f"{'x':<12}{'f[.]':<14}"
        for j in range(1, n):
            dots = ",".join(["."] * (j + 1))
            header += f"{'f[' + dots + ']':<14}"
        print(header)
        print("-" * (12 + 14 * n))

        # table rows
        for i in range(n):
            row = f"{self._x[i]:<12.6f}"
            for j in range(i + 1):
                row += f"{table[i][j]:<14.6f}"
            print(row)

        # polynomial coefficients
        print("\n--- Polynomial Coefficients ---")
        for k in range(n):
            label = "f[x₀" + "".join(f", x{j}" for j in range(1, k + 1)) + "]"
            print(f"  {label} = {self._coefficients[k]:.6f}")

        # polynomial expression
        expr = f"\n  P(x) = {self._coefficients[0]:.6f}"
        for k in range(1, n):
            coeff = self._coefficients[k]
            if coeff >= 0:
                expr += f" + {coeff:.6f}"
            else:
                expr += f" - {abs(coeff):.6f}"
            for j in range(k):
                xj = self._x[j]
                if xj >= 0:
                    expr += f"(x - {xj:.6f})"
                else:
                    expr += f"(x + {abs(xj):.6f})"
        print(expr)

    def plot(self, num_samples: int = 200, title: str | None = None,
             show_points: bool = True, save_path: str | None = None,
             func=None, func_label: str | None = None) -> None:
        """Plot with Newton-specific title."""
        if title is None:
            title = "Newton Divided Difference Interpolation"
        super().plot(
            num_samples=num_samples, title=title,
            show_points=show_points, save_path=save_path,
            func=func, func_label=func_label,
        )
