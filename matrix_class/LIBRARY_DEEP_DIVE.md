# Libraries Deep Dive — From Source Code to `.so` and `.a`

Everything you need to know about how C/C++ libraries work, from the ground up. Written in the context of this Matrix library, but the concepts apply universally.

---

## Table of Contents

1. [The Compilation Pipeline](#1-the-compilation-pipeline)
2. [Object Files — The `.o` Building Blocks](#2-object-files--the-o-building-blocks)
3. [Static Libraries — `.a` Archives](#3-static-libraries--a-archives)
4. [Shared Libraries — `.so` / `.dylib`](#4-shared-libraries--so--dylib)
5. [CUDA Libraries — GPU Shared Objects](#5-cuda-libraries--gpu-shared-objects)
6. [How This Project Maps to These Concepts](#6-how-this-project-maps-to-these-concepts)
7. [Tools Reference — Your Swiss Army Knife](#7-tools-reference--your-swiss-army-knife)

---

## 1. The Compilation Pipeline

Before we touch libraries, let's be crystal clear on what happens when you compile a C++ file.

### 1.1 The Four Stages

```
  Source Code (.cpp)
       │
       ▼
  ┌────────────┐
  │ Preprocessor│  ← #include, #define, #ifdef
  └────────────┘
       │
       ▼
  ┌────────────┐
  │  Compiler   │  ← C++ → assembly
  └────────────┘
       │
       ▼
  ┌────────────┐
  │  Assembler  │  ← assembly → machine code (object file)
  └────────────┘
       │
       ▼
  ┌────────────┐
  │   Linker    │  ← resolves symbols, produces executable or library
  └────────────┘
```

Let's trace a real file from this project:

```bash
# Stage 1: Preprocessor — expands #includes, macros
g++ -E src/Matrix.cpp -o Matrix.i
# This produces a massive .i file where every #include has been pasted in

# Stage 2: Compiler — translates C++ to assembly
g++ -S src/Matrix.cpp -o Matrix.s
# Produces human-readable assembly (AT&T syntax on Linux, Intel on some systems)

# Stage 3: Assembler — converts assembly to machine code
g++ -c src/Matrix.cpp -o Matrix.o
# This is an OBJECT FILE — machine code, but NOT yet executable

# Stage 4: Linker — combines object files into final output
g++ Matrix.o MatrixException.o ... -o matrix_program
# NOW it's an executable
```

### 1.2 Why Does This Matter for Libraries?

Libraries interrupt the pipeline at stage 3. Instead of going directly to the linker to produce an executable, we **collect the `.o` files** and package them:

```
                    ┌──── ar rcs ──────→  libmatrix.a      (static archive)
  .cpp → .o files──┤
                    └──── g++ -shared ─→  libmatrix_cpu.so (shared library)
```

That's the fundamental idea. A library is just **a package of pre-compiled object files** that other programs can use.

### 1.3 Translation Units

Each `.cpp` file is a **translation unit** — the compiler processes it independently. When `Matrix.cpp` calls a function defined in `Doolittle.cpp`, the compiler doesn't check `Doolittle.cpp`. It just trusts the declaration in the header (`LUDecomposition.hpp`) and leaves a placeholder: *"I need a function called `Doolittle::solve` — linker, please fill this in."*

These placeholders are called **undefined symbols**, and resolving them is the linker's main job.

---

## 2. Object Files — The `.o` Building Blocks

### 2.1 What's Inside a `.o` File?

An object file is a structured binary (on Linux: **ELF** format; on macOS: **Mach-O** format). It contains:

| Section | Purpose | Example from `Matrix.o` |
|---------|---------|------------------------|
| `.text` | Machine code (your compiled functions) | `Matrix::add()`, `Matrix::multiply()` |
| `.data` | Initialized global/static variables | `static int counter = 0;` |
| `.bss` | Uninitialized global/static vars (just reserves space) | `static double cache[100];` |
| `.rodata` | Read-only data (string literals, constants) | `"Error: matrix dimensions don't match"` |
| **Symbol Table** | List of all function/variable names defined or needed | `Matrix::add` (defined), `cout` (undefined) |
| **Relocation Table** | Instructions for the linker on what to patch | "At offset 0x42 in `.text`, insert the address of `MatrixException::getMessage`" |

### 2.2 The Symbol Table — The Heart of Linking

Every `.o` file has a symbol table. Each entry is either:

- **Defined** (`T` = text/code, `D` = data, `B` = BSS): "I provide this symbol"
- **Undefined** (`U`): "I need this symbol from somewhere else"

```bash
# See the symbol table of Matrix.o
nm src/Matrix.o

# Typical output (simplified, C++ names are mangled):
0000000000000000 T _ZN6Matrix3addES_          # Matrix::add — DEFINED here
0000000000000120 T _ZN6Matrix8multiplyES_     # Matrix::multiply — DEFINED here
                 U _ZN15MatrixException10getMessageEv  # MatrixException::getMessage — UNDEFINED
                 U _ZSt4cout                         # std::cout — UNDEFINED
```

### 2.3 Name Mangling

C++ supports overloading (same name, different parameters), so the compiler **mangles** function names to make them unique:

```
Matrix::add(Matrix)  →  _ZN6Matrix3addE6Matrix
Matrix::add(double)  →  _ZN6Matrix3addEd
```

The `c++filt` tool demangles them:

```bash
echo "_ZN6Matrix3addE6Matrix" | c++filt
# Output: Matrix::add(Matrix)
```

### 2.4 Relocation Entries

When the compiler sees a function call, it doesn't know the final address of that function yet (it could be in a different `.o` file). So it writes a placeholder (`0x00000000`) and adds a **relocation entry**:

```
Relocation entry at offset 0x42 in .text:
  Type:   R_X86_64_PLT32
  Symbol: _ZN15MatrixExceptionC1ESs    (MatrixException::MatrixException(string))
  Addend: -4
```

The linker reads these entries and patches in the real addresses. This is the core mechanism that makes separate compilation work.

### 2.5 Inspecting Object Files

```bash
# Compile Matrix.cpp to an object file
g++ -std=c++11 -c src/Matrix.cpp -o /tmp/Matrix.o

# List all symbols (demangled)
nm -C /tmp/Matrix.o

# Show section headers
objdump -h /tmp/Matrix.o          # Linux
otool -l /tmp/Matrix.o            # macOS

# Show relocations
objdump -r /tmp/Matrix.o          # Linux
otool -r /tmp/Matrix.o            # macOS

# Full disassembly
objdump -d -C /tmp/Matrix.o       # Linux
otool -tvV /tmp/Matrix.o          # macOS
```

---

## 3. Static Libraries — `.a` Archives

### 3.1 What Is a Static Library?

A static library (`.a` = "archive") is literally just **a bundle of `.o` files packed together** using the `ar` (archiver) tool. Think of it as a ZIP file of object files.

```bash
# Create a static library from object files
ar rcs libmatrix.a Matrix.o MatrixException.o Doolittle.o Crout.o ...
#  r = insert/replace files
#  c = create the archive if it doesn't exist
#  s = write an index (symbol → object file mapping)
```

### 3.2 What's Inside?

```bash
# List the .o files inside the archive
ar -t lib/libmatrix.a

# Output:
Matrix.o
MatrixException.o
SystemOfLinearEquationSolver.o
GaussianElimination.o
LUDecomposition.o
Doolittle.o
Crout.o
Cholesky.o
GaussJacobi.o
MatrixOperations.o
```

It's truly just the `.o` files concatenated with a table of contents.

### 3.3 How the Linker Uses a Static Library

When you link against a static library:

```bash
g++ -std=c++11 -o my_app my_app.o lib/libmatrix.a
```

The linker does this:

1. Start with `my_app.o`. Collect all undefined symbols.
2. Open `libmatrix.a`. For each undefined symbol, search the archive's index.
3. If a `.o` file in the archive defines that symbol, **extract the entire .o file** and add it to the link.
4. Extracting a `.o` file may introduce NEW undefined symbols (because that `.o` depends on yet other `.o` files). Repeat step 2-3 until no more undefined symbols can be resolved.
5. Any remaining undefined symbols → linker error.

**Key insight**: The linker only pulls in the `.o` files you **actually use**. If your program only calls `Matrix::add()`, the linker pulls in `Matrix.o` and `MatrixOperations.o` but might skip `GaussJacobi.o` entirely.

### 3.4 Static Linking — Pros and Cons

| Aspect | Pro | Con |
|--------|-----|-----|
| **Deployment** | Single self-contained binary, no runtime dependencies | Larger binary size |
| **Performance** | No dynamic lookup overhead at runtime | — |
| **Versioning** | No "DLL hell" — the library is baked in | Must recompile to update the library |
| **Memory** | — | Each process gets its own copy of the library code |
| **Compatibility** | Works everywhere, no shared lib version issues | — |

### 3.5 The `-l` Flag and Library Search

When you write `-lmatrix`, the linker searches for:
1. `libmatrix.so` (shared library, preferred)
2. `libmatrix.a` (static library, fallback)

In these directories (in this order):
1. Directories specified with `-L` (e.g., `-Llib`)
2. Directories in `LIBRARY_PATH` environment variable
3. Default system directories: `/usr/lib`, `/usr/local/lib`

To force static linking, use `-static` or specify the full `.a` path:
```bash
g++ my_app.cpp lib/libmatrix.a       # explicit path
g++ my_app.cpp -Llib -Wl,-Bstatic -lmatrix  # force static (Linux)
```

---

## 4. Shared Libraries — `.so` / `.dylib`

### 4.1 The Problem Static Libraries Don't Solve

Imagine 10 programs on your system all use `libmatrix.a`. Each one has a **copy** of the library code embedded in its binary. That's:
- Wasted disk space (10 copies of the same code)
- Wasted RAM (10 copies loaded into memory)
- Painful updates (must recompile all 10 programs to fix a bug in the library)

Shared libraries solve all three problems.

### 4.2 How Shared Libraries Work

A shared library (`.so` on Linux, `.dylib` on macOS) is **loaded at runtime** by the dynamic linker/loader (`ld-linux.so` on Linux, `dyld` on macOS). Multiple programs share a single copy in memory.

```
  Program A ──┐                     ┌── Program A's code
              │                     │
  Program B ──┼── all link to ─→  ┌─┴─── libmatrix_cpu.so (ONE copy in RAM)
              │                   │
  Program C ──┘                   └── mmap'd into each process's address space
```

### 4.3 Position Independent Code (PIC)

Here's the crucial problem: when compiling a regular `.o` file, the compiler writes instructions like:

```asm
call 0x00401234    # call MatrixException::getMessage at address 0x401234
```

But with a shared library, **we don't know where the library will be loaded in memory**. The OS loads `libmatrix_cpu.so` at a different address in every process (for security — this is called **ASLR**, Address Space Layout Randomization).

So we need **Position Independent Code (PIC)**: code that works no matter where it's loaded. The `-fPIC` flag tells the compiler to generate this:

```bash
# WITHOUT -fPIC (absolute addressing — breaks in shared libs):
movl  $0x00601020, %eax    # load from absolute address

# WITH -fPIC (PC-relative addressing — works anywhere):
leaq  -0x1234(%rip), %rax  # load from address relative to current instruction
```

On x86-64, most data access is already RIP-relative, so PIC has near-zero overhead. On 32-bit x86, PIC was slower because it needed a register for the GOT base address.

### 4.4 The GOT and PLT — How Dynamic Function Calls Actually Work

When your program calls a function in a shared library, it can't just `call 0xSOMEADDRESS` because the address isn't known until runtime. This is solved by two tables:

#### Global Offset Table (GOT)
A table of **pointers** in the data section. Each entry holds the address of a global variable or function from a shared library.

#### Procedure Linkage Table (PLT)
A table of **code stubs** that indirect through the GOT to call shared library functions.

Here's what happens when `my_app` calls `Matrix::add()` from `libmatrix_cpu.so`:

```
Step 1: First call to Matrix::add()
─────────────────────────────────────

my_app code:
    call Matrix::add@PLT          ← calls the PLT stub (not the real function)

PLT stub for Matrix::add:
    jmp  *GOT[Matrix::add]        ← GOT entry initially points back to PLT
    push index_of_Matrix::add     ← push function ID
    jmp  dynamic_linker_resolve   ← call the dynamic linker

Dynamic linker (ld-linux.so / dyld):
    1. Looks up "Matrix::add" in libmatrix_cpu.so's symbol table
    2. Finds its address: 0x7f3a12345678
    3. Writes 0x7f3a12345678 into GOT[Matrix::add]
    4. Jumps to Matrix::add (the real function)


Step 2: Second call to Matrix::add() (FAST)
────────────────────────────────────────────

my_app code:
    call Matrix::add@PLT

PLT stub for Matrix::add:
    jmp  *GOT[Matrix::add]        ← GOT now has the REAL address
                                     jumps directly, no resolver needed
```

This is called **lazy binding** — symbols are only resolved the first time they're used. After that, calls are just one extra indirection through the GOT (typically ~1 nanosecond overhead).

### 4.5 Creating a Shared Library

```bash
# Step 1: Compile ALL source files with -fPIC
g++ -std=c++11 -fPIC -c src/Matrix.cpp -o build/cpu/Matrix.o
g++ -std=c++11 -fPIC -c src/Doolittle.cpp -o build/cpu/Doolittle.o
# ... repeat for all source files

# Step 2: Link into a shared library
g++ -shared -o lib/libmatrix_cpu.so build/cpu/*.o
```

The `-shared` flag tells the linker to produce a `.so` file instead of an executable. Key differences from an executable:
- No `main()` required
- All symbols are accessible to programs that load the library
- Contains a **dynamic symbol table** (in addition to the regular symbol table)

### 4.6 Sonames — Library Versioning (Linux)

Real-world shared libraries use **sonames** for versioning:

```
libmatrix_cpu.so           ← "linker name" (symlink, used at compile time with -l)
libmatrix_cpu.so.1         ← "soname" (symlink, encodes MAJOR version)
libmatrix_cpu.so.1.0.0     ← "real name" (actual file, full version)
```

When you build with a soname:
```bash
g++ -shared -Wl,-soname,libmatrix_cpu.so.1 -o libmatrix_cpu.so.1.0.0 *.o
```

The soname is **embedded** in the `.so` file. Programs linked against it record the soname (not the filename), so the dynamic linker can find the right version at runtime. This allows:
- Bug fixes: Replace `libmatrix_cpu.so.1.0.0` with `libmatrix_cpu.so.1.0.1` → all programs automatically use new version
- Breaking changes: Create `libmatrix_cpu.so.2.0.0` → old programs still use `libmatrix_cpu.so.1`, new programs use `libmatrix_cpu.so.2`

### 4.7 Finding Shared Libraries at Runtime

When you run a program that needs `libmatrix_cpu.so`, the dynamic linker searches for it in this order:

| Priority | Mechanism | Example |
|----------|-----------|---------|
| 1 | **RPATH** embedded in the binary | `-Wl,-rpath,/home/user/matrix/lib` |
| 2 | **`LD_LIBRARY_PATH`** env var | `export LD_LIBRARY_PATH=/home/user/matrix/lib` |
| 3 | **`/etc/ld.so.cache`** (built by `ldconfig`) | System-wide cache of library locations |
| 4 | Default dirs: `/lib`, `/usr/lib` | After `make install` |

On macOS, the equivalent is:
- `@rpath` / `@loader_path` / `@executable_path` in the binary
- `DYLD_LIBRARY_PATH` environment variable
- Default system directories

**The `-rpath` approach** is the most robust for development:
```bash
# Embed the library path into the binary itself
g++ -std=c++11 -o my_app my_app.cpp -Llib -lmatrix_cpu -Wl,-rpath,/absolute/path/to/lib
```

### 4.8 Shared Libraries — Pros and Cons

| Aspect | Pro | Con |
|--------|-----|-----|
| **Memory** | Shared across all processes | — |
| **Updates** | Replace `.so` file → all programs updated | ABI compatibility required |
| **Disk** | One copy on disk | — |
| **Deployment** | — | Must ship library with your program or install it |
| **Startup** | — | Slightly slower startup (dynamic linking) |
| **Versioning** | Sonames allow parallel major versions | "DLL hell" if managed poorly |

---

## 5. CUDA Libraries — GPU Shared Objects

### 5.1 CUDA Compilation is Different

CUDA uses `nvcc`, which is a **compiler driver** — it orchestrates multiple compilers:

```
  my_kernel.cu
       │
       ▼
  ┌─────────────────────────────────┐
  │           nvcc                   │
  │                                  │
  │  Host code (.cpp) ──→ g++       │  ← compiles the non-CUDA parts
  │  Device code (__global__) ──→   │
  │       cicc (NVIDIA PTX)         │  ← compiles to PTX (GPU assembly)
  │       ptxas (PTX → SASS)        │  ← assembles to SASS (GPU machine code)
  │       fatbinary                  │  ← bundles multiple GPU architectures
  │                                  │
  │  Final: Host .o + embedded GPU  │
  └─────────────────────────────────┘
```

A single `.cu` file produces a **fat binary** containing:
- Normal x86 host code
- PTX code (GPU intermediate representation, forward-compatible)
- SASS code (GPU machine code for specific architectures, e.g., `sm_50`)

### 5.2 Building a CUDA Shared Library

```bash
# Step 1: Compile C++ files with -fPIC and -DUSE_CUDA
g++ -std=c++11 -fPIC -DUSE_CUDA -c src/Matrix.cpp -o build/gpu/Matrix.o

# Step 2: Compile CUDA files with -Xcompiler -fPIC
nvcc -std=c++11 -arch=sm_50 -Xcompiler -fPIC -c cuda/src/gpu_kernels.cu -o build/gpu/gpu_kernels.o
#          │                    │
#          │                    └── Passes -fPIC to the HOST compiler (g++)
#          │                        (nvcc itself handles GPU PIC differently)
#          │
#          └── Target GPU architecture (Maxwell, compute capability 5.0)

# Step 3: Link everything into a shared library using nvcc
nvcc -std=c++11 -arch=sm_50 -shared -o lib/libmatrix_gpu.so build/gpu/*.o
#                            │
#                            └── nvcc adds: -lcudart -lcudadevrt automatically
```

### 5.3 Why Link with `nvcc` Instead of `g++`?

When you have CUDA device code in your library, `nvcc` needs to perform **device code linking**:
- Resolves `__device__` function calls across translation units
- Links in the CUDA runtime library (`libcudart.so`)
- Embeds the GPU fat binary into the final shared object

If you try to link with plain `g++`, it won't know about the GPU code and will produce a broken library.

### 5.4 CUDA Runtime Dependencies

A program using `libmatrix_gpu.so` needs the CUDA runtime at execution time:

```bash
ldd lib/libmatrix_gpu.so
# Output includes:
#   libcudart.so.12.0 => /usr/local/cuda/lib64/libcudart.so.12.0
#   libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6
```

The user needs the CUDA toolkit installed, but **NOT** the CUDA compiler — just the runtime libraries. This is important for deployment:
- Development machine: needs `nvcc` (CUDA toolkit)
- End user machine: only needs `libcudart.so` (CUDA runtime, much smaller)

### 5.5 `-arch` and Fat Binaries

The `-arch=sm_50` flag specifies the target GPU architecture:

| Architecture | GPUs | Flag |
|-------------|------|------|
| Maxwell | GTX 750 Ti, GTX 9xx | `-arch=sm_50` |
| Pascal | GTX 10xx | `-arch=sm_60` |
| Volta | V100 | `-arch=sm_70` |
| Turing | RTX 20xx | `-arch=sm_75` |
| Ampere | RTX 30xx, A100 | `-arch=sm_80` |
| Ada Lovelace | RTX 40xx | `-arch=sm_89` |

To support multiple architectures, generate a fat binary:
```bash
nvcc -gencode arch=compute_50,code=sm_50 \
     -gencode arch=compute_60,code=sm_60 \
     -gencode arch=compute_70,code=sm_70 \
     -gencode arch=compute_80,code=sm_80 \
     -shared -o libmatrix_gpu.so *.o
```

Our project uses `-arch=sm_50` for broadest compatibility (Maxwell+).

---

## 6. How This Project Maps to These Concepts

### 6.1 The Build Flow

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    make -f Makefile.lib                                  │
│                                                                         │
│  ┌─── cpu_shared ──────────────────────────────────────────────────┐    │
│  │                                                                  │    │
│  │  src/Matrix.cpp ───→ g++ -fPIC -c ───→ build/cpu/Matrix.o      │    │
│  │  src/Doolittle.cpp ──→ g++ -fPIC -c ──→ build/cpu/Doolittle.o  │    │
│  │  ... (10 files)                                                  │    │
│  │            │                                                     │    │
│  │            └──→ g++ -shared ──→ lib/libmatrix_cpu.so            │    │
│  └──────────────────────────────────────────────────────────────────┘    │
│                                                                         │
│  ┌─── static ─────────────────────────────────────────────────────┐    │
│  │                                                                  │    │
│  │  (reuses build/cpu/*.o from above)                               │    │
│  │            │                                                     │    │
│  │            └──→ ar rcs ──→ lib/libmatrix.a                      │    │
│  └──────────────────────────────────────────────────────────────────┘    │
│                                                                         │
│  ┌─── gpu_shared ──────────────────────────────────────────────────┐    │
│  │                                                                  │    │
│  │  src/Matrix.cpp ───→ g++ -fPIC -DUSE_CUDA -c → build/gpu/...   │    │
│  │  ... (10 C++ files, same sources, different defines)             │    │
│  │                                                                  │    │
│  │  cuda/src/gpu_kernels.cu ──→ nvcc -Xcompiler -fPIC → build/gpu/ │    │
│  │  cuda/src/gpu_dispatch.cu ──→ nvcc -Xcompiler -fPIC → build/gpu/│    │
│  │            │                                                     │    │
│  │            └──→ nvcc -shared ──→ lib/libmatrix_gpu.so           │    │
│  └──────────────────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────────────────┘
```

### 6.2 Why Separate `build/cpu/` and `build/gpu/` Directories?

The same source files (`src/Matrix.cpp`, etc.) are compiled **twice with different flags**:
- CPU build: `g++ -fPIC -c src/Matrix.cpp -o build/cpu/Matrix.o`
- GPU build: `g++ -fPIC -DUSE_CUDA -c src/Matrix.cpp -o build/gpu/Matrix.o`

The `-DUSE_CUDA` flag enables the `#ifdef USE_CUDA` blocks that call GPU functions. These are fundamentally different object files — they contain different machine code — so they must live in separate directories.

### 6.3 Why `utils/` Is Excluded

The `utils/` directory contains `Input.cpp` and `Display.cpp` — interactive I/O helpers for the menu-driven `main.cpp`. These:
- Read from `cin` (console input)
- Print menus and formatted output
- Are specific to the interactive program, not general-purpose

A library should provide **computation**, not interactive I/O. Users link against the library and write their own I/O code.

### 6.4 What the `-DUSE_CUDA` Flag Does

In `src/Matrix.cpp` and `src/MatrixOperations.cpp`, there are conditional blocks:

```cpp
Matrix Matrix::operator+(const Matrix &other) {
#ifdef USE_CUDA
    if (BackendDispatcher::shouldUseGPU(rows, "matadd")) {
        // ... GPU path: flatten → gpuMatAdd() → unflatten
    }
#endif
    // CPU fallback
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            result.data[i][j] = data[i][j] + other.data[i][j];
}
```

- **Without `-DUSE_CUDA`** (for `libmatrix_cpu.so`): The `#ifdef` blocks are stripped by the preprocessor. The compiled code is pure CPU. No CUDA symbols, no GPU dependencies.
- **With `-DUSE_CUDA`** (for `libmatrix_gpu.so`): The `#ifdef` blocks are included. The compiled code calls `BackendDispatcher` and GPU functions. These undefined symbols are resolved at link time when we link in `gpu_kernels.o` and `gpu_dispatch.o`.

### 6.5 Symbol Visibility in the Libraries

Let's trace what symbols are in each library:

```bash
# Symbols DEFINED in libmatrix_cpu.so:
nm -C lib/libmatrix_cpu.so | grep " T "
# Matrix::Matrix()
# Matrix::add(Matrix)
# Matrix::multiply(Matrix)
# Matrix::determinant()
# Doolittle::solve(double*, int)
# GaussianElimination::solve(double*, int)
# ... (all class methods)

# Symbols DEFINED in libmatrix_gpu.so:
nm -C lib/libmatrix_gpu.so | grep " T "
# (same as above, PLUS:)
# BackendDispatcher::shouldUseGPU(int, std::string)
# gpuMatAdd(double*, double*, double*, int, int)
# gpuMatMul(double*, double*, double*, int, int, int)
# ... (all GPU functions)

# Symbols UNDEFINED in libmatrix_gpu.so (resolved at runtime):
nm -C lib/libmatrix_gpu.so | grep " U "
# cudaMalloc
# cudaMemcpy
# cudaFree
# ... (CUDA runtime functions from libcudart.so)
```

---

## 7. Tools Reference — Your Swiss Army Knife

### 7.1 `nm` — Symbol Table Viewer

```bash
# List all symbols (demangled)
nm -C lib/libmatrix.a

# Only external (public) symbols
nm -g -C lib/libmatrix_cpu.so

# Only undefined symbols (what the library NEEDS)
nm -u -C lib/libmatrix_gpu.so

# Only defined symbols (what the library PROVIDES)
nm -C --defined-only lib/libmatrix_cpu.so
```

Symbol types:
| Letter | Meaning |
|--------|---------|
| `T`/`t` | Code (text section), uppercase = global, lowercase = local |
| `D`/`d` | Initialized data |
| `B`/`b` | Uninitialized data (BSS) |
| `U` | Undefined (needed from elsewhere) |
| `W`/`w` | Weak symbol (can be overridden) |
| `R`/`r` | Read-only data |

### 7.2 `ldd` — Shared Library Dependencies (Linux)

```bash
ldd lib/libmatrix_gpu.so
# Output:
#   linux-vdso.so.1 => (0x00007fff...)
#   libcudart.so.12 => /usr/local/cuda/lib64/libcudart.so.12
#   libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6
#   libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6

ldd lib/libmatrix_cpu.so
# Output (no CUDA dependency):
#   libstdc++.so.6 => /usr/lib/...
#   libc.so.6 => /lib/...
```

On macOS, use `otool -L` instead of `ldd`.

### 7.3 `ar` — Archive Tool

```bash
# List contents
ar -t lib/libmatrix.a

# Extract a specific .o file
ar -x lib/libmatrix.a Matrix.o

# Add a .o file to an existing archive
ar -r lib/libmatrix.a NewFile.o
```

### 7.4 `readelf` — ELF File Inspector (Linux)

```bash
# Show all section headers
readelf -S lib/libmatrix_cpu.so

# Show dynamic symbol table (what's exported)
readelf --dyn-syms lib/libmatrix_cpu.so

# Show needed shared libraries
readelf -d lib/libmatrix_gpu.so | grep NEEDED

# Show RPATH/RUNPATH
readelf -d my_app | grep -i path
```

On macOS, use `otool` equivalents:
```bash
otool -L lib/libmatrix_cpu.dylib       # show dependencies
otool -l lib/libmatrix_cpu.dylib       # show load commands
otool -tvV lib/libmatrix_cpu.dylib     # disassemble
```

### 7.5 `objdump` — The All-In-One Tool

```bash
# Disassemble with source interleaving
objdump -d -S -C lib/libmatrix_cpu.so | head -100

# Show all sections with contents
objdump -s lib/libmatrix_cpu.so

# Show relocation entries
objdump -R lib/libmatrix_cpu.so

# Show dynamic symbols
objdump -T lib/libmatrix_cpu.so
```

### 7.6 Quick Reference Card

| Task | Linux | macOS |
|------|-------|-------|
| List symbols | `nm -C` | `nm -C` |
| Show shared lib dependencies | `ldd` | `otool -L` |
| Disassemble | `objdump -d -C` | `otool -tvV` |
| Show sections | `readelf -S` | `otool -l` |
| Show relocations | `readelf -r` | `otool -r` |
| Show dynamic symbols | `readelf --dyn-syms` | `nm -gU` |
| Inspect archive | `ar -t` | `ar -t` |
| Demangle name | `c++filt` | `c++filt` |
| Update linker cache | `sudo ldconfig` | Automatic (`dyld`) |

---

## Glossary

| Term | Definition |
|------|------------|
| **ABI** | Application Binary Interface — the binary-level contract between caller and callee (calling conventions, name mangling, vtable layout) |
| **ASLR** | Address Space Layout Randomization — OS loads libraries at random addresses for security |
| **BSS** | Block Started by Symbol — section for uninitialized static/global variables |
| **ELF** | Executable and Linkable Format — binary format used on Linux |
| **GOT** | Global Offset Table — runtime-patched table of addresses for shared library symbols |
| **Mach-O** | Binary format used on macOS |
| **Name mangling** | Encoding function signatures into unique symbol names (C++ specific) |
| **PIC** | Position Independent Code — code that works at any load address |
| **PLT** | Procedure Linkage Table — code stubs for lazy binding of shared library calls |
| **PTX** | Parallel Thread Execution — NVIDIA's GPU intermediate representation |
| **Relocation** | A linker instruction to patch an address at link time or load time |
| **RPATH** | Runtime library search path embedded in a binary |
| **SASS** | Shader ASSembly — NVIDIA's GPU machine code |
| **Soname** | Shared Object Name — versioned name embedded in a `.so` for the dynamic linker |
| **Symbol** | A named entity (function, variable) in an object file |
| **Translation unit** | One `.cpp` file after preprocessing — the unit of compilation |

---

*Written for the Matrix Operations Library by Aditya Gowari.*
