"""
Test suite for Numerical Integration methods.
"""

import pytest
import math
from pynumerics.integration.trapezoidal import TrapezoidalRule
from pynumerics.integration.simpsons13 import Simpsons13
from pynumerics.integration.simpsons38 import Simpsons38
from pynumerics.integration.base import BUILTIN_FUNCTIONS


# ── test functions (same as built-in registry) ────────────────────

def f_exp(x):
    return math.exp(x)

def F_exp(a, b):
    return math.exp(b) - math.exp(a)

def f_sin(x):
    return math.sin(x)

def F_sin(a, b):
    return -math.cos(b) + math.cos(a)

def f_x2(x):
    return x ** 2

def F_x2(a, b):
    return (b**3 - a**3) / 3

def f_poly(x):
    return x**3 - 2*x + 1

def F_poly(a, b):
    return (b**4/4 - b**2 + b) - (a**4/4 - a**2 + a)

def f_gauss(x):
    return math.exp(-x**2)

def F_gauss(a, b):
    return math.sqrt(math.pi) / 2 * (math.erf(b) - math.erf(a))

def f_lorentz(x):
    return 1 / (1 + x**2)

def F_lorentz(a, b):
    return math.atan(b) - math.atan(a)

def f_ln(x):
    return math.log(x)

def F_ln(a, b):
    return (b * math.log(b) - b) - (a * math.log(a) - a)

def f_inv(x):
    return 1 / x

def F_inv(a, b):
    return math.log(b) - math.log(a)


# ── helpers ───────────────────────────────────────────────────────

def _setup(method):
    """Register test functions and sub-interval counts on *method*."""
    method.add_function("e^x", f_exp, F_exp)
    method.add_function("sin(x)", f_sin, F_sin)
    method.add_function("x^2", f_x2, F_x2)
    method.add_function("x^3-2x+1", f_poly, F_poly)
    method.add_function("e^(-x^2)", f_gauss, F_gauss)
    method.add_function("1/(1+x^2)", f_lorentz, F_lorentz)
    # n values: multiples of 6 to satisfy all rules
    method.set_sub_intervals([6, 12, 24, 48, 96])


# ── TrapezoidalRule ──────────────────────────────────────────────

class TestTrapezoidalRule:
    def test_method_name(self):
        trap = TrapezoidalRule()
        assert trap.get_method_name() == "Trapezoidal Rule"

    def test_known_integral_exp(self):
        trap = TrapezoidalRule()
        # ∫₀¹ e^x dx = e - 1 ≈ 1.71828
        approx = trap.integrate(f_exp, 0.0, 1.0, 100)
        assert approx == pytest.approx(math.e - 1, abs=1e-4)

    def test_known_integral_sin(self):
        trap = TrapezoidalRule()
        # ∫₀^π sin(x) dx = 2.0
        approx = trap.integrate(f_sin, 0.0, math.pi, 100)
        assert approx == pytest.approx(2.0, abs=1e-3)

    def test_known_integral_x2(self):
        trap = TrapezoidalRule()
        # ∫₀³ x² dx = 9.0
        approx = trap.integrate(f_x2, 0.0, 3.0, 100)
        assert approx == pytest.approx(9.0, abs=1e-3)

    def test_known_integral_gauss(self):
        trap = TrapezoidalRule()
        # ∫₀¹ e^(-x²) dx ≈ 0.74682
        exact = math.sqrt(math.pi) / 2 * math.erf(1)
        approx = trap.integrate(f_gauss, 0.0, 1.0, 100)
        assert approx == pytest.approx(exact, abs=1e-4)

    def test_known_integral_lorentz(self):
        trap = TrapezoidalRule()
        # ∫₀¹ 1/(1+x²) dx = π/4
        approx = trap.integrate(f_lorentz, 0.0, 1.0, 100)
        assert approx == pytest.approx(math.pi / 4, abs=1e-4)

    def test_known_integral_ln(self):
        trap = TrapezoidalRule()
        # ∫₁ᵉ ln(x) dx = 1.0
        approx = trap.integrate(f_ln, 1.0, math.e, 100)
        assert approx == pytest.approx(1.0, abs=1e-3)

    def test_known_integral_inv(self):
        trap = TrapezoidalRule()
        # ∫₁⁵ 1/x dx = ln(5) ≈ 1.60944
        approx = trap.integrate(f_inv, 1.0, 5.0, 100)
        assert approx == pytest.approx(math.log(5), abs=1e-3)

    def test_convergence(self):
        """Error should decrease as n increases (O(h²))."""
        trap = TrapezoidalRule()
        exact = F_exp(0.0, 1.0)
        errors = []
        for n in [10, 100, 1000]:
            approx = trap.integrate(f_exp, 0.0, 1.0, n)
            errors.append(abs(exact - approx))
        assert errors[0] > errors[1] > errors[2]

    def test_n_too_small_raises(self):
        trap = TrapezoidalRule()
        with pytest.raises(ValueError, match="n >= 1"):
            trap.integrate(f_exp, 0.0, 1.0, 0)


# ── Simpsons13 ───────────────────────────────────────────────────

class TestSimpsons13:
    def test_method_name(self):
        s13 = Simpsons13()
        assert s13.get_method_name() == "Simpson's 1/3 Rule"

    def test_known_integral_exp(self):
        s13 = Simpsons13()
        approx = s13.integrate(f_exp, 0.0, 1.0, 100)
        assert approx == pytest.approx(math.e - 1, abs=1e-8)

    def test_known_integral_sin(self):
        s13 = Simpsons13()
        approx = s13.integrate(f_sin, 0.0, math.pi, 100)
        assert approx == pytest.approx(2.0, abs=1e-7)

    def test_known_integral_x2(self):
        s13 = Simpsons13()
        # x² is degree 2 — Simpson's 1/3 is exact for polynomials up to degree 3
        approx = s13.integrate(f_x2, 0.0, 3.0, 2)
        assert approx == pytest.approx(9.0, abs=1e-12)

    def test_known_integral_poly(self):
        s13 = Simpsons13()
        # x³ - 2x + 1 is degree 3 — Simpson's 1/3 is exact for cubics
        approx = s13.integrate(f_poly, 0.0, 2.0, 2)
        assert approx == pytest.approx(2.0, abs=1e-12)

    def test_known_integral_gauss(self):
        s13 = Simpsons13()
        exact = math.sqrt(math.pi) / 2 * math.erf(1)
        approx = s13.integrate(f_gauss, 0.0, 1.0, 100)
        assert approx == pytest.approx(exact, abs=1e-8)

    def test_known_integral_lorentz(self):
        s13 = Simpsons13()
        approx = s13.integrate(f_lorentz, 0.0, 1.0, 100)
        assert approx == pytest.approx(math.pi / 4, abs=1e-8)

    def test_more_accurate_than_trapezoidal(self):
        """Simpson's 1/3 (O(h⁴)) should beat trapezoidal (O(h²)) for same n."""
        n = 10
        exact = F_exp(0.0, 1.0)
        trap_err = abs(exact - TrapezoidalRule().integrate(f_exp, 0.0, 1.0, n))
        s13_err = abs(exact - Simpsons13().integrate(f_exp, 0.0, 1.0, n))
        assert s13_err < trap_err

    def test_convergence(self):
        s13 = Simpsons13()
        exact = F_exp(0.0, 1.0)
        errors = []
        for n in [10, 100, 1000]:
            approx = s13.integrate(f_exp, 0.0, 1.0, n)
            errors.append(abs(exact - approx))
        assert errors[0] > errors[1] > errors[2]

    def test_odd_n_raises(self):
        s13 = Simpsons13()
        with pytest.raises(ValueError, match="even"):
            s13.integrate(f_exp, 0.0, 1.0, 3)

    def test_n_too_small_raises(self):
        s13 = Simpsons13()
        with pytest.raises(ValueError, match="even"):
            s13.integrate(f_exp, 0.0, 1.0, 1)


# ── Simpsons38 ───────────────────────────────────────────────────

class TestSimpsons38:
    def test_method_name(self):
        s38 = Simpsons38()
        assert s38.get_method_name() == "Simpson's 3/8 Rule"

    def test_known_integral_exp(self):
        s38 = Simpsons38()
        approx = s38.integrate(f_exp, 0.0, 1.0, 99)
        assert approx == pytest.approx(math.e - 1, abs=1e-8)

    def test_known_integral_sin(self):
        s38 = Simpsons38()
        approx = s38.integrate(f_sin, 0.0, math.pi, 99)
        assert approx == pytest.approx(2.0, abs=1e-7)

    def test_known_integral_x2(self):
        s38 = Simpsons38()
        # x² is degree 2 — Simpson's 3/8 is exact for polynomials up to degree 3
        approx = s38.integrate(f_x2, 0.0, 3.0, 3)
        assert approx == pytest.approx(9.0, abs=1e-12)

    def test_known_integral_poly(self):
        s38 = Simpsons38()
        # x³ - 2x + 1 is degree 3 — Simpson's 3/8 is exact for cubics
        approx = s38.integrate(f_poly, 0.0, 2.0, 3)
        assert approx == pytest.approx(2.0, abs=1e-12)

    def test_known_integral_gauss(self):
        s38 = Simpsons38()
        exact = math.sqrt(math.pi) / 2 * math.erf(1)
        approx = s38.integrate(f_gauss, 0.0, 1.0, 99)
        assert approx == pytest.approx(exact, abs=1e-8)

    def test_known_integral_lorentz(self):
        s38 = Simpsons38()
        approx = s38.integrate(f_lorentz, 0.0, 1.0, 99)
        assert approx == pytest.approx(math.pi / 4, abs=1e-8)

    def test_known_integral_ln(self):
        s38 = Simpsons38()
        approx = s38.integrate(f_ln, 1.0, math.e, 99)
        assert approx == pytest.approx(1.0, abs=1e-8)

    def test_known_integral_inv(self):
        s38 = Simpsons38()
        approx = s38.integrate(f_inv, 1.0, 5.0, 99)
        assert approx == pytest.approx(math.log(5), abs=1e-6)

    def test_convergence(self):
        s38 = Simpsons38()
        exact = F_exp(0.0, 1.0)
        errors = []
        for n in [9, 99, 999]:
            approx = s38.integrate(f_exp, 0.0, 1.0, n)
            errors.append(abs(exact - approx))
        assert errors[0] > errors[1] > errors[2]

    def test_n_not_multiple_of_3_raises(self):
        s38 = Simpsons38()
        with pytest.raises(ValueError, match="multiple of 3"):
            s38.integrate(f_exp, 0.0, 1.0, 4)

    def test_n_too_small_raises(self):
        s38 = Simpsons38()
        with pytest.raises(ValueError, match="multiple of 3"):
            s38.integrate(f_exp, 0.0, 1.0, 1)


# ── compute_all / display / edge cases ───────────────────────────

class TestComputeAll:
    def test_results_count(self):
        trap = TrapezoidalRule()
        _setup(trap)
        results = trap.compute_all(0.0, 1.0)
        # 6 functions × 5 sub-interval counts = 30 rows
        assert len(results) == 30

    def test_results_structure(self):
        s13 = Simpsons13()
        _setup(s13)
        results = s13.compute_all(0.0, 1.0)
        for r in results:
            assert hasattr(r, "function_name")
            assert hasattr(r, "n")
            assert hasattr(r, "exact")
            assert hasattr(r, "approx")
            assert hasattr(r, "error")
            assert r.error >= 0

    def test_no_functions_raises(self):
        trap = TrapezoidalRule()
        trap.set_sub_intervals([6])
        with pytest.raises(ValueError, match="no functions registered"):
            trap.compute_all(0.0, 1.0)

    def test_no_sub_intervals_raises(self):
        trap = TrapezoidalRule()
        trap.add_function("e^x", f_exp, F_exp)
        with pytest.raises(ValueError, match="no sub-interval counts set"):
            trap.compute_all(0.0, 1.0)

    def test_invalid_bounds_raises(self):
        trap = TrapezoidalRule()
        trap.add_function("e^x", f_exp, F_exp)
        trap.set_sub_intervals([10])
        with pytest.raises(ValueError, match="a must be less than b"):
            trap.compute_all(1.0, 0.0)

    def test_properties(self):
        trap = TrapezoidalRule()
        _setup(trap)
        assert trap.num_functions == 6
        assert trap.num_n == 5
        assert trap.get_function(0).name == "e^x"
        assert trap.get_n(0) == 6


# ── builtin functions ────────────────────────────────────────────

class TestBuiltinFunctions:
    def test_builtin_count(self):
        assert len(BUILTIN_FUNCTIONS) == 8

    def test_add_builtin_functions(self):
        trap = TrapezoidalRule()
        trap.add_builtin_functions()
        assert trap.num_functions == 8

    def test_builtin_names(self):
        expected_names = [
            "e^x", "sin(x)", "x^2", "x^3-2x+1",
            "e^(-x^2)", "1/(1+x^2)", "ln(x)", "1/x",
        ]
        for i, (name, _, _) in enumerate(BUILTIN_FUNCTIONS):
            assert name == expected_names[i]

    def test_builtin_exact_integrals(self):
        """Verify exact integral functions against known values."""
        _, _, F = BUILTIN_FUNCTIONS[0]  # e^x
        assert F(0, 1) == pytest.approx(math.e - 1)

        _, _, F = BUILTIN_FUNCTIONS[1]  # sin(x)
        assert F(0, math.pi) == pytest.approx(2.0)

        _, _, F = BUILTIN_FUNCTIONS[2]  # x^2
        assert F(0, 3) == pytest.approx(9.0)

        _, _, F = BUILTIN_FUNCTIONS[4]  # e^(-x^2)
        assert F(0, 1) == pytest.approx(0.746824, abs=1e-5)

        _, _, F = BUILTIN_FUNCTIONS[5]  # 1/(1+x^2)
        assert F(0, 1) == pytest.approx(math.pi / 4)
