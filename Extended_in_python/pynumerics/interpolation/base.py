"""
Interpolation base class.

ABC for all interpolation methods. Uses COMPOSITION — holds data arrays.
"""

from abc import ABC, abstractmethod
from dataclasses import dataclass
import math


@dataclass
class FitResult:
    """Result of a curve fitting operation."""
    coefficients: dict[str, float]
    sse: float = 0.0
    mse: float = 0.0
    rmse: float = 0.0


class Interpolation(ABC):
    """Abstract base class for interpolation methods."""

    def __init__(self, x_data: list[float], y_data: list[float]):
        if len(x_data) != len(y_data):
            raise ValueError("x and y data must have same length")
        self._x = list(x_data)
        self._y = list(y_data)
        self.num_points = len(x_data)

    def get_x(self, i: int) -> float:
        return self._x[i]

    def get_y(self, i: int) -> float:
        return self._y[i]

    @abstractmethod
    def evaluate(self, x: float) -> float:
        """Evaluate the interpolation/fit at point x."""
        pass

    def compute_errors(self) -> FitResult:
        """Compute SSE, MSE, RMSE for the fit against original data."""
        sse = 0.0
        for i in range(self.num_points):
            pred = self.evaluate(self._x[i])
            err = self._y[i] - pred
            sse += err * err
        mse = sse / self.num_points if self.num_points > 0 else 0.0
        rmse = math.sqrt(mse)
        return FitResult(coefficients={}, sse=sse, mse=mse, rmse=rmse)
