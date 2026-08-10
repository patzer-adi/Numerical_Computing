"""
CUDA Acceleration Layer for PyNumerics.

This module provides a dispatch mechanism that routes computationally intensive
operations to CUDA-backed implementations when available, falling back to pure
Python implementations otherwise.

Since CUDA requires NVIDIA GPU hardware and the CUDA toolkit (not available on
macOS), this module serves as an architectural "seam" — the interface is defined
now so that future CUDA backends (via PyCUDA, CuPy, or Numba CUDA) can be
plugged in without modifying any solver or matrix code.

Usage:
    from pynumerics.accel import get_backend

    backend = get_backend()
    result = backend.mat_mul(A, B)  # Dispatches to CUDA or Python
"""

import os
import logging
from abc import ABC, abstractmethod

logger = logging.getLogger(__name__)


def cuda_available() -> bool:
    """Check if CUDA acceleration is available.

    Checks for:
    1. PYNUMERICS_USE_CUDA environment variable
    2. cupy importability
    3. numba.cuda importability
    """
    if os.environ.get("PYNUMERICS_USE_CUDA", "").lower() in ("0", "false", "no"):
        return False

    try:
        import cupy  # noqa: F401
        return True
    except ImportError:
        pass

    try:
        from numba import cuda  # noqa: F401
        return cuda.is_available()
    except ImportError:
        pass

    return False


class ComputeBackend(ABC):
    """Abstract backend for compute-intensive operations."""

    @property
    @abstractmethod
    def name(self) -> str:
        pass

    @abstractmethod
    def mat_mul(self, A: list[list[float]], B: list[list[float]]) -> list[list[float]]:
        """Matrix multiplication."""
        pass

    @abstractmethod
    def mat_vec_mul(self, A: list[list[float]], x: list[float]) -> list[float]:
        """Matrix-vector multiplication."""
        pass

    @abstractmethod
    def solve_triangular_lower(self, L: list[list[float]], b: list[float]) -> list[float]:
        """Forward substitution: solve Lx = b where L is lower triangular."""
        pass

    @abstractmethod
    def solve_triangular_upper(self, U: list[list[float]], b: list[float]) -> list[float]:
        """Back substitution: solve Ux = b where U is upper triangular."""
        pass


class PythonBackend(ComputeBackend):
    """Pure Python fallback backend (always available)."""

    @property
    def name(self) -> str:
        return "python"

    def mat_mul(self, A: list[list[float]], B: list[list[float]]) -> list[list[float]]:
        rows_a, cols_a = len(A), len(A[0])
        rows_b, cols_b = len(B), len(B[0])
        assert cols_a == rows_b
        result = [[0.0] * cols_b for _ in range(rows_a)]
        for i in range(rows_a):
            for j in range(cols_b):
                result[i][j] = sum(A[i][k] * B[k][j] for k in range(cols_a))
        return result

    def mat_vec_mul(self, A: list[list[float]], x: list[float]) -> list[float]:
        return [sum(A[i][j] * x[j] for j in range(len(x))) for i in range(len(A))]

    def solve_triangular_lower(self, L: list[list[float]], b: list[float]) -> list[float]:
        n = len(b)
        x = [0.0] * n
        for i in range(n):
            x[i] = (b[i] - sum(L[i][j] * x[j] for j in range(i))) / L[i][i]
        return x

    def solve_triangular_upper(self, U: list[list[float]], b: list[float]) -> list[float]:
        n = len(b)
        x = [0.0] * n
        for i in range(n - 1, -1, -1):
            x[i] = (b[i] - sum(U[i][j] * x[j] for j in range(i + 1, n))) / U[i][i]
        return x


class CUDABackend(ComputeBackend):
    """CUDA-accelerated backend (requires cupy or numba.cuda).

    This class is a stub — it will be populated when CUDA hardware is available.
    Currently, it falls back to Python implementations with a warning.
    """

    @property
    def name(self) -> str:
        return "cuda"

    def mat_mul(self, A, B):
        # TODO: Implement with cupy.matmul or custom CUDA kernel
        logger.warning("CUDA mat_mul not implemented, falling back to Python")
        return PythonBackend().mat_mul(A, B)

    def mat_vec_mul(self, A, x):
        logger.warning("CUDA mat_vec_mul not implemented, falling back to Python")
        return PythonBackend().mat_vec_mul(A, x)

    def solve_triangular_lower(self, L, b):
        logger.warning("CUDA solve_triangular_lower not implemented, falling back to Python")
        return PythonBackend().solve_triangular_lower(L, b)

    def solve_triangular_upper(self, U, b):
        logger.warning("CUDA solve_triangular_upper not implemented, falling back to Python")
        return PythonBackend().solve_triangular_upper(U, b)


# Singleton backend instance
_backend: ComputeBackend | None = None


def get_backend() -> ComputeBackend:
    """Get the active compute backend (CUDA if available, else Python)."""
    global _backend
    if _backend is None:
        if cuda_available():
            _backend = CUDABackend()
            logger.info("CUDA backend activated")
        else:
            _backend = PythonBackend()
            logger.debug("Using Python backend (CUDA not available)")
    return _backend


def set_backend(backend: ComputeBackend | None) -> None:
    """Override the active compute backend (for testing or manual control)."""
    global _backend
    _backend = backend
