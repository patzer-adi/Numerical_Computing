"""
Interpolation package for PyNumerics.
"""

from pynumerics.interpolation.base import Interpolation, FitResult
from pynumerics.interpolation.lagrange import Lagrange
from pynumerics.interpolation.newton_divided_difference import NewtonDividedDifference
from pynumerics.interpolation.least_square_line import LeastSquareLine
from pynumerics.interpolation.least_square_parabola import LeastSquareParabola
from pynumerics.interpolation.test_functions import (
    generate_data,
    FUNCTIONS,
    sin_func,
    cos_func,
    poly_func,
    exp_func,
    log_func,
)

__all__ = [
    "Interpolation",
    "FitResult",
    "Lagrange",
    "NewtonDividedDifference",
    "LeastSquareLine",
    "LeastSquareParabola",
    "generate_data",
    "FUNCTIONS",
    "sin_func",
    "cos_func",
    "poly_func",
    "exp_func",
    "log_func",
]
