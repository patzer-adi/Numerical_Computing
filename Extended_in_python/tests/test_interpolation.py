"""
Test suite for Interpolation methods.
"""

import pytest
import math
from pynumerics.exceptions import MatrixException
from pynumerics.interpolation.lagrange import Lagrange
from pynumerics.interpolation.least_square_line import LeastSquareLine
from pynumerics.interpolation.least_square_parabola import LeastSquareParabola


class TestLagrange:
    def test_linear_interpolation(self):
        # y = 2x + 1 through (0,1) and (1,3)
        interp = Lagrange([0.0, 1.0], [1.0, 3.0])
        assert interp.evaluate(0.5) == pytest.approx(2.0)

    def test_quadratic_interpolation(self):
        # y = x^2 through (0,0), (1,1), (2,4)
        interp = Lagrange([0.0, 1.0, 2.0], [0.0, 1.0, 4.0])
        assert interp.evaluate(1.5) == pytest.approx(2.25)
        assert interp.evaluate(3.0) == pytest.approx(9.0)

    def test_exact_at_data_points(self):
        interp = Lagrange([1.0, 2.0, 3.0], [10.0, 20.0, 30.0])
        for i in range(3):
            assert interp.evaluate(interp.get_x(i)) == pytest.approx(interp.get_y(i))

    def test_duplicate_x_raises(self):
        interp = Lagrange([1.0, 1.0, 2.0], [10.0, 20.0, 30.0])
        with pytest.raises(MatrixException, match="duplicate"):
            interp.evaluate(1.5)


class TestLeastSquareLine:
    def test_perfect_line(self):
        # y = 2 + 3x exactly
        x = [0.0, 1.0, 2.0, 3.0]
        y = [2.0, 5.0, 8.0, 11.0]
        lsl = LeastSquareLine(x, y)
        lsl.fit()
        assert lsl.a == pytest.approx(2.0, abs=1e-6)
        assert lsl.b == pytest.approx(3.0, abs=1e-6)

    def test_evaluate(self):
        lsl = LeastSquareLine([0.0, 1.0, 2.0], [1.0, 3.0, 5.0])
        assert lsl.evaluate(1.5) == pytest.approx(4.0, abs=1e-6)

    def test_fit_result(self):
        x = [1.0, 2.0, 3.0, 4.0, 5.0]
        y = [2.2, 4.1, 5.8, 8.2, 9.9]
        lsl = LeastSquareLine(x, y)
        result = lsl.get_fit_result()
        assert "a" in result.coefficients
        assert "b" in result.coefficients
        assert result.sse >= 0

    def test_too_few_points_raises(self):
        lsl = LeastSquareLine([1.0], [1.0])
        with pytest.raises(MatrixException, match="at least 2"):
            lsl.fit()


class TestLeastSquareParabola:
    def test_perfect_parabola(self):
        # y = 1 + 2x + 3x^2
        x = [0.0, 1.0, 2.0, 3.0, 4.0]
        y = [1.0, 6.0, 17.0, 34.0, 57.0]
        lsp = LeastSquareParabola(x, y)
        lsp.fit()
        assert lsp.a == pytest.approx(1.0, abs=1e-4)
        assert lsp.b == pytest.approx(2.0, abs=1e-4)
        assert lsp.c == pytest.approx(3.0, abs=1e-4)

    def test_evaluate(self):
        x = [0.0, 1.0, 2.0, 3.0]
        y = [0.0, 1.0, 4.0, 9.0]  # y = x^2
        lsp = LeastSquareParabola(x, y)
        assert lsp.evaluate(2.5) == pytest.approx(6.25, abs=0.1)

    def test_too_few_points_raises(self):
        lsp = LeastSquareParabola([1.0, 2.0], [1.0, 2.0])
        with pytest.raises(MatrixException, match="at least 3"):
            lsp.fit()

    def test_fit_result(self):
        x = [0.0, 1.0, 2.0, 3.0, 4.0]
        y = [1.0, 3.0, 7.0, 13.0, 21.0]
        lsp = LeastSquareParabola(x, y)
        result = lsp.get_fit_result()
        assert "a" in result.coefficients
        assert "b" in result.coefficients
        assert "c" in result.coefficients
