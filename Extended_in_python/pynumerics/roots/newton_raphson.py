"""
Newton-Raphson method for root finding.

Ported from C++ NewtonRaphson class. Uses the derivative:
x_{n+1} = x_n - f(x_n) / f'(x_n)
"""

import math
from typing import Callable
from pynumerics.roots.base import RootHunter
from pynumerics.roots.result import RootResult


class NewtonRaphson(RootHunter):
    """Newton-Raphson root-finding method."""

    def solve(
        self,
        f: Callable[[float], float],
        df: Callable[[float], float],
        x0: float
    ) -> RootResult:
        """Find a root of f starting from initial guess x0.

        Args:
            f: The function whose root is sought.
            df: The derivative of f.
            x0: Initial guess.

        Returns:
            RootResult containing the approximate root and metrics.

        Raises:
            ValueError: If the derivative is near-zero during iteration.
        """
        previous = x0
        dfp = df(previous)
        if abs(dfp) < 1e-14:
            raise ValueError("Derivative is zero at initial guess. Newton-Raphson cannot proceed.")

        current = previous - f(previous) / dfp
        iterations = 1

        while abs(current - previous) > self.tolerance and iterations < self.max_iter:
            previous = current
            dfp = df(previous)
            if abs(dfp) < 1e-14:
                raise ValueError(
                    f"Derivative is near-zero at x={previous}. Newton-Raphson cannot proceed."
                )
            current = previous - f(previous) / dfp
            iterations += 1

        return RootResult(
            root=current,
            iterations=iterations,
            converged=abs(current - previous) <= self.tolerance,
            error=abs(current - previous)
        )
