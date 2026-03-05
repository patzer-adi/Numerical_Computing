#ifndef GPU_BACKEND_CUH
#define GPU_BACKEND_CUH

// GPU Backend for Matrix Operations
// automatically detects CUDA GPU at runtime
// falls back to CPU if no GPU found
// designed for CUDA 7.0+ / compute capability 5.0+
//
// NOTE: this header is included by BOTH g++ (.cpp files)
//       and nvcc (.cu files). Do NOT put any CUDA-specific
//       headers or types here — only plain C++ declarations.

// GPU Detection

// check if a CUDA GPU is available
bool gpuAvailable();

// print GPU info (name, memory, compute capability)
void gpuPrintInfo();

// GPU Matrix Operations
// all matrices are flat row-major double arrays

// matrix addition: C = A + B
void gpuMatAdd(double *A, double *B, double *C, int rows, int cols);

// matrix subtraction: C = A - B
void gpuMatSub(double *A, double *B, double *C, int rows, int cols);

// scalar multiplication: B = A * scalar
void gpuScalarMul(double *A, double *B, double scalar, int rows, int cols);

// matrix transpose: B = A^T (A is rows x cols, B is cols x rows)
void gpuTranspose(double *A, double *B, int rows, int cols);

// matrix multiplication: C = A * B (tiled, shared memory)
// A is rows_a x cols_a, B is cols_a x cols_b
void gpuMatMul(double *A, double *B, double *C, int rows_a, int cols_a,
               int cols_b);

// GPU-accelerated LU Factorization (Doolittle)
// A, L, U are all flat arrays of size n*n (row-major)
void gpuLU_Doolittle(double *A, double *L, double *U, int n);

#endif
