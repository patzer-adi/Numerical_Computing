"""
RootHunter — abstract base class for all root-finding methods.

Ported from C++ RootHunter class.
"""

from abc import ABC, abstractmethod
from typing import Callable
from pynumerics.roots.result import RootResult


class RootHunter(ABC):
    """Abstract base class for root-finding algorithms."""

    def __init__(self, tol: float = 1e-10, max_iter: int = 10000):
        self.tolerance = tol
        self.max_iter = max_iter

    @abstractmethod
    def solve(self, *args, **kwargs) -> RootResult:
        """Find a root. Signature varies by subclass."""
        pass
