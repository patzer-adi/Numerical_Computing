"""
SolverResult dataclass.

Ported from C++ SolverResult.hpp. Encapsulates the result of a linear
system solve, providing the solution vector along with convergence metrics,
iteration counts, and decomposition matrices if applicable.
"""

from dataclasses import dataclass, field
from pynumerics.matrix import Matrix

@dataclass
class SolverResult:
    """The result of executing a numerical solver.
    
    Attributes:
        x: The solution vector.
        n: The size of the solution vector.
        iterations: Number of iterations taken (0 for direct methods).
        converged: Whether the solver converged to a solution.
        error: Maximum residual or difference error.
        dominance_achieved: Whether diagonal dominance was achieved.
        L: Lower triangular matrix (for LU decompositions).
        U: Upper triangular matrix (for LU decompositions).
    """
    x: list[float] = field(default_factory=list)
    n: int = 0
    iterations: int = 0
    converged: bool = False
    error: float = 0.0
    dominance_achieved: bool = False
    
    # LU decomposition matrices
    L: Matrix | None = None
    U: Matrix | None = None
