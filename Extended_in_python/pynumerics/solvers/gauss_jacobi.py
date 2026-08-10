"""
Gauss-Jacobi iterative solver.

Ported from C++ GaussJacobi class. Solves Ax = b iteratively using
only old values at each step. Convergence requires diagonal dominance.
"""

import math
from pynumerics.solvers.base import SystemOfLinearEquationSolver
from pynumerics.solvers.result import SolverResult
from pynumerics.exceptions import MatrixException


class GaussJacobi(SystemOfLinearEquationSolver):
    """Gauss-Jacobi iterative solver for Ax = b.

    Splits A into D (diagonal) and R (remainder).
    x_new[i] = (b[i] - sum(A[i][j]*x_old[j] for j!=i)) / A[i][i]
    Uses ONLY old values per iteration — does NOT overwrite in-place.
    """

    def solve(self, b: list[float], max_iter: int = 10000, tol: float = 1e-10) -> SolverResult:
        """Solve Ax = b using Jacobi iteration.

        Args:
            b: Right-hand side vector.
            max_iter: Maximum number of iterations allowed.
            tol: Convergence tolerance (max absolute difference between iterations).

        Returns:
            SolverResult with solution vector and convergence metrics.
        """
        n = len(b)
        if self.rows != self.cols or self.rows != n:
            raise MatrixException("matrix dimensions are sus for Gauss-Jacobi")

        result = SolverResult(n=n, iterations=0, converged=False, error=0.0, dominance_achieved=True)

        # Work on copies so the original matrix/b are not mutated
        A = [row[:] for row in self._data]
        rhs = list(b)

        # Try to enforce diagonal dominance via row swaps
        if not self.is_diagonally_dominant():
            result.dominance_achieved = SystemOfLinearEquationSolver.make_diag_dominant(A, rhs)

        # Check for zero diagonals after potential reordering
        for i in range(n):
            if abs(A[i][i]) < 1e-12:
                raise MatrixException("zero on diagonal... Jacobi can't work with this")

        # Initial guess: x = 0
        x_old = [0.0] * n
        x_new = [0.0] * n

        last_max_diff = 0.0

        for iteration in range(max_iter):
            diverged = False

            # Compute ALL x_new from x_old (use only old values)
            for i in range(n):
                s = sum(A[i][j] * x_old[j] for j in range(n) if j != i)
                x_new[i] = (rhs[i] - s) / A[i][i]

                # Check for NaN/Inf divergence
                if math.isnan(x_new[i]) or math.isinf(x_new[i]):
                    diverged = True
                    break

            if diverged:
                result.error = float('inf')
                result.x = list(x_new)
                result.iterations = iteration + 1
                result.converged = False
                return result

            # Check convergence
            max_diff = max(abs(x_new[i] - x_old[i]) for i in range(n))
            last_max_diff = max_diff

            # Copy new → old
            x_old = list(x_new)

            if max_diff < tol:
                result.iterations = iteration + 1
                result.converged = True
                result.error = max_diff
                break

        if not result.converged:
            result.iterations = max_iter
            result.error = last_max_diff

        result.x = list(x_new)
        return result
