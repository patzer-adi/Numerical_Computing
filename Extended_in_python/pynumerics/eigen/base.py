"""
Gershgorin disc and EigenSolver base.
"""

from dataclasses import dataclass
from abc import ABC, abstractmethod
from pynumerics.matrix import Matrix


@dataclass
class GershgorinDisc:
    """Represents one Gershgorin disc for a single matrix row."""
    row: int
    center: float
    radius: float
    low: float
    high: float


class EigenSolver(ABC):
    """Abstract base class for eigenvalue analysis methods.

    Uses COMPOSITION — holds a Matrix reference, does NOT inherit Matrix.
    """

    def __init__(self, matrix: Matrix):
        self.mat = matrix

    @abstractmethod
    def solve(self) -> list[GershgorinDisc]:
        pass

    @abstractmethod
    def get_analysis(self) -> dict:
        pass
