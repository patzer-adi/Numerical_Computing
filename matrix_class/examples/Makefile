# Example Programs Makefile
# Run from the matrix_class/ directory: make -f examples/Makefile
#
# This shows how a user would build their own programs against the library.

CXX = g++
CXXFLAGS = -std=c++11 -Wall

# library source files (compile once, use everywhere)
LIB_CORE = src/Matrix.cpp src/MatrixException.cpp src/MatrixOperations.cpp
LIB_SLE = src/SystemOfLinearEquationSolver.cpp
LIB_GE = src/GaussianElimination.cpp
LIB_LU = src/LUDecomposition.cpp src/Doolittle.cpp src/Crout.cpp src/Cholesky.cpp
LIB_GJ = src/GaussJacobi.cpp
LIB_UTILS = utils/Input.cpp utils/Display.cpp

# all library sources
LIB_ALL = $(LIB_CORE) $(LIB_SLE) $(LIB_GE) $(LIB_LU) $(LIB_GJ)

# === BUILD TARGETS ===

all: example_basic example_solve

# basic operations — only needs core library files
example_basic: examples/example_basic.cpp $(LIB_CORE)
	$(CXX) $(CXXFLAGS) -o $@ $^

# solvers — needs core + SLE + specific solvers
example_solve: examples/example_solve.cpp $(LIB_CORE) $(LIB_SLE) $(LIB_GE) $(LIB_LU)
	$(CXX) $(CXXFLAGS) -o $@ $^

# ============================================
# Link against pre-built libraries
# (run 'make -f Makefile.lib cpu_shared static' first)
# ============================================

# shared library linking
example_basic_shared: examples/example_basic.cpp
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ $< -Llib -lmatrix_cpu -Wl,-rpath,$(CURDIR)/lib

example_solve_shared: examples/example_solve.cpp
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ $< -Llib -lmatrix_cpu -Wl,-rpath,$(CURDIR)/lib

# static library linking
example_basic_static: examples/example_basic.cpp
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ $< lib/libmatrix.a

example_solve_static: examples/example_solve.cpp
	$(CXX) $(CXXFLAGS) -Iinclude -o $@ $< lib/libmatrix.a

clean:
	rm -f example_basic example_solve
	rm -f example_basic_shared example_solve_shared
	rm -f example_basic_static example_solve_static

.PHONY: all clean
