"""
Built-in test functions for interpolation.

Provides ready-to-use data generators for common mathematical functions.
Used by the CLI and plotting for comparison overlays.
"""

import math


def generate_data(
    func, x_start: float, x_end: float, num_points: int
) -> tuple[list[float], list[float]]:
    """Generate (x, y) data from a function over [x_start, x_end].

    Args:
        func: Callable that takes a float and returns a float.
        x_start: Start of the interval.
        x_end: End of the interval.
        num_points: Number of evenly spaced data points.

    Returns:
        Tuple of (x_data, y_data) lists.
    """
    if num_points < 2:
        raise ValueError("need at least 2 data points")
    if x_start >= x_end:
        raise ValueError("x_start must be less than x_end")

    step = (x_end - x_start) / (num_points - 1)
    x_data = [x_start + i * step for i in range(num_points)]
    y_data = [func(x) for x in x_data]
    return x_data, y_data


# ── Pre-defined functions ────────────────────────

def sin_func(x: float) -> float:
    """f(x) = sin(x)"""
    return math.sin(x)


def cos_func(x: float) -> float:
    """f(x) = cos(x)"""
    return math.cos(x)


def poly_func(x: float) -> float:
    """f(x) = x² + 3x + 4"""
    return x ** 2 + 3 * x + 4


def exp_func(x: float) -> float:
    """f(x) = eˣ"""
    return math.exp(x)


def log_func(x: float) -> float:
    """f(x) = ln(x)  (x > 0)"""
    return math.log(x)


def tan_func(x: float) -> float:
    """f(x) = tan(x)"""
    return math.tan(x)


def sqrt_func(x: float) -> float:
    """f(x) = √x  (x ≥ 0)"""
    return math.sqrt(x)


# ── Registry for CLI use ─────────────────────────

FUNCTIONS: dict[str, tuple] = {
    "sin(x)": (sin_func, -3.14, 3.14),
    "cos(x)": (cos_func, -3.14, 3.14),
    "x^2+3x+4": (poly_func, -5.0, 5.0),
    "e^x": (exp_func, 0.0, 3.0),
    "ln(x)": (log_func, 0.1, 5.0),
    "tan(x)": (tan_func, -1.0, 1.0),
    "sqrt(x)": (sqrt_func, 0.0, 10.0),
}
"""Registry mapping function names to (callable, default_x_start, default_x_end)."""
