"""
Tests for advanced operations in pynumerics.matrix.Matrix — TDD test suite.

Written BEFORE implementation per PLAN-004 / AGENT_MD rules.
Covers: property checks, minor matrix, cofactor, determinant,
adjoint, inverse, and make_diagonally_dominant.
"""

import pytest
from pynumerics.matrix import Matrix
from pynumerics.exceptions import (
    MatrixException,
    NotSquareError,
    SingularMatrixError,
)

# ─────────────────────────────────────────────
# Property Checks
# ─────────────────────────────────────────────

class TestPropertyChecks:
    def test_is_square(self):
        assert Matrix(3, 3).is_square()
        assert not Matrix(2, 3).is_square()

    def test_is_symmetric(self):
        m1 = Matrix.from_list([[1, 2], [2, 4]])
        assert m1.is_symmetric()
        
        m2 = Matrix.from_list([[1, 2], [3, 4]])
        assert not m2.is_symmetric()
        
        m3 = Matrix(2, 3)
        assert not m3.is_symmetric()

    def test_is_identity(self):
        assert Matrix.identity(3).is_identity()
        assert not Matrix.from_list([[1, 0], [0, 2]]).is_identity()
        assert not Matrix(3, 4).is_identity()

    def test_is_null(self):
        assert Matrix(2, 3).is_null()
        assert not Matrix.from_list([[0, 0], [0, 1e-9]]).is_null()

    def test_is_diagonal(self):
        m1 = Matrix.from_list([[1, 0, 0], [0, 2, 0], [0, 0, 3]])
        assert m1.is_diagonal()
        
        m2 = Matrix.from_list([[1, 0, 0], [0, 2, 0], [0, 1, 3]])
        assert not m2.is_diagonal()
        
        assert not Matrix(2, 3).is_diagonal()

    def test_is_diagonally_dominant(self):
        # |3| >= |1| + |1| -> 3 >= 2 (Yes)
        # |-4| >= |1| + |2| -> 4 >= 3 (Yes)
        # |5| >= |2| + |2| -> 5 >= 4 (Yes)
        m1 = Matrix.from_list([
            [3, 1, 1],
            [1, -4, 2],
            [2, 2, 5]
        ])
        assert m1.is_diagonally_dominant()

        m2 = Matrix.from_list([
            [1, 2, 3],
            [4, 5, 6],
            [7, 8, 9]
        ])
        assert not m2.is_diagonally_dominant()
        assert not Matrix(2, 3).is_diagonally_dominant()


# ─────────────────────────────────────────────
# Minor, Cofactor, Determinant
# ─────────────────────────────────────────────

class TestDeterminantAndFriends:
    def test_minor_matrix(self):
        m = Matrix.from_list([
            [1, 2, 3],
            [4, 5, 6],
            [7, 8, 9]
        ])
        minor = m.minor_matrix(1, 1) # remove row 1, col 1
        assert minor.rows == 2
        assert minor.cols == 2
        assert minor == Matrix.from_list([[1, 3], [7, 9]])

    def test_minor_matrix_errors(self):
        with pytest.raises(NotSquareError):
            Matrix(2, 3).minor_matrix(0, 0)
        with pytest.raises(MatrixException):
            Matrix(1, 1).minor_matrix(0, 0) # Too small

    def test_cofactor(self):
        m = Matrix.from_list([
            [1, 2, 3],
            [0, 1, 4],
            [5, 6, 0]
        ])
        # Minor(0, 0) = [[1, 4], [6, 0]] -> det = -24
        # Cofactor(0, 0) = (-1)^(0+0) * -24 = -24
        assert m.cofactor(0, 0) == pytest.approx(-24.0)
        
        # Minor(0, 1) = [[0, 4], [5, 0]] -> det = -20
        # Cofactor(0, 1) = (-1)^(0+1) * -20 = 20
        assert m.cofactor(0, 1) == pytest.approx(20.0)

    def test_determinant_2x2(self):
        m = Matrix.from_list([[1, 2], [3, 4]])
        assert m.determinant() == pytest.approx(-2.0)

    def test_determinant_3x3(self):
        m = Matrix.from_list([
            [1, 2, 3],
            [0, 1, 4],
            [5, 6, 0]
        ])
        # 1*(0 - 24) - 2*(0 - 20) + 3*(0 - 5) = -24 + 40 - 15 = 1
        assert m.determinant() == pytest.approx(1.0)

    def test_determinant_singular(self):
        m = Matrix.from_list([
            [1, 2, 3],
            [4, 5, 6],
            [7, 8, 9]
        ])
        assert m.determinant() == pytest.approx(0.0, abs=1e-10)

    def test_determinant_not_square(self):
        with pytest.raises(NotSquareError):
            Matrix(2, 3).determinant()


# ─────────────────────────────────────────────
# Adjoint and Inverse
# ─────────────────────────────────────────────

class TestAdjointAndInverse:
    def test_adjoint(self):
        m = Matrix.from_list([
            [1, 2],
            [3, 4]
        ])
        # Cofactor matrix: [[4, -3], [-2, 1]]
        # Adjoint: [[4, -2], [-3, 1]]
        adj = m.adjoint()
        assert adj == Matrix.from_list([[4, -2], [-3, 1]])

    def test_adjoint_not_square(self):
        with pytest.raises(NotSquareError):
            Matrix(2, 3).adjoint()

    def test_inverse_2x2(self):
        m = Matrix.from_list([
            [1, 2],
            [3, 4]
        ])
        inv = m.inverse()
        # det = -2. adj = [[4, -2], [-3, 1]]
        # inv = [[-2, 1], [1.5, -0.5]]
        assert inv == Matrix.from_list([[-2.0, 1.0], [1.5, -0.5]])
        
        # Verify A * A_inv == I
        assert (m * inv) == Matrix.identity(2)

    def test_inverse_3x3(self):
        m = Matrix.from_list([
            [1, 2, 3],
            [0, 1, 4],
            [5, 6, 0]
        ])
        inv = m.inverse()
        assert (m * inv) == Matrix.identity(3)

    def test_inverse_singular(self):
        m = Matrix.from_list([
            [1, 2, 3],
            [4, 5, 6],
            [7, 8, 9]
        ])
        with pytest.raises(SingularMatrixError):
            m.inverse()

    def test_inverse_not_square(self):
        with pytest.raises(NotSquareError):
            Matrix(2, 3).inverse()


# ─────────────────────────────────────────────
# Make Diagonally Dominant
# ─────────────────────────────────────────────

class TestMakeDiagonallyDominant:
    def test_make_diagonally_dominant(self):
        m = Matrix.from_list([
            [1, 5, 1],
            [6, 1, 1],
            [1, 1, 7]
        ])
        # Row 0 is not dominant. Max in col 0 is row 1.
        # Swapped should be:
        # [[6, 1, 1],
        #  [1, 5, 1],
        #  [1, 1, 7]]
        dom = m.make_diagonally_dominant()
        assert dom.is_diagonally_dominant()
        assert dom == Matrix.from_list([
            [6, 1, 1],
            [1, 5, 1],
            [1, 1, 7]
        ])
        
        # Original should be unchanged
        assert not m.is_diagonally_dominant()

    def test_make_diagonally_dominant_not_square(self):
        with pytest.raises(NotSquareError):
            Matrix(2, 3).make_diagonally_dominant()
