"""
Root-finding package for PyNumerics.
"""

from pynumerics.roots.result import RootResult
from pynumerics.roots.base import RootHunter
from pynumerics.roots.bisection import Bisection
from pynumerics.roots.newton_raphson import NewtonRaphson
from pynumerics.roots.fixed_point import FixedPoint

__all__ = [
    "RootResult",
    "RootHunter",
    "Bisection",
    "NewtonRaphson",
    "FixedPoint",
]
