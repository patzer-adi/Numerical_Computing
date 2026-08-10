"""
Test suite for CUDA acceleration layer (tests Python backend).
"""

import pytest
from pynumerics.accel import PythonBackend, get_backend, set_backend


class TestPythonBackend:
    def setup_method(self):
        self.backend = PythonBackend()

    def test_name(self):
        assert self.backend.name == "python"

    def test_mat_mul(self):
        A = [[1.0, 2.0], [3.0, 4.0]]
        B = [[5.0, 6.0], [7.0, 8.0]]
        result = self.backend.mat_mul(A, B)
        assert result[0][0] == pytest.approx(19.0)
        assert result[0][1] == pytest.approx(22.0)
        assert result[1][0] == pytest.approx(43.0)
        assert result[1][1] == pytest.approx(50.0)

    def test_mat_vec_mul(self):
        A = [[1.0, 2.0], [3.0, 4.0]]
        x = [1.0, 1.0]
        result = self.backend.mat_vec_mul(A, x)
        assert result[0] == pytest.approx(3.0)
        assert result[1] == pytest.approx(7.0)

    def test_solve_triangular_lower(self):
        L = [[2.0, 0.0], [1.0, 3.0]]
        b = [4.0, 7.0]
        x = self.backend.solve_triangular_lower(L, b)
        assert x[0] == pytest.approx(2.0)
        assert x[1] == pytest.approx(5.0 / 3.0)

    def test_solve_triangular_upper(self):
        U = [[2.0, 1.0], [0.0, 3.0]]
        b = [5.0, 6.0]
        x = self.backend.solve_triangular_upper(U, b)
        assert x[1] == pytest.approx(2.0)
        assert x[0] == pytest.approx(1.5)

    def test_get_backend_returns_python(self):
        # Reset and ensure fallback is Python on Mac
        set_backend(None)
        backend = get_backend()
        assert backend.name == "python"
