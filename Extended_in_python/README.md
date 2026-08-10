# PyNumerics - Comprehensive Documentation & README

PyNumerics is a comprehensive, interactive mathematical sandbox written in Python. Ported from a C++ codebase, it provides a robust suite of tools for numerical computing, including complex number arithmetic, matrix operations, root-finding algorithms, and solving systems of linear equations.

---

## 1. Directory Structure

The project is structured as a Python package named `pynumerics`, with modular sub-packages for different mathematical concepts.

```text
Numerical_Computing_codes/NC_in_python/
├── main.py                # Main entry point for the interactive CLI menu
├── pyproject.toml         # Python project configuration file
├── tests/                 # Unit tests for the package
└── pynumerics/            # Main source code package
    ├── __init__.py
    ├── cli.py             # CLI menu for Complex Number operations
    ├── cli_roots.py       # CLI menu for Root Finding Methods
    ├── cli_solvers.py     # CLI menu for Linear System Solvers
    ├── complex.py         # Complex number class implementation
    ├── matrix.py          # Matrix class implementation
    ├── exceptions.py      # Custom exception classes
    ├── roots/             # Root-finding algorithms
    │   ├── base.py        # Abstract base class `RootHunter`
    │   ├── bisection.py
    │   ├── fixed_point.py
    │   ├── newton_raphson.py
    │   └── result.py
    ├── solvers/           # Linear equations solvers
    │   ├── base.py        # Abstract base class `SystemOfLinearEquationSolver`
    │   ├── cholesky.py
    │   ├── crout.py
    │   ├── doolittle.py
    │   ├── gauss_jacobi.py
    │   ├── gauss_seidel.py
    │   ├── gaussian.py
    │   ├── lu_base.py
    │   └── result.py
    ├── eigen/             # Eigenvalue algorithms
    │   ├── base.py
    │   └── gershgorin.py
    ├── interpolation/     # Interpolation and curve fitting algorithms
    │   ├── base.py
    │   ├── lagrange.py
    │   ├── least_square_line.py
    │   └── least_square_parabola.py
    └── accel/             # Numerical acceleration methods
```

---

## 2. How it is Written: Classes, Functions, and Code Flow

The code is written using strong Object-Oriented design patterns. Mathematical entities and algorithms are represented as classes.

### Core Classes & Functions

- **`Complex` (`complex.py`)**: 
  - **Purpose**: Represents a complex number (re + im).
  - **Functions/Methods**: Implements `@classmethod` constructors like `from_real` and `from_imaginary`. It overloads standard Python operators (`__add__`, `__sub__`, `__mul__`, `__truediv__`) so you can add complex numbers together just like normal numbers. It also includes mathematical methods like `norm()`, `conjugate()`, and `arg()`.

- **`Matrix` (`matrix.py`)**: 
  - **Purpose**: Represents a 2D matrix of floating-point numbers.
  - **Functions/Methods**: Stores data in a row-major 2D list `_data`. It overloads operators (`__add__`, `__mul__`) for matrix arithmetic. It includes advanced mathematical functions like `transpose()`, `determinant()` (calculated via Gaussian elimination), `adjoint()`, `inverse()`, and property checkers like `is_symmetric()` and `is_diagonally_dominant()`.

### Algorithmic Packages

- **Solvers (`solvers/`)**: Contains classes to solve systems of linear equations ($Ax = b$).
  - **`SystemOfLinearEquationSolver` (`solvers/base.py`)**: An abstract base class that *inherits* from `Matrix`. This means every solver instance is itself a coefficient matrix. It defines an abstract `solve()` method.
  - **Specific Solvers**: Subclasses like `GaussianElimination`, `GaussJacobi`, and `GaussSeidel` implement the `solve()` method using their specific mathematical algorithms.

- **Roots (`roots/`)**: Contains classes for finding roots of functions ($f(x) = 0$).
  - **`RootHunter` (`roots/base.py`)**: An abstract base class defining the blueprint for root-finding algorithms.
  - **Specific Finders**: Subclasses like `BisectionMethod` and `NewtonRaphson` implement the actual logic in their `solve()` methods.

- **Interpolation (`interpolation/`) & Eigen (`eigen/`)**: Follow the same pattern—a base class defining the interface and specific algorithm classes implementing the logic.

### User Interface (CLI)
The program provides an interactive terminal UI. `main.py` presents a master console loop that routes user choices to sub-menus implemented in `cli.py` (Complex numbers), `cli_solvers.py` (Linear systems), and `cli_roots.py` (Root finding).

---

## 3. Object-Oriented Programming (OOP) Concepts Used

The codebase makes extensive use of the four pillars of OOP:

1. **Encapsulation**:
   - Internal states are hidden and manipulated through methods. For example, `Matrix` hides its 2D list `_data` and provides methods like `__getitem__` and `__setitem__` to interact with elements safely.

2. **Abstraction**:
   - The `abc` (Abstract Base Classes) module is heavily used. `SystemOfLinearEquationSolver` and `RootHunter` are abstract classes (`ABC`). They define a `solve()` method decorated with `@abstractmethod`. This abstracts away the implementation details; the user only needs to know they can call `solve()` on any solver object without worrying about how it works internally.

3. **Inheritance**:
   - The code relies on inheritance to share logic and build hierarchies. `SystemOfLinearEquationSolver` inherits from `Matrix`, automatically gaining all matrix operations (addition, determinant, etc.) while adding equation-solving capabilities. Individual algorithms (like `GaussSeidel`) then inherit from `SystemOfLinearEquationSolver`.

4. **Polymorphism**:
   - **Operator Overloading**: Python "dunder" methods (e.g., `__add__`, `__mul__`, `__str__`) are used extensively in `Complex` and `Matrix` to allow objects to be manipulated with standard mathematical operators (`a + b`, `a * b`).
   - **Method Overriding**: Subclasses provide their own specific implementation of the abstract `solve()` method, allowing the program to execute different code dynamically based on the object type (e.g., Gaussian vs. Gauss-Jacobi).

---

## 4. How to Use the Code

### Running the Application (Interactive CLI)
The project is designed to be run as an interactive Command Line Application. Start it by running `main.py` from the root directory:

```bash
python3 main.py
```

This launches the "PyNumerics Master Console," presenting a numbered menu to choose between Complex Numbers, Linear Systems, or Root finding.

### Using as a Library in Python Scripts
You can import and use the classes programmatically for numerical computing:

```python
from pynumerics.matrix import Matrix
from pynumerics.complex import Complex

# Matrix Operations
A = Matrix.from_list([[1, 2], [3, 4]])
B = Matrix.from_list([[5, 6], [7, 8]])
C = A * B
print("Matrix Multiplication:\n", C)

# Complex Numbers
z1 = Complex(1, 2)
z2 = Complex(3, 4)
print("Complex Addition: ", z1 + z2)
```

---

## 5. How to Make Documentation for this Codebase

If you wanted to generate formal documentation (like a website, HTML files, or PDF) for this project, the groundwork is already beautifully laid out in the code. Here is exactly how that documentation process works:

1. **Docstrings (Already Done)**: 
   The code uses excellent standard Python docstrings (following Google or Sphinx style) for all classes and methods. It correctly documents arguments, return types, and provides usage examples right in the code.
2. **Type Hinting (Already Done)**: 
   The code uses Python type hints (e.g., `-> Matrix`, `cols: int`). Documentation tools automatically parse these to display the exact expected types.
3. **Documentation Generators (The Next Step)**: 
   To actually build the documentation files from this code, you would use an automated parsing tool:
   - **Sphinx**: The industry standard for Python documentation. You run `sphinx-quickstart`, point it to your `pynumerics` folder, and extensions like `sphinx.ext.autodoc` and `sphinx.ext.napoleon` pull in your docstrings to generate a professional HTML website.
   - **pdoc**: A simpler, zero-configuration alternative. Running `pdoc ./pynumerics` from the terminal instantly generates HTML documentation based on your code.
   - **MkDocs** (with MkDocstrings): A modern alternative that generates Markdown-based static sites.
