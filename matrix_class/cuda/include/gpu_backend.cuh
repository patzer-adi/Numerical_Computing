#ifndef GPU_BACKEND_CUH
#define GPU_BACKEND_CUH

// ============================================
// GPU Backend for Matrix Operations
// ============================================
// automatically detects CUDA GPU at runtime
// falls back to CPU if no GPU found
// designed for CUDA 11.x + compute capability 5.0+
// (GTX 750 Ti = compute 5.0)
// ============================================

// check if a CUDA GPU is available
bool gpuAvailable();

// print GPU info (name, memory, compute capability)
void gpuPrintInfo();

// === GPU Matrix Operations ===

// matrix addition: C = A + B (all flat arrays, row-major)
void gpuMatAdd(double *A, double *B, double *C, int rows, int cols);

// matrix subtraction: C = A - B
void gpuMatSub(double *A, double *B, double *C, int rows, int cols);

// scalar multiplication: B = A * scalar
void gpuScalarMul(double *A, double *B, double scalar, int rows, int cols);

// matrix transpose: B = A^T (A is rows x cols, B is cols x rows)
void gpuTranspose(double *A, double *B, int rows, int cols);

// matrix multiplication: C = A * B
// A is rows_a x cols_a, B is cols_a x cols_b
void gpuMatMul(double *A, double *B, double *C, int rows_a, int cols_a,
               int cols_b);

// === GPU-accelerated LU factorization (Doolittle) ===
// factorizes A into L and U on the GPU
// A, L, U are all flat arrays of size n*n (row-major)
void gpuLU_Doolittle(double *A, double *L, double *U, int n);

#endif
