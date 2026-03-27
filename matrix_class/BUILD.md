# Building the Matrix Operations Library

This document explains how to build the library in **CPU-only** mode and in **GPU (CUDA)** mode, what each mode does, and what you need installed.

---

## Quick Start

```bash
# CPU build (default — works on any machine)
make cpu

# GPU build (requires NVIDIA GPU + CUDA toolkit)
make gpu

# Clean all build artifacts
make clean

# Run the program
./matrix_program       # CPU version
./matrix_program_gpu   # GPU version
```

---

## CPU Build (`make cpu`)

### What it does

Compiles the entire library using **g++** only. All matrix operations (add, subtract, multiply, determinant, Gaussian elimination, LU decomposition, Gauss-Jacobi, Gauss-Seidel) run on the CPU using standard nested loops.

### Requirements

| Tool | Minimum Version |
|------|----------------|
| g++ | 4.8+ (C++11 support) |
| make | any |

That's it — no special hardware or libraries needed.

### How it works internally

```
make cpu
  │
  ├── Compiles each .cpp file into a .o object file:
  │     g++ -std=c++11 -Wall -c main.cpp -o main.o
  │     g++ -std=c++11 -Wall -c src/Matrix.cpp -o src/Matrix.o
  │     g++ -std=c++11 -Wall -c src/GaussianElimination.cpp -o src/GaussianElimination.o
  │     ... (13 source files total)
  │
  └── Links all .o files into one executable:
        g++ -std=c++11 -Wall -o matrix_program main.o src/Matrix.o ...
```

### Source files compiled

```
main.cpp                              ← entry point (6 lines, calls runMenu)
src/Matrix.cpp                        ← core matrix class
src/MatrixException.cpp               ← error handling
src/SystemOfLinearEquationSolver.cpp   ← base solver class
src/GaussianElimination.cpp           ← GE with/without pivoting
src/LUDecomposition.cpp               ← LU base class
src/Doolittle.cpp                     ← Doolittle decomposition
src/Crout.cpp                         ← Crout decomposition
src/Cholesky.cpp                      ← Cholesky decomposition
src/GaussJacobi.cpp                   ← Gauss-Jacobi iterative solver
src/GaussSeidel.cpp                   ← Gauss-Seidel iterative solver
src/MatrixOperations.cpp              ← operator overloads, property checks
utils/Input.cpp                       ← console & file input
utils/Display.cpp                     ← output, solver status, file save
app/Menu.cpp                          ← application menu + handler functions
```

### Output

Creates a single executable: **`matrix_program`** (~180 KB)

### When to use

- You don't have an NVIDIA GPU
- You're on macOS (no CUDA support on Apple Silicon)
- Your matrices are small (< 64×64) — CPU is actually faster for these
- You just want it to work without extra setup

---

## GPU Build (`make gpu`)

### What it does

Compiles the same C++ source files **plus** two CUDA files. The resulting binary automatically decides at runtime whether to use the GPU or CPU for each operation, based on matrix size and operation type.

### Requirements

| Tool | Minimum Version | Notes |
|------|----------------|-------|
| g++ | 4.8+ | C++11 support |
| nvcc | 7.0+ | NVIDIA CUDA Compiler (comes with CUDA toolkit) |
| CUDA Toolkit | 7.0+ | Install from [developer.nvidia.com/cuda-downloads](https://developer.nvidia.com/cuda-downloads) |
| NVIDIA GPU | Compute Capability 5.0+ | Maxwell or newer (GTX 750 Ti and above) |
| make | any | |

### How it works internally

```
make gpu
  │
  ├── Step 1: Compile all .cpp files WITH the -DUSE_CUDA flag
  │     g++ -std=c++11 -Wall -DUSE_CUDA -c main.cpp -o main.o
  │     g++ -std=c++11 -Wall -DUSE_CUDA -c src/Matrix.cpp -o src/Matrix.o
  │     ...
  │
  ├── Step 2: Compile CUDA .cu files with nvcc
  │     nvcc -std=c++11 -arch=sm_50 -Icuda/include -c cuda/src/gpu_kernels.cu -o cuda/src/gpu_kernels.o
  │     nvcc -std=c++11 -arch=sm_50 -Icuda/include -c cuda/src/gpu_dispatch.cu -o cuda/src/gpu_dispatch.o
  │
  └── Step 3: Link everything together with nvcc
        nvcc -std=c++11 -arch=sm_50 -Icuda/include -o matrix_program_gpu *.o cuda/src/*.o
```

### The `-DUSE_CUDA` flag

This is the key difference. When `-DUSE_CUDA` is defined at compile time, the C++ code includes the GPU headers and calls the `BackendDispatcher` to decide CPU vs GPU at **runtime**:

```cpp
// Inside src/Matrix.cpp (simplified)
Matrix Matrix::add(const Matrix& other) {
#ifdef USE_CUDA
    if (BackendDispatcher::shouldUseGPU(rows, "matadd")) {
        // flatten matrices → call gpuMatAdd() → unflatten result
        return result;
    }
#endif
    // CPU fallback: simple nested loop
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.data[i][j] = data[i][j] + other.data[i][j];
    return result;
}
```

Without `-DUSE_CUDA`, all the GPU code is completely invisible — the `#ifdef USE_CUDA` blocks are skipped during compilation.

### CUDA source files

```
cuda/
├── include/
│   ├── gpu_backend.cuh       ← GPU function declarations (gpuMatAdd, gpuMatMul, etc.)
│   └── gpu_dispatch.hpp      ← BackendDispatcher class (CPU/GPU decision logic)
└── src/
    ├── gpu_kernels.cu        ← all CUDA kernels (addition, multiplication, LU, etc.)
    └── gpu_dispatch.cu       ← runtime GPU detection & threshold logic
```

### Output

Creates a single executable: **`matrix_program_gpu`**

### Runtime behavior

When you run `./matrix_program_gpu`, it first prints the detected backend:

```
=== GPU Backend Available ===
GPU 0: NVIDIA GeForce RTX 3060
  Compute capability: 8.6
  Total memory: 12288 MB
  SM count: 28
```

Then, for every operation, it automatically picks CPU or GPU:

| Operation | GPU used when | Stays on CPU when |
|-----------|---------------|-------------------|
| Addition, Subtraction, Scalar Multiply, Transpose | Matrix ≥ 64×64 | Matrix < 64×64 |
| Matrix Multiplication (tiled, shared memory) | Matrix ≥ 64×64 | Matrix < 64×64 |
| LU Decomposition (Doolittle) | Matrix ≥ 128×128 | Matrix < 128×128 |
| Gaussian Elimination | Always CPU | Pivoting is inherently serial |
| Gauss-Jacobi | Always CPU | Convergence check is serial |

You don't need to choose — the dispatcher handles it transparently.

---

## Important: Switching Between CPU and GPU Builds

If you previously built with `make cpu` and now want to build with `make gpu` (or vice versa), **always clean first**:

```bash
make clean
make gpu     # or: make cpu
```

**Why?** The `.o` files from a CPU build were compiled **without** `-DUSE_CUDA`. If you then run `make gpu`, `make` sees the `.o` files already exist and skips recompilation — but they don't have the GPU code paths. This is also why you might see `Nothing to be done for 'cpu'` — `make` thinks everything is already built.

```bash
# ❌ DON'T do this (will use stale .o files)
make cpu
make gpu     # ← skips recompilation, GPU code missing!

# ✅ DO this
make cpu
make clean   # ← removes all .o files
make gpu     # ← recompiles everything with -DUSE_CUDA
```

---

## Troubleshooting

### `Nothing to be done for 'cpu'`
Everything is already compiled and up-to-date. Either:
- Run `./matrix_program` — it's ready
- Run `make clean && make cpu` to force a rebuild

### `nvcc: command not found`
CUDA toolkit is not installed or not in your PATH. Install it from [developer.nvidia.com](https://developer.nvidia.com/cuda-downloads) and add to your PATH:
```bash
export PATH=/usr/local/cuda/bin:$PATH
```

### `no CUDA-capable device detected`
You have the toolkit installed but no NVIDIA GPU. The GPU build will still work — it just falls back to CPU for everything.

### Linker errors about undefined CUDA symbols
Make sure you're linking with `nvcc` (the Makefile does this automatically for the `gpu` target). Don't try to link CUDA object files with plain `g++`.

---

## Examples

The `examples/` directory has standalone programs that show how to use the library:

```bash
# Build examples (run from matrix_class/ directory)
make -f examples/Makefile

# Run them
./example_basic    # basic matrix operations
./example_solve    # solving linear systems
```

These are CPU-only and don't require CUDA.

---

## File Structure Reference

```
matrix_class/
├── Makefile                  ← main build file (cpu/gpu/clean targets)
├── main.cpp                  ← entry point (6 lines)
├── app/                      ← application layer (menu + routing)
│   ├── Menu.hpp
│   └── Menu.cpp
├── include/                  ← core library headers (NO cin/cout)
│   ├── SolverResult.hpp          ← return type for all solvers
│   └── ... (other .hpp files)
├── src/                      ← C++ implementations (NO cin/cout)
├── utils/                    ← I/O helper functions
├── cuda/                     ← GPU backend (only used with make gpu)
│   ├── include/              ← GPU headers (.cuh, .hpp)
│   └── src/                  ← CUDA kernels (.cu)
├── examples/                 ← standalone example programs
├── 49/                       ← 49×49 test data
├── 225/                      ← 225×225 test data
└── test_cases/               ← test generators
```
