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

    def plot(self, num_samples: int = 200, title: str | None = None,
             show_points: bool = True, save_path: str | None = None,
             func=None, func_label: str | None = None) -> None:
        """Plot the interpolated curve, original data points, and optionally
        the actual function for comparison.

        Args:
            num_samples: Number of points for the smooth curve.
            title: Plot title (default: class name).
            show_points: Whether to mark the original data points.
            save_path: If given, save the figure to this path.
            func: Optional callable — the actual function for comparison.
            func_label: Label for the actual function curve.
        """
        import matplotlib.pyplot as plt
        import numpy as np

        if title is None:
            title = self.__class__.__name__ + " Interpolation"

        x_min = min(self._x)
        x_max = max(self._x)

        # extend range slightly for visual padding
        padding = (x_max - x_min) * 0.05
        x_plot = np.linspace(x_min - padding, x_max + padding, num_samples)
        y_plot = [self.evaluate(xi) for xi in x_plot]

        fig, ax = plt.subplots(figsize=(10, 6))

        # interpolated curve
        ax.plot(x_plot, y_plot, '-', linewidth=2, label="Interpolated Curve",
                color='#2196F3')

        # original data points
        if show_points:
            ax.scatter(self._x, self._y, color='#F44336', zorder=5, s=60,
                       edgecolors='white', linewidths=1.5,
                       label="Data Points")

        # actual function overlay
        if func is not None:
            y_actual = [func(xi) for xi in x_plot]
            label = func_label if func_label else "Actual Function"
            ax.plot(x_plot, y_actual, '--', linewidth=1.5, label=label,
                    color='#4CAF50', alpha=0.8)

        ax.set_title(title, fontsize=14, fontweight='bold')
        ax.set_xlabel("x", fontsize=12)
        ax.set_ylabel("y", fontsize=12)
        ax.legend(fontsize=10)
        ax.grid(True, alpha=0.3)
        fig.tight_layout()

        if save_path:
            fig.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"  Plot saved to {save_path}")

        plt.show()

