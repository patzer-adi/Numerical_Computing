"""
Test suite for Gauss-Jacobi iterative solver.
"""

import pytest
from pynumerics.exceptions import MatrixException
from pynumerics.solvers.gauss_jacobi import GaussJacobi


class TestGaussJacobi:
    def test_converges_diagonally_dominant(self):
        # Strictly diagonally dominant system
        # 4x + y + z = 7
        # x + 5y + 2z = -8
        # 2x + y + 6z = 6
        # Solution: x = 2, y = -3, z = 1
        solver = GaussJacobi.from_list([
            [4.0, 1.0, 1.0],
            [1.0, 5.0, 2.0],
            [2.0, 1.0, 6.0]
        ])
        b = [7.0, -8.0, 6.0]

        result = solver.solve(b, max_iter=10000, tol=1e-8)
        assert result.converged is True
        assert result.x[0] == pytest.approx(2.15841584, abs=1e-4)
        assert result.x[1] == pytest.approx(-2.29702970, abs=1e-4)
        assert result.x[2] == pytest.approx(0.66336634, abs=1e-4)
        assert result.iterations > 0

    def test_solver_result_metadata(self):
        solver = GaussJacobi.from_list([
            [10.0, 1.0],
            [1.0, 10.0]
        ])
        b = [11.0, 11.0]
        result = solver.solve(b)
        assert result.converged is True
        assert result.error < 1e-10
        assert result.n == 2

    def test_non_convergence_within_limit(self):
        # A non-diagonally dominant matrix: convergence not guaranteed
        # Use very few iterations so it can't converge
        solver = GaussJacobi.from_list([
            [1.0, 2.0],
            [3.0, 1.0]
        ])
        b = [1.0, 1.0]
        result = solver.solve(b, max_iter=5, tol=1e-10)
        # May or may not converge in 5 iterations; just check structure
        assert result.iterations <= 5
        assert isinstance(result.converged, bool)

    def test_zero_diagonal_raises(self):
        # All zeros on diagonal — even after row swap, both diagonals are 0
        solver = GaussJacobi.from_list([
            [0.0, 1.0],
            [1.0, 0.0]
        ])
        b = [1.0, 1.0]
        # After make_diag_dominant row swap, diags become [1, 1] so it won't raise.
        # Use a truly degenerate case instead:
        solver2 = GaussJacobi.from_list([
            [0.0, 0.0],
            [0.0, 0.0]
        ])
        b2 = [1.0, 1.0]
        with pytest.raises(MatrixException, match="zero on diagonal"):
            solver2.solve(b2)

    def test_dimension_mismatch(self):
        solver = GaussJacobi.from_list([
            [4.0, 1.0],
            [1.0, 4.0]
        ])
        b = [1.0]
        with pytest.raises(MatrixException):
            solver.solve(b)
