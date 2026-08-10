"""
Matrix class with arithmetic, transpose, I/O, and operator overloading.

Ported from C++ Matrix class (matrix_class/include/Matrix.hpp,
src/Matrix.cpp, src/MatrixOperations.cpp). Provides core matrix
operations without external dependencies.

Usage:
    >>> from pynumerics.matrix import Matrix
    >>> a = Matrix.from_list([[1, 2], [3, 4]])
    >>> b = Matrix.from_list([[5, 6], [7, 8]])
    >>> print(a + b)
    >>> print(a * b)
    >>> print(a.transpose())
"""

from __future__ import annotations

import copy
import math
from pynumerics.exceptions import (
    MatrixException,
    DimensionMismatchError,
    SingularMatrixError,
    NotSquareError,
)


class Matrix:
    """A matrix of floating-point numbers with arithmetic operations.

    Mirrors the C++ Matrix class. Storage is a list of lists (row-major).

    Attributes:
        rows: Number of rows.
        cols: Number of columns.
    """

    def __init__(self, rows: int, cols: int) -> None:
        """Create a rows × cols zero matrix.

        Args:
            rows: Number of rows.
            cols: Number of columns.

        Examples:
            >>> Matrix(2, 3)  # 2×3 zero matrix
        """
        self.rows = rows
        self.cols = cols
        self._data: list[list[float]] = [
            [0.0] * cols for _ in range(rows)
        ]

    # ── @classmethod constructors ──────────────────────────────

    @classmethod
    def from_list(cls, data: list[list[float]]) -> Matrix:
        """Create a matrix from a 2D list.

        Args:
            data: 2D list of numbers. Each inner list is a row.

        Returns:
            Matrix with the given data (deep-copied).

        Examples:
            >>> Matrix.from_list([[1, 2], [3, 4]])
        """
        rows = len(data)
        cols = len(data[0]) if rows > 0 else 0
        m = cls(rows, cols)
        for i in range(rows):
            for j in range(cols):
                m._data[i][j] = float(data[i][j])
        return m

    @classmethod
    def from_file(cls, filename: str) -> Matrix:
        """Load a matrix from a space-separated text file.

        Auto-detects format: if the first line has exactly 2 numbers
        and the second line has more, treats the first line as a
        "rows cols" header. Otherwise, treats all lines as data rows.

        Args:
            filename: Path to the text file.

        Returns:
            Matrix loaded from the file.

        Raises:
            MatrixException: If the file cannot be opened.

        Examples:
            >>> m = Matrix.from_file("data.txt")
        """
        try:
            with open(filename, "r") as f:
                lines = f.readlines()
        except FileNotFoundError:
            raise MatrixException(f"Cannot open file '{filename}'")

        # Parse all non-empty lines into lists of floats
        parsed: list[list[float]] = []
        for line in lines:
            stripped = line.strip()
            if not stripped:
                continue
            try:
                row = [float(x) for x in stripped.split()]
                if row:
                    parsed.append(row)
            except ValueError:
                continue

        if not parsed:
            raise MatrixException(f"No valid data found in '{filename}'")

        # Auto-detect header: first line has 2 values, second has more
        if (len(parsed) >= 2
                and len(parsed[0]) == 2
                and len(parsed[1]) > 2):
            # First line is "rows cols" header
            data_rows = parsed[1:]
        else:
            data_rows = parsed

        return cls.from_list(data_rows)

    @classmethod
    def identity(cls, n: int) -> Matrix:
        """Create an n × n identity matrix.

        Args:
            n: Size of the identity matrix.

        Returns:
            Identity matrix with 1s on the diagonal.

        Examples:
            >>> Matrix.identity(3)
        """
        m = cls(n, n)
        for i in range(n):
            m._data[i][i] = 1.0
        return m

    # ── Element access ─────────────────────────────────────────

    def __getitem__(self, key: tuple[int, int]) -> float:
        """Access element at (row, col) using m[i, j] syntax.

        Args:
            key: Tuple of (row_index, col_index).

        Returns:
            The element value.

        Raises:
            MatrixException: If indices are out of bounds.

        Examples:
            >>> m = Matrix.from_list([[1, 2], [3, 4]])
            >>> m[0, 1]
            2.0
        """
        i, j = key
        if i < 0 or i >= self.rows or j < 0 or j >= self.cols:
            raise MatrixException(
                f"Index ({i}, {j}) out of bounds for {self.rows}×{self.cols} matrix"
            )
        return self._data[i][j]

    def __setitem__(self, key: tuple[int, int], value: float) -> None:
        """Set element at (row, col) using m[i, j] = value syntax.

        Args:
            key: Tuple of (row_index, col_index).
            value: The value to set.

        Raises:
            MatrixException: If indices are out of bounds.

        Examples:
            >>> m = Matrix(2, 2)
            >>> m[0, 0] = 5.0
        """
        i, j = key
        if i < 0 or i >= self.rows or j < 0 or j >= self.cols:
            raise MatrixException(
                f"Index ({i}, {j}) out of bounds for {self.rows}×{self.cols} matrix"
            )
        self._data[i][j] = float(value)

    # ── Arithmetic operators ───────────────────────────────────

    def __add__(self, other: Matrix) -> Matrix:
        """Add two matrices element-wise.

        Raises:
            DimensionMismatchError: If dimensions don't match.

        Examples:
            >>> a = Matrix.from_list([[1, 2], [3, 4]])
            >>> b = Matrix.from_list([[5, 6], [7, 8]])
            >>> c = a + b  # [[6,8],[10,12]]
        """
        if not isinstance(other, Matrix):
            return NotImplemented
        if self.rows != other.rows or self.cols != other.cols:
            raise DimensionMismatchError(
                f"Cannot add {self.rows}×{self.cols} and {other.rows}×{other.cols} matrices"
            )
        result = Matrix(self.rows, self.cols)
        for i in range(self.rows):
            for j in range(self.cols):
                result._data[i][j] = self._data[i][j] + other._data[i][j]
        return result

    def __sub__(self, other: Matrix) -> Matrix:
        """Subtract two matrices element-wise.

        Raises:
            DimensionMismatchError: If dimensions don't match.
        """
        if not isinstance(other, Matrix):
            return NotImplemented
        if self.rows != other.rows or self.cols != other.cols:
            raise DimensionMismatchError(
                f"Cannot subtract {self.rows}×{self.cols} and {other.rows}×{other.cols} matrices"
            )
        result = Matrix(self.rows, self.cols)
        for i in range(self.rows):
            for j in range(self.cols):
                result._data[i][j] = self._data[i][j] - other._data[i][j]
        return result

    def __mul__(self, other: Matrix | float | int) -> Matrix:
        """Multiply two matrices, or multiply a matrix by a scalar.

        Matrix × Matrix: standard matrix multiplication (dot product).
        Matrix × scalar: element-wise multiplication.

        Raises:
            DimensionMismatchError: If matrix dimensions are incompatible.

        Examples:
            >>> a = Matrix.from_list([[1, 2], [3, 4]])
            >>> b = Matrix.from_list([[5, 6], [7, 8]])
            >>> c = a * b  # [[19,22],[43,50]]
            >>> d = a * 2.0  # [[2,4],[6,8]]
        """
        if isinstance(other, Matrix):
            if self.cols != other.rows:
                raise DimensionMismatchError(
                    f"Cannot multiply {self.rows}×{self.cols} and "
                    f"{other.rows}×{other.cols} matrices"
                )
            result = Matrix(self.rows, other.cols)
            for i in range(self.rows):
                for j in range(other.cols):
                    s = 0.0
                    for k in range(self.cols):
                        s += self._data[i][k] * other._data[k][j]
                    result._data[i][j] = s
            return result
        if isinstance(other, (int, float)):
            result = Matrix(self.rows, self.cols)
            for i in range(self.rows):
                for j in range(self.cols):
                    result._data[i][j] = self._data[i][j] * other
            return result
        return NotImplemented

    def __rmul__(self, other: float | int) -> Matrix:
        """Enable scalar * Matrix.

        Examples:
            >>> m = Matrix.from_list([[1, 2], [3, 4]])
            >>> result = 3.0 * m  # [[3,6],[9,12]]
        """
        if isinstance(other, (int, float)):
            return self.__mul__(other)
        return NotImplemented

    # ── Transpose ──────────────────────────────────────────────

    def transpose(self) -> Matrix:
        """Return the transpose of this matrix.

        Returns:
            New matrix where rows and columns are swapped.

        Examples:
            >>> m = Matrix.from_list([[1, 2, 3], [4, 5, 6]])
            >>> t = m.transpose()  # 3×2 matrix
        """
        result = Matrix(self.cols, self.rows)
        for i in range(self.rows):
            for j in range(self.cols):
                result._data[j][i] = self._data[i][j]
        return result

    # ── Equality ───────────────────────────────────────────────

    def __eq__(self, other: object) -> bool:
        """Check equality with floating-point tolerance (1e-10).

        Mirrors the C++ operator== with `fabs(diff) > 1e-10` threshold.

        Examples:
            >>> a = Matrix.from_list([[1, 2], [3, 4]])
            >>> b = Matrix.from_list([[1, 2], [3, 4]])
            >>> a == b
            True
        """
        if not isinstance(other, Matrix):
            return NotImplemented
        if self.rows != other.rows or self.cols != other.cols:
            return False
        for i in range(self.rows):
            for j in range(self.cols):
                if abs(self._data[i][j] - other._data[i][j]) > 1e-10:
                    return False
        return True

    # ── Copy ───────────────────────────────────────────────────

    def copy(self) -> Matrix:
        """Create a deep copy of this matrix.

        Returns:
            New independent Matrix with the same data.

        Examples:
            >>> a = Matrix.from_list([[1, 2], [3, 4]])
            >>> b = a.copy()
            >>> b[0, 0] = 999  # doesn't affect a
        """
        result = Matrix(self.rows, self.cols)
        for i in range(self.rows):
            for j in range(self.cols):
                result._data[i][j] = self._data[i][j]
        return result

    # ── I/O methods ────────────────────────────────────────────

    def read_from_console(self) -> None:
        """Read matrix elements from stdin interactively.

        Prompts for each row. Replaces current data.
        """
        print(f"Enter elements for {self.rows}×{self.cols} matrix:")
        for i in range(self.rows):
            while True:
                try:
                    values = input(f"  Row {i + 1}: ").split()
                    if len(values) != self.cols:
                        print(f"  ⚠ Expected {self.cols} values, got {len(values)}. Try again.")
                        continue
                    for j, v in enumerate(values):
                        self._data[i][j] = float(v)
                    break
                except ValueError:
                    print("  ⚠ Invalid input — please enter numeric values.")

    def save_to_file(self, filename: str) -> None:
        """Write matrix to a space-separated text file.

        Args:
            filename: Output file path.

        Raises:
            MatrixException: If the file cannot be written.
        """
        try:
            with open(filename, "w") as f:
                for i in range(self.rows):
                    row_str = "  ".join(f"{self._data[i][j]:.7f}" for j in range(self.cols))
                    f.write(row_str + "\n")
        except OSError:
            raise MatrixException(f"Cannot write to file '{filename}'")

    def display(self) -> None:
        """Pretty-print the matrix to stdout.

        Mirrors the C++ display() method format with fixed precision.
        """
        print(str(self))

    # ── String representations ─────────────────────────────────

    def __str__(self) -> str:
        """Formatted string representation of the matrix.

        Each element is right-aligned with 6 decimal places,
        matching the C++ display() format.

        Examples:
            >>> print(Matrix.from_list([[1, 2], [3, 4]]))
              1.000000      2.000000
              3.000000      4.000000
        """
        lines = []
        for i in range(self.rows):
            row_str = "".join(f"{self._data[i][j]:14.6f}" for j in range(self.cols))
            lines.append(row_str)
        return "\n".join(lines)

    def __repr__(self) -> str:
        """Debug representation.

        Examples:
            >>> repr(Matrix(2, 3))
            'Matrix(2×3)'
        """
        return f"Matrix({self.rows}×{self.cols})"

    # ── Property checks ────────────────────────────────────────

    def is_square(self) -> bool:
        """Check if the matrix is square (rows == cols)."""
        return self.rows == self.cols

    def is_symmetric(self) -> bool:
        """Check if the matrix is symmetric (A == A^T)."""
        if not self.is_square():
            return False
        for i in range(self.rows):
            for j in range(i + 1, self.cols):
                if abs(self._data[i][j] - self._data[j][i]) > 1e-10:
                    return False
        return True

    def is_identity(self) -> bool:
        """Check if the matrix is an identity matrix."""
        if not self.is_square():
            return False
        for i in range(self.rows):
            for j in range(self.cols):
                if i == j:
                    if abs(self._data[i][j] - 1.0) > 1e-10:
                        return False
                else:
                    if abs(self._data[i][j]) > 1e-10:
                        return False
        return True

    def is_null(self) -> bool:
        """Check if all elements are zero."""
        for i in range(self.rows):
            for j in range(self.cols):
                if abs(self._data[i][j]) > 1e-10:
                    return False
        return True

    def is_diagonal(self) -> bool:
        """Check if the matrix is diagonal (non-zeros only on main diagonal)."""
        if not self.is_square():
            return False
        for i in range(self.rows):
            for j in range(self.cols):
                if i != j and abs(self._data[i][j]) > 1e-10:
                    return False
        return True

    def is_diagonally_dominant(self) -> bool:
        """Check if the matrix is strictly diagonally dominant."""
        if not self.is_square():
            return False
        for i in range(self.rows):
            diag_val = abs(self._data[i][i])
            off_diag_sum = sum(abs(self._data[i][j]) for j in range(self.cols) if j != i)
            if diag_val < off_diag_sum:
                return False
        return True

    # ── Advanced operations ────────────────────────────────────

    def minor_matrix(self, r: int, c: int) -> Matrix:
        """Return the (N-1)×(N-1) sub-matrix with row r and column c removed.

        Raises:
            NotSquareError: If the matrix is not square.
            MatrixException: If the matrix is 1×1 (cannot take minor).
        """
        if not self.is_square():
            raise NotSquareError("Minor matrix requires a square matrix")
        if self.rows <= 1:
            raise MatrixException("Matrix too small to take a minor (must be > 1×1)")
        
        result = Matrix(self.rows - 1, self.cols - 1)
        ri = 0
        for i in range(self.rows):
            if i == r:
                continue
            ci = 0
            for j in range(self.cols):
                if j == c:
                    continue
                result._data[ri][ci] = self._data[i][j]
                ci += 1
            ri += 1
        return result

    def cofactor(self, r: int, c: int) -> float:
        """Calculate the cofactor of element (r, c).

        Formula: (-1)^(r+c) * det(minor(r,c))
        """
        minor = self.minor_matrix(r, c)
        det = minor.determinant()
        sign = 1.0 if (r + c) % 2 == 0 else -1.0
        return sign * det

    def determinant(self) -> float:
        """Calculate the determinant using Gaussian elimination (partial pivoting).

        O(N^3) complexity. Matches the C++ implementation.

        Raises:
            NotSquareError: If the matrix is not square.
        """
        if not self.is_square():
            raise NotSquareError("Determinant requires a square matrix")
        
        n = self.rows
        # Make a working copy so we don't modify self
        temp = [row[:] for row in self._data]
        
        det = 1.0
        for k in range(n):
            # Partial pivoting
            max_row = k
            max_val = abs(temp[k][k])
            for i in range(k + 1, n):
                if abs(temp[i][k]) > max_val:
                    max_val = abs(temp[i][k])
                    max_row = i
            
            if max_val < 1e-12:
                return 0.0
            
            # Swap rows if needed
            if max_row != k:
                temp[k], temp[max_row] = temp[max_row], temp[k]
                det *= -1.0
                
            det *= temp[k][k]
            
            # Eliminate below
            for i in range(k + 1, n):
                factor = temp[i][k] / temp[k][k]
                for j in range(k, n):
                    temp[i][j] -= factor * temp[k][j]
                    
        return det

    def adjoint(self) -> Matrix:
        """Calculate the adjoint (adjugate) matrix.

        The adjoint is the transpose of the cofactor matrix.

        Raises:
            NotSquareError: If the matrix is not square.
        """
        if not self.is_square():
            raise NotSquareError("Adjoint requires a square matrix")
        
        n = self.rows
        cofactor_matrix = Matrix(n, n)
        for i in range(n):
            for j in range(n):
                cofactor_matrix._data[i][j] = self.cofactor(i, j)
                
        return cofactor_matrix.transpose()

    def inverse(self) -> Matrix:
        """Calculate the inverse matrix.

        Formula: adj(A) / det(A)

        Raises:
            NotSquareError: If the matrix is not square.
            SingularMatrixError: If the matrix is singular (det ≈ 0).
        """
        if not self.is_square():
            raise NotSquareError("Inverse requires a square matrix")
            
        det = self.determinant()
        if abs(det) < 1e-12:
            raise SingularMatrixError("Matrix is singular (determinant = 0)")
            
        adj = self.adjoint()
        inv_det = 1.0 / det
        return adj * inv_det

    def make_diagonally_dominant(self) -> Matrix:
        """Try to make the matrix diagonally dominant by swapping rows.

        Returns a new Matrix. Original matrix is unmodified.
        Picks the best qualifying row (largest |A[r][i]|) for each position.

        Raises:
            NotSquareError: If the matrix is not square.
        """
        if not self.is_square():
            raise NotSquareError("make_diagonally_dominant requires a square matrix")
            
        n = self.rows
        result = self.copy()
        
        for i in range(n):
            # Check if current row is already dominant
            diag = abs(result._data[i][i])
            row_sum = sum(abs(result._data[i][j]) for j in range(n) if j != i)
            if diag >= row_sum:
                continue
                
            # Find the best qualifying row
            best_row = -1
            best_diag = -1.0
            for r in range(i + 1, n):
                d = abs(result._data[r][i])
                s = sum(abs(result._data[r][j]) for j in range(n) if j != i)
                if d >= s and d > best_diag:
                    best_diag = d
                    best_row = r
                    
            # Swap if found
            if best_row != -1:
                result._data[i], result._data[best_row] = result._data[best_row], result._data[i]
                
        return result

