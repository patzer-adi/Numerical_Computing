"""
PyNumerics — A modular Python library for numerical computing.

Ported from the C++ Numerical_Computing library. Provides:
- Complex number arithmetic with full operator overloading
- Matrix algebra with arithmetic, transpose, and I/O
- Custom exception hierarchy for descriptive error messages
- Linear system solvers (planned)
- Root-finding algorithms (planned)
"""

from pynumerics.complex import Complex
from pynumerics.matrix import Matrix
from pynumerics.exceptions import (
    MatrixException,
    DimensionMismatchError,
    SingularMatrixError,
    NotSquareError,
)

__all__ = [
    "Complex",
    "Matrix",
    "MatrixException",
    "DimensionMismatchError",
    "SingularMatrixError",
    "NotSquareError",
]
__version__ = "0.2.0"
