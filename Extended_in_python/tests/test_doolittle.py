"""
Test suite for LU Decomposition (Doolittle method).
"""

import pytest
from pynumerics.exceptions import MatrixException
from pynumerics.matrix import Matrix
from pynumerics.solvers.doolittle import Doolittle


class TestDoolittle:
    def test_solve_3x3(self):
        # A = [[2, -1, -2],
        #      [-4, 6, 3],
        #      [-4, -2, 8]]
        # b = [-1, 13, -6]
        # Solution: x = [2, 3, 1]
        solver = Doolittle.from_list([
            [2.0, -1.0, -2.0],
            [-4.0, 6.0, 3.0],
            [-4.0, -2.0, 8.0]
        ])
        b = [-1.0, 13.0, -6.0]
        
        result = solver.solve(b)
        assert result.converged is True
        assert result.x[0] == pytest.approx(2.0)
        assert result.x[1] == pytest.approx(3.0)
        assert result.x[2] == pytest.approx(1.0)
        
        # Verify L matrix
        # L should have 1s on diagonal
        assert result.L[0, 0] == 1.0
        assert result.L[1, 1] == 1.0
        assert result.L[2, 2] == 1.0
        
        # Verify L * U == A
        reconstructed = result.L * result.U
        for i in range(3):
            for j in range(3):
                assert reconstructed[i, j] == pytest.approx(solver[i, j])

    def test_error_calculation(self):
        solver = Doolittle.from_list([
            [1.0, 2.0],
            [3.0, 4.0]
        ])
        b = [1.0, 1.0]
        result = solver.solve(b)
        # error should be extremely small
        assert result.error < 1e-10

    def test_zero_pivot(self):
        # A matrix requiring pivoting will fail in basic Doolittle
        solver = Doolittle.from_list([
            [0.0, 1.0],
            [1.0, 0.0]
        ])
        b = [1.0, 2.0]
        
        with pytest.raises(MatrixException, match="zero pivot"):
            solver.solve(b)

    def test_dimension_mismatch(self):
        solver = Doolittle.from_list([
            [1.0, 2.0],
            [3.0, 4.0]
        ])
        b = [1.0] # Only 1 element
        
        with pytest.raises(MatrixException):
            solver.solve(b)
