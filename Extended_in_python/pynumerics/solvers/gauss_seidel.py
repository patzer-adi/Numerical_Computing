"""
Gauss-Seidel iterative solver.

Ported from C++ GaussSeidel class. Like Jacobi but uses updated x values
as soon as they're computed (in-place updates within each iteration).
"""

import math
from pynumerics.solvers.base import SystemOfLinearEquationSolver
from pynumerics.solvers.result import SolverResult
from pynumerics.exceptions import MatrixException


class GaussSeidel(SystemOfLinearEquationSolver):
    """Gauss-Seidel iterative solver for Ax = b.

    Like Jacobi, but uses already-updated x[j] values for j < i
    within each iteration step, achieving faster convergence.
    """

    def solve(self, b: list[float], max_iter: int = 10000, tol: float = 1e-10) -> SolverResult:
        n = len(b)
        if self.rows != self.cols or self.rows != n:
            raise MatrixException("matrix dimensions are sus for Gauss-Seidel")

        result = SolverResult(n=n, iterations=0, converged=False, error=0.0, dominance_achieved=True)

        A = [row[:] for row in self._data]
        rhs = list(b)

        if not self.is_diagonally_dominant():
            result.dominance_achieved = SystemOfLinearEquationSolver.make_diag_dominant(A, rhs)

        for i in range(n):
            if abs(A[i][i]) < 1e-12:
                raise MatrixException("zero on diagonal... Gauss-Seidel can't work with this")

        x = [0.0] * n
        last_max_diff = 0.0

        for iteration in range(max_iter):
            max_diff = 0.0
            diverged = False

            for i in range(n):
                # Use already-updated x[j] for j < i (Seidel difference!)
                s = sum(A[i][j] * x[j] for j in range(n) if j != i)
                new_val = (rhs[i] - s) / A[i][i]

                if math.isnan(new_val) or math.isinf(new_val):
                    diverged = True
                    break

                diff = abs(new_val - x[i])
                if diff > max_diff:
                    max_diff = diff

                x[i] = new_val  # Update immediately (Seidel style)

            last_max_diff = max_diff

            if diverged:
                result.error = float('inf')
                result.x = list(x)
                result.iterations = iteration + 1
                result.converged = False
                return result

            if max_diff < tol:
                result.iterations = iteration + 1
                result.converged = True
                result.error = max_diff
                break

        if not result.converged:
            result.iterations = max_iter
            result.error = last_max_diff

        result.x = list(x)
        return result
