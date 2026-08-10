"""
Test suite for Gershgorin Circle Theorem Analyzer.
"""

import pytest
from pynumerics.matrix import Matrix
from pynumerics.eigen.gershgorin import GershgorinAnalyzer


class TestGershgorinAnalyzer:
    def test_3x3_diagonally_dominant(self):
        m = Matrix.from_list([
            [10.0, 1.0, 1.0],
            [1.0, 10.0, 1.0],
            [1.0, 1.0, 10.0]
        ])
        ga = GershgorinAnalyzer(m)
        discs = ga.solve()
        assert len(discs) == 3
        for d in discs:
            assert d.center == 10.0
            assert d.radius == 2.0
            assert d.low == 8.0
            assert d.high == 12.0

    def test_analysis_insights(self):
        m = Matrix.from_list([
            [10.0, 1.0, 1.0],
            [1.0, 10.0, 1.0],
            [1.0, 1.0, 10.0]
        ])
        ga = GershgorinAnalyzer(m)
        analysis = ga.get_analysis()
        assert analysis["all_eigenvalues_positive"] is True
        assert analysis["could_be_singular"] is False
        assert analysis["diagonally_dominant"] is True
        assert analysis["symmetric"] is True

    def test_singular_detection(self):
        m = Matrix.from_list([
            [1.0, 2.0],
            [2.0, 1.0]
        ])
        ga = GershgorinAnalyzer(m)
        analysis = ga.get_analysis()
        # Disc 0: center=1, radius=2, low=-1, high=3 → contains 0
        assert analysis["could_be_singular"] is True

    def test_get_disc(self):
        m = Matrix.from_list([[5.0, 1.0], [1.0, 5.0]])
        ga = GershgorinAnalyzer(m)
        ga.solve()
        d = ga.get_disc(0)
        assert d.center == 5.0
        assert d.radius == 1.0

    def test_union_interval(self):
        m = Matrix.from_list([
            [4.0, 1.0],
            [1.0, 6.0]
        ])
        ga = GershgorinAnalyzer(m)
        analysis = ga.get_analysis()
        assert analysis["union_interval"] == (3.0, 7.0)
