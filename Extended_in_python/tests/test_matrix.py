"""
Tests for pynumerics.matrix.Matrix — TDD test suite.

Written BEFORE implementation per PLAN-003 / AGENT_MD rules.
Covers: constructors, element access, arithmetic, transpose,
formatting, equality, copy, file I/O, and exceptions.
"""

import os
import pytest
from pynumerics.matrix import Matrix
from pynumerics.exceptions import (
    MatrixException,
    DimensionMismatchError,
    NotSquareError,
)


# ─────────────────────────────────────────────
# Constructors & factories
# ─────────────────────────────────────────────

class TestConstructors:
    """Test Matrix creation methods."""

    def test_zero_matrix(self):
        m = Matrix(2, 3)
        assert m.rows == 2
        assert m.cols == 3
        for i in range(2):
            for j in range(3):
                assert m[i, j] == 0.0

    def test_from_list(self):
        data = [[1, 2, 3], [4, 5, 6]]
        m = Matrix.from_list(data)
        assert m.rows == 2
        assert m.cols == 3
        assert m[0, 0] == 1.0
        assert m[1, 2] == 6.0

    def test_from_list_single_element(self):
        m = Matrix.from_list([[42]])
        assert m.rows == 1
        assert m.cols == 1
        assert m[0, 0] == 42.0

    def test_from_list_copies_data(self):
        """Modifying original list should not affect the matrix."""
        data = [[1, 2], [3, 4]]
        m = Matrix.from_list(data)
        data[0][0] = 999
        assert m[0, 0] == 1.0

    def test_identity(self):
        m = Matrix.identity(3)
        assert m.rows == 3
        assert m.cols == 3
        for i in range(3):
            for j in range(3):
                expected = 1.0 if i == j else 0.0
                assert m[i, j] == expected


# ─────────────────────────────────────────────
# Element access
# ─────────────────────────────────────────────

class TestElementAccess:
    """Test __getitem__ and __setitem__."""

    def test_getitem(self):
        m = Matrix.from_list([[1, 2], [3, 4]])
        assert m[0, 0] == 1.0
        assert m[0, 1] == 2.0
        assert m[1, 0] == 3.0
        assert m[1, 1] == 4.0

    def test_setitem(self):
        m = Matrix(2, 2)
        m[0, 0] = 5.0
        m[1, 1] = 10.0
        assert m[0, 0] == 5.0
        assert m[1, 1] == 10.0

    def test_getitem_out_of_bounds(self):
        m = Matrix(2, 2)
        with pytest.raises(MatrixException):
            _ = m[5, 0]

    def test_setitem_out_of_bounds(self):
        m = Matrix(2, 2)
        with pytest.raises(MatrixException):
            m[0, 5] = 1.0

    def test_negative_index_out_of_bounds(self):
        m = Matrix(2, 2)
        with pytest.raises(MatrixException):
            _ = m[-1, 0]


# ─────────────────────────────────────────────
# Arithmetic: Matrix + Matrix, Matrix - Matrix
# ─────────────────────────────────────────────

class TestArithmetic:
    """Test +, -, * operators."""

    def test_add(self):
        a = Matrix.from_list([[1, 2], [3, 4]])
        b = Matrix.from_list([[5, 6], [7, 8]])
        c = a + b
        assert c[0, 0] == pytest.approx(6.0)
        assert c[0, 1] == pytest.approx(8.0)
        assert c[1, 0] == pytest.approx(10.0)
        assert c[1, 1] == pytest.approx(12.0)

    def test_subtract(self):
        a = Matrix.from_list([[5, 6], [7, 8]])
        b = Matrix.from_list([[1, 2], [3, 4]])
        c = a - b
        assert c[0, 0] == pytest.approx(4.0)
        assert c[1, 1] == pytest.approx(4.0)

    def test_add_dimension_mismatch(self):
        a = Matrix(2, 3)
        b = Matrix(3, 2)
        with pytest.raises(DimensionMismatchError):
            _ = a + b

    def test_sub_dimension_mismatch(self):
        a = Matrix(2, 3)
        b = Matrix(3, 2)
        with pytest.raises(DimensionMismatchError):
            _ = a - b

    def test_multiply_2x2(self):
        # [[1,2],[3,4]] * [[5,6],[7,8]] = [[19,22],[43,50]]
        a = Matrix.from_list([[1, 2], [3, 4]])
        b = Matrix.from_list([[5, 6], [7, 8]])
        c = a * b
        assert c[0, 0] == pytest.approx(19.0)
        assert c[0, 1] == pytest.approx(22.0)
        assert c[1, 0] == pytest.approx(43.0)
        assert c[1, 1] == pytest.approx(50.0)

    def test_multiply_non_square(self):
        # (2x3) * (3x2) = (2x2)
        a = Matrix.from_list([[1, 2, 3], [4, 5, 6]])
        b = Matrix.from_list([[7, 8], [9, 10], [11, 12]])
        c = a * b
        assert c.rows == 2
        assert c.cols == 2
        assert c[0, 0] == pytest.approx(58.0)   # 1*7 + 2*9 + 3*11
        assert c[0, 1] == pytest.approx(64.0)   # 1*8 + 2*10 + 3*12

    def test_multiply_dimension_mismatch(self):
        a = Matrix(2, 3)
        b = Matrix(2, 3)
        with pytest.raises(DimensionMismatchError):
            _ = a * b

    def test_add_returns_new_matrix(self):
        a = Matrix.from_list([[1, 2], [3, 4]])
        b = Matrix.from_list([[5, 6], [7, 8]])
        c = a + b
        assert a[0, 0] == 1.0  # original unchanged
        assert c is not a


# ─────────────────────────────────────────────
# Scalar multiplication
# ─────────────────────────────────────────────

class TestScalarMultiply:
    """Test Matrix * scalar and scalar * Matrix."""

    def test_matrix_times_scalar(self):
        m = Matrix.from_list([[1, 2], [3, 4]])
        result = m * 2.0
        assert result[0, 0] == pytest.approx(2.0)
        assert result[1, 1] == pytest.approx(8.0)

    def test_scalar_times_matrix(self):
        m = Matrix.from_list([[1, 2], [3, 4]])
        result = 3.0 * m
        assert result[0, 0] == pytest.approx(3.0)
        assert result[1, 1] == pytest.approx(12.0)

    def test_multiply_by_zero(self):
        m = Matrix.from_list([[1, 2], [3, 4]])
        result = m * 0.0
        for i in range(2):
            for j in range(2):
                assert result[i, j] == pytest.approx(0.0)

    def test_int_scalar(self):
        m = Matrix.from_list([[1, 2], [3, 4]])
        result = m * 2
        assert result[0, 0] == pytest.approx(2.0)


# ─────────────────────────────────────────────
# Transpose
# ─────────────────────────────────────────────

class TestTranspose:
    """Test transpose operation."""

    def test_transpose_square(self):
        m = Matrix.from_list([[1, 2], [3, 4]])
        t = m.transpose()
        assert t[0, 0] == 1.0
        assert t[0, 1] == 3.0
        assert t[1, 0] == 2.0
        assert t[1, 1] == 4.0

    def test_transpose_non_square(self):
        m = Matrix.from_list([[1, 2, 3], [4, 5, 6]])
        t = m.transpose()
        assert t.rows == 3
        assert t.cols == 2
        assert t[0, 0] == 1.0
        assert t[2, 1] == 6.0

    def test_double_transpose_equals_original(self):
        m = Matrix.from_list([[1, 2, 3], [4, 5, 6]])
        assert m.transpose().transpose() == m

    def test_transpose_identity(self):
        m = Matrix.identity(3)
        assert m.transpose() == m


# ─────────────────────────────────────────────
# Formatting: __str__, __repr__
# ─────────────────────────────────────────────

class TestFormatting:
    """Test string representations."""

    def test_str_contains_values(self):
        m = Matrix.from_list([[1, 2], [3, 4]])
        s = str(m)
        assert "1" in s
        assert "4" in s

    def test_repr(self):
        m = Matrix(2, 3)
        r = repr(m)
        assert "Matrix" in r
        assert "2" in r
        assert "3" in r


# ─────────────────────────────────────────────
# Equality and copy
# ─────────────────────────────────────────────

class TestEqualityAndCopy:
    """Test __eq__ and copy()."""

    def test_equal_matrices(self):
        a = Matrix.from_list([[1, 2], [3, 4]])
        b = Matrix.from_list([[1, 2], [3, 4]])
        assert a == b

    def test_unequal_matrices(self):
        a = Matrix.from_list([[1, 2], [3, 4]])
        b = Matrix.from_list([[1, 2], [3, 5]])
        assert a != b

    def test_different_dimensions_not_equal(self):
        a = Matrix(2, 3)
        b = Matrix(3, 2)
        assert a != b

    def test_equality_with_tolerance(self):
        """Floating-point tolerance: 1e-10 differences are equal."""
        a = Matrix.from_list([[1.0]])
        b = Matrix.from_list([[1.0 + 1e-11]])
        assert a == b

    def test_copy_creates_independent_matrix(self):
        a = Matrix.from_list([[1, 2], [3, 4]])
        b = a.copy()
        assert a == b
        b[0, 0] = 999
        assert a[0, 0] == 1.0  # original unchanged

    def test_eq_with_non_matrix(self):
        m = Matrix(2, 2)
        assert m != "not a matrix"


# ─────────────────────────────────────────────
# File I/O
# ─────────────────────────────────────────────

class TestFileIO:
    """Test save_to_file and from_file."""

    def test_save_and_load_round_trip(self, tmp_path):
        original = Matrix.from_list([[1.5, 2.5, 3.5], [4.5, 5.5, 6.5]])
        filepath = str(tmp_path / "test_matrix.txt")
        original.save_to_file(filepath)
        loaded = Matrix.from_file(filepath)
        assert loaded == original

    def test_save_and_load_identity(self, tmp_path):
        original = Matrix.identity(3)
        filepath = str(tmp_path / "identity.txt")
        original.save_to_file(filepath)
        loaded = Matrix.from_file(filepath)
        assert loaded == original

    def test_load_nonexistent_file(self):
        with pytest.raises(MatrixException):
            Matrix.from_file("nonexistent_file_12345.txt")

    def test_save_and_load_single_element(self, tmp_path):
        original = Matrix.from_list([[42.0]])
        filepath = str(tmp_path / "single.txt")
        original.save_to_file(filepath)
        loaded = Matrix.from_file(filepath)
        assert loaded == original


# ─────────────────────────────────────────────
# Exception hierarchy
# ─────────────────────────────────────────────

class TestExceptions:
    """Test the custom exception classes."""

    def test_matrix_exception_is_exception(self):
        with pytest.raises(Exception):
            raise MatrixException("test")

    def test_dimension_mismatch_inherits(self):
        with pytest.raises(MatrixException):
            raise DimensionMismatchError("bad dims")

    def test_not_square_inherits(self):
        with pytest.raises(MatrixException):
            raise NotSquareError("not square")

    def test_exception_message(self):
        e = DimensionMismatchError("custom message")
        assert "custom message" in str(e)
