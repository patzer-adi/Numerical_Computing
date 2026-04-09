# ============================================
# Makefile for Matrix Operations Library
# ============================================
# Two build modes:
#   make cpu    — build CPU-only version (default)
#   make gpu    — build with CUDA GPU support
#   make clean  — remove all build artifacts
# ============================================

CXX = g++
CXXFLAGS = -std=c++11 -Wall
TARGET = matrix_program

# CPU source files
SRCS = main.cpp \
       src/Matrix.cpp \
       src/MatrixException.cpp \
       src/SystemOfLinearEquationSolver.cpp \
       src/GaussianElimination.cpp \
       src/LUDecomposition.cpp \
       src/Doolittle.cpp \
       src/Crout.cpp \
       src/Cholesky.cpp \
       src/GaussJacobi.cpp \
       src/GaussSeidel.cpp \
       src/MatrixOperations.cpp \
       src/EigenSolver.cpp \
       src/GershgorinAnalyzer.cpp \
       src/Interpolation.cpp \
       src/Lagrange.cpp \
       utils/Input.cpp \
       utils/Display.cpp \
       app/Menu.cpp

OBJS = $(SRCS:.cpp=.o)

# ============================================
# CPU-only build (default)
# ============================================
cpu: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# ============================================
# GPU build (requires nvcc and CUDA toolkit)
# ============================================
NVCC = nvcc
NVCCFLAGS = -std=c++11 -arch=sm_50 -Icuda/include -ccbin g++-10 -Wno-deprecated-gpu-targets
CUDA_SRCS = cuda/src/gpu_kernels.cu cuda/src/gpu_dispatch.cu
CUDA_OBJS = cuda/src/gpu_kernels.o cuda/src/gpu_dispatch.o
GPU_TARGET = matrix_program_gpu

gpu: CXXFLAGS += -DUSE_CUDA
gpu: $(GPU_TARGET)

$(GPU_TARGET): $(OBJS) $(CUDA_OBJS)
	$(NVCC) $(NVCCFLAGS) -o $@ $(OBJS) $(CUDA_OBJS)

cuda/src/%.o: cuda/src/%.cu
	$(NVCC) $(NVCCFLAGS) -c $< -o $@

# ============================================
# Verification suite
# ============================================
VERIFY_TARGET = verify_program
VERIFY_SRCS = verify.cpp \
       src/Matrix.cpp \
       src/MatrixException.cpp \
       src/SystemOfLinearEquationSolver.cpp \
       src/GaussianElimination.cpp \
       src/LUDecomposition.cpp \
       src/Doolittle.cpp \
       src/Crout.cpp \
       src/Cholesky.cpp \
       src/GaussJacobi.cpp \
       src/GaussSeidel.cpp \
       src/MatrixOperations.cpp

verify: $(VERIFY_TARGET)

$(VERIFY_TARGET): $(VERIFY_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $(VERIFY_SRCS)

# ============================================
# Cleanup
# ============================================
clean:
	rm -f $(OBJS) $(CUDA_OBJS) $(TARGET) $(GPU_TARGET) $(VERIFY_TARGET)

.PHONY: cpu gpu verify clean
