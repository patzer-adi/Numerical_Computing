"""
Tests for pynumerics.cli — CLI helpers for Complex number demo.

Written BEFORE implementation per PLAN-002 / AGENT_MD rules.
Covers: read_complex(), display_operations(), display_properties().
"""

import math
import pytest
from unittest.mock import patch
from io import StringIO

from pynumerics.complex import Complex
from pynumerics.cli import read_complex, display_operations, display_properties


# ─────────────────────────────────────────────
# Task 4: read_complex() — valid and invalid input
# ─────────────────────────────────────────────

class TestReadComplex:
    """Test the interactive complex number reader."""

    @patch("builtins.input", side_effect=["3.0", "4.0"])
    def test_read_valid_complex(self, mock_input):
        c = read_complex("Enter complex: ")
        assert c.re == pytest.approx(3.0)
        assert c.im == pytest.approx(4.0)

    @patch("builtins.input", side_effect=["0", "0"])
    def test_read_zero_complex(self, mock_input):
        c = read_complex("Enter complex: ")
        assert c.re == pytest.approx(0.0)
        assert c.im == pytest.approx(0.0)

    @patch("builtins.input", side_effect=["-1.5", "2.7"])
    def test_read_negative_parts(self, mock_input):
        c = read_complex("Enter complex: ")
        assert c.re == pytest.approx(-1.5)
        assert c.im == pytest.approx(2.7)

    @patch("builtins.input", side_effect=["abc", "3.0", "4.0"])
    def test_read_invalid_then_valid(self, mock_input, capsys):
        """First input is invalid — should print error and re-prompt."""
        c = read_complex("Enter complex: ")
        captured = capsys.readouterr()
        assert "Invalid input" in captured.out
        assert c.re == pytest.approx(3.0)
        assert c.im == pytest.approx(4.0)

    @patch("builtins.input", side_effect=["3.0", "xyz", "3.0", "4.0"])
    def test_read_invalid_imag_then_valid(self, mock_input, capsys):
        """Real part valid but imaginary is invalid — re-prompt both."""
        c = read_complex("Enter complex: ")
        captured = capsys.readouterr()
        assert "Invalid input" in captured.out
        assert c.re == pytest.approx(3.0)
        assert c.im == pytest.approx(4.0)

    @patch("builtins.input", side_effect=["5", "0"])
    def test_read_integer_input(self, mock_input):
        """Integers should work as valid input."""
        c = read_complex("Enter complex: ")
        assert c.re == pytest.approx(5.0)
        assert c.im == pytest.approx(0.0)


# ─────────────────────────────────────────────
# Task 5: display_operations() output
# ─────────────────────────────────────────────

class TestDisplayOperations:
    """Test that arithmetic operations are displayed correctly."""

    def test_display_basic_operations(self, capsys):
        c1 = Complex(1.0, 2.0)
        c2 = Complex(3.0, 4.0)
        display_operations(c1, c2)
        output = capsys.readouterr().out

        # Check addition result appears
        assert "4.00 + 6.00i" in output
        # Check subtraction result appears
        assert "-2.00 - 2.00i" in output
        # (1+2i)(3+4i) = (3-8) + (4+6)i = -5 + 10i
        assert "-5.00 + 10.00i" in output

    def test_display_operations_labels(self, capsys):
        c1 = Complex(1.0, 0.0)
        c2 = Complex(1.0, 0.0)
        display_operations(c1, c2)
        output = capsys.readouterr().out

        # Should contain operator labels
        for label in ["+", "-", "*", "/"]:
            assert label in output

    def test_display_operations_division_by_zero(self, capsys):
        """Division by zero should show error, not crash."""
        c1 = Complex(1.0, 2.0)
        c2 = Complex(0.0, 0.0)
        display_operations(c1, c2)
        output = capsys.readouterr().out

        # Should show addition/subtraction/multiplication normally
        assert "1.00 + 2.00i" in output  # sum with zero
        # Division should show an error message, not crash
        assert "undefined" in output.lower() or "zero" in output.lower()


# ─────────────────────────────────────────────
# Task 6: display_properties() output
# ─────────────────────────────────────────────

class TestDisplayProperties:
    """Test that mathematical properties are displayed correctly."""

    def test_display_properties_3_4(self, capsys):
        c = Complex(3.0, 4.0)
        display_properties(c, "z")
        output = capsys.readouterr().out

        # Norm of 3+4i = 5.0
        assert "5.00" in output
        # Conjugate = 3 - 4i
        assert "3.00 - 4.00i" in output
        # Arg ≈ 0.9273 rad
        assert "0.93" in output or "0.9273" in output or "53.13" in output

    def test_display_properties_labels(self, capsys):
        c = Complex(1.0, 1.0)
        display_properties(c, "w")
        output = capsys.readouterr().out

        # Should mention the label
        assert "w" in output
        # Should have norm, conjugate, arg keywords
        output_lower = output.lower()
        assert "norm" in output_lower
        assert "conjugate" in output_lower or "conj" in output_lower
        assert "arg" in output_lower or "angle" in output_lower

    def test_display_properties_zero(self, capsys):
        c = Complex.zero()
        display_properties(c, "z")
        output = capsys.readouterr().out

        # Norm of 0+0i = 0.0
        assert "0.00" in output
