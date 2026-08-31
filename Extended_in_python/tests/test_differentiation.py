"""
Test suite for Numerical Differentiation methods.
"""

import pytest
import math
from pynumerics.differentiation.forward import ForwardDifference
from pynumerics.differentiation.backward import BackwardDifference
from pynumerics.differentiation.central import CentralDifference
from pynumerics.differentiation.richardson import RichardsonExtrapolation


# ── test functions (same as C++ differentiation_example.cpp) ──────

def f_exp(x):
    return math.exp(x)

def df_exp(x):
    return math.exp(x)

def f_sin(x):
    return math.sin(x)

def df_sin(x):
    return math.cos(x)

def f_poly(x):
    return x**3 - 2*x + 1

def df_poly(x):
    return 3*x**2 - 2


# ── helpers ───────────────────────────────────────────────────────

def _setup(method):
    """Register test functions and step sizes on *method*."""
    method.add_function("e^x", f_exp, df_exp)
    method.add_function("sin(x)", f_sin, df_sin)
    method.add_function("x^3-2x+1", f_poly, df_poly)
    method.set_step_sizes([0.1, 0.01, 0.001, 0.0001])


# ── ForwardDifference ────────────────────────────────────────────

class TestForwardDifference:
    def test_method_name(self):
        fwd = ForwardDifference()
        assert fwd.get_method_name() == "Forward Difference"

    def test_known_derivative_exp(self):
        fwd = ForwardDifference()
        # f(x) = e^x, f'(1) = e ≈ 2.71828
        approx = fwd.compute_derivative(f_exp, 1.0, 0.001)
        assert approx == pytest.approx(math.e, abs=1e-2)

    def test_known_derivative_poly(self):
        fwd = ForwardDifference()
        # f(x) = x^3 - 2x + 1, f'(1) = 3 - 2 = 1
        approx = fwd.compute_derivative(f_poly, 1.0, 0.0001)
        assert approx == pytest.approx(1.0, abs=1e-3)

    def test_convergence(self):
        """Error should decrease as h shrinks (O(h))."""
        fwd = ForwardDifference()
        exact = df_exp(1.0)
        errors = []
        for h in [0.1, 0.01, 0.001]:
            approx = fwd.compute_derivative(f_exp, 1.0, h)
            errors.append(abs(exact - approx))
        assert errors[0] > errors[1] > errors[2]


# ── BackwardDifference ───────────────────────────────────────────

class TestBackwardDifference:
    def test_method_name(self):
        bwd = BackwardDifference()
        assert bwd.get_method_name() == "Backward Difference"

    def test_known_derivative_exp(self):
        bwd = BackwardDifference()
        approx = bwd.compute_derivative(f_exp, 1.0, 0.001)
        assert approx == pytest.approx(math.e, abs=1e-2)

    def test_known_derivative_sin(self):
        bwd = BackwardDifference()
        # f(x) = sin(x), f'(π/4) = cos(π/4) ≈ 0.70711
        approx = bwd.compute_derivative(f_sin, math.pi / 4, 0.0001)
        assert approx == pytest.approx(math.cos(math.pi / 4), abs=1e-3)

    def test_convergence(self):
        bwd = BackwardDifference()
        exact = df_sin(1.0)
        errors = []
        for h in [0.1, 0.01, 0.001]:
            approx = bwd.compute_derivative(f_sin, 1.0, h)
            errors.append(abs(exact - approx))
        assert errors[0] > errors[1] > errors[2]


# ── CentralDifference ────────────────────────────────────────────

class TestCentralDifference:
    def test_method_name(self):
        cen = CentralDifference()
        assert cen.get_method_name() == "Central Difference"

    def test_known_derivative_exp(self):
        cen = CentralDifference()
        approx = cen.compute_derivative(f_exp, 1.0, 0.01)
        assert approx == pytest.approx(math.e, abs=1e-4)

    def test_more_accurate_than_forward(self):
        """Central (O(h²)) should beat forward (O(h)) for same h."""
        h = 0.01
        exact = df_exp(1.0)
        fwd_err = abs(exact - ForwardDifference().compute_derivative(f_exp, 1.0, h))
        cen_err = abs(exact - CentralDifference().compute_derivative(f_exp, 1.0, h))
        assert cen_err < fwd_err

    def test_convergence(self):
        cen = CentralDifference()
        exact = df_exp(1.0)
        errors = []
        for h in [0.1, 0.01, 0.001]:
            approx = cen.compute_derivative(f_exp, 1.0, h)
            errors.append(abs(exact - approx))
        assert errors[0] > errors[1] > errors[2]


# ── RichardsonExtrapolation ──────────────────────────────────────

class TestRichardsonExtrapolation:
    def test_method_name(self):
        rich = RichardsonExtrapolation()
        assert rich.get_method_name() == "Richardson Extrapolation"

    def test_known_derivative_exp(self):
        rich = RichardsonExtrapolation()
        approx = rich.compute_derivative(f_exp, 1.0, 0.1)
        assert approx == pytest.approx(math.e, abs=1e-6)

    def test_more_accurate_than_central(self):
        """Richardson (O(h⁴)) should beat central (O(h²)) for same h."""
        h = 0.1
        exact = df_exp(1.0)
        cen_err = abs(exact - CentralDifference().compute_derivative(f_exp, 1.0, h))
        rich_err = abs(exact - RichardsonExtrapolation().compute_derivative(f_exp, 1.0, h))
        assert rich_err < cen_err

    def test_poly_exact(self):
        """For a cubic, Richardson with central diff should be nearly exact."""
        rich = RichardsonExtrapolation()
        # f(x) = x^3 - 2x + 1, f'(2) = 3*4 - 2 = 10
        approx = rich.compute_derivative(f_poly, 2.0, 0.1)
        assert approx == pytest.approx(10.0, abs=1e-10)


# ── compute_all / display / edge cases ───────────────────────────

class TestComputeAll:
    def test_results_count(self):
        fwd = ForwardDifference()
        _setup(fwd)
        results = fwd.compute_all(1.0)
        # 3 functions × 4 step sizes = 12 rows
        assert len(results) == 12

    def test_results_structure(self):
        cen = CentralDifference()
        _setup(cen)
        results = cen.compute_all(1.0)
        for r in results:
            assert hasattr(r, "function_name")
            assert hasattr(r, "h")
            assert hasattr(r, "exact")
            assert hasattr(r, "approx")
            assert hasattr(r, "error")
            assert r.error >= 0

    def test_no_functions_raises(self):
        fwd = ForwardDifference()
        fwd.set_step_sizes([0.1])
        with pytest.raises(ValueError, match="no functions registered"):
            fwd.compute_all(1.0)

    def test_no_step_sizes_raises(self):
        fwd = ForwardDifference()
        fwd.add_function("e^x", f_exp, df_exp)
        with pytest.raises(ValueError, match="no step sizes set"):
            fwd.compute_all(1.0)

    def test_properties(self):
        fwd = ForwardDifference()
        _setup(fwd)
        assert fwd.num_functions == 3
        assert fwd.num_h == 4
        assert fwd.get_function(0).name == "e^x"
        assert fwd.get_h(0) == 0.1
