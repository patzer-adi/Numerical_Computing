"""
CLI helpers for PyNumerics random number generation module.

Provides an interactive menu for:
- Seed and parameter configuration
- Sample generation
- Display and file output
- Sequence and histogram plotting
"""

from __future__ import annotations

from pynumerics.rng.lcg import LCG
from pynumerics.rng.base import RandomNumberGenerator


def run_rng_cli() -> None:
    """Run the interactive random number generation calculator."""
    print("\n" + "=" * 44)
    print("  PyNumerics — Random Number Generation")
    print("=" * 44)

    rng: RandomNumberGenerator | None = None
    samples: list[float] | None = None

    # defaults
    seed = 42
    n_samples = 100
    a = 1103515245
    c = 12345
    m = 2**31

    while True:
        print("\n╔══════════════════════════════════════════╗")
        print("║      Random Number Generation            ║")
        print("╠══════════════════════════════════════════╣")
        print("║  Setup:                                  ║")
        print("║  1. Set seed                             ║")
        print("║  2. Set number of samples                ║")
        print("║  3. Set LCG parameters (a, c, m)         ║")
        print("║                                          ║")
        print("║  Generate:                               ║")
        print("║  4. Generate samples                     ║")
        print("║  5. Reset generator                      ║")
        print("║                                          ║")
        print("║  Output:                                 ║")
        print("║  6. Display sample table                 ║")
        print("║  7. Save results to file                 ║")
        print("║  8. Plot sequence (U_n vs n)             ║")
        print("║  9. Plot histogram                       ║")
        print("║                                          ║")
        print("║  0. Return to Main Menu                  ║")
        print("╚══════════════════════════════════════════╝")

        status = f"  [seed={seed}, N={n_samples}, a={a}, c={c}, m={m}]"
        print(status)

        try:
            choice = input("\n  Enter choice (0-9): ").strip()
        except (EOFError, KeyboardInterrupt):
            break

        # ── 1. Set seed ──────────────────────────────────────────
        if choice == "1":
            try:
                seed = int(input("  Enter seed: ").strip())
                rng = None  # invalidate — will recreate on generate
                samples = None
                print(f"  ✓ Seed set to {seed}")
            except ValueError:
                print("  ⚠ Invalid integer.")

        # ── 2. Set number of samples ─────────────────────────────
        elif choice == "2":
            try:
                n_samples = int(input("  Enter number of samples: ").strip())
                if n_samples < 0:
                    print("  ⚠ Must be non-negative.")
                    continue
                print(f"  ✓ Sample count set to {n_samples}")
            except ValueError:
                print("  ⚠ Invalid integer.")

        # ── 3. Set LCG parameters ────────────────────────────────
        elif choice == "3":
            try:
                a_str = input(f"  Multiplier a [{a}]: ").strip()
                c_str = input(f"  Increment c [{c}]: ").strip()
                m_str = input(f"  Modulus m [{m}]: ").strip()
                if a_str:
                    a = int(a_str)
                if c_str:
                    c = int(c_str)
                if m_str:
                    m = int(m_str)
                rng = None  # invalidate
                samples = None
                print(f"  ✓ Parameters: a={a}, c={c}, m={m}")
            except ValueError:
                print("  ⚠ Invalid integer.")

        # ── 4. Generate samples ──────────────────────────────────
        elif choice == "4":
            try:
                rng = LCG(seed=seed, a=a, c=c, m=m)
                samples = rng.generate(n_samples)
                print(f"\n  ✅ Generated {n_samples} samples "
                      f"(seed={seed}, a={a}, c={c}, m={m})")
            except ValueError as e:
                print(f"  ⚠ Error: {e}")
                rng = None
                samples = None

        # ── 5. Reset generator ───────────────────────────────────
        elif choice == "5":
            if rng is None:
                print("  ⚠ No generator created yet. Generate first (option 4).")
                continue
            rng.reset()
            samples = None
            print("  ✓ Generator reset to initial seed.")

        # ── 6. Display ───────────────────────────────────────────
        elif choice == "6":
            if rng is None or samples is None:
                print("  ⚠ No samples to display. Generate first (option 4).")
                continue
            # for large N, ask whether to show all or first/last
            if len(samples) > 40:
                show_all = input(
                    f"  {len(samples)} samples — show all? (y/n) [n]: "
                ).strip().lower()
                if show_all == "y":
                    rng.display(samples)
                else:
                    print(f"\n  First 20:")
                    rng.display(samples[:20])
                    print(f"  Last 20:")
                    rng.display(samples[-20:])
            else:
                rng.display(samples)

        # ── 7. Save results ──────────────────────────────────────
        elif choice == "7":
            if rng is None or samples is None:
                print("  ⚠ No samples to save. Generate first (option 4).")
                continue
            filename = input("  Enter filename: ").strip()
            if filename:
                try:
                    rng.save_results(filename, samples)
                except Exception as e:
                    print(f"  ⚠ Error: {e}")

        # ── 8. Plot sequence ─────────────────────────────────────
        elif choice == "8":
            if rng is None or samples is None:
                print("  ⚠ No samples to plot. Generate first (option 4).")
                continue
            try:
                save_str = input("  Save to file? Enter path or press Enter to skip: ").strip()
                save_path = save_str if save_str else None
                rng.plot_sequence(samples, save_path=save_path)
            except Exception as e:
                print(f"  ⚠ Error: {e}")

        # ── 9. Plot histogram ────────────────────────────────────
        elif choice == "9":
            if rng is None or samples is None:
                print("  ⚠ No samples to plot. Generate first (option 4).")
                continue
            try:
                bins_str = input("  Number of bins [50]: ").strip()
                bins = int(bins_str) if bins_str else 50
                save_str = input("  Save to file? Enter path or press Enter to skip: ").strip()
                save_path = save_str if save_str else None
                rng.plot_histogram(samples, bins=bins, save_path=save_path)
            except ValueError:
                print("  ⚠ Invalid bin count.")
            except Exception as e:
                print(f"  ⚠ Error: {e}")

        # ── 0. Exit ──────────────────────────────────────────────
        elif choice == "0":
            break
        else:
            print("  ⚠ Invalid choice.")
