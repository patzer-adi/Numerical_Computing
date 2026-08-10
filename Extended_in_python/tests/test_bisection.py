"""
Test suite for Bisection root-finding method.
"""

import pytest
import math
from pynumerics.roots.bisection import Bisection


class TestBisection:
    def test_find_sqrt_4(self):
        # f(x) = x^2 - 4, root at x = 2 in [1, 3]
        f = lambda x: x**2 - 4
        solver = Bisection(tol=1e-10)
        result = solver.solve(f, 1.0, 3.0)
        assert result.converged is True
        assert result.root == pytest.approx(2.0, abs=1e-8)

    def test_find_sqrt_2(self):
        # f(x) = x^2 - 2, root at x = sqrt(2) in [1, 2]
        f = lambda x: x**2 - 2
        solver = Bisection(tol=1e-10)
        result = solver.solve(f, 1.0, 2.0)
        assert result.converged is True
        assert result.root == pytest.approx(math.sqrt(2), abs=1e-8)

    def test_cos_root(self):
        # f(x) = cos(x), root at x = pi/2 in [0, pi]
        f = lambda x: math.cos(x)
        solver = Bisection(tol=1e-10)
        result = solver.solve(f, 0.0, math.pi)
        assert result.converged is True
        assert result.root == pytest.approx(math.pi / 2, abs=1e-8)

    def test_no_sign_change_raises(self):
        f = lambda x: x**2 + 1  # Always positive
        solver = Bisection()
        with pytest.raises(ValueError, match="opposite signs"):
            solver.solve(f, 0.0, 5.0)

    def test_find_interval_positive(self):
        f = lambda x: x**2 - 4
        interval = Bisection.find_interval(f, start=0.0, step=0.5, max_steps=100)
        assert interval is not None
        a, b = interval
        assert f(a) * f(b) <= 0

    def test_find_interval_not_found(self):
        f = lambda x: x**2 + 1  # No real roots
        interval = Bisection.find_interval(f, start=0.0, step=0.5, max_steps=20)
        assert interval is None

    def test_iterations_positive(self):
        f = lambda x: x**2 - 4
        solver = Bisection(tol=1e-10)
        result = solver.solve(f, 1.0, 3.0)
        assert result.iterations > 0
