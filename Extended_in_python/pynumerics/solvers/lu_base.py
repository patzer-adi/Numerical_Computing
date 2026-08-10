"""
LU Decomposition base class.
"""

from abc import ABC
from pynumerics.solvers.base import SystemOfLinearEquationSolver

class LUDecomposition(SystemOfLinearEquationSolver, ABC):
    """Abstract base class for all LU Decomposition solvers."""
    pass
