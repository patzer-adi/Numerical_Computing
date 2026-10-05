"""
Test suite for Random Number Generation — LCG.
"""

import pytest
from pynumerics.rng.lcg import LCG
from pynumerics.rng.base import RandomNumberGenerator


# ── LCG recurrence correctness ───────────────────────────────────

class TestLCGRecurrence:
    def test_known_sequence_small(self):
        """Verify X_{n+1} = (a*X_n + c) mod m with small parameters."""
        # a=5, c=3, m=16, seed=0
        # X_1 = (5*0 + 3) % 16 = 3
        # X_2 = (5*3 + 3) % 16 = 18 % 16 = 2
        # X_3 = (5*2 + 3) % 16 = 13
        # X_4 = (5*13 + 3) % 16 = 68 % 16 = 4
        lcg = LCG(seed=0, a=5, c=3, m=16)
        assert lcg.next_int() == 3
        assert lcg.next_int() == 2
        assert lcg.next_int() == 13
        assert lcg.next_int() == 4

    def test_known_sequence_glibc(self):
        """Verify first few values with glibc defaults (a=1103515245, c=12345, m=2^31)."""
        lcg = LCG(seed=0)
        # X_1 = (1103515245 * 0 + 12345) % 2^31 = 12345
        assert lcg.next_int() == 12345
        # X_2 = (1103515245 * 12345 + 12345) % 2^31
        expected = (1103515245 * 12345 + 12345) % (2**31)
        assert lcg.next_int() == expected

    def test_manual_recurrence(self):
        """Verify against manual computation for several steps."""
        a, c, m, seed = 7, 3, 32, 1
        lcg = LCG(seed=seed, a=a, c=c, m=m)
        state = seed
        for _ in range(20):
            state = (a * state + c) % m
            assert lcg.next_int() == state


# ── Reproducibility ──────────────────────────────────────────────

class TestReproducibility:
    def test_same_seed_same_sequence(self):
        """Same seed → identical sequence."""
        s1 = LCG(seed=42).generate(100)
        s2 = LCG(seed=42).generate(100)
        assert s1 == s2

    def test_different_seeds_different_sequence(self):
        """Different seeds → different sequences."""
        s1 = LCG(seed=1).generate(100)
        s2 = LCG(seed=2).generate(100)
        assert s1 != s2

    def test_reset_reproduces(self):
        """reset() restores initial seed state."""
        lcg = LCG(seed=42)
        s1 = lcg.generate(50)
        lcg.reset()
        s2 = lcg.generate(50)
        assert s1 == s2

    def test_reset_state(self):
        """After reset, state equals initial seed."""
        lcg = LCG(seed=99, a=5, c=3, m=128)
        lcg.generate(10)
        assert lcg.state != 99
        lcg.reset()
        assert lcg.state == 99

    def test_generate_advances_state(self):
        """generate() advances state — subsequent calls give different values."""
        lcg = LCG(seed=42)
        s1 = lcg.generate(5)
        s2 = lcg.generate(5)
        assert s1 != s2  # second batch continues from where first left off


# ── Range and normalization ──────────────────────────────────────

class TestRange:
    def test_uniform_in_unit_interval(self):
        """All U_n must be in [0, 1)."""
        samples = LCG(seed=42).generate(10000)
        assert all(0.0 <= u < 1.0 for u in samples)

    def test_int_in_range(self):
        """All X_n must be in [0, m)."""
        m = 2**31
        lcg = LCG(seed=42, m=m)
        for _ in range(1000):
            x = lcg.next_int()
            assert 0 <= x < m

    def test_normalization(self):
        """U_n = X_n / m."""
        m = 100
        lcg = LCG(seed=7, a=13, c=5, m=m)
        # get the first raw int
        x = lcg.next_int()
        # reset, get the first uniform
        lcg.reset()
        u = lcg.next_uniform()
        assert u == pytest.approx(x / m)

    def test_small_m_covers_range(self):
        """With small m, generated values should cover [0, 1) spread."""
        # a=1, c=1, m=8 → cycle 1,2,3,4,5,6,7,0,1,2,...
        lcg = LCG(seed=0, a=1, c=1, m=8)
        samples = lcg.generate(8)
        # should contain values near 0 and near 1
        assert min(samples) < 0.2
        assert max(samples) > 0.7


# ── Generate count ───────────────────────────────────────────────

class TestGenerateCount:
    def test_exact_count(self):
        assert len(LCG(seed=0).generate(1000)) == 1000

    def test_generate_zero(self):
        assert LCG(seed=0).generate(0) == []

    def test_generate_one(self):
        assert len(LCG(seed=0).generate(1)) == 1

    def test_generate_ints_count(self):
        assert len(LCG(seed=0).generate_ints(500)) == 500

    def test_negative_n_raises(self):
        with pytest.raises(ValueError, match="non-negative"):
            LCG(seed=0).generate(-1)


# ── Parameter validation ─────────────────────────────────────────

class TestValidation:
    def test_m_zero_raises(self):
        with pytest.raises(ValueError, match="positive"):
            LCG(seed=0, m=0)

    def test_m_negative_raises(self):
        with pytest.raises(ValueError, match="positive"):
            LCG(seed=0, m=-1)

    def test_a_too_large_raises(self):
        with pytest.raises(ValueError, match="0 <= a < m"):
            LCG(seed=0, a=100, m=50)

    def test_a_negative_raises(self):
        with pytest.raises(ValueError, match="0 <= a < m"):
            LCG(seed=0, a=-1, m=16)

    def test_c_too_large_raises(self):
        with pytest.raises(ValueError, match="0 <= c < m"):
            LCG(seed=0, a=3, c=100, m=50)

    def test_c_negative_raises(self):
        with pytest.raises(ValueError, match="0 <= c < m"):
            LCG(seed=0, a=3, c=-1, m=16)

    def test_seed_too_large_raises(self):
        with pytest.raises(ValueError, match="0 <= seed < m"):
            LCG(seed=100, a=3, c=1, m=50)

    def test_seed_negative_raises(self):
        with pytest.raises(ValueError, match="0 <= seed < m"):
            LCG(seed=-1, a=3, c=1, m=16)


# ── Edge cases ───────────────────────────────────────────────────

class TestEdgeCases:
    def test_seed_zero_valid(self):
        """seed=0 is valid and produces deterministic output."""
        lcg = LCG(seed=0)
        x = lcg.next_int()
        assert isinstance(x, int)
        assert x >= 0

    def test_small_cycle(self):
        """LCG with a=1, c=1, m=4 cycles through 0→1→2→3→0."""
        lcg = LCG(seed=0, a=1, c=1, m=4)
        vals = lcg.generate_ints(8)
        assert vals[:4] == [1, 2, 3, 0]
        assert vals[4:] == [1, 2, 3, 0]

    def test_m_equals_one(self):
        """m=1 means all values are 0 (degenerate case)."""
        lcg = LCG(seed=0, a=0, c=0, m=1)
        samples = lcg.generate(5)
        assert all(u == 0.0 for u in samples)

    def test_identity_generator(self):
        """a=1, c=0 → X_{n+1} = X_n. Constant sequence."""
        lcg = LCG(seed=5, a=1, c=0, m=16)
        vals = lcg.generate_ints(5)
        assert all(v == 5 for v in vals)


# ── ABC contract ─────────────────────────────────────────────────

class TestABCContract:
    def test_is_instance_of_base(self):
        lcg = LCG(seed=42)
        assert isinstance(lcg, RandomNumberGenerator)

    def test_method_name(self):
        lcg = LCG(seed=42)
        assert "LCG" in lcg.get_method_name()

    def test_seed_property(self):
        lcg = LCG(seed=99)
        assert lcg.seed == 99

    def test_modulus_property(self):
        lcg = LCG(seed=0, a=5, c=3, m=256)
        assert lcg.modulus == 256

    def test_state_property(self):
        lcg = LCG(seed=42, a=5, c=3, m=128)
        assert lcg.state == 42
        lcg.next_int()
        assert lcg.state != 42

    def test_lcg_parameters(self):
        lcg = LCG(seed=0, a=7, c=3, m=32)
        assert lcg.a == 7
        assert lcg.c == 3
        assert lcg.m == 32


# ── Display and save ─────────────────────────────────────────────

class TestDisplaySave:
    def test_display_no_crash(self, capsys):
        lcg = LCG(seed=42)
        samples = lcg.generate(5)
        lcg.display(samples)
        captured = capsys.readouterr()
        assert "LCG" in captured.out
        assert "Seed: 42" in captured.out

    def test_display_uses_stored(self, capsys):
        lcg = LCG(seed=42)
        lcg.generate(3)
        lcg.display()  # no explicit samples — should use stored
        captured = capsys.readouterr()
        assert "LCG" in captured.out

    def test_display_no_samples_raises(self):
        lcg = LCG(seed=42)
        with pytest.raises(ValueError, match="no samples"):
            lcg.display()

    def test_save_results(self, tmp_path):
        lcg = LCG(seed=42)
        samples = lcg.generate(10)
        filepath = str(tmp_path / "rng_output.txt")
        lcg.save_results(filepath, samples)
        with open(filepath) as f:
            content = f.read()
        assert "LCG" in content
        assert "Seed: 42" in content
        lines = [l for l in content.strip().split("\n") if not l.startswith("#")]
        assert len(lines) == 11  # header + 10 data rows


# ── Sanity statistical check ─────────────────────────────────────

class TestStatisticalSanity:
    def test_mean_approximately_half(self):
        """With glibc defaults and N=10000, mean ≈ 0.5.

        This is a sanity check, not a rigorous statistical test.
        Generous tolerance: within 0.02 of 0.5.
        """
        samples = LCG(seed=42).generate(10000)
        mean = sum(samples) / len(samples)
        assert abs(mean - 0.5) < 0.02


# ── Plot smoke tests ─────────────────────────────────────────────

class TestPlotSmoke:
    def test_plot_sequence(self, tmp_path):
        import matplotlib
        matplotlib.use('Agg')
        lcg = LCG(seed=42)
        samples = lcg.generate(100)
        lcg.plot_sequence(samples, save_path=str(tmp_path / "seq.png"))

    def test_plot_histogram(self, tmp_path):
        import matplotlib
        matplotlib.use('Agg')
        lcg = LCG(seed=42)
        samples = lcg.generate(1000)
        lcg.plot_histogram(samples, save_path=str(tmp_path / "hist.png"))

    def test_plot_sequence_uses_stored(self, tmp_path):
        import matplotlib
        matplotlib.use('Agg')
        lcg = LCG(seed=42)
        lcg.generate(50)
        lcg.plot_sequence(save_path=str(tmp_path / "seq2.png"))

    def test_plot_histogram_uses_stored(self, tmp_path):
        import matplotlib
        matplotlib.use('Agg')
        lcg = LCG(seed=42)
        lcg.generate(500)
        lcg.plot_histogram(save_path=str(tmp_path / "hist2.png"))
