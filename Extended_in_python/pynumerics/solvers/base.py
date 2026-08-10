"""
Base class for linear system solvers.

Ported from C++ SystemOfLinearEquationSolver class. All solvers
inherit from this class and Matrix.
"""

from abc import ABC, abstractmethod
from pynumerics.matrix import Matrix
from pynumerics.solvers.result import SolverResult


class SystemOfLinearEquationSolver(Matrix, ABC):
    """Abstract base class for all linear equation solvers (GE, LU, Jacobi, Seidel).
    
    Inherits from Matrix, meaning the solver instance *is* the coefficient matrix A.
    """

    @abstractmethod
    def solve(self, b: list[float], max_iter: int = 10000, tol: float = 1e-10) -> SolverResult:
        """Solve the system Ax = b.
        
        Args:
            b: The right-hand side constant vector.
            max_iter: Maximum iterations (ignored for direct methods).
            tol: Tolerance for convergence (ignored for direct methods).
            
        Returns:
            SolverResult containing the solution vector and metrics.
        """
        pass

    @staticmethod
    def check_diag_dominant(a_data: list[list[float]]) -> bool:
        """Check if a raw 2D list is strictly diagonally dominant."""
        n = len(a_data)
        for i in range(n):
            diag = abs(a_data[i][i])
            off_sum = sum(abs(a_data[i][j]) for j in range(n) if j != i)
            if diag < off_sum:
                return False
        return True

    @staticmethod
    def make_diag_dominant(a_data: list[list[float]], b: list[float]) -> bool:
        """Attempt to make the system diagonally dominant via row swaps.
        
        Modifies a_data and b in-place.
        
        Returns:
            True if the resulting system is diagonally dominant, False otherwise.
        """
        n = len(a_data)
        for i in range(n):
            diag = abs(a_data[i][i])
            off_sum = sum(abs(a_data[i][j]) for j in range(n) if j != i)
            if diag >= off_sum:
                continue  # Already dominant at this row
                
            # Find the best row to swap with (partial pivoting style)
            best_row = -1
            best_diag = -1.0
            for r in range(i + 1, n):
                d = abs(a_data[r][i])
                s = sum(abs(a_data[r][j]) for j in range(n) if j != i)
                if d >= s and d > best_diag:
                    best_diag = d
                    best_row = r
                    
            if best_row != -1:
                # Swap rows in A
                a_data[i], a_data[best_row] = a_data[best_row], a_data[i]
                # Swap corresponding elements in b
                b[i], b[best_row] = b[best_row], b[i]
                
        return SystemOfLinearEquationSolver.check_diag_dominant(a_data)
