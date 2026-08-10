"""
Gaussian Elimination solver.

Ported from C++ GaussianElimination class. Solves Ax = b using row reduction.
Provides methods with and without partial pivoting.
"""

from pynumerics.solvers.base import SystemOfLinearEquationSolver
from pynumerics.solvers.result import SolverResult
from pynumerics.exceptions import MatrixException, SingularMatrixError


class GaussianElimination(SystemOfLinearEquationSolver):
    """Gaussian Elimination solver for Ax = b."""

    def solve(self, b: list[float], max_iter: int = 10000, tol: float = 1e-10) -> SolverResult:
        """Default solve method. Uses partial pivoting.
        
        Args:
            b: Right-hand side vector.
            max_iter: Ignored for direct methods.
            tol: Ignored for direct methods.
            
        Returns:
            SolverResult with the solution vector.
        """
        x = self.solve_with_pivoting(b)
        return SolverResult(
            x=x,
            n=len(x),
            iterations=0,
            converged=True,
            dominance_achieved=True,
            error=0.0
        )

    def solve_with_pivoting(self, b: list[float]) -> list[float]:
        """Solve using Gaussian elimination with partial pivoting."""
        n = len(b)
        if self.rows != self.cols or self.rows != n:
            raise MatrixException("Matrix dimensions must be square and match vector length.")

        # Create working copies
        A = [row[:] for row in self._data]
        rhs = list(b)

        # Forward elimination
        for k in range(n):
            # Find pivot
            max_row = k
            max_val = abs(A[k][k])
            for i in range(k + 1, n):
                if abs(A[i][k]) > max_val:
                    max_val = abs(A[i][k])
                    max_row = i

            if max_val < 1e-12:
                raise SingularMatrixError("Matrix is singular (zero pivot encountered).")

            # Swap rows if needed
            if max_row != k:
                A[k], A[max_row] = A[max_row], A[k]
                rhs[k], rhs[max_row] = rhs[max_row], rhs[k]

            # Eliminate below
            for i in range(k + 1, n):
                factor = A[i][k] / A[k][k]
                for j in range(k, n):
                    A[i][j] -= factor * A[k][j]
                rhs[i] -= factor * rhs[k]

        # Back substitution
        x = [0.0] * n
        for i in range(n - 1, -1, -1):
            s = rhs[i]
            for j in range(i + 1, n):
                s -= A[i][j] * x[j]
            if abs(A[i][i]) < 1e-12:
                raise SingularMatrixError("Zero on diagonal during back substitution.")
            x[i] = s / A[i][i]

        return x

    def solve_without_pivoting(self, b: list[float]) -> list[float]:
        """Solve using basic Gaussian elimination (no row swaps).
        
        May fail on matrices that require pivoting even if they are invertible.
        """
        n = len(b)
        if self.rows != self.cols or self.rows != n:
            raise MatrixException("Matrix dimensions must be square and match vector length.")

        # Create working copies
        A = [row[:] for row in self._data]
        rhs = list(b)

        # Forward elimination
        for k in range(n):
            if abs(A[k][k]) < 1e-12:
                raise MatrixException("zero pivot found without pivoting... shoulda used pivoting bro")

            # Eliminate below
            for i in range(k + 1, n):
                factor = A[i][k] / A[k][k]
                for j in range(k, n):
                    A[i][j] -= factor * A[k][j]
                rhs[i] -= factor * rhs[k]

        # Back substitution
        x = [0.0] * n
        for i in range(n - 1, -1, -1):
            s = rhs[i]
            for j in range(i + 1, n):
                s -= A[i][j] * x[j]
            if abs(A[i][i]) < 1e-12:
                raise SingularMatrixError("zero diagonal in back sub without pivoting")
            x[i] = s / A[i][i]

        return x
