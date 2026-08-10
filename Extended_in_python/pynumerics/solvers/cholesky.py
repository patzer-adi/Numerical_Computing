"""
Cholesky LU Decomposition.

Ported from C++ Cholesky class. Decomposes A into L * L^T for symmetric
positive definite matrices.
"""

import math
from pynumerics.matrix import Matrix
from pynumerics.solvers.result import SolverResult
from pynumerics.solvers.lu_base import LUDecomposition
from pynumerics.exceptions import MatrixException


class Cholesky(LUDecomposition):
    """LU Decomposition using Cholesky algorithm (LL^T)."""

    def solve(self, b: list[float], max_iter: int = 10000, tol: float = 1e-10) -> SolverResult:
        """Solve Ax = b by computing LL^T, then substituting.
        
        Args:
            b: Right-hand side vector.
            max_iter: Ignored for direct methods.
            tol: Ignored for direct methods.
            
        Returns:
            SolverResult containing x, L, U (which is L^T), and error.
        """
        n = len(b)
        if self.rows != self.cols or self.rows != n:
            raise MatrixException("matrix dimensions don't match for Cholesky")

        if not self.is_symmetric():
            raise MatrixException(
                "matrix is not symmetric... Cholesky only works on symmetric positive "
                "definite matrices. try Doolittle or Crout instead"
            )

        L = Matrix(n, n)

        for i in range(n):
            for j in range(i + 1):
                s = sum(L[i, k] * L[j, k] for k in range(j))
                
                if i == j:
                    val = self._data[j][j] - s
                    if val <= 0.0:
                        raise MatrixException(
                            "matrix is not positive definite... Cholesky can't decompose "
                            "this. the diagonal went negative"
                        )
                    L[j, j] = math.sqrt(val)
                else:
                    if abs(L[j, j]) < 1e-12:
                        raise MatrixException("zero on L diagonal in Cholesky... rip")
                    L[i, j] = (self._data[i][j] - s) / L[j, j]

        # Explicitly build U as L^T for the SolverResult
        U = L.transpose()

        max_error = 0.0
        for i in range(n):
            for j in range(n):
                s = sum(L[i, k] * U[k, j] for k in range(n))
                err = abs(s - self._data[i][j])
                if err > max_error:
                    max_error = err

        # Ly = b
        y = [0.0] * n
        for i in range(n):
            s = sum(L[i, j] * y[j] for j in range(i))
            y[i] = (b[i] - s) / L[i, i]

        # L^T x = y (which is Ux = y)
        x = [0.0] * n
        for i in range(n - 1, -1, -1):
            s = sum(U[i, j] * x[j] for j in range(i + 1, n))
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
