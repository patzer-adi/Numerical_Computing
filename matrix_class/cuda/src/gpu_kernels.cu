#include "../include/gpu_backend.cuh"
#include <cmath>
#include <cstdio>
#include <cuda_runtime.h>

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
  int bytes = total * sizeof(double);

  double *d_A, *d_B, *d_C;
  cudaMalloc(&d_A, bytes);
  cudaMalloc(&d_B, bytes);
  cudaMalloc(&d_C, bytes);

  cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice);
  cudaMemcpy(d_B, B, bytes, cudaMemcpyHostToDevice);

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  addKernel<<<numBlocks, blockSize>>>(d_A, d_B, d_C, total);

  cudaMemcpy(C, d_C, bytes, cudaMemcpyDeviceToHost);

  cudaFree(d_A);
  cudaFree(d_B);
  cudaFree(d_C);
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
  int bytes = total * sizeof(double);

  double *d_A, *d_B, *d_C;
  cudaMalloc(&d_A, bytes);
  cudaMalloc(&d_B, bytes);
  cudaMalloc(&d_C, bytes);

  cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice);
  cudaMemcpy(d_B, B, bytes, cudaMemcpyHostToDevice);

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  subKernel<<<numBlocks, blockSize>>>(d_A, d_B, d_C, total);

  cudaMemcpy(C, d_C, bytes, cudaMemcpyDeviceToHost);

  cudaFree(d_A);
  cudaFree(d_B);
  cudaFree(d_C);
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
  int bytes = total * sizeof(double);

  double *d_A, *d_B;
  cudaMalloc(&d_A, bytes);
  cudaMalloc(&d_B, bytes);

  cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice);

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  scalarMulKernel<<<numBlocks, blockSize>>>(d_A, d_B, scalar, total);

  cudaMemcpy(B, d_B, bytes, cudaMemcpyDeviceToHost);

  cudaFree(d_A);
  cudaFree(d_B);
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
  int bytes = total * sizeof(double);

  double *d_A, *d_B;
  cudaMalloc(&d_A, bytes);
  cudaMalloc(&d_B, bytes);

  cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice);

  int blockSize = 256;
  int numBlocks = (total + blockSize - 1) / blockSize;
  transposeKernel<<<numBlocks, blockSize>>>(d_A, d_B, rows, cols);

  cudaMemcpy(B, d_B, bytes, cudaMemcpyDeviceToHost);

  cudaFree(d_A);
  cudaFree(d_B);
}

// ============================================
// Matrix Multiplication Kernel (tiled)
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
  int bytesA = rows_a * cols_a * sizeof(double);
  int bytesB = cols_a * cols_b * sizeof(double);
  int bytesC = rows_a * cols_b * sizeof(double);

  double *d_A, *d_B, *d_C;
  cudaMalloc(&d_A, bytesA);
  cudaMalloc(&d_B, bytesB);
  cudaMalloc(&d_C, bytesC);

  cudaMemcpy(d_A, A, bytesA, cudaMemcpyHostToDevice);
  cudaMemcpy(d_B, B, bytesB, cudaMemcpyHostToDevice);

  dim3 blockDim(TILE_SIZE, TILE_SIZE);
  dim3 gridDim((cols_b + TILE_SIZE - 1) / TILE_SIZE,
               (rows_a + TILE_SIZE - 1) / TILE_SIZE);
  mulKernel<<<gridDim, blockDim>>>(d_A, d_B, d_C, rows_a, cols_a, cols_b);

  cudaMemcpy(C, d_C, bytesC, cudaMemcpyDeviceToHost);

  cudaFree(d_A);
  cudaFree(d_B);
  cudaFree(d_C);
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
  int bytes = n * n * sizeof(double);

  double *d_A, *d_L, *d_U;
  cudaMalloc(&d_A, bytes);
  cudaMalloc(&d_L, bytes);
  cudaMalloc(&d_U, bytes);

  cudaMemcpy(d_A, A, bytes, cudaMemcpyHostToDevice);
  cudaMemcpy(d_L, L, bytes, cudaMemcpyHostToDevice);
  cudaMemcpy(d_U, U, bytes, cudaMemcpyHostToDevice);

  int blockSize = 256;

  for (int k = 0; k < n; k++) {
    int numBlocks = (n + blockSize - 1) / blockSize;

    // compute U row k (parallel over columns)
    luRowKernel<<<numBlocks, blockSize>>>(d_A, d_L, d_U, n, k);
    cudaDeviceSynchronize();

    // compute L column k (parallel over rows)
    luColKernel<<<numBlocks, blockSize>>>(d_A, d_L, d_U, n, k);
    cudaDeviceSynchronize();
  }

  cudaMemcpy(L, d_L, bytes, cudaMemcpyDeviceToHost);
  cudaMemcpy(U, d_U, bytes, cudaMemcpyDeviceToHost);

  cudaFree(d_A);
  cudaFree(d_L);
  cudaFree(d_U);
}
