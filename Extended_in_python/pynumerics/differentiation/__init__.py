"""
Numerical Differentiation package for PyNumerics.
"""

from pynumerics.differentiation.base import Differentiation, FunctionEntry, DiffResult
from pynumerics.differentiation.forward import ForwardDifference
from pynumerics.differentiation.backward import BackwardDifference
from pynumerics.differentiation.central import CentralDifference
from pynumerics.differentiation.richardson import RichardsonExtrapolation

__all__ = [
    "Differentiation",
    "FunctionEntry",
    "DiffResult",
    "ForwardDifference",
    "BackwardDifference",
    "CentralDifference",
    "RichardsonExtrapolation",
]
