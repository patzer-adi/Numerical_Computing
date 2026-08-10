"""
Test suite for LU Decomposition (Cholesky method).
"""

import pytest
from pynumerics.exceptions import MatrixException
from pynumerics.matrix import Matrix
from pynumerics.solvers.cholesky import Cholesky


class TestCholesky:
    def test_solve_3x3_positive_definite(self):
        # A = [[4, 12, -16],
        #      [12, 37, -43],
        #      [-16, -43, 98]]
        # b = [1, 2, 3]
        # Solution: x approx [33.833, -9.166, -1.833]
        solver = Cholesky.from_list([
            [4.0, 12.0, -16.0],
            [12.0, 37.0, -43.0],
            [-16.0, -43.0, 98.0]
        ])
        b = [1.0, 2.0, 3.0]
        
        result = solver.solve(b)
        assert result.converged is True
        
        # Verify L * L^T == A
        # Since U is L^T, L * U should be exactly A
        reconstructed = result.L * result.U
        for i in range(3):
            for j in range(3):
                assert reconstructed[i, j] == pytest.approx(solver[i, j])
                
        # U should literally be the transpose of L
        for i in range(3):
            for j in range(3):
                assert result.U[i, j] == result.L[j, i]

    def test_not_symmetric(self):
        solver = Cholesky.from_list([
            [1.0, 2.0],
            [3.0, 4.0]
        ])
        b = [1.0, 1.0]
        with pytest.raises(MatrixException, match="not symmetric"):
            solver.solve(b)

    def test_not_positive_definite(self):
        # Symmetric but not positive definite (negative determinant or eigenvalue)
        solver = Cholesky.from_list([
            [1.0, 2.0],
            [2.0, 1.0]
        ])
        b = [1.0, 1.0]
        with pytest.raises(MatrixException, match="not positive definite"):
            solver.solve(b)

    def test_dimension_mismatch(self):
        solver = Cholesky.from_list([
            [4.0, 2.0],
            [2.0, 4.0]
        ])
        b = [1.0]
        
        with pytest.raises(MatrixException):
            solver.solve(b)
