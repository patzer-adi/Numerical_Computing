"""
Test suite for Gaussian Elimination solver.
"""

import pytest
from pynumerics.exceptions import SingularMatrixError, MatrixException
from pynumerics.solvers.gaussian import GaussianElimination


class TestGaussianElimination:
    def test_solve_2x2_with_pivoting(self):
        # 2x + y = 5
        # x - 3y = -1
        # Solution: x = 2, y = 1
        solver = GaussianElimination.from_list([
            [2.0, 1.0],
            [1.0, -3.0]
        ])
        b = [5.0, -1.0]
        
        result = solver.solve(b)
        assert result.converged is True
        assert result.x[0] == pytest.approx(2.0)
        assert result.x[1] == pytest.approx(1.0)

    def test_solve_3x3_with_pivoting(self):
        # x + y + z = 6
        # 2x + 5y - z = 27
        # 2x + y - z = 11
        # Solution: x = 3, y = 4, z = -1
        solver = GaussianElimination.from_list([
            [1.0, 1.0, 1.0],
            [2.0, 5.0, -1.0],
            [2.0, 1.0, -1.0]
        ])
        b = [6.0, 27.0, 11.0]
        
        result = solver.solve(b)
        assert result.x[0] == pytest.approx(3.0)
        assert result.x[1] == pytest.approx(4.0)
        assert result.x[2] == pytest.approx(-1.0)

    def test_solve_without_pivoting_success(self):
        solver = GaussianElimination.from_list([
            [2.0, 1.0],
            [1.0, -3.0]
        ])
        b = [5.0, -1.0]
        
        x = solver.solve_without_pivoting(b)
        assert x[0] == pytest.approx(2.0)
        assert x[1] == pytest.approx(1.0)

    def test_solve_without_pivoting_zero_pivot(self):
        # Zero on diagonal initially requires pivoting
        # 0x + y = 1
        # x + 0y = 2
        # Solution: x = 2, y = 1
        solver = GaussianElimination.from_list([
            [0.0, 1.0],
            [1.0, 0.0]
        ])
        b = [1.0, 2.0]
        
        # Without pivoting, should fail due to zero pivot
        with pytest.raises(MatrixException, match="zero pivot"):
            solver.solve_without_pivoting(b)
            
        # With pivoting (default solve), it should work perfectly
        result = solver.solve(b)
        assert result.x[0] == pytest.approx(2.0)
        assert result.x[1] == pytest.approx(1.0)

    def test_singular_matrix(self):
        # Linearly dependent rows
        # x + y = 1
        # 2x + 2y = 2
        solver = GaussianElimination.from_list([
            [1.0, 1.0],
            [2.0, 2.0]
        ])
        b = [1.0, 2.0]
        
        with pytest.raises(SingularMatrixError):
            solver.solve(b)

    def test_dimension_mismatch(self):
        solver = GaussianElimination.from_list([
            [1.0, 2.0],
            [3.0, 4.0]
        ])
        b = [1.0] # Only 1 element, expecting 2
        
        with pytest.raises(MatrixException):
            solver.solve(b)
            
    def test_non_square_system(self):
        solver = GaussianElimination(2, 3) # Non-square
        b = [1.0, 2.0]
        with pytest.raises(MatrixException):
            solver.solve(b)
