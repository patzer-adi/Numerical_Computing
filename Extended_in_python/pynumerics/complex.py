"""
Complex number class with full operator overloading.

Ported from C++ Complex class (Complex_class_assignment/).
Provides arithmetic operators, mathematical methods, and formatted output.

Usage:
    >>> from pynumerics.complex import Complex
    >>> a = Complex(3.0, 4.0)
    >>> b = Complex(1.0, -2.0)
    >>> print(a + b)
    4.00 + 2.00i
    >>> a.norm()
    5.0
"""

from __future__ import annotations

import math


class Complex:
    """A complex number with real and imaginary parts.

    Mirrors the C++ Complex class with idiomatic Python additions:
    - ``@classmethod`` factories replace constructor overloading
    - ``__dunder__`` methods replace C++ operator overloading
    - Reverse operators (``__radd__`` etc.) enable ``float + Complex``

    Attributes:
        re: The real part (float).
        im: The imaginary part (float).
    """

    def __init__(self, re: float = 0.0, im: float = 0.0) -> None:
        """Create a complex number.

        Args:
            re: Real part. Defaults to 0.0.
            im: Imaginary part. Defaults to 0.0.

        Examples:
            >>> Complex(3.0, 4.0)
            Complex(3.0, 4.0)
            >>> Complex()
            Complex(0.0, 0.0)
        """
        self.re = float(re)
        self.im = float(im)

    # ── @classmethod constructors ──────────────────────────────

    @classmethod
    def from_real(cls, re: float) -> Complex:
        """Create a complex number with only a real part.

        Args:
            re: The real part.

        Returns:
            Complex with im = 0.0.

        Examples:
            >>> Complex.from_real(5.0)
            Complex(5.0, 0.0)
        """
        return cls(re, 0.0)

    @classmethod
    def from_imaginary(cls, im: float) -> Complex:
        """Create a complex number with only an imaginary part.

        Args:
            im: The imaginary part.

        Returns:
            Complex with re = 0.0.

        Examples:
            >>> Complex.from_imaginary(3.0)
            Complex(0.0, 3.0)
        """
        return cls(0.0, im)

    @classmethod
    def zero(cls) -> Complex:
        """Create the zero complex number (0 + 0i).

        Returns:
            Complex(0.0, 0.0).
        """
        return cls(0.0, 0.0)

    @classmethod
    def one(cls) -> Complex:
        """Create the multiplicative identity (1 + 0i).

        Returns:
            Complex(1.0, 0.0).
        """
        return cls(1.0, 0.0)

    # ── Arithmetic operators (Complex × Complex) ──────────────

    def __add__(self, other: Complex | float | int) -> Complex:
        """Add two complex numbers, or a complex and a scalar.

        Examples:
            >>> Complex(1, 2) + Complex(3, 4)
            Complex(4.0, 6.0)
            >>> Complex(1, 2) + 3.0
            Complex(4.0, 2.0)
        """
        if isinstance(other, Complex):
            return Complex(self.re + other.re, self.im + other.im)
        if isinstance(other, (int, float)):
            return Complex(self.re + other, self.im)
        return NotImplemented

    def __sub__(self, other: Complex | float | int) -> Complex:
        """Subtract two complex numbers, or a scalar from a complex.

        Examples:
            >>> Complex(5, 3) - Complex(2, 1)
            Complex(3.0, 2.0)
        """
        if isinstance(other, Complex):
            return Complex(self.re - other.re, self.im - other.im)
        if isinstance(other, (int, float)):
            return Complex(self.re - other, self.im)
        return NotImplemented

    def __mul__(self, other: Complex | float | int) -> Complex:
        """Multiply two complex numbers, or a complex by a scalar.

        Formula: (a+bi)(c+di) = (ac-bd) + (ad+bc)i

        Examples:
            >>> Complex(2, 3) * Complex(4, 5)
            Complex(-7.0, 22.0)
            >>> Complex(2, 3) * 4.0
            Complex(8.0, 12.0)
        """
        if isinstance(other, Complex):
            re = (self.re * other.re) - (self.im * other.im)
            im = (self.re * other.im) + (self.im * other.re)
            return Complex(re, im)
        if isinstance(other, (int, float)):
            return Complex(self.re * other, self.im * other)
        return NotImplemented

    def __truediv__(self, other: Complex | float | int) -> Complex:
        """Divide two complex numbers, or a complex by a scalar.

        Formula: (a+bi)/(c+di) = [(ac+bd) + (bc-ad)i] / (c²+d²)

        Raises:
            ZeroDivisionError: If dividing by zero.

        Examples:
            >>> Complex(3, 4) / Complex(1, -2)
            Complex(-1.0, 2.0)
        """
        if isinstance(other, Complex):
            denominator = other.re * other.re + other.im * other.im
            if denominator == 0.0:
                raise ZeroDivisionError("Cannot divide by zero complex number (0 + 0i)")
            re = (self.re * other.re + self.im * other.im) / denominator
            im = (self.im * other.re - self.re * other.im) / denominator
            return Complex(re, im)
        if isinstance(other, (int, float)):
            if other == 0.0:
                raise ZeroDivisionError("Cannot divide complex number by zero")
            return Complex(self.re / other, self.im / other)
        return NotImplemented

    # ── Reverse operators (float × Complex) ───────────────────

    def __radd__(self, other: float | int) -> Complex:
        """Enable ``float + Complex``.

        Examples:
            >>> 3.0 + Complex(1, 2)
            Complex(4.0, 2.0)
        """
        if isinstance(other, (int, float)):
            return Complex(other + self.re, self.im)
        return NotImplemented

    def __rsub__(self, other: float | int) -> Complex:
        """Enable ``float - Complex``.

        Examples:
            >>> 5.0 - Complex(2, 3)
            Complex(3.0, -3.0)
        """
        if isinstance(other, (int, float)):
            return Complex(other - self.re, -self.im)
        return NotImplemented

    def __rmul__(self, other: float | int) -> Complex:
        """Enable ``float * Complex``.

        Examples:
            >>> 4.0 * Complex(2, 3)
            Complex(8.0, 12.0)
        """
        if isinstance(other, (int, float)):
            return Complex(other * self.re, other * self.im)
        return NotImplemented

    def __rtruediv__(self, other: float | int) -> Complex:
        """Enable ``float / Complex``.

        Computes: other / self = other * conj(self) / |self|²

        Examples:
            >>> 5.0 / Complex(1, 2)
            Complex(1.0, -2.0)
        """
        if isinstance(other, (int, float)):
            denominator = self.re * self.re + self.im * self.im
            if denominator == 0.0:
                raise ZeroDivisionError("Cannot divide by zero complex number (0 + 0i)")
            re = (other * self.re) / denominator
            im = (-other * self.im) / denominator
            return Complex(re, im)
        return NotImplemented

    # ── Unary operators ───────────────────────────────────────

    def __neg__(self) -> Complex:
        """Negate the complex number.

        Examples:
            >>> -Complex(3, 4)
            Complex(-3.0, -4.0)
        """
        return Complex(-self.re, -self.im)

    # ── Equality ──────────────────────────────────────────────

    def __eq__(self, other: object) -> bool:
        """Check equality between two complex numbers.

        Uses exact float comparison. For approximate comparison,
        use ``pytest.approx`` or a custom tolerance check.

        Examples:
            >>> Complex(1, 2) == Complex(1, 2)
            True
        """
        if isinstance(other, Complex):
            return self.re == other.re and self.im == other.im
        return NotImplemented

    # ── Mathematical methods ──────────────────────────────────

    def norm(self) -> float:
        """Compute the norm (absolute value / modulus) of the complex number.

        Returns:
            sqrt(re² + im²)

        Examples:
            >>> Complex(3, 4).norm()
            5.0
        """
        return math.sqrt(self.re * self.re + self.im * self.im)

    def norm_squared(self) -> float:
        """Compute the squared norm (avoids the sqrt call).

        Returns:
            re² + im²

        Examples:
            >>> Complex(3, 4).norm_squared()
            25.0
        """
        return self.re * self.re + self.im * self.im

    def conjugate(self) -> Complex:
        """Compute the complex conjugate.

        Returns:
            Complex(re, -im)

        Examples:
            >>> Complex(3, 4).conjugate()
            Complex(3.0, -4.0)
        """
        return Complex(self.re, -self.im)

    def arg(self) -> float:
        """Compute the argument (angle) of the complex number in radians.

        Returns:
            atan2(im, re) — the angle in the range [-π, π].

        Examples:
            >>> Complex(1, 1).arg()  # pi/4
            0.7853981633974483
        """
        return math.atan2(self.im, self.re)

    # ── String representations ────────────────────────────────

    def __str__(self) -> str:
        """Formatted string: ``3.00 + 4.00i`` or ``3.00 - 4.00i``.

        Mirrors the C++ ``display()`` method's output format.

        Examples:
            >>> str(Complex(3, 4))
            '3.00 + 4.00i'
            >>> str(Complex(3, -4))
            '3.00 - 4.00i'
        """
        if self.im >= 0:
            return f"{self.re:.2f} + {self.im:.2f}i"
        else:
            return f"{self.re:.2f} - {-self.im:.2f}i"

    def __repr__(self) -> str:
        """Debug representation: ``Complex(3.0, 4.0)``.

        Examples:
            >>> repr(Complex(3, 4))
            'Complex(3.0, 4.0)'
        """
        return f"Complex({self.re}, {self.im})"
