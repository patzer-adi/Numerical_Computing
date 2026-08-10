"""
Test suite for Newton-Raphson root-finding method.
"""

import pytest
import math
from pynumerics.roots.newton_raphson import NewtonRaphson


class TestNewtonRaphson:
    def test_find_sqrt_4(self):
        # f(x) = x^2 - 4, f'(x) = 2x, root at x = 2
        f = lambda x: x**2 - 4
        df = lambda x: 2 * x
        solver = NewtonRaphson(tol=1e-10)
        result = solver.solve(f, df, 3.0)
        assert result.converged is True
        assert result.root == pytest.approx(2.0, abs=1e-8)

    def test_find_cube_root_27(self):
        # f(x) = x^3 - 27, f'(x) = 3x^2, root at x = 3
        f = lambda x: x**3 - 27
        df = lambda x: 3 * x**2
        solver = NewtonRaphson(tol=1e-10)
        result = solver.solve(f, df, 5.0)
        assert result.converged is True
        assert result.root == pytest.approx(3.0, abs=1e-8)

    def test_quadratic_convergence(self):
        # Should converge in very few iterations
        f = lambda x: x**2 - 4
        df = lambda x: 2 * x
        solver = NewtonRaphson(tol=1e-10)
        result = solver.solve(f, df, 3.0)
        assert result.iterations < 20  # Should be much less

    def test_zero_derivative_raises(self):
        # f(x) = x^3, f'(0) = 0
        f = lambda x: x**3
        df = lambda x: 3 * x**2
        solver = NewtonRaphson(tol=1e-10)
        with pytest.raises(ValueError, match="zero"):
            solver.solve(f, df, 0.0)

    def test_cos_root(self):
        # f(x) = cos(x), f'(x) = -sin(x), root near pi/2
        f = lambda x: math.cos(x)
        df = lambda x: -math.sin(x)
        solver = NewtonRaphson(tol=1e-10)
        result = solver.solve(f, df, 1.0)
        assert result.converged is True
        assert result.root == pytest.approx(math.pi / 2, abs=1e-8)
