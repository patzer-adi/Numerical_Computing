#include "../include/gpu_backend.cuh"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cuda_runtime.h>

// ============================================
// CUDA Error Checking Macros
// ============================================
// only used here in .cu files (compiled by nvcc)

#define CUDA_CHECK(call)                                                       \
  do {                                                                         \
    cudaError_t err = (call);                                                  \
    if (err != cudaSuccess) {                                                  \
      fprintf(stderr, "CUDA error in %s at line %d: %s\n", __FILE__, __LINE__, \
              cudaGetErrorString(err));                                        \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

#define CUDA_CHECK_KERNEL()                                                    \
  do {                                                                         \
    cudaError_t err = cudaGetLastError();                                      \
    if (err != cudaSuccess) {                                                  \
      fprintf(stderr, "CUDA kernel launch error in %s at line %d: %s\n",       \
              __FILE__, __LINE__, cudaGetErrorString(err));                    \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
    err = cudaDeviceSynchronize();                                             \
    if (err != cudaSuccess) {                                                  \
      fprintf(stderr, "CUDA kernel sync error in %s at line %d: %s\n",         \
              __FILE__, __LINE__, cudaGetErrorString(err));                    \
      exit(EXIT_FAILURE);                                                      \
    }                                                                          \
  } while (0)

// ============================================
// GPU Detection
// ============================================

bool gpuAvailable() {
  int deviceCount = 0;
  cudaError_t err = cudaGetDeviceCount(&deviceCount);
  if (err != cudaSuccess || deviceCount == 0)
    return false;
  return true;
}

void gpuPrintInfo() {
  int deviceCount = 0;
  cudaGetDeviceCount(&deviceCount);
  if (deviceCount == 0) {
    printf("No CUDA GPU detected. Running on CPU only.\n");
    return;
  }
  for (int i = 0; i < deviceCount; i++) {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, i);
    printf("GPU %d: %s\n", i, prop.name);
    printf("  Compute capability: %d.%d\n", prop.major, prop.minor);
    printf("  Total memory: %lu MB\n",
           (unsigned long)(prop.totalGlobalMem / (1024 * 1024)));
    printf("  SM count: %d\n", prop.multiProcessorCount);
  }
}

// ============================================
// Matrix Addition Kernel
// ============================================

__global__ void addKernel(double *A, double *B, double *C, int total) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < total) {
    C[idx] = A[idx] + B[idx];
  }
}

void gpuMatAdd(double *A, double *B, double *C, int rows, int cols) {
  int total = rows * cols;
  size_t bytes = total * sizeof(double);

  double *d_A, *d_B, *d_C;
  CUDA_CHECK(cudaMalloc(&d_A, bytes));
  CUDA_CHECK(cudaMalloc(&d_B, bytes));
  CUDA_CHECK(cudaMalloc(&d_C, bytes));

  CUDA_CHECK(cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(d_B, B, bytes, cudaMemcpyHostToDevice));

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  addKernel<<<numBlocks, blockSize>>>(d_A, d_B, d_C, total);
  CUDA_CHECK_KERNEL();

  CUDA_CHECK(cudaMemcpy(C, d_C, bytes, cudaMemcpyDeviceToHost));

  CUDA_CHECK(cudaFree(d_A));
  CUDA_CHECK(cudaFree(d_B));
  CUDA_CHECK(cudaFree(d_C));
}

// ============================================
// Matrix Subtraction Kernel
// ============================================

__global__ void subKernel(double *A, double *B, double *C, int total) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < total) {
    C[idx] = A[idx] - B[idx];
  }
}

void gpuMatSub(double *A, double *B, double *C, int rows, int cols) {
  int total = rows * cols;
  size_t bytes = total * sizeof(double);

  double *d_A, *d_B, *d_C;
  CUDA_CHECK(cudaMalloc(&d_A, bytes));
  CUDA_CHECK(cudaMalloc(&d_B, bytes));
  CUDA_CHECK(cudaMalloc(&d_C, bytes));

  CUDA_CHECK(cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(d_B, B, bytes, cudaMemcpyHostToDevice));

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  subKernel<<<numBlocks, blockSize>>>(d_A, d_B, d_C, total);
  CUDA_CHECK_KERNEL();

  CUDA_CHECK(cudaMemcpy(C, d_C, bytes, cudaMemcpyDeviceToHost));

  CUDA_CHECK(cudaFree(d_A));
  CUDA_CHECK(cudaFree(d_B));
  CUDA_CHECK(cudaFree(d_C));
}

// ============================================
// Scalar Multiplication Kernel
// ============================================

__global__ void scalarMulKernel(double *A, double *B, double scalar,
                                int total) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < total) {
    B[idx] = A[idx] * scalar;
  }
}

void gpuScalarMul(double *A, double *B, double scalar, int rows, int cols) {
  int total = rows * cols;
  size_t bytes = total * sizeof(double);

  double *d_A, *d_B;
  CUDA_CHECK(cudaMalloc(&d_A, bytes));
  CUDA_CHECK(cudaMalloc(&d_B, bytes));

  CUDA_CHECK(cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice));

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  scalarMulKernel<<<numBlocks, blockSize>>>(d_A, d_B, scalar, total);
  CUDA_CHECK_KERNEL();

  CUDA_CHECK(cudaMemcpy(B, d_B, bytes, cudaMemcpyDeviceToHost));

  CUDA_CHECK(cudaFree(d_A));
  CUDA_CHECK(cudaFree(d_B));
}

// ============================================
// Matrix Transpose Kernel
// ============================================

__global__ void transposeKernel(double *A, double *B, int rows, int cols) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < rows * cols) {
    int i = idx / cols;
    int j = idx % cols;
    B[j * rows + i] = A[i * cols + j];
  }
}

void gpuTranspose(double *A, double *B, int rows, int cols) {
  int total = rows * cols;
  size_t bytes = total * sizeof(double);

  double *d_A, *d_B;
  CUDA_CHECK(cudaMalloc(&d_A, bytes));
  CUDA_CHECK(cudaMalloc(&d_B, bytes));

  CUDA_CHECK(cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice));

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  transposeKernel<<<numBlocks, blockSize>>>(d_A, d_B, rows, cols);
  CUDA_CHECK_KERNEL();

  CUDA_CHECK(cudaMemcpy(B, d_B, bytes, cudaMemcpyDeviceToHost));

  CUDA_CHECK(cudaFree(d_A));
  CUDA_CHECK(cudaFree(d_B));
}

// ============================================
// Matrix Multiplication Kernel (tiled, shared memory)
// ============================================

#define TILE_SIZE 16

__global__ void mulKernel(double *A, double *B, double *C, int rows_a,
                          int cols_a, int cols_b) {
  __shared__ double tileA[TILE_SIZE][TILE_SIZE];
  __shared__ double tileB[TILE_SIZE][TILE_SIZE];

  int row = blockIdx.y * TILE_SIZE + threadIdx.y;
  int col = blockIdx.x * TILE_SIZE + threadIdx.x;

  double sum = 0.0;

  int numTiles = (cols_a + TILE_SIZE - 1) / TILE_SIZE;
  for (int t = 0; t < numTiles; t++) {
    // load tile from A
    int aCol = t * TILE_SIZE + threadIdx.x;
    if (row < rows_a && aCol < cols_a)
      tileA[threadIdx.y][threadIdx.x] = A[row * cols_a + aCol];
    else
      tileA[threadIdx.y][threadIdx.x] = 0.0;

    // load tile from B
    int bRow = t * TILE_SIZE + threadIdx.y;
    if (bRow < cols_a && col < cols_b)
      tileB[threadIdx.y][threadIdx.x] = B[bRow * cols_b + col];
    else
      tileB[threadIdx.y][threadIdx.x] = 0.0;

    __syncthreads();

    for (int k = 0; k < TILE_SIZE; k++)
      sum += tileA[threadIdx.y][k] * tileB[k][threadIdx.x];

    __syncthreads();
  }

  if (row < rows_a && col < cols_b)
    C[row * cols_b + col] = sum;
}

void gpuMatMul(double *A, double *B, double *C, int rows_a, int cols_a,
               int cols_b) {
  size_t bytesA = rows_a * cols_a * sizeof(double);
  size_t bytesB = cols_a * cols_b * sizeof(double);
  size_t bytesC = rows_a * cols_b * sizeof(double);

  double *d_A, *d_B, *d_C;
  CUDA_CHECK(cudaMalloc(&d_A, bytesA));
  CUDA_CHECK(cudaMalloc(&d_B, bytesB));
  CUDA_CHECK(cudaMalloc(&d_C, bytesC));

  CUDA_CHECK(cudaMemcpy(d_A, A, bytesA, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(d_B, B, bytesB, cudaMemcpyHostToDevice));

  dim3 blockDim(TILE_SIZE, TILE_SIZE);
  dim3 gridDim((cols_b + TILE_SIZE - 1) / TILE_SIZE,
               (rows_a + TILE_SIZE - 1) / TILE_SIZE);
  mulKernel<<<gridDim, blockDim>>>(d_A, d_B, d_C, rows_a, cols_a, cols_b);
  CUDA_CHECK_KERNEL();

  CUDA_CHECK(cudaMemcpy(C, d_C, bytesC, cudaMemcpyDeviceToHost));

  CUDA_CHECK(cudaFree(d_A));
  CUDA_CHECK(cudaFree(d_B));
  CUDA_CHECK(cudaFree(d_C));
}

// ============================================
// GPU LU Decomposition (Doolittle)
// ============================================
// parallelizes the row elimination step
// A, L, U are flat row-major arrays of size n*n

__global__ void luRowKernel(double *A, double *L, double *U, int n, int k) {
  int j = blockIdx.x * blockDim.x + threadIdx.x;

  // compute U[k][j] for j >= k
  if (j >= k && j < n) {
    double sum = 0.0;
    for (int s = 0; s < k; s++)
      sum += L[k * n + s] * U[s * n + j];
    U[k * n + j] = A[k * n + j] - sum;
  }
}

__global__ void luColKernel(double *A, double *L, double *U, int n, int k) {
  int i = blockIdx.x * blockDim.x + threadIdx.x;

  // compute L[i][k] for i > k
  if (i > k && i < n) {
    double sum = 0.0;
    for (int s = 0; s < k; s++)
      sum += L[i * n + s] * U[s * n + k];
    double ukk = U[k * n + k];
    if (fabs(ukk) > 1e-12)
      L[i * n + k] = (A[i * n + k] - sum) / ukk;
    else
      L[i * n + k] = 0.0;
  }
}

void gpuLU_Doolittle(double *A, double *L, double *U, int n) {
  size_t bytes = n * n * sizeof(double);

  double *d_A, *d_L, *d_U;
  CUDA_CHECK(cudaMalloc(&d_A, bytes));
  CUDA_CHECK(cudaMalloc(&d_L, bytes));
  CUDA_CHECK(cudaMalloc(&d_U, bytes));

  CUDA_CHECK(cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(d_L, L, bytes, cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(d_U, U, bytes, cudaMemcpyHostToDevice));

  int blockSize = 256;

  for (int k = 0; k < n; k++) {
    int numBlocks = (n + blockSize - 1) / blockSize;

    // compute U row k (parallel over columns)
    luRowKernel<<<numBlocks, blockSize>>>(d_A, d_L, d_U, n, k);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    // compute L column k (parallel over rows)
    luColKernel<<<numBlocks, blockSize>>>(d_A, d_L, d_U, n, k);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());
  }

  CUDA_CHECK(cudaMemcpy(L, d_L, bytes, cudaMemcpyDeviceToHost));
  CUDA_CHECK(cudaMemcpy(U, d_U, bytes, cudaMemcpyDeviceToHost));

  CUDA_CHECK(cudaFree(d_A));
  CUDA_CHECK(cudaFree(d_L));
  CUDA_CHECK(cudaFree(d_U));
}

// ============================================
// GPU Matrix Check: Is Identity
// ============================================
// each thread checks one element — writes 1 if pass, 0 if fail

__global__ void isIdentityKernel(double *A, int *flags, int n) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < n * n) {
    int i = idx / n;
    int j = idx % n;
    if (i == j) {
      flags[idx] = (fabs(A[idx] - 1.0) < 1e-10) ? 1 : 0;
    } else {
      flags[idx] = (fabs(A[idx]) < 1e-10) ? 1 : 0;
    }
  }
}

int gpuIsIdentity(double *A, int n) {
  int total = n * n;
  size_t bytesA = total * sizeof(double);
  size_t bytesF = total * sizeof(int);

  double *d_A;
  int *d_flags;
  CUDA_CHECK(cudaMalloc(&d_A, bytesA));
  CUDA_CHECK(cudaMalloc(&d_flags, bytesF));

  CUDA_CHECK(cudaMemcpy(d_A, A, bytesA, cudaMemcpyHostToDevice));

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  isIdentityKernel<<<numBlocks, blockSize>>>(d_A, d_flags, n);
  CUDA_CHECK_KERNEL();

  int *hostFlags = new int[total];
  CUDA_CHECK(cudaMemcpy(hostFlags, d_flags, bytesF, cudaMemcpyDeviceToHost));

  int result = 1;
  for (int i = 0; i < total; i++) {
    if (hostFlags[i] == 0) {
      result = 0;
      break;
    }
  }

  delete[] hostFlags;
  CUDA_CHECK(cudaFree(d_A));
  CUDA_CHECK(cudaFree(d_flags));
  return result;
}

// ============================================
// GPU Matrix Check: Is Null
// ============================================

__global__ void isNullKernel(double *A, int *flags, int total) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < total) {
    flags[idx] = (fabs(A[idx]) < 1e-10) ? 1 : 0;
  }
}

int gpuIsNull(double *A, int rows, int cols) {
  int total = rows * cols;
  size_t bytesA = total * sizeof(double);
  size_t bytesF = total * sizeof(int);

  double *d_A;
  int *d_flags;
  CUDA_CHECK(cudaMalloc(&d_A, bytesA));
  CUDA_CHECK(cudaMalloc(&d_flags, bytesF));

  CUDA_CHECK(cudaMemcpy(d_A, A, bytesA, cudaMemcpyHostToDevice));

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  isNullKernel<<<numBlocks, blockSize>>>(d_A, d_flags, total);
  CUDA_CHECK_KERNEL();

  int *hostFlags = new int[total];
  CUDA_CHECK(cudaMemcpy(hostFlags, d_flags, bytesF, cudaMemcpyDeviceToHost));

  int result = 1;
  for (int i = 0; i < total; i++) {
    if (hostFlags[i] == 0) {
      result = 0;
      break;
    }
  }

  delete[] hostFlags;
  CUDA_CHECK(cudaFree(d_A));
  CUDA_CHECK(cudaFree(d_flags));
  return result;
}

// ============================================
// GPU Matrix Check: Is Diagonal
// ============================================

__global__ void isDiagonalKernel(double *A, int *flags, int n) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx < n * n) {
    int i = idx / n;
    int j = idx % n;
    if (i != j) {
      flags[idx] = (fabs(A[idx]) < 1e-10) ? 1 : 0;
    } else {
      flags[idx] = 1; // diagonal elements can be anything
    }
  }
}

int gpuIsDiagonal(double *A, int n) {
  int total = n * n;
  size_t bytesA = total * sizeof(double);
  size_t bytesF = total * sizeof(int);

  double *d_A;
  int *d_flags;
  CUDA_CHECK(cudaMalloc(&d_A, bytesA));
  CUDA_CHECK(cudaMalloc(&d_flags, bytesF));

  CUDA_CHECK(cudaMemcpy(d_A, A, bytesA, cudaMemcpyHostToDevice));

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  isDiagonalKernel<<<numBlocks, blockSize>>>(d_A, d_flags, n);
  CUDA_CHECK_KERNEL();

  int *hostFlags = new int[total];
  CUDA_CHECK(cudaMemcpy(hostFlags, d_flags, bytesF, cudaMemcpyDeviceToHost));

  int result = 1;
  for (int i = 0; i < total; i++) {
    if (hostFlags[i] == 0) {
      result = 0;
      break;
    }
  }

  delete[] hostFlags;
  CUDA_CHECK(cudaFree(d_A));
  CUDA_CHECK(cudaFree(d_flags));
  return result;
}

// ============================================
// GPU Matrix Check: Is Diagonally Dominant
// ============================================
// each thread handles one row

__global__ void isDiagDomKernel(double *A, int *flags, int n) {
  int row = blockIdx.x * blockDim.x + threadIdx.x;
  if (row < n) {
    double diagVal = fabs(A[row * n + row]);
    double offDiagSum = 0.0;
    for (int j = 0; j < n; j++) {
      if (j != row)
        offDiagSum += fabs(A[row * n + j]);
    }
    flags[row] = (diagVal >= offDiagSum) ? 1 : 0;
  }
}

int gpuIsDiagonallyDominant(double *A, int n) {
  size_t bytesA = n * n * sizeof(double);
  size_t bytesF = n * sizeof(int);

  double *d_A;
  int *d_flags;
  CUDA_CHECK(cudaMalloc(&d_A, bytesA));
  CUDA_CHECK(cudaMalloc(&d_flags, bytesF));

  CUDA_CHECK(cudaMemcpy(d_A, A, bytesA, cudaMemcpyHostToDevice));

  int blockSize = 256;
  int numBlocks = (n + blockSize - 1) / blockSize;
  isDiagDomKernel<<<numBlocks, blockSize>>>(d_A, d_flags, n);
  CUDA_CHECK_KERNEL();

  int *hostFlags = new int[n];
  CUDA_CHECK(cudaMemcpy(hostFlags, d_flags, bytesF, cudaMemcpyDeviceToHost));

  int result = 1;
  for (int i = 0; i < n; i++) {
    if (hostFlags[i] == 0) {
      result = 0;
      break;
    }
  }

  delete[] hostFlags;
  CUDA_CHECK(cudaFree(d_A));
  CUDA_CHECK(cudaFree(d_flags));
  return result;
}
