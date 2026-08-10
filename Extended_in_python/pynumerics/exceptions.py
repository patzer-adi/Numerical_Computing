"""
Custom exception hierarchy for the PyNumerics library.

Ported from C++ MatrixException class. Provides descriptive error
messages for matrix operation failures.
"""


class MatrixException(Exception):
    """Base exception for all matrix-related errors.

    Mirrors the C++ MatrixException class with a message attribute.

    Args:
        message: Descriptive error message.
    """

    def __init__(self, message: str = "Matrix operation failed") -> None:
        self.message = message
        super().__init__(self.message)


class DimensionMismatchError(MatrixException):
    """Raised when matrix dimensions are incompatible for an operation.

    Examples:
        - Adding a 2×3 matrix to a 3×2 matrix
        - Multiplying matrices with incompatible inner dimensions
    """

    def __init__(self, message: str = "Matrix dimensions do not match") -> None:
        super().__init__(message)


class SingularMatrixError(MatrixException):
    """Raised when a matrix is singular (determinant = 0) and the operation requires invertibility.

    Examples:
        - Computing the inverse of a singular matrix
    """

    def __init__(self, message: str = "Matrix is singular (determinant = 0)") -> None:
        super().__init__(message)


class NotSquareError(MatrixException):
    """Raised when a square matrix is required but a non-square matrix is provided.

    Examples:
        - Computing the determinant of a non-square matrix
        - Computing the inverse of a non-square matrix
    """

    def __init__(self, message: str = "Matrix is not square") -> None:
        super().__init__(message)
