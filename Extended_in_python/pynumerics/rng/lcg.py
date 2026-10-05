"""
Linear Congruential Generator (LCG).

    X_{n+1} = (a * X_n + c) mod m

Normalized uniform values:

    U_n = X_n / m

Default parameters are the glibc constants:
    a = 1103515245
    c = 12345
    m = 2^31 = 2147483648

These produce a full period of 2^31 values and are widely documented
(Knuth, TAOCP Vol 2).  Students can verify their implementation
against published tables using these defaults.

Constraints:
    m > 0
    0 <= a < m
    0 <= c < m
    0 <= seed < m

C++ equivalent:
    class LCG : public RandomNumberGenerator { ... };
"""

from pynumerics.rng.base import RandomNumberGenerator


class LCG(RandomNumberGenerator):
    """Linear Congruential Generator.

    X_{n+1} = (a * X_n + c) mod m

    Args:
        seed: Initial state X_0.  Must satisfy 0 <= seed < m.
        a: Multiplier.  Must satisfy 0 <= a < m.
        c: Increment.   Must satisfy 0 <= c < m.
        m: Modulus.      Must be > 0.
    """

    def __init__(
        self,
        seed: int = 42,
        a: int = 1103515245,
        c: int = 12345,
        m: int = 2**31,
    ) -> None:
        # validate parameters
        if m <= 0:
            raise ValueError(f"modulus m must be positive, got {m}")
        if not (0 <= a < m):
            raise ValueError(f"multiplier a must satisfy 0 <= a < m, "
                             f"got a={a}, m={m}")
        if not (0 <= c < m):
            raise ValueError(f"increment c must satisfy 0 <= c < m, "
                             f"got c={c}, m={m}")
        if not (0 <= seed < m):
            raise ValueError(f"seed must satisfy 0 <= seed < m, "
                             f"got seed={seed}, m={m}")

        super().__init__(seed=seed, modulus=m)
        self._a = a
        self._c = c
        self._m = m
        self._state = seed

    # ── core recurrence ───────────────────────────────────────────

    def next_int(self) -> int:
        """Advance state: X_{n+1} = (a * X_n + c) mod m.

        Returns:
            The new state X_{n+1} as an integer in [0, m).
        """
        self._state = (self._a * self._state + self._c) % self._m
        return self._state

    def get_method_name(self) -> str:
        return "Linear Congruential Generator (LCG)"

    # ── reset hook ────────────────────────────────────────────────

    def _on_reset(self) -> None:
        """Reset internal state to the initial seed."""
        self._state = self._initial_seed

    # ── properties ────────────────────────────────────────────────

    @property
    def state(self) -> int:
        """Current internal state X_n."""
        return self._state

    @property
    def a(self) -> int:
        """Multiplier."""
        return self._a

    @property
    def c(self) -> int:
        """Increment."""
        return self._c

    @property
    def m(self) -> int:
        """Modulus."""
        return self._m

    # ── display override ──────────────────────────────────────────

    def _print_parameters(self) -> None:
        """Print LCG-specific parameters."""
        print(f"Parameters: a = {self._a}, c = {self._c}, m = {self._m}")
        print(f"Seed: {self._initial_seed}")
