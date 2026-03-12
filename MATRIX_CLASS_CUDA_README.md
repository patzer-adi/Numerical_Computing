# CUDA GPU Backend — How It All Works

This document explains every CUDA function in the library, how GPU parallelism works, and why certain operations are perfect for GPUs while others aren't.

---

## Table of Contents

1. [What is CUDA and Why Use It?](#1-what-is-cuda-and-why-use-it)
2. [GPU vs CPU — When Does GPU Win?](#2-gpu-vs-cpu--when-does-gpu-win)
3. [How Our Backend Dispatcher Works](#3-how-our-backend-dispatcher-works)
4. [Memory Model — Host vs Device](#4-memory-model--host-vs-device)
5. [Kernel Launch — Blocks and Threads](#5-kernel-launch--blocks-and-threads)
6. [GPU Matrix Addition](#6-gpu-matrix-addition)
7. [GPU Matrix Subtraction](#7-gpu-matrix-subtraction)
8. [GPU Scalar Multiplication](#8-gpu-scalar-multiplication)
9. [GPU Matrix Transpose](#9-gpu-matrix-transpose)
10. [GPU Matrix Multiplication (Tiled)](#10-gpu-matrix-multiplication-tiled)
11. [GPU LU Decomposition (Doolittle)](#11-gpu-lu-decomposition-doolittle)
12. [Why Gauss-Jacobi and GE Stay on CPU](#12-why-gauss-jacobi-and-ge-stay-on-cpu)
13. [Putting It All Together](#13-putting-it-all-together)

---

## 1. What is CUDA and Why Use It?

**CUDA** (Compute Unified Device Architecture) is NVIDIA's platform for running code on their GPUs. A GPU has **thousands of small cores** designed for doing the **same simple operation on lots of data simultaneously**.

**CPU vs GPU analogy:**
- **CPU** = A few brilliant professors. Each can solve hard problems, but there's only 4-16 of them.
- **GPU** = An army of 1000+ students. Each can only do simple addition, but they all work **at the same time**.

**When GPU wins:** Operations where you do the **same thing to every element** independently. Example: adding two 225×225 matrices = 50,625 additions that don't depend on each other. One GPU thread per element → all done in one step.

**When CPU wins:** Operations with **branching** (if/else decisions), **serial dependencies** (step 2 needs step 1's result), or **small data** (overhead of copying to GPU > computation time).

---

## 2. GPU vs CPU — When Does GPU Win?

Here's a concrete comparison. To add two 225×225 matrices:

### CPU approach:
```
for i = 0 to 224:          ← one row at a time
    for j = 0 to 224:      ← one column at a time
        C[i][j] = A[i][j] + B[i][j]    ← 50,625 operations, done ONE at a time
```
Total time: 50,625 sequential steps.

### GPU approach:
```
Thread 0:    C[0] = A[0] + B[0]           ← all 50,625 threads
Thread 1:    C[1] = A[1] + B[1]              run at THE SAME TIME
Thread 2:    C[2] = A[2] + B[2]
...
Thread 50624: C[50624] = A[50624] + B[50624]
```
Total time: ~1 step (plus memory copy overhead).

**But there's a catch:** You have to copy the data to the GPU (Host → Device), run the kernel, then copy back (Device → Host). For small matrices, this copying takes longer than just doing it on the CPU.

### Decision table in our library:

| Operation | GPU Threshold | Why? |
|---|---|---|
| Addition | n ≥ 64 | Element-wise, perfectly parallel |
| Subtraction | n ≥ 64 | Same as addition |
| Scalar Multiply | n ≥ 64 | Element-wise, perfectly parallel |
| Transpose | n ≥ 64 | Index remapping, parallel |
| Multiplication | n ≥ 64 | Highly parallel (tiled for cache) |
| LU Factorization | n ≥ 128 | Partially parallel (row/col elimination) |
| Gaussian Elimination | CPU only | Pivoting requires serial row swaps |
| Gauss-Jacobi | CPU only | Convergence check every iteration |

---

## 3. How Our Backend Dispatcher Works

The `BackendDispatcher` class in `cuda/include/gpu_dispatch.hpp` makes the CPU/GPU decision at runtime.

```cpp
// file: cuda/include/gpu_dispatch.hpp

class BackendDispatcher {
public:
    static const int GPU_THRESHOLD = 64;
    static bool shouldUseGPU(int matrixSize, string operation);
    static void printBackendInfo();
};
```

**How it decides:**

```
shouldUseGPU(n, "matmul")
    │
    ├── Step 1: gpuAvailable()?
    │   └── calls cudaGetDeviceCount() — is there even an NVIDIA GPU?
    │   └── if NO → return false (use CPU)
    │
    ├── Step 2: n >= GPU_THRESHOLD (64)?
    │   └── if NO → return false (matrix too small, CPU is faster)
    │
    └── Step 3: Is operation suitable?
        ├── "matmul" or "matadd" → YES (element-wise parallel)
        ├── "lu_factorize" && n >= 128 → YES (big enough to benefit)
        └── anything else → NO (use CPU)
```

**At startup**, the program calls `printBackendInfo()` which prints:
```
=== GPU Backend Available ===
GPU 0: NVIDIA GeForce RTX 3060
  Compute capability: 8.6
  Total memory: 12288 MB
  SM count: 28
```
or:
```
=== CPU Backend Active (no CUDA GPU detected) ===
```

---

## 4. Memory Model — Host vs Device

This is the most important concept. The CPU and GPU have **completely separate memory**.

```
┌─────────────────────┐           ┌─────────────────────┐
│      CPU (Host)     │           │     GPU (Device)     │
│                     │           │                      │
│  double *A          │  ──copy──→│  double *d_A         │
│  (your matrix data) │           │  (GPU copy of data)  │
│                     │           │                      │
│  double *C          │  ←─copy── │  double *d_C         │
│  (result)           │           │  (GPU computed result)│
└─────────────────────┘           └──────────────────────┘
```

**Every GPU function follows this pattern:**

1. **Allocate GPU memory** — `cudaMalloc(&d_A, bytes)`
2. **Copy data CPU → GPU** — `cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice)`
3. **Run the kernel** — `myKernel<<<numBlocks, blockSize>>>(d_A, d_C, n)`
4. **Copy result GPU → CPU** — `cudaMemcpy(C, d_C, bytes, cudaMemcpyDeviceToHost)`
5. **Free GPU memory** — `cudaFree(d_A)`

**This is why small matrices are slower on GPU** — the copying overhead (steps 1,2,4,5) dominates for small data.

---

## 5. Kernel Launch — Blocks and Threads

When you launch a CUDA kernel, you specify how many **threads** to run, organized into **blocks**.

```cpp
int blockSize = 256;                              // 256 threads per block
int numBlocks = (total + blockSize - 1) / blockSize;  // enough blocks to cover all elements
myKernel<<<numBlocks, blockSize>>>(d_A, d_B, d_C, total);
```

**Example:** For a 225×225 matrix (total = 50,625 elements):
```
blockSize = 256
numBlocks = (50625 + 255) / 256 = 198 blocks

Total threads launched: 198 × 256 = 50,688
(that's 63 extra threads — they just check "am I out of bounds?" and do nothing)
```

**Why blocks?** GPU hardware groups threads into **warps** (32 threads). Blocks are a higher-level grouping. The GPU scheduler assigns blocks to **Streaming Multiprocessors (SMs)**.

```
┌──── GPU ──────────────────────────────────────┐
│  SM 0: Block 0 (threads 0-255)                │
│  SM 1: Block 1 (threads 256-511)              │
│  SM 2: Block 2 (threads 512-767)              │
│  ...                                          │
│  SM 27: Block 197 (threads 50432-50687)       │
│  (SM 0 picks up Block 198 when it's free)     │
└───────────────────────────────────────────────┘
```

Each thread gets a unique ID:
```cpp
int idx = blockIdx.x * blockDim.x + threadIdx.x;
// Block 3, Thread 42:  idx = 3 * 256 + 42 = 810
```

---

## 6. GPU Matrix Addition

**File:** `cuda/src/gpu_kernels.cu` → `gpuMatAdd()`

### The Kernel

```cpp
__global__ void addKernel(double *A, double *B, double *C, int total) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < total) {
        C[idx] = A[idx] + B[idx];    // that's it — one addition per thread
    }
}
```

**Key insight:** The matrices are stored as **flat 1D arrays** (row-major) on the GPU. A 3×3 matrix `[[1,2,3],[4,5,6],[7,8,9]]` becomes `[1,2,3,4,5,6,7,8,9]`.

### Example walkthrough

```
A = | 1  2 |  → flat: [1, 2, 3, 4]      B = | 5  6 |  → flat: [5, 6, 7, 8]
    | 3  4 |                                  | 7  8 |

total = 4, blockSize = 256, numBlocks = 1

Thread 0: C[0] = A[0] + B[0] = 1 + 5 = 6     (all 4 threads run
Thread 1: C[1] = A[1] + B[1] = 2 + 6 = 8      simultaneously!)
Thread 2: C[2] = A[2] + B[2] = 3 + 7 = 10
Thread 3: C[3] = A[3] + B[3] = 4 + 8 = 12
Threads 4-255: idx >= total, do nothing

C = [6, 8, 10, 12] → | 6   8  |
                       | 10  12 |
```

### Full function flow

```
gpuMatAdd(A, B, C, 2, 2)
    │
    ├── total = 2*2 = 4, bytes = 4*8 = 32 bytes
    │
    ├── cudaMalloc(&d_A, 32)     ← allocate 32 bytes on GPU
    ├── cudaMalloc(&d_B, 32)
    ├── cudaMalloc(&d_C, 32)
    │
    ├── cudaMemcpy(d_A, A, 32, HostToDevice)  ← copy A to GPU
    ├── cudaMemcpy(d_B, B, 32, HostToDevice)  ← copy B to GPU
    │
    ├── addKernel<<<1, 256>>>(d_A, d_B, d_C, 4)  ← launch 256 threads
    │   └── thread 0: d_C[0] = d_A[0] + d_B[0]
    │   └── thread 1: d_C[1] = d_A[1] + d_B[1]
    │   └── thread 2: d_C[2] = d_A[2] + d_B[2]
    │   └── thread 3: d_C[3] = d_A[3] + d_B[3]
    │   └── threads 4-255: skip (idx >= total)
    │
    ├── cudaMemcpy(C, d_C, 32, DeviceToHost)  ← copy result back
    │
    └── cudaFree(d_A, d_B, d_C)  ← cleanup GPU memory
```

---

## 7. GPU Matrix Subtraction

**Identical to addition** but `C[idx] = A[idx] - B[idx]`. Same kernel pattern, same memory pattern. The only difference is the minus sign.

```cpp
__global__ void subKernel(double *A, double *B, double *C, int total) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < total) {
        C[idx] = A[idx] - B[idx];    // minus instead of plus
    }
}
```

---

## 8. GPU Scalar Multiplication

**File:** `cuda/src/gpu_kernels.cu` → `gpuScalarMul()`

### The Kernel

```cpp
__global__ void scalarMulKernel(double *A, double *B, double scalar, int total) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < total) {
        B[idx] = A[idx] * scalar;    // multiply each element by the scalar
    }
}
```

**Why this is perfect for GPU:** Every element is multiplied by the **same constant**. No dependencies between elements. Each thread reads one value, multiplies, writes one value.

### Example

```
A = | 2  4 |  scalar = 3
    | 6  8 |

flat A = [2, 4, 6, 8]

Thread 0: B[0] = 2 * 3 = 6
Thread 1: B[1] = 4 * 3 = 12
Thread 2: B[2] = 6 * 3 = 18
Thread 3: B[3] = 8 * 3 = 24

B = | 6   12 |
    | 18  24 |
```

### Difference from addition

Only **one** input matrix is needed (plus the scalar value), so we only allocate `d_A` and `d_B` (no `d_C`):
```
cudaMalloc(&d_A, bytes)    ← input
cudaMalloc(&d_B, bytes)    ← output
// no d_C needed — scalar is passed directly as a parameter
```

The scalar itself is just a `double` value — it's passed directly to the kernel as an argument. It doesn't need to be copied to GPU memory because kernel arguments are automatically placed in each thread's **constant memory**.

---

## 9. GPU Matrix Transpose

**File:** `cuda/src/gpu_kernels.cu` → `gpuTranspose()`

### The Kernel

```cpp
__global__ void transposeKernel(double *A, double *B, int rows, int cols) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < rows * cols) {
        int i = idx / cols;         // which row
        int j = idx % cols;         // which column
        B[j * rows + i] = A[i * cols + j];    // swap row and col indices
    }
}
```

### How the index math works

A is stored row-major. For a 2×3 matrix:
```
A = | a  b  c |   stored as: [a, b, c, d, e, f]
    | d  e  f |              indices: 0  1  2  3  4  5

Position (i,j) → index = i * cols + j
(0,0)→0  (0,1)→1  (0,2)→2  (1,0)→3  (1,1)→4  (1,2)→5
```

Transpose is 3×2:
```
B = | a  d |   stored as: [a, d, b, e, c, f]
    | b  e |              indices: 0  1  2  3  4  5
    | c  f |

Position (j,i) → index in B = j * rows + i
```

### Example walkthrough

```
A (2×3) = | 1  2  3 |   flat: [1, 2, 3, 4, 5, 6]
          | 4  5  6 |

Thread 0: idx=0 → i=0, j=0 → B[0*2+0] = B[0] = A[0] = 1
Thread 1: idx=1 → i=0, j=1 → B[1*2+0] = B[2] = A[1] = 2
Thread 2: idx=2 → i=0, j=2 → B[2*2+0] = B[4] = A[2] = 3
Thread 3: idx=3 → i=1, j=0 → B[0*2+1] = B[1] = A[3] = 4
Thread 4: idx=4 → i=1, j=1 → B[1*2+1] = B[3] = A[4] = 5
Thread 5: idx=5 → i=1, j=2 → B[2*2+1] = B[5] = A[5] = 6

B flat: [1, 4, 2, 5, 3, 6]

B (3×2) = | 1  4 |   ✅
          | 2  5 |
          | 3  6 |
```

---

## 10. GPU Matrix Multiplication (Tiled)

**File:** `cuda/src/gpu_kernels.cu` → `gpuMatMul()`

This is the most complex kernel. We use **tiling** (also called **blocking**) to exploit GPU **shared memory**, which is ~100× faster than global memory.

### Why tiling?

Naive matrix multiply: each thread computes one element of C by reading an entire row of A and column of B from **global memory** (slow). If two threads need the same row of A, they both read it separately — wasteful!

**Tiled approach:** Threads in a block cooperate to load a **tile** (small square) of A and B into **shared memory** (fast, on-chip). Then all threads in the block can access this shared data without going to slow global memory.

### The Kernel (annotated)

```cpp
#define TILE_SIZE 16    // 16×16 tiles

__global__ void mulKernel(double *A, double *B, double *C,
                          int rows_a, int cols_a, int cols_b) {

    // shared memory — accessible by ALL threads in this block
    __shared__ double tileA[16][16];
    __shared__ double tileB[16][16];

    // which element of C does this thread compute?
    int row = blockIdx.y * 16 + threadIdx.y;
    int col = blockIdx.x * 16 + threadIdx.x;

    double sum = 0.0;

    // loop over tiles
    int numTiles = (cols_a + 15) / 16;
    for (int t = 0; t < numTiles; t++) {

        // --- PHASE 1: Load tile from global to shared memory ---
        // each thread loads ONE element of tileA and ONE element of tileB
        int aCol = t * 16 + threadIdx.x;
        if (row < rows_a && aCol < cols_a)
            tileA[threadIdx.y][threadIdx.x] = A[row * cols_a + aCol];
        else
            tileA[threadIdx.y][threadIdx.x] = 0.0;

        int bRow = t * 16 + threadIdx.y;
        if (bRow < cols_a && col < cols_b)
            tileB[threadIdx.y][threadIdx.x] = B[bRow * cols_b + col];
        else
            tileB[threadIdx.y][threadIdx.x] = 0.0;

        // wait for ALL threads to finish loading
        __syncthreads();

        // --- PHASE 2: Compute partial sum using shared memory ---
        for (int k = 0; k < 16; k++)
            sum += tileA[threadIdx.y][k] * tileB[k][threadIdx.x];

        // wait for ALL threads to finish computing before loading next tile
        __syncthreads();
    }

    // write result
    if (row < rows_a && col < cols_b)
        C[row * cols_b + col] = sum;
}
```

### Example (4×4 × 4×4, tile size 2 for simplicity)

```
A = | 1  2  3  4 |    B = | 1  0  0  0 |
    | 5  6  7  8 |        | 0  1  0  0 |
    | 9 10 11 12 |        | 0  0  1  0 |
    |13 14 15 16 |        | 0  0  0  1 |

B is the identity matrix, so A × B = A.

With tile size 2:
- Grid: 2×2 blocks
- Each block: 2×2 threads
- Total: 4 blocks × 4 threads = 16 threads (one per element)

Block(0,0) computes C[0:2][0:2] (top-left 2×2)
Block(0,1) computes C[0:2][2:4] (top-right 2×2)
Block(1,0) computes C[2:4][0:2] (bottom-left 2×2)
Block(1,1) computes C[2:4][2:4] (bottom-right 2×2)

Each block processes 2 tiles (since cols_a=4, tile=2 → 2 tiles).

Tile 0 of Block(0,0):
  tileA = | 1  2 |    tileB = | 1  0 |
          | 5  6 |            | 0  1 |
  partial sums: C[0][0] += 1*1 + 2*0 = 1
                C[0][1] += 1*0 + 2*1 = 2
                C[1][0] += 5*1 + 6*0 = 5
                C[1][1] += 5*0 + 6*1 = 6

Tile 1 of Block(0,0):
  tileA = | 3  4 |    tileB = | 0  0 |
          | 7  8 |            | 0  0 |
  partial sums: C[0][0] += 3*0 + 4*0 = 0  → total C[0][0] = 1
                C[0][1] += 3*0 + 4*0 = 0  → total C[0][1] = 2
                etc.

Final C = A (because B was identity). ✅
```

### Performance benefit

For a 225×225 multiply:
- **Naive:** Every thread reads 225 values from A and 225 from B = 450 global memory reads per thread
- **Tiled (16×16):** Each tile loads 16×16 values to shared memory. 15 tiles needed. Each thread reads ~30 values from global memory (rest from fast shared memory)

**Shared memory is ~100× faster than global memory**, so tiling gives a massive speedup.

---

## 11. GPU LU Decomposition (Doolittle)

**File:** `cuda/src/gpu_kernels.cu` → `gpuLU_Doolittle()`

### The Idea

LU decomposition has a **serial outer loop** (iterate over columns k=0,1,...,n-1), but **within each step**, the row elimination can be parallelized.

```
for k = 0 to n-1:
    // Step 1: Compute U row k — each column j is INDEPENDENT
    //         → parallelize over j
    for j = k to n-1:    ← GPU: one thread per j
        U[k][j] = A[k][j] - sum(L[k][s] * U[s][j])

    // Step 2: Compute L column k — each row i is INDEPENDENT
    //         → parallelize over i
    for i = k+1 to n-1:  ← GPU: one thread per i
        L[i][k] = (A[i][k] - sum(L[i][s] * U[s][k])) / U[k][k]
```

### Two kernels

```cpp
// Kernel 1: compute U row k (parallel over columns j)
__global__ void luRowKernel(double *A, double *L, double *U, int n, int k) {
    int j = blockIdx.x * blockDim.x + threadIdx.x;
    if (j >= k && j < n) {
        double sum = 0.0;
        for (int s = 0; s < k; s++)
            sum += L[k * n + s] * U[s * n + j];
        U[k * n + j] = A[k * n + j] - sum;
    }
}

// Kernel 2: compute L column k (parallel over rows i)
__global__ void luColKernel(double *A, double *L, double *U, int n, int k) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i > k && i < n) {
        double sum = 0.0;
        for (int s = 0; s < k; s++)
            sum += L[i * n + s] * U[s * n + k];
        L[i * n + k] = (A[i * n + k] - sum) / U[k * n + k];
    }
}
```

### Why two kernels per step?

Step 2 (computing L column k) **depends on** the result of step 1 (U row k needs to be computed first, because we divide by `U[k][k]`). So we must:
1. Launch `luRowKernel` → `cudaDeviceSynchronize()` (wait for it to finish)
2. Then launch `luColKernel` → `cudaDeviceSynchronize()`
3. Then move to the next k

### The host function

```cpp
void gpuLU_Doolittle(double *A, double *L, double *U, int n) {
    // allocate on GPU
    cudaMalloc(&d_A, n*n*8);
    cudaMalloc(&d_L, n*n*8);
    cudaMalloc(&d_U, n*n*8);

    // copy A, L, U to GPU
    cudaMemcpy(d_A, A, ...);
    cudaMemcpy(d_L, L, ...);   // L starts with 1s on diagonal
    cudaMemcpy(d_U, U, ...);   // U starts as zeros

    for (int k = 0; k < n; k++) {
        luRowKernel<<<blocks, 256>>>(d_A, d_L, d_U, n, k);
        cudaDeviceSynchronize();    // MUST wait — luCol needs U[k][k]

        luColKernel<<<blocks, 256>>>(d_A, d_L, d_U, n, k);
        cudaDeviceSynchronize();    // MUST wait — next k needs this L column
    }

    // copy L, U back to CPU
    cudaMemcpy(L, d_L, ...);
    cudaMemcpy(U, d_U, ...);
}
```

### Parallelism analysis

At step k, there are `n-k` columns in U to compute and `n-k-1` rows in L. For n=225, k=0:
- 225 threads compute U row 0 simultaneously
- 224 threads compute L column 0 simultaneously

This is where GPU starts to help significantly — but only for large n (threshold = 128).

---

## 12. Why Gauss-Jacobi and GE Stay on CPU

### Gaussian Elimination (with pivoting)

The pivoting step requires:
1. **Finding the max element** in a column (reduction — possible on GPU but complex)
2. **Swapping two rows** (serial, and every subsequent step depends on this swap)
3. **Deciding which row to swap** (branching — GPUs are bad at this)

It's technically possible to parallelize parts of GE, but the constant row swaps and serial dependencies make the GPU overhead not worth it for our matrix sizes.

### Gauss-Jacobi

Each iteration needs:
1. Compute new x values (parallelizable — each x[i] is independent)
2. **Check convergence** — compare max(|x_new - x_old|) against tolerance
3. **Decision:** continue or stop

Step 2 requires a **global reduction** across all threads, and step 3 is a serial decision. The overhead of launching a kernel per iteration and copying the convergence flag back to CPU makes it slower than just doing it on CPU.

---

## 13. Putting It All Together

### Complete flow when user selects "Add (A + B)" in the program

```
User selects option 1 → main.cpp
    │
    ├── Reads Matrix A and Matrix B
    │
    ├── Calls A + B → operator+(const Matrix& other)
    │   │
    │   └── Calls add(other)
    │       │
    │       ├── [If CUDA build & BackendDispatcher::shouldUseGPU(n, "matadd")]
    │       │   │
    │       │   ├── Flatten A.data[][] into flat array hostA[]
    │       │   ├── Flatten B.data[][] into flat array hostB[]
    │       │   ├── gpuMatAdd(hostA, hostB, hostC, rows, cols)
    │       │   │   ├── cudaMalloc (3 arrays on GPU)
    │       │   │   ├── cudaMemcpy (A,B → GPU)
    │       │   │   ├── addKernel<<<blocks,256>>> (ALL elements in parallel)
    │       │   │   ├── cudaMemcpy (C ← GPU)
    │       │   │   └── cudaFree (cleanup)
    │       │   └── Unflatten hostC[] back into result.data[][]
    │       │
    │       └── [Else: CPU path]
    │           └── Simple nested loop: result[i][j] = A[i][j] + B[i][j]
    │
    └── Displays result
```

### Memory lifecycle

```
Time →  ──────────────────────────────────────────────>

CPU:    A, B allocated    A, B alive    A, B freed (destructor)
         │                  │              │
GPU:  (none)    d_A, d_B    d_A,d_B,d_C   d_A,d_B,d_C freed
                allocated   contain data    │
                             │              │
                        HostToDevice    DeviceToHost
                        copy A,B        copy C back
```

The GPU memory exists **only during the operation**. It's allocated, used, and freed within the single function call. This means there's no persistent GPU state — each operation is self-contained.

---

## File Summary

```
cuda/
├── include/
│   ├── gpu_backend.cuh        ← all GPU function declarations
│   │   gpuAvailable()             detect GPU
│   │   gpuPrintInfo()             print GPU specs
│   │   gpuMatAdd()                addition kernel wrapper
│   │   gpuMatSub()                subtraction kernel wrapper
│   │   gpuScalarMul()             scalar multiply wrapper
│   │   gpuTranspose()             transpose kernel wrapper
│   │   gpuMatMul()                tiled multiply wrapper
│   │   gpuLU_Doolittle()          LU factorization wrapper
│   │
│   └── gpu_dispatch.hpp       ← BackendDispatcher class
│       shouldUseGPU(n, op)        runtime cpu/gpu decision
│       printBackendInfo()         print active backend
│
└── src/
    ├── gpu_kernels.cu         ← all CUDA kernels + wrappers
    │   addKernel()                1 thread = 1 addition
    │   subKernel()                1 thread = 1 subtraction
    │   scalarMulKernel()          1 thread = 1 multiplication
    │   transposeKernel()          1 thread = 1 index swap
    │   mulKernel()                tiled multiply (shared memory)
    │   luRowKernel()              parallelize U row computation
    │   luColKernel()              parallelize L column computation
    │
    └── gpu_dispatch.cu        ← BackendDispatcher implementation
        shouldUseGPU()             checks GPU + size + operation
        printBackendInfo()         calls gpuAvailable() + gpuPrintInfo()
```
