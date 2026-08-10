#!/usr/bin/env python3
"""
PyNumerics — Interactive Mathematical Sandbox.

Entry point for the menu-driven CLI demo. Links together all modules.
"""

from pynumerics.cli import run_cli as run_complex_cli
from pynumerics.cli_solvers import run_solvers_cli
from pynumerics.cli_roots import run_roots_cli


def run_main_menu() -> None:
    while True:
        print("\n╔══════════════════════════════════════════╗")
        print("║      PyNumerics Master Console           ║")
        print("╠══════════════════════════════════════════╣")
        print("║  1. Complex Number Calculator            ║")
        print("║  2. Linear System Solvers & Matrices     ║")
        print("║  3. Root Finding Methods                 ║")
        print("║  9. Exit                                 ║")
        print("╚══════════════════════════════════════════╝")

        try:
            choice = input("\n  Enter choice (1-9): ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\n\n  Goodbye!")
            break

        if choice == "1":
            run_complex_cli()
        elif choice == "2":
            run_solvers_cli()
        elif choice == "3":
            run_roots_cli()
        elif choice == "9":
            print("\n  Goodbye!")
            break
        else:
            print("  ⚠ Invalid choice.")


if __name__ == "__main__":
    run_main_menu()
