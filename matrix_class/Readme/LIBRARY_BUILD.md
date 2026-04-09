# Building & Using the Matrix Libraries

How to build `libmatrix_cpu.so`, `libmatrix_gpu.so`, and `libmatrix.a`, and link them into your own programs.

> For a deep dive into how libraries work under the hood, see [LIBRARY_DEEP_DIVE.md](LIBRARY_DEEP_DIVE.md).

---

## Prerequisites

| Tool | Required For | Install |
|------|-------------|---------|
| g++ 4.8+ | Everything | `sudo apt install g++` or Xcode Command Line Tools |
| make | Everything | Usually pre-installed |
| CUDA Toolkit 7.0+ | `libmatrix_gpu.so` only | [developer.nvidia.com/cuda-downloads](https://developer.nvidia.com/cuda-downloads) |
| NVIDIA GPU (CC 5.0+) | Running GPU library | Maxwell+ (GTX 750 Ti and above) |

---

## Quick Start

```bash
cd matrix_class/

# Build the CPU shared library
make -f Makefile.lib cpu_shared

# Build the static library
make -f Makefile.lib static

# Build the GPU shared library (requires CUDA)
make -f Makefile.lib gpu_shared

# Build everything (CPU shared + static)
make -f Makefile.lib all

# See all available targets
make -f Makefile.lib info

# Clean build artifacts
make -f Makefile.lib clean
```

---

## What Gets Built

```
matrix_class/
├── build/
│   ├── cpu/           ← intermediate object files (CPU)
│   └── gpu/           ← intermediate object files (GPU)
└── lib/
    ├── libmatrix_cpu.so   ← shared library, CPU only (~150 KB)
    ├── libmatrix_gpu.so   ← shared library, CPU + CUDA GPU
    └── libmatrix.a        ← static archive, CPU only (~200 KB)
```

### Which library should I use?

| Library | When to use |
|---------|------------|
| `libmatrix_cpu.so` | Default choice. Shared library, no GPU required. |
| `libmatrix_gpu.so` | You have an NVIDIA GPU and want auto-accelerated operations. |
| `libmatrix.a` | You want a single self-contained binary with no runtime deps. |

---

## Linking Against the Libraries

### Option 1: Shared Library (`libmatrix_cpu.so`)

```bash
# Compile your program
g++ -std=c++11 -Iinclude -o my_app my_app.cpp \
    -Llib -lmatrix_cpu -Wl,-rpath,/full/path/to/matrix_class/lib

# Run it
./my_app
```

**Flags explained:**
- `-Iinclude` — tells the compiler where `Matrix.hpp` and other headers are
- `-Llib` — tells the linker where `libmatrix_cpu.so` is
- `-lmatrix_cpu` — links against `libmatrix_cpu.so` (the `lib` prefix and `.so` suffix are implicit)
- `-Wl,-rpath,...` — embeds the library path so the binary finds the `.so` at runtime

### Option 2: Static Library (`libmatrix.a`)

```bash
# Compile your program (library code is embedded into the binary)
g++ -std=c++11 -Iinclude -o my_app my_app.cpp lib/libmatrix.a

# Run it — no library files needed at runtime
./my_app
```

### Option 3: GPU Shared Library (`libmatrix_gpu.so`)

```bash
# Compile your program
g++ -std=c++11 -Iinclude -o my_app my_app.cpp \
    -Llib -lmatrix_gpu -lcudart -Wl,-rpath,/full/path/to/matrix_class/lib

# Run it
./my_app
```

The program automatically uses GPU for large matrices and CPU for small ones. No code changes needed — the dispatch logic is inside the library.

---

## Minimal Example

```cpp
// my_app.cpp
#include "Matrix.hpp"
#include "LUDecomposition.hpp"
#include <iostream>
using namespace std;

int main() {
    // Create a 3x3 matrix
    Matrix A(3, 3);
    A.setData(0, 0, 2); A.setData(0, 1, 1); A.setData(0, 2, -1);
    A.setData(1, 0, -3); A.setData(1, 1, -1); A.setData(1, 2, 2);
    A.setData(2, 0, -2); A.setData(2, 1, 1); A.setData(2, 2, 2);

    cout << "Matrix A:" << endl;
    A.display();

    cout << "det(A) = " << A.determinant() << endl;

    cout << "Inverse:" << endl;
    A.inverse().display();

    // Solve Ax = b using Doolittle LU
    Doolittle solver(3, 3);
    solver.copyFrom(A);
    double b[] = {8, -11, -3};
    double *x = solver.solve(b, 3);
    cout << "Solution: x = [" << x[0] << ", " << x[1] << ", " << x[2] << "]" << endl;
    delete[] x;

    return 0;
}
```

Build and run:
```bash
# With shared library
g++ -std=c++11 -Iinclude -o my_app my_app.cpp -Llib -lmatrix_cpu -Wl,-rpath,$(pwd)/lib

# Or with static library
g++ -std=c++11 -Iinclude -o my_app my_app.cpp lib/libmatrix.a

./my_app
```

---

## System-Wide Install

```bash
# Install (copies libs to /usr/local/lib, headers to /usr/local/include/matrix/)
sudo make -f Makefile.lib install

# Now you can compile from anywhere:
g++ -std=c++11 -I/usr/local/include/matrix my_app.cpp -lmatrix_cpu

# Uninstall
sudo make -f Makefile.lib uninstall
```

After installing on Linux, run `sudo ldconfig` to update the shared library cache.

---

## Available Headers

| Header | What It Provides |
|--------|-----------------|
| `Matrix.hpp` | Core `Matrix` class — constructors, add, subtract, multiply, transpose, determinant, inverse |
| `GaussianElimination.hpp` | Gaussian elimination solver (with/without pivoting) |
| `LUDecomposition.hpp` | Doolittle, Crout, and Cholesky LU decomposition solvers |
| `GaussJacobi.hpp` | Gauss-Jacobi iterative solver |
| `MatrixException.hpp` | Exception class for matrix errors |
| `SystemOfLinearEquationSolver.hpp` | Abstract base class for all solvers |

---

## Examples

Pre-built example programs are in `examples/`:

```bash
# Direct compilation (no library needed)
make -f examples/Makefile

# Link against shared library (build the library first)
make -f Makefile.lib cpu_shared
make -f examples/Makefile example_basic_shared
./example_basic_shared

# Link against static library
make -f Makefile.lib static
make -f examples/Makefile example_basic_static
./example_basic_static
```

---

## Troubleshooting

### `error while loading shared libraries: libmatrix_cpu.so: cannot open shared object file`

The dynamic linker can't find the `.so` file. Fix with one of:
```bash
# Option A: Set LD_LIBRARY_PATH (temporary)
export LD_LIBRARY_PATH=/path/to/matrix_class/lib:$LD_LIBRARY_PATH
./my_app

# Option B: Recompile with -rpath (permanent)
g++ ... -Wl,-rpath,/path/to/matrix_class/lib

# Option C: Install system-wide
sudo make -f Makefile.lib install && sudo ldconfig
```

### `fatal error: Matrix.hpp: No such file or directory`

You forgot `-Iinclude`:
```bash
g++ -std=c++11 -Iinclude my_app.cpp ...
#              ^^^^^^^^^^
```

### `undefined reference to Matrix::add(Matrix)`

You linked the wrong library or forgot to link entirely:
```bash
# Make sure you include -lmatrix_cpu (or the .a path)
g++ ... -Llib -lmatrix_cpu
```

### `undefined reference to cudaMalloc`

You're linking against `libmatrix_gpu.so` but forgot `-lcudart`:
```bash
g++ ... -Llib -lmatrix_gpu -lcudart
```

---

*Part of the Matrix Operations Library by Aditya Gowari.*
