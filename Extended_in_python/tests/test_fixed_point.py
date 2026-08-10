"""
Test suite for Fixed-Point Iteration root-finding method.
"""

import pytest
import math
from pynumerics.roots.fixed_point import FixedPoint


class TestFixedPoint:
    def test_find_sqrt_4(self):
        # x = g(x) = (x + 4/x) / 2  converges to sqrt(4) = 2
        g = lambda x: (x + 4.0 / x) / 2.0
        solver = FixedPoint(tol=1e-10)
        result = solver.solve(g, 3.0)
        assert result.converged is True
        assert result.root == pytest.approx(2.0, abs=1e-8)

    def test_find_sqrt_2(self):
        # x = g(x) = (x + 2/x) / 2  converges to sqrt(2)
        g = lambda x: (x + 2.0 / x) / 2.0
        solver = FixedPoint(tol=1e-10)
        result = solver.solve(g, 1.0)
        assert result.converged is True
        assert result.root == pytest.approx(math.sqrt(2), abs=1e-8)

    def test_convergence_iterations(self):
        g = lambda x: (x + 4.0 / x) / 2.0
        solver = FixedPoint(tol=1e-10)
        result = solver.solve(g, 10.0)
        assert result.iterations > 0

    def test_divergence_detection(self):
        # g(x) = 2x diverges
        g = lambda x: 2.0 * x
        solver = FixedPoint(tol=1e-10, max_iter=100)
        result = solver.solve(g, 1.0)
        assert result.converged is False

    def test_max_iter_reached(self):
        # Very slow convergence with tiny max_iter
        g = lambda x: (x + 4.0 / x) / 2.0
        solver = FixedPoint(tol=1e-100, max_iter=3)
        result = solver.solve(g, 100.0)
        # Won't converge to 1e-100 in 3 iterations
        assert result.iterations <= 3
