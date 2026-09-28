"""
Numerical Integration package for PyNumerics.
"""

from pynumerics.integration.base import (
    Integration,
    IntegrationFunctionEntry,
    IntegrationResult,
    BUILTIN_FUNCTIONS,
)
from pynumerics.integration.trapezoidal import TrapezoidalRule
from pynumerics.integration.simpsons13 import Simpsons13
from pynumerics.integration.simpsons38 import Simpsons38

__all__ = [
    "Integration",
    "IntegrationFunctionEntry",
    "IntegrationResult",
    "BUILTIN_FUNCTIONS",
    "TrapezoidalRule",
    "Simpsons13",
    "Simpsons38",
]
