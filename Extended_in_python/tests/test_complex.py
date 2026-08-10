"""
Tests for pynumerics.complex.Complex — TDD test suite.

Written BEFORE implementation per PLAN-001 / AGENT_MD rules.
Covers: constructors, @classmethod factories, arithmetic operators,
reverse operators, unary neg, equality, mathematical methods,
__str__/__repr__ formatting, and edge cases.
"""

import math
import pytest
from pynumerics.complex import Complex


# ─────────────────────────────────────────────
# Task 2: Constructors & @classmethod factories
# ─────────────────────────────────────────────

class TestConstructors:
    """Test default, parameterised, and @classmethod constructors."""

    def test_default_constructor(self):
        c = Complex()
        assert c.re == 0.0
        assert c.im == 0.0

    def test_parameterised_constructor(self):
        c = Complex(3.0, 4.0)
        assert c.re == 3.0
        assert c.im == 4.0

    def test_partial_constructor_real_only(self):
        c = Complex(5.0)
        assert c.re == 5.0
        assert c.im == 0.0

    def test_from_real(self):
        c = Complex.from_real(7.5)
        assert c.re == 7.5
        assert c.im == 0.0

    def test_from_imaginary(self):
        c = Complex.from_imaginary(3.2)
        assert c.re == 0.0
        assert c.im == 3.2

    def test_zero(self):
        c = Complex.zero()
        assert c.re == 0.0
        assert c.im == 0.0

    def test_one(self):
        c = Complex.one()
        assert c.re == 1.0
        assert c.im == 0.0


# ─────────────────────────────────────────────
# Task 3: Arithmetic operators (Complex × Complex)
# ─────────────────────────────────────────────

class TestArithmeticComplexComplex:
    """Test +, -, *, / between two Complex numbers."""

    def test_add(self):
        result = Complex(1.0, 2.0) + Complex(3.0, 4.0)
        assert result.re == pytest.approx(4.0)
        assert result.im == pytest.approx(6.0)

    def test_subtract(self):
        result = Complex(5.0, 3.0) - Complex(2.0, 1.0)
        assert result.re == pytest.approx(3.0)
        assert result.im == pytest.approx(2.0)

    def test_multiply(self):
        # (2 + 3i)(4 + 5i) = (8-15) + (10+12)i = -7 + 22i
        result = Complex(2.0, 3.0) * Complex(4.0, 5.0)
        assert result.re == pytest.approx(-7.0)
        assert result.im == pytest.approx(22.0)

    def test_divide(self):
        # (3 + 4i) / (1 - 2i) = (3+4i)(1+2i) / (1+4) = (-5+10i)/5 = -1+2i
        result = Complex(3.0, 4.0) / Complex(1.0, -2.0)
        assert result.re == pytest.approx(-1.0)
        assert result.im == pytest.approx(2.0)

    def test_multiply_conjugate_pair(self):
        # (a + bi)(a - bi) = a² + b²
        a, b = 3.0, 4.0
        result = Complex(a, b) * Complex(a, -b)
        assert result.re == pytest.approx(a**2 + b**2)
        assert result.im == pytest.approx(0.0)

    def test_add_returns_new_complex(self):
        """Verify operators return new objects, not mutate in place."""
        c1 = Complex(1.0, 2.0)
        c2 = Complex(3.0, 4.0)
        result = c1 + c2
        assert c1.re == 1.0  # c1 unchanged
        assert c1.im == 2.0
        assert result is not c1
        assert result is not c2


# ─────────────────────────────────────────────
# Task 4: Scalar operators (Complex × float, float × Complex)
# ─────────────────────────────────────────────

class TestArithmeticScalar:
    """Test +, -, *, / between Complex and float (both directions)."""

    def test_add_complex_float(self):
        result = Complex(1.0, 2.0) + 3.0
        assert result.re == pytest.approx(4.0)
        assert result.im == pytest.approx(2.0)

    def test_add_float_complex(self):
        result = 3.0 + Complex(1.0, 2.0)
        assert result.re == pytest.approx(4.0)
        assert result.im == pytest.approx(2.0)

    def test_sub_complex_float(self):
        result = Complex(5.0, 3.0) - 2.0
        assert result.re == pytest.approx(3.0)
        assert result.im == pytest.approx(3.0)

    def test_sub_float_complex(self):
        result = 5.0 - Complex(2.0, 3.0)
        assert result.re == pytest.approx(3.0)
        assert result.im == pytest.approx(-3.0)

    def test_mul_complex_float(self):
        result = Complex(2.0, 3.0) * 4.0
        assert result.re == pytest.approx(8.0)
        assert result.im == pytest.approx(12.0)

    def test_mul_float_complex(self):
        result = 4.0 * Complex(2.0, 3.0)
        assert result.re == pytest.approx(8.0)
        assert result.im == pytest.approx(12.0)

    def test_div_complex_float(self):
        result = Complex(6.0, 4.0) / 2.0
        assert result.re == pytest.approx(3.0)
        assert result.im == pytest.approx(2.0)

    def test_div_float_complex(self):
        # 5.0 / (1 + 2i) = 5(1-2i) / (1+4) = (5-10i)/5 = 1-2i
        result = 5.0 / Complex(1.0, 2.0)
        assert result.re == pytest.approx(1.0)
        assert result.im == pytest.approx(-2.0)

    def test_add_complex_int(self):
        """Ensure int scalars also work."""
        result = Complex(1.0, 2.0) + 3
        assert result.re == pytest.approx(4.0)
        assert result.im == pytest.approx(2.0)

    def test_mul_int_complex(self):
        result = 2 * Complex(3.0, 4.0)
        assert result.re == pytest.approx(6.0)
        assert result.im == pytest.approx(8.0)


# ─────────────────────────────────────────────
# Task 5: Unary neg and equality
# ─────────────────────────────────────────────

class TestUnaryAndEquality:
    """Test __neg__ and __eq__."""

    def test_neg(self):
        c = -Complex(3.0, 4.0)
        assert c.re == pytest.approx(-3.0)
        assert c.im == pytest.approx(-4.0)

    def test_neg_already_negative(self):
        c = -Complex(-1.0, -2.0)
        assert c.re == pytest.approx(1.0)
        assert c.im == pytest.approx(2.0)

    def test_eq_same(self):
        assert Complex(1.0, 2.0) == Complex(1.0, 2.0)

    def test_eq_different(self):
        assert not (Complex(1.0, 2.0) == Complex(1.0, 3.0))

    def test_ne(self):
        assert Complex(1.0, 2.0) != Complex(3.0, 4.0)

    def test_eq_with_non_complex(self):
        """Equality with non-Complex should return NotImplemented / False."""
        assert Complex(1.0, 0.0) != "not a complex"


# ─────────────────────────────────────────────
# Task 6: Mathematical methods
# ─────────────────────────────────────────────

class TestMathMethods:
    """Test norm, norm_squared, conjugate, arg."""

    def test_norm_3_4(self):
        assert Complex(3.0, 4.0).norm() == pytest.approx(5.0)

    def test_norm_zero(self):
        assert Complex.zero().norm() == pytest.approx(0.0)

    def test_norm_real_only(self):
        assert Complex(5.0, 0.0).norm() == pytest.approx(5.0)

    def test_norm_imaginary_only(self):
        assert Complex(0.0, 7.0).norm() == pytest.approx(7.0)

    def test_norm_squared(self):
        assert Complex(3.0, 4.0).norm_squared() == pytest.approx(25.0)

    def test_conjugate(self):
        c = Complex(3.0, 4.0).conjugate()
        assert c.re == pytest.approx(3.0)
        assert c.im == pytest.approx(-4.0)

    def test_conjugate_negative_imag(self):
        c = Complex(1.0, -2.0).conjugate()
        assert c.re == pytest.approx(1.0)
        assert c.im == pytest.approx(2.0)

    def test_conjugate_zero(self):
        c = Complex.zero().conjugate()
        assert c.re == pytest.approx(0.0)
        assert c.im == pytest.approx(0.0)

    def test_arg_first_quadrant(self):
        # arg(1 + 1i) = pi/4
        assert Complex(1.0, 1.0).arg() == pytest.approx(math.pi / 4)

    def test_arg_positive_real_axis(self):
        # arg(1 + 0i) = 0
        assert Complex(1.0, 0.0).arg() == pytest.approx(0.0)

    def test_arg_positive_imag_axis(self):
        # arg(0 + 1i) = pi/2
        assert Complex(0.0, 1.0).arg() == pytest.approx(math.pi / 2)

    def test_arg_negative_real(self):
        # arg(-1 + 0i) = pi
        assert Complex(-1.0, 0.0).arg() == pytest.approx(math.pi)

    def test_arg_third_quadrant(self):
        # arg(-1 - 1i) = -3pi/4
        assert Complex(-1.0, -1.0).arg() == pytest.approx(-3 * math.pi / 4)


# ─────────────────────────────────────────────
# Task 7: __str__ and __repr__ formatting
# ─────────────────────────────────────────────

class TestFormatting:
    """Test string representations."""

    def test_str_positive_both(self):
        assert str(Complex(3.0, 4.0)) == "3.00 + 4.00i"

    def test_str_negative_imag(self):
        assert str(Complex(3.0, -4.0)) == "3.00 - 4.00i"

    def test_str_zero_imag(self):
        assert str(Complex(5.0, 0.0)) == "5.00 + 0.00i"

    def test_str_zero_real(self):
        assert str(Complex(0.0, 3.0)) == "0.00 + 3.00i"

    def test_str_both_zero(self):
        assert str(Complex(0.0, 0.0)) == "0.00 + 0.00i"

    def test_str_negative_both(self):
        assert str(Complex(-2.0, -3.0)) == "-2.00 - 3.00i"

    def test_repr(self):
        assert repr(Complex(3.0, 4.0)) == "Complex(3.0, 4.0)"

    def test_repr_negative(self):
        assert repr(Complex(-1.0, -2.0)) == "Complex(-1.0, -2.0)"


# ─────────────────────────────────────────────
# Task 8: Edge cases
# ─────────────────────────────────────────────

class TestEdgeCases:
    """Test division by zero and other edge cases."""

    def test_divide_by_zero_complex(self):
        with pytest.raises(ZeroDivisionError):
            Complex(1.0, 2.0) / Complex(0.0, 0.0)

    def test_divide_by_zero_scalar(self):
        with pytest.raises(ZeroDivisionError):
            Complex(1.0, 2.0) / 0.0

    def test_add_identity(self):
        """Adding zero complex doesn't change the value."""
        c = Complex(3.0, 4.0)
        result = c + Complex.zero()
        assert result == c

    def test_mul_identity(self):
        """Multiplying by one doesn't change the value."""
        c = Complex(3.0, 4.0)
        result = c * Complex.one()
        assert result == c

    def test_mul_by_zero(self):
        result = Complex(3.0, 4.0) * Complex.zero()
        assert result == Complex.zero()

    def test_conjugate_of_conjugate(self):
        """Conjugate of conjugate should return original."""
        c = Complex(3.0, 4.0)
        assert c.conjugate().conjugate() == c

    def test_norm_of_conjugate(self):
        """Norm of a number equals norm of its conjugate."""
        c = Complex(3.0, 4.0)
        assert c.norm() == pytest.approx(c.conjugate().norm())

    def test_product_with_conjugate_equals_norm_squared(self):
        """z * conj(z) = |z|²"""
        c = Complex(3.0, 4.0)
        product = c * c.conjugate()
        assert product.re == pytest.approx(c.norm_squared())
        assert product.im == pytest.approx(0.0)
