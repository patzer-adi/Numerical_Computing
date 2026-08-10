"""
CLI helpers for the PyNumerics interactive complex number calculator.

Provides reusable, testable functions for reading complex numbers from
stdin and displaying operation results. Used by ``main.py``.

Ported from C++ ``main.cpp`` interactive demo.
"""

from __future__ import annotations

import math
from pynumerics.complex import Complex


def read_complex(prompt: str = "Enter complex number") -> Complex:
    """Read a complex number from stdin (real and imaginary parts).

    Prompts the user for real and imaginary parts separately.
    Retries on invalid (non-numeric) input with a helpful error message.

    Args:
        prompt: Label shown before prompting for parts.

    Returns:
        A Complex number constructed from user input.

    Example interaction::

        Enter complex number
          Real part: 3.0
          Imaginary part: 4.0
        → Complex(3.0, 4.0)
    """
    while True:
        try:
            print(prompt)
            re = float(input("  Real part: "))
            im = float(input("  Imaginary part: "))
            return Complex(re, im)
        except ValueError:
            print("  ⚠ Invalid input — please enter numeric values.\n")


def display_operations(c1: Complex, c2: Complex) -> None:
    """Display all arithmetic operations between two complex numbers.

    Shows addition, subtraction, multiplication, and division.
    Handles division by zero gracefully.

    Args:
        c1: First complex number.
        c2: Second complex number.
    """
    print("\n┌─────────────────────────────────────────┐")
    print("│         Arithmetic Operations           │")
    print("├─────────────────────────────────────────┤")
    print(f"│  z₁ = {c1}")
    print(f"│  z₂ = {c2}")
    print("├─────────────────────────────────────────┤")

    # Addition
    result = c1 + c2
    print(f"│  z₁ + z₂  =  {result}")

    # Subtraction
    result = c1 - c2
    print(f"│  z₁ - z₂  =  {result}")

    # Multiplication
    result = c1 * c2
    print(f"│  z₁ * z₂  =  {result}")

    # Division — handle zero divisor
    try:
        result = c1 / c2
        print(f"│  z₁ / z₂  =  {result}")
    except ZeroDivisionError:
        print("│  z₁ / z₂  =  undefined (division by zero)")

    print("└─────────────────────────────────────────┘")


def display_properties(c: Complex, label: str = "z") -> None:
    """Display mathematical properties of a complex number.

    Shows norm (modulus), conjugate, and argument (angle).

    Args:
        c: The complex number to analyse.
        label: Display label for the number (e.g., "z₁", "z₂").
    """
    print(f"\n┌─────────────────────────────────────────┐")
    print(f"│  Properties of {label} = {c}")
    print(f"├─────────────────────────────────────────┤")
    print(f"│  Norm (|{label}|)       =  {c.norm():.4f}")
    print(f"│  Norm² (|{label}|²)     =  {c.norm_squared():.4f}")
    print(f"│  Conjugate ({label}*)   =  {c.conjugate()}")
    print(f"│  Arg ({label})          =  {c.arg():.4f} rad  ({math.degrees(c.arg()):.2f}°)")
    print(f"└─────────────────────────────────────────┘")


def print_menu() -> None:
    """Display the main menu."""
    print("\n╔══════════════════════════════════════════╗")
    print("║     PyNumerics — Complex Calculator      ║")
    print("╠══════════════════════════════════════════╣")
    print("║  1. Enter two complex numbers             ║")
    print("║  2. Show all operations (+ - * /)         ║")
    print("║  3. Show properties (norm, conj, arg)     ║")
    print("║  4. Show everything                       ║")
    print("║  5. Exit                                  ║")
    print("╚══════════════════════════════════════════╝")


def run_cli() -> None:
    """Run the interactive complex number calculator.

    Main loop that presents a menu and handles user choices.
    This is the entry point called by ``main.py``.
    """
    print("\n" + "=" * 44)
    print("  PyNumerics — Complex Number Calculator")
    print("  Ported from C++ Numerical Computing Library")
    print("=" * 44)

    c1: Complex | None = None
    c2: Complex | None = None

    while True:
        print_menu()

        try:
            choice = input("\n  Enter choice (1-5): ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\n\n  Goodbye!")
            break

        if choice == "1":
            print("\n── Enter Complex Numbers ──")
            c1 = read_complex("  First complex number (z₁):")
            c2 = read_complex("  Second complex number (z₂):")
            print(f"\n  ✓ z₁ = {c1}")
            print(f"  ✓ z₂ = {c2}")

        elif choice == "2":
            if c1 is None or c2 is None:
                print("\n  ⚠ Please enter two complex numbers first (option 1).")
            else:
                display_operations(c1, c2)

        elif choice == "3":
            if c1 is None or c2 is None:
                print("\n  ⚠ Please enter two complex numbers first (option 1).")
            else:
                display_properties(c1, "z₁")
                display_properties(c2, "z₂")

        elif choice == "4":
            if c1 is None or c2 is None:
                print("\n  ⚠ Please enter two complex numbers first (option 1).")
            else:
                display_operations(c1, c2)
                display_properties(c1, "z₁")
                display_properties(c2, "z₂")

        elif choice == "5":
            print("\n  Goodbye!")
            break

        else:
            print("\n  ⚠ Invalid choice — please enter a number between 1 and 5.")
