"""
Test suite for Gauss-Seidel iterative solver.
"""

import pytest
from pynumerics.exceptions import MatrixException
from pynumerics.solvers.gauss_seidel import GaussSeidel


class TestGaussSeidel:
    def test_converges_diagonally_dominant(self):
        solver = GaussSeidel.from_list([
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

    def test_faster_than_jacobi(self):
        """Seidel should converge in fewer iterations than Jacobi for same system."""
        from pynumerics.solvers.gauss_jacobi import GaussJacobi
        A = [[10.0, 1.0, 1.0], [1.0, 10.0, 1.0], [1.0, 1.0, 10.0]]
        b = [12.0, 12.0, 12.0]

        jacobi = GaussJacobi.from_list(A)
        seidel = GaussSeidel.from_list(A)

        r_j = jacobi.solve(b, tol=1e-10)
        r_s = seidel.solve(b, tol=1e-10)

        assert r_j.converged and r_s.converged
        assert r_s.iterations <= r_j.iterations

    def test_zero_diagonal_raises(self):
        solver = GaussSeidel.from_list([[0.0, 0.0], [0.0, 0.0]])
        with pytest.raises(MatrixException, match="zero on diagonal"):
            solver.solve([1.0, 1.0])

    def test_dimension_mismatch(self):
        solver = GaussSeidel.from_list([[4.0, 1.0], [1.0, 4.0]])
        with pytest.raises(MatrixException):
            solver.solve([1.0])

    def test_solver_metadata(self):
        solver = GaussSeidel.from_list([[10.0, 1.0], [1.0, 10.0]])
        result = solver.solve([11.0, 11.0])
        assert result.converged is True
        assert result.n == 2
        assert result.error < 1e-10
