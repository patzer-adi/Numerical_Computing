"""
Bisection method for root finding.

Ported from C++ Bisection class. Brackets a root in [a, b] where
f(a)*f(b) < 0, then iteratively halves the interval.
"""

import math
from typing import Callable
from pynumerics.roots.base import RootHunter
from pynumerics.roots.result import RootResult


class Bisection(RootHunter):
    """Bisection root-finding method."""

    def solve(self, f: Callable[[float], float], a: float, b: float) -> RootResult:
        """Find a root of f in [a, b] using the bisection method.

        Args:
            f: The function whose root is sought.
            a: Left endpoint of the interval.
            b: Right endpoint of the interval.

        Returns:
            RootResult containing the approximate root and metrics.

        Raises:
            ValueError: If f(a) and f(b) have the same sign.
        """
        fa, fb = f(a), f(b)
        if fa * fb > 0:
            raise ValueError(
                f"f(a) and f(b) must have opposite signs. "
                f"Got f({a})={fa}, f({b})={fb}"
            )

        iterations = 0

        while (b - a) > self.tolerance and iterations < self.max_iter:
            mid = (a + b) / 2.0
            fmid = f(mid)

            if fmid == 0.0:
                # Exact root found
                return RootResult(
                    root=mid,
                    iterations=iterations + 1,
                    converged=True,
                    error=0.0,
                    interval=(a, b)
                )

            if fa * fmid < 0:
                b = mid
                fb = fmid
            else:
                a = mid
                fa = fmid

            iterations += 1

        mid = (a + b) / 2.0
        return RootResult(
            root=mid,
            iterations=iterations,
            converged=(b - a) <= self.tolerance,
            error=b - a,
            interval=(a, b)
        )

    @staticmethod
    def find_interval(
        f: Callable[[float], float],
        start: float = 0.0,
        step: float = 0.5,
        max_steps: int = 100
    ) -> tuple[float, float] | None:
        """Search for an interval [a, b] where f(a)*f(b) < 0.

        Searches both positive and negative directions from start.

        Args:
            f: The function.
            start: Starting point for the search.
            step: Step size.
            max_steps: Maximum number of steps to try.

        Returns:
            A tuple (a, b) where f(a)*f(b) < 0, or None if not found.
        """
        # Positive direction
        left = start
        for _ in range(max_steps):
            right = left + step
            if f(left) * f(right) <= 0:
                return (left, right)
            left = right

        # Negative direction
        left = start
        for _ in range(max_steps):
            right = left - step
            if f(left) * f(right) <= 0:
                return (min(left, right), max(left, right))
            left = right

        return None

