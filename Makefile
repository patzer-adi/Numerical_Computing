# ============================================
# Makefile — Unified Numerical Computing Library
# ============================================
# Cross-platform: macOS (.dylib) and Linux (.so)
#
# Builds:
#   libnumcomp.dylib / libnumcomp.so  — dynamic shared library
#   libnumcomp.a                      — static library
#
# Usage:
#   make dylib         → lib/libnumcomp.{dylib,so}
#   make static        → lib/libnumcomp.a
#   make all           → dylib + static
#   make example       → build example program
#   make install       → install to /usr/local
#   make uninstall     → remove from /usr/local
#   make clean         → remove build artifacts
#   make info          → show build info
# ============================================

# ── OS Detection ─────────────────────────────
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    # macOS
    SHARED_EXT     = dylib
    SHARED_FLAGS   = -dynamiclib -install_name @rpath/libnumcomp.dylib
    RPATH_FLAG     = -Wl,-rpath,@executable_path/../lib
    RPATH_LDCONFIG = @echo "No ldconfig needed on macOS."
    LIB_PATH_VAR   = DYLD_LIBRARY_PATH
    PLATFORM       = macOS
else
    # Linux (and other Unix-like)
    SHARED_EXT     = so
    SHARED_FLAGS   = -shared
    RPATH_FLAG     = -Wl,-rpath,'$$ORIGIN/../lib'
    RPATH_LDCONFIG = @echo "Run: sudo ldconfig"
    LIB_PATH_VAR   = LD_LIBRARY_PATH
    PLATFORM       = Linux
endif

SHARED_LIB = libnumcomp.$(SHARED_EXT)

# ── Toolchain ────────────────────────────────
CXX       = g++
CXXFLAGS  = -std=c++11 -Wall -Wextra
AR        = ar
ARFLAGS   = rcs

# ── Directories ──────────────────────────────
BUILD_DIR   = build/lib
LIB_DIR     = lib

# ── Include paths ────────────────────────────
INCLUDES = -Imatrix_class \
           -Iroot_finding_methods \
           -IComplex_class_assignment

# ── Install paths ────────────────────────────
PREFIX     = /usr/local
INST_LIB   = $(PREFIX)/lib
INST_INC   = $(PREFIX)/include/numcomp

# ═══════════════════════════════════════════════
# Source Files
# ═══════════════════════════════════════════════

# --- Matrix module (10 files) ---
MATRIX_SRCS = matrix_class/src/Matrix.cpp \
              matrix_class/src/MatrixOperations.cpp \
              matrix_class/src/MatrixException.cpp \
              matrix_class/src/SystemOfLinearEquationSolver.cpp \
              matrix_class/src/GaussianElimination.cpp \
              matrix_class/src/LUDecomposition.cpp \
              matrix_class/src/Doolittle.cpp \
              matrix_class/src/Crout.cpp \
              matrix_class/src/Cholesky.cpp \
              matrix_class/src/GaussJacobi.cpp \
              matrix_class/src/GaussSeidel.cpp \
              matrix_class/src/Interpolation.cpp \
              matrix_class/src/Lagrange.cpp \
              matrix_class/src/LeastSquareLine.cpp \
              matrix_class/src/LeastSquareParabola.cpp \
              matrix_class/src/Differentiation.cpp \
              matrix_class/src/ForwardDifference.cpp \
              matrix_class/src/BackwardDifference.cpp \
              matrix_class/src/CentralDifference.cpp \
              matrix_class/src/RichardsonExtrapolation.cpp

# --- Matrix utils (2 files) ---
MATRIX_UTILS_SRCS = matrix_class/utils/Input.cpp \
                    matrix_class/utils/Display.cpp

# --- Root-finding module (4 files) ---
ROOT_SRCS = root_finding_methods/src/RootHunter.cpp \
            root_finding_methods/src/Biscetion.cpp \
            root_finding_methods/src/NewtonRaphson.cpp \
            root_finding_methods/src/FixedPointer.cpp

# --- Root-finding utils (2 files) ---
ROOT_UTILS_SRCS = root_finding_methods/utils/DisplayUtils.cpp \
                  root_finding_methods/utils/InputUtils.cpp

# --- Complex module (1 file) ---
COMPLEX_SRCS = Complex_class_assignment/complexClass.cpp

# --- All sources ---
ALL_SRCS = $(MATRIX_SRCS) $(MATRIX_UTILS_SRCS) \
           $(ROOT_SRCS) $(ROOT_UTILS_SRCS) \
           $(COMPLEX_SRCS)

# ── Object file list ─────────────────────────
ALL_OBJS = $(foreach src,$(ALL_SRCS),$(BUILD_DIR)/$(subst /,_,$(src:.cpp=.o)))

# ═══════════════════════════════════════════════
# Default target
# ═══════════════════════════════════════════════
all: dylib static

# ═══════════════════════════════════════════════
# 1. Dynamic Library — libnumcomp.{dylib,so}
# ═══════════════════════════════════════════════
dylib: $(LIB_DIR)/$(SHARED_LIB)

$(LIB_DIR)/$(SHARED_LIB): $(ALL_OBJS) | $(LIB_DIR)
	$(CXX) $(SHARED_FLAGS) -o $@ $(ALL_OBJS)
	@echo ""
	@echo "═══════════════════════════════════════════"
	@echo "  ✓ Built: $@"
	@echo "    Platform: $(PLATFORM)"
	@echo "    Type:     dynamic shared library"
	@echo "    Link:     -Llib -lnumcomp"
	@echo "═══════════════════════════════════════════"

# ═══════════════════════════════════════════════
# 2. Static Library — libnumcomp.a
# ═══════════════════════════════════════════════
static: $(LIB_DIR)/libnumcomp.a

$(LIB_DIR)/libnumcomp.a: $(ALL_OBJS) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $(ALL_OBJS)
	@echo ""
	@echo "═══════════════════════════════════════════"
	@echo "  ✓ Built: $@"
	@echo "    Type:  static archive"
	@echo "    Link:  lib/libnumcomp.a (or -Llib -lnumcomp)"
	@echo "═══════════════════════════════════════════"

# ═══════════════════════════════════════════════
# Compile rules — one rule per source directory
# ═══════════════════════════════════════════════

# Matrix src/
$(BUILD_DIR)/matrix_class_src_%.o: matrix_class/src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -fPIC -c $< -o $@

# Matrix utils/
$(BUILD_DIR)/matrix_class_utils_%.o: matrix_class/utils/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -fPIC -c $< -o $@

# Root-finding src/
$(BUILD_DIR)/root_finding_methods_src_%.o: root_finding_methods/src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -fPIC -c $< -o $@

# Root-finding utils/
$(BUILD_DIR)/root_finding_methods_utils_%.o: root_finding_methods/utils/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -fPIC -c $< -o $@

# Complex class
$(BUILD_DIR)/Complex_class_assignment_%.o: Complex_class_assignment/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -fPIC -c $< -o $@

# ═══════════════════════════════════════════════
# 3. Example Program
# ═══════════════════════════════════════════════
example: dylib examples/example_usage.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -I. \
		examples/example_usage.cpp \
		-Llib -lnumcomp \
		$(RPATH_FLAG) \
		-o examples/example_usage
	@echo ""
	@echo "═══════════════════════════════════════════"
	@echo "  ✓ Built: examples/example_usage"
	@echo "    Run:   ./examples/example_usage"
	@echo "═══════════════════════════════════════════"

# ═══════════════════════════════════════════════
# Install / Uninstall
# ═══════════════════════════════════════════════
install:
	@echo "Installing library to $(INST_LIB)... ($(PLATFORM))"
	@mkdir -p $(INST_LIB)
	@mkdir -p $(INST_INC)/matrix
	@mkdir -p $(INST_INC)/rootfinding
	@mkdir -p $(INST_INC)/complex
	@# Copy shared library
	@if [ -f $(LIB_DIR)/$(SHARED_LIB) ]; then \
		cp $(LIB_DIR)/$(SHARED_LIB) $(INST_LIB)/; \
		echo "  ✓ $(INST_LIB)/$(SHARED_LIB)"; \
	fi
	@# Copy static library
	@if [ -f $(LIB_DIR)/libnumcomp.a ]; then \
		cp $(LIB_DIR)/libnumcomp.a $(INST_LIB)/; \
		echo "  ✓ $(INST_LIB)/libnumcomp.a"; \
	fi
	@# Copy headers
	@cp matrix_class/include/*.hpp $(INST_INC)/matrix/
	@cp matrix_class/utils/*.hpp $(INST_INC)/matrix/
	@cp root_finding_methods/include/*.hpp $(INST_INC)/rootfinding/
	@cp root_finding_methods/utils/*.hpp $(INST_INC)/rootfinding/
	@cp Complex_class_assignment/complexClass_header.hpp $(INST_INC)/complex/
	@cp numcomp.hpp $(INST_INC)/
	@echo "  ✓ Headers → $(INST_INC)/"
	@echo ""
	@echo "Done! Compile with:"
	@echo "  g++ -std=c++11 -I$(INST_INC) my_app.cpp -lnumcomp"
	$(RPATH_LDCONFIG)

uninstall:
	@echo "Removing installed files..."
	rm -f $(INST_LIB)/$(SHARED_LIB)
	rm -f $(INST_LIB)/libnumcomp.a
	rm -rf $(INST_INC)
	@echo "Done."

# ═══════════════════════════════════════════════
# Info
# ═══════════════════════════════════════════════
info:
	@echo ""
	@echo "── Numerical Computing Library Build ──────"
	@echo ""
	@echo "  Platform:    $(PLATFORM) ($(UNAME_S))"
	@echo "  Shared ext:  .$(SHARED_EXT)"
	@echo "  Lib path var: $(LIB_PATH_VAR)"
	@echo "  Compiler:    $(CXX)"
	@echo ""
	@echo "  Source files:  $(words $(ALL_SRCS)) total"
	@echo "    Matrix:      $(words $(MATRIX_SRCS)) + $(words $(MATRIX_UTILS_SRCS)) utils"
	@echo "    Root-find:   $(words $(ROOT_SRCS)) + $(words $(ROOT_UTILS_SRCS)) utils"
	@echo "    Complex:     $(words $(COMPLEX_SRCS))"
	@echo ""
	@echo "  Targets:"
	@echo "    dylib    → $(LIB_DIR)/$(SHARED_LIB)"
	@echo "    static   → $(LIB_DIR)/libnumcomp.a"
	@echo "    all      → dylib + static"
	@echo "    example  → examples/example_usage"
	@echo "    install  → copy to $(PREFIX)"
	@echo "    uninstall→ remove from $(PREFIX)"
	@echo "    clean    → remove build artifacts"
	@echo "    info     → this message"
	@echo ""

# ═══════════════════════════════════════════════
# Directory creation
# ═══════════════════════════════════════════════
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(LIB_DIR):
	@mkdir -p $(LIB_DIR)

# ═══════════════════════════════════════════════
# Clean
# ═══════════════════════════════════════════════
clean:
	rm -rf build/lib $(LIB_DIR)/$(SHARED_LIB) $(LIB_DIR)/libnumcomp.a
	rm -f examples/example_usage
	@echo "Cleaned library build artifacts."

.PHONY: all dylib static example install uninstall clean info
