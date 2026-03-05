#include "../include/gpu_backend.cuh"
#include "../include/gpu_dispatch.hpp"
#include <iostream>
using namespace std;

bool BackendDispatcher::shouldUseGPU(int matrixSize, string operation) {
  // first check: do we even have a GPU?
  if (!gpuAvailable())
    return false;

  // second check: is the matrix big enough?
  if (matrixSize < GPU_THRESHOLD)
    return false;

  // third check: is this operation good for GPU?
  // matrix multiply and addition are great on GPU
  // LU factorization is decent on GPU for large matrices
  // Gauss-Jacobi has branching so stays on CPU
  // Gaussian Elimination with pivoting has branching so stays on CPU
  if (operation == "matmul" || operation == "matadd")
    return true;
  if (operation == "lu_factorize" && matrixSize >= 128)
    return true;

  // everything else defaults to CPU
  return false;
}

void BackendDispatcher::printBackendInfo() {
  if (gpuAvailable()) {
    cout << "=== GPU Backend Available ===" << endl;
    gpuPrintInfo();
  } else {
    cout << "=== CPU Backend Active (no CUDA GPU detected) ===" << endl;
  }
}
