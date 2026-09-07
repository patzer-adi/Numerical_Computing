"""
Test suite for Newton Divided Difference interpolation.
"""

import pytest
import math
from pynumerics.exceptions import MatrixException
from pynumerics.interpolation.newton_divided_difference import NewtonDividedDifference
from pynumerics.interpolation.lagrange import Lagrange
from pynumerics.interpolation.test_functions import generate_data, sin_func, poly_func


class TestNewtonDividedDifference:
    def test_linear_interpolation(self):
        """y = 2x + 1 through (0,1) and (1,3)."""
        interp = NewtonDividedDifference([0.0, 1.0], [1.0, 3.0])
        assert interp.evaluate(0.5) == pytest.approx(2.0)

    def test_quadratic_interpolation(self):
        """y = x^2 through (0,0), (1,1), (2,4)."""
        interp = NewtonDividedDifference([0.0, 1.0, 2.0], [0.0, 1.0, 4.0])
        assert interp.evaluate(1.5) == pytest.approx(2.25)
        assert interp.evaluate(3.0) == pytest.approx(9.0)

    def test_exact_at_data_points(self):
        """Interpolation must reproduce original data exactly."""
        x = [1.0, 2.0, 3.0, 4.0]
        y = [10.0, 20.0, 30.0, 40.0]
        interp = NewtonDividedDifference(x, y)
        for i in range(len(x)):
            assert interp.evaluate(x[i]) == pytest.approx(y[i])

    def test_duplicate_x_raises(self):
        """Duplicate x values should raise MatrixException."""
        interp = NewtonDividedDifference([1.0, 1.0, 2.0], [10.0, 20.0, 30.0])
        with pytest.raises(MatrixException, match="duplicate"):
            interp.evaluate(1.5)

    def test_matches_lagrange(self):
        """Newton DD and Lagrange must give identical results."""
        x = [0.0, 1.0, 2.0, 3.0, 4.0]
        y = [1.0, 2.7, 5.8, 6.3, 7.1]
        newton = NewtonDividedDifference(x, y)
        lagrange = Lagrange(x, y)

        test_points = [-0.5, 0.5, 1.5, 2.5, 3.5, 4.5]
        for xp in test_points:
            assert newton.evaluate(xp) == pytest.approx(
                lagrange.evaluate(xp), abs=1e-10
            )

    def test_divided_difference_table(self):
        """Verify table values for a known simple case.

        Data: (1, 0), (2, 1), (4, 3)
        f[x₀] = 0
        f[x₀, x₁] = (1 - 0) / (2 - 1) = 1
        f[x₀, x₁, x₂] = ((3-1)/(4-2) - 1) / (4 - 1) = (1 - 1)/3 = 0
        """
        interp = NewtonDividedDifference([1.0, 2.0, 4.0], [0.0, 1.0, 3.0])
        table = interp.get_table()

        # column 0: y values
        assert table[0][0] == pytest.approx(0.0)
        assert table[1][0] == pytest.approx(1.0)
        assert table[2][0] == pytest.approx(3.0)

        # column 1: first-order divided differences
        # f[x₀, x₁] = (1-0)/(2-1) = 1
        assert table[1][1] == pytest.approx(1.0)
        # f[x₁, x₂] = (3-1)/(4-2) = 1
        assert table[2][1] == pytest.approx(1.0)

        # column 2: second-order
        # f[x₀, x₁, x₂] = (1 - 1) / (4 - 1) = 0
        assert table[2][2] == pytest.approx(0.0)

    def test_coefficients(self):
        """Verify diagonal coefficients for known data."""
        interp = NewtonDividedDifference([1.0, 2.0, 4.0], [0.0, 1.0, 3.0])
        coeffs = interp.get_coefficients()

        assert len(coeffs) == 3
        assert coeffs[0] == pytest.approx(0.0)   # f[x₀]
        assert coeffs[1] == pytest.approx(1.0)   # f[x₀, x₁]
        assert coeffs[2] == pytest.approx(0.0)   # f[x₀, x₁, x₂]

    def test_sin_function(self):
        """Interpolate sin(x) with 6 points and check accuracy."""
        x_data, y_data = generate_data(sin_func, 0, math.pi, 6)
        interp = NewtonDividedDifference(x_data, y_data)

        # check at midpoints — should be reasonably accurate
        test_x = math.pi / 4  # sin(pi/4) ≈ 0.7071
        result = interp.evaluate(test_x)
        assert result == pytest.approx(math.sin(test_x), abs=0.01)

    def test_polynomial_exact(self):
        """Newton DD on n+1 points of degree-n polynomial must be exact.

        y = x^2 + 3x + 4 through 3 points (degree 2 needs 3 points).
        """
        x_data, y_data = generate_data(poly_func, -2.0, 2.0, 3)
        interp = NewtonDividedDifference(x_data, y_data)

        # must match exactly at any point
        test_points = [-3.0, -1.5, 0.0, 0.7, 1.3, 3.0]
        for xp in test_points:
            expected = poly_func(xp)
            assert interp.evaluate(xp) == pytest.approx(expected, abs=1e-10)

    def test_cubic(self):
        """Newton DD with 4 points on a cubic must be exact."""
        def cubic(x): return x ** 3 - 2 * x ** 2 + x - 1
        x_data, y_data = generate_data(cubic, -2.0, 3.0, 4)
        interp = NewtonDividedDifference(x_data, y_data)

        for xp in [-1.0, 0.5, 1.5, 2.5]:
            assert interp.evaluate(xp) == pytest.approx(cubic(xp), abs=1e-8)

    def test_single_point_raises(self):
        """Need at least 2 data points."""
        interp = NewtonDividedDifference([1.0], [2.0])
        with pytest.raises(MatrixException, match="at least 2"):
            interp.evaluate(1.0)

    def test_unequal_lengths_raises(self):
        """x and y must have same length."""
        with pytest.raises(ValueError, match="same length"):
            NewtonDividedDifference([1.0, 2.0], [1.0])


class TestTestFunctions:
    """Test the test_functions module itself."""

    def test_generate_data_length(self):
        x, y = generate_data(sin_func, 0, 1, 10)
        assert len(x) == 10
        assert len(y) == 10

    def test_generate_data_range(self):
        x, y = generate_data(sin_func, -1, 1, 5)
        assert x[0] == pytest.approx(-1.0)
        assert x[-1] == pytest.approx(1.0)

    def test_generate_data_too_few_raises(self):
        with pytest.raises(ValueError, match="at least 2"):
            generate_data(sin_func, 0, 1, 1)

    def test_generate_data_bad_range_raises(self):
        with pytest.raises(ValueError, match="x_start must be less"):
            generate_data(sin_func, 1, 0, 5)

    def test_poly_func_values(self):
        # x^2 + 3x + 4 at x=2 → 4 + 6 + 4 = 14
        assert poly_func(2.0) == pytest.approx(14.0)
