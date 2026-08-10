"""
Interpolation package for PyNumerics.
"""

from pynumerics.interpolation.base import Interpolation, FitResult
from pynumerics.interpolation.lagrange import Lagrange
from pynumerics.interpolation.least_square_line import LeastSquareLine
from pynumerics.interpolation.least_square_parabola import LeastSquareParabola

__all__ = [
    "Interpolation",
    "FitResult",
    "Lagrange",
    "LeastSquareLine",
    "LeastSquareParabola",
]
