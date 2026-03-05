#ifndef GPU_DISPATCH_CUH
#define GPU_DISPATCH_CUH

// Backend Dispatcher
// decides whether to use CPU or GPU for an operation
// based on:
//   1. is a CUDA GPU available?
//   2. is the matrix big enough to benefit from GPU?
//   3. is the operation suitable for GPU?
//
// usage:
//   if (BackendDispatcher::shouldUseGPU(n, "matmul"))
//     // use GPU path
//   else
//     // use CPU path

#include <string>
using namespace std;

class BackendDispatcher {
public:
  // minimum matrix size to consider GPU (smaller matrices are faster on CPU)
  static const int GPU_THRESHOLD = 64;

  // check if we should use GPU for this operation
  static bool shouldUseGPU(int matrixSize, string operation);

  // print which backend is active
  static void printBackendInfo();
};

#endif
