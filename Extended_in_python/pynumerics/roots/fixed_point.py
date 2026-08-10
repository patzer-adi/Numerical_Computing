"""
Fixed-Point Iteration method for root finding.

Ported from C++ FixedPoint class. Rewrites f(x) = 0 as x = g(x)
and iterates x_{n+1} = g(x_n).
"""

import math
from typing import Callable
from pynumerics.roots.base import RootHunter
from pynumerics.roots.result import RootResult


class FixedPoint(RootHunter):
    """Fixed-point iteration root-finding method."""

    def solve(self, g: Callable[[float], float], x0: float) -> RootResult:
        """Find a fixed point x = g(x) starting from x0.

        Args:
            g: The iteration function (derived from rewriting f(x)=0 as x=g(x)).
            x0: Initial guess.

        Returns:
            RootResult containing the approximate root and metrics.
        """
        previous = x0
        current = g(previous)
        iterations = 1

        while abs(current - previous) > self.tolerance and iterations < self.max_iter:
            previous = current
            current = g(previous)
            iterations += 1

            # Divergence detection
            if math.isnan(current) or math.isinf(current):
                return RootResult(
                    root=current,
                    iterations=iterations,
                    converged=False,
                    error=float('inf')
                )

        return RootResult(
            root=current,
            iterations=iterations,
            converged=abs(current - previous) <= self.tolerance,
            error=abs(current - previous)
        )
