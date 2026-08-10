"""
Gershgorin Circle Theorem Analyzer.

Ported from C++ GershgorinAnalyzer. Computes Gershgorin discs for each row
of a square matrix. Every eigenvalue lies inside at least one disc.
"""

from pynumerics.matrix import Matrix
from pynumerics.eigen.base import EigenSolver, GershgorinDisc
from pynumerics.exceptions import MatrixException


class GershgorinAnalyzer(EigenSolver):
    """Gershgorin Circle Theorem implementation."""

    def __init__(self, matrix: Matrix):
        super().__init__(matrix)
        self.discs: list[GershgorinDisc] = []

    def solve(self) -> list[GershgorinDisc]:
        """Compute Gershgorin discs — one per row."""
        n = self.mat.rows
        if n != self.mat.cols:
            raise MatrixException("Gershgorin analysis requires a square matrix")

        self.discs = []
        for i in range(n):
            center = self.mat[i, i]
            radius = sum(abs(self.mat[i, j]) for j in range(n) if j != i)
            self.discs.append(GershgorinDisc(
                row=i,
                center=center,
                radius=radius,
                low=center - radius,
                high=center + radius
            ))
        return self.discs

    def get_analysis(self) -> dict:
        """Return a dictionary with full Gershgorin analysis."""
        if not self.discs:
            self.solve()

        union_low = min(d.low for d in self.discs)
        union_high = max(d.high for d in self.discs)

        all_positive = all(d.low > 0 for d in self.discs)
        all_negative = all(d.high < 0 for d in self.discs)
        could_be_zero = any(d.low <= 0 <= d.high for d in self.discs)
        all_dominant = all(abs(d.center) > d.radius for d in self.discs)

        return {
            "discs": self.discs,
            "union_interval": (union_low, union_high),
            "all_eigenvalues_positive": all_positive,
            "all_eigenvalues_negative": all_negative,
            "could_be_singular": could_be_zero,
            "diagonally_dominant": all_dominant,
            "symmetric": self.mat.is_symmetric(),
        }

    def get_disc(self, i: int) -> GershgorinDisc:
        if not self.discs:
            self.solve()
        if i < 0 or i >= len(self.discs):
            raise MatrixException("disc index out of bounds in get_disc()")
        return self.discs[i]
