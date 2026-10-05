"""
Random Number Generator — abstract base class.

ABC for all pseudo-random number generators.  Independent of Matrix —
the RNG is a stateful generator, not a data container.

Follows the same pattern as RootHunter(ABC): abstract base with shared
infrastructure (display, save, plot) and a pure-virtual core method
that subclasses override.

Hierarchy:
    RandomNumberGenerator (abstract base)
        └── LCG
        └── (future: MersenneTwister, etc.)

C++ equivalent:
    class RandomNumberGenerator {
        virtual int nextInt() = 0;
        ...
    };
"""

from abc import ABC, abstractmethod


class RandomNumberGenerator(ABC):
    """Abstract base class for pseudo-random number generators.

    Subclasses override:
        next_int()        — advance state and return the next raw integer
        get_method_name() — human-readable generator name

    Provides shared infrastructure:
        next_uniform()    — normalized value in [0, 1)
        generate(n)       — batch of n uniform values
        generate_ints(n)  — batch of n raw integer states
        reset()           — restore initial seed
        display()         — formatted table to stdout
        save_results()    — write to file
        plot_sequence()   — U_n vs n scatter
        plot_histogram()  — histogram of generated values
    """

    def __init__(self, seed: int, modulus: int) -> None:
        """Initialize with a seed and modulus (for normalization).

        Args:
            seed: Initial state / seed value.
            modulus: The modulus m (used by next_uniform to normalize).
        """
        self._initial_seed = seed
        self._seed = seed
        self._modulus = modulus
        self._last_samples: list[float] | None = None
        self._last_raw: list[int] | None = None

    # ── pure-virtual interface ────────────────────────────────────

    @abstractmethod
    def next_int(self) -> int:
        """Advance the internal state and return the next raw integer."""
        pass

    @abstractmethod
    def get_method_name(self) -> str:
        """Return a human-readable name for this generator."""
        pass

    # ── concrete methods ──────────────────────────────────────────

    def next_uniform(self) -> float:
        """Advance state and return the next value normalized to [0, 1)."""
        return self.next_int() / self._modulus

    def generate(self, n: int) -> list[float]:
        """Generate *n* uniform values in [0, 1).

        Advances the state n times. Does NOT reset first — call
        ``reset()`` explicitly if you want to restart.

        The generated samples are also stored internally and can be
        accessed by ``display()`` and ``plot_*()`` without passing
        them explicitly.

        Returns:
            A list of n floats in [0, 1).
        """
        if n < 0:
            raise ValueError("n must be non-negative")
        samples = [self.next_uniform() for _ in range(n)]
        self._last_samples = samples
        return list(samples)

    def generate_ints(self, n: int) -> list[int]:
        """Generate *n* raw integer states.

        Advances the state n times. Also stores normalized values
        internally.

        Returns:
            A list of n integers in [0, m).
        """
        if n < 0:
            raise ValueError("n must be non-negative")
        raw = [self.next_int() for _ in range(n)]
        self._last_raw = raw
        self._last_samples = [x / self._modulus for x in raw]
        return list(raw)

    def reset(self) -> None:
        """Reset the generator to its initial seed state."""
        self._seed = self._initial_seed
        self._on_reset()

    def _on_reset(self) -> None:
        """Hook for subclasses to reset additional state."""
        pass

    # ── properties ────────────────────────────────────────────────

    @property
    def seed(self) -> int:
        """The initial seed (the value passed to the constructor)."""
        return self._initial_seed

    @property
    def modulus(self) -> int:
        """The modulus m."""
        return self._modulus

    # ── display ───────────────────────────────────────────────────

    def display(self, samples: list[float] | None = None) -> None:
        """Print a formatted table of generated values.

        Args:
            samples: Values to display. If None, uses the last batch
                     from ``generate()`` or ``generate_ints()``.
        """
        samples = self._resolve_samples(samples)

        title = f"=== {self.get_method_name()} ==="
        print(f"\n{title}")
        self._print_parameters()
        print()

        header = f"{'n':>8}{'U_n':>16}"
        print(header)
        print("-" * len(header))

        for i, u in enumerate(samples, 1):
            print(f"{i:>8d}{u:>16.6f}")
        print()

    def _print_parameters(self) -> None:
        """Print generator-specific parameters. Override in subclass."""
        print(f"Seed: {self._initial_seed}")

    # ── save to file ──────────────────────────────────────────────

    def save_results(self, filename: str,
                     samples: list[float] | None = None) -> None:
        """Write generated values to *filename*.

        Args:
            filename: Output file path.
            samples: Values to save. If None, uses the last batch.
        """
        samples = self._resolve_samples(samples)

        with open(filename, "w") as fout:
            fout.write(f"# {self.get_method_name()}\n")
            fout.write(f"# Seed: {self._initial_seed}\n")
            fout.write(f"{'n':>8}{'U_n':>16}\n")
            for i, u in enumerate(samples, 1):
                fout.write(f"{i:>8d}{u:>16.10f}\n")
        print(f"Results saved to {filename}")

    # ── plotting ──────────────────────────────────────────────────

    def plot_sequence(
        self,
        samples: list[float] | None = None,
        title: str | None = None,
        save_path: str | None = None,
    ) -> None:
        """Scatter plot of U_n vs sample index n.

        Args:
            samples: Values to plot. If None, uses the last batch.
            title: Plot title (auto-generated if None).
            save_path: If given, save the figure to this path.
        """
        import matplotlib.pyplot as plt

        samples = self._resolve_samples(samples)
        n_vals = list(range(1, len(samples) + 1))

        fig, ax = plt.subplots(figsize=(10, 6))

        ax.scatter(n_vals, samples, s=2, color='#2196F3', alpha=0.6)

        ax.axhline(0, color='#cccccc', linewidth=0.8)
        ax.axhline(1, color='#cccccc', linewidth=0.8)
        ax.set_ylim(-0.05, 1.05)

        if title is None:
            title = (f"{self.get_method_name()} — Sequence"
                     f"  (N = {len(samples)}, seed = {self._initial_seed})")
        ax.set_title(title, fontsize=14, fontweight='bold')
        ax.set_xlabel("n", fontsize=12)
        ax.set_ylabel("U_n", fontsize=12)
        ax.grid(True, alpha=0.3)
        fig.tight_layout()

        if save_path:
            fig.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"  Plot saved to {save_path}")

        plt.show()

    def plot_histogram(
        self,
        samples: list[float] | None = None,
        bins: int = 50,
        title: str | None = None,
        save_path: str | None = None,
    ) -> None:
        """Histogram of generated values to inspect uniformity.

        Args:
            samples: Values to plot. If None, uses the last batch.
            bins: Number of histogram bins.
            title: Plot title (auto-generated if None).
            save_path: If given, save the figure to this path.
        """
        import matplotlib.pyplot as plt

        samples = self._resolve_samples(samples)

        fig, ax = plt.subplots(figsize=(10, 6))

        ax.hist(samples, bins=bins, range=(0, 1), color='#2196F3',
                edgecolor='white', alpha=0.8, label='Generated')

        # expected count per bin for uniform distribution
        expected = len(samples) / bins
        ax.axhline(expected, color='#F44336', linestyle='--',
                   linewidth=1.5, label=f'Expected uniform ({expected:.0f})')

        if title is None:
            title = (f"{self.get_method_name()} — Histogram"
                     f"  (N = {len(samples)}, bins = {bins})")
        ax.set_title(title, fontsize=14, fontweight='bold')
        ax.set_xlabel("Value", fontsize=12)
        ax.set_ylabel("Count", fontsize=12)
        ax.legend(fontsize=10)
        ax.grid(True, alpha=0.3)
        fig.tight_layout()

        if save_path:
            fig.savefig(save_path, dpi=150, bbox_inches='tight')
            print(f"  Plot saved to {save_path}")

        plt.show()

    # ── internal helpers ──────────────────────────────────────────

    def _resolve_samples(self, samples: list[float] | None) -> list[float]:
        """Return explicit samples if given, else fall back to stored batch."""
        if samples is not None:
            return samples
        if self._last_samples is not None:
            return self._last_samples
        raise ValueError("no samples available — call generate() first "
                         "or pass samples explicitly")
