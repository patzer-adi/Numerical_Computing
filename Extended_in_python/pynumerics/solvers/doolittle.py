"""
Doolittle LU Decomposition.

Ported from C++ Doolittle class. Decomposes A into L (unit diagonal) and U.
"""

from pynumerics.matrix import Matrix
from pynumerics.solvers.result import SolverResult
from pynumerics.solvers.lu_base import LUDecomposition
from pynumerics.exceptions import MatrixException


class Doolittle(LUDecomposition):
    """LU Decomposition using the Doolittle algorithm (L has unit diagonal)."""

    def solve(self, b: list[float], max_iter: int = 10000, tol: float = 1e-10) -> SolverResult:
        """Solve Ax = b by computing L and U, then substituting.
        
        Args:
            b: Right-hand side vector.
            max_iter: Ignored for direct methods.
            tol: Ignored for direct methods.
            
        Returns:
            SolverResult containing x, L, U, and maximum reconstruction error.
        """
        n = len(b)
        if self.rows != self.cols or self.rows != n:
            raise MatrixException("matrix dimensions don't match for Doolittle LU")

        # Initialize L and U matrices
        L = Matrix(n, n)
        U = Matrix(n, n)
        
        for i in range(n):
            L[i, i] = 1.0

        # Decomposition
        for k in range(n):
            # Upper Triangular
            for j in range(k, n):
                s = sum(L[k, idx] * U[idx, j] for idx in range(k))
                U[k, j] = self._data[k][j] - s
                
            # Lower Triangular
            for i in range(k + 1, n):
                s = sum(L[i, idx] * U[idx, k] for idx in range(k))
                if abs(U[k, k]) < 1e-12:
                    raise MatrixException("zero pivot in Doolittle... matrix might be singular bro")
                L[i, k] = (self._data[i][k] - s) / U[k, k]

        # Calculate max |L*U - A| error
        max_error = 0.0
        for i in range(n):
            for j in range(n):
                s = sum(L[i, k] * U[k, j] for k in range(n))
                err = abs(s - self._data[i][j])
                if err > max_error:
                    max_error = err

        # Forward substitution: Ly = b
        y = [0.0] * n
        for i in range(n):
            s = sum(L[i, j] * y[j] for j in range(i))
            y[i] = b[i] - s

        # Back substitution: Ux = y
        x = [0.0] * n
        for i in range(n - 1, -1, -1):
            s = sum(U[i, j] * x[j] for j in range(i + 1, n))
            if abs(U[i, i]) < 1e-12:
                raise MatrixException("zero on U diagonal in back sub... rip")
            x[i] = (y[i] - s) / U[i, i]

        return SolverResult(
            x=x,
            n=n,
            iterations=0,
            converged=True,
            dominance_achieved=True,
            error=max_error,
            L=L,
            U=U
        )
