# ============================================
# Makefile Template for Matrix Operations
# ============================================
# YOUR TASK: Fill in the blanks marked with ???
# 
# Hints:
#   - CXX is the compiler (g++ or clang++)
#   - CXXFLAGS are flags passed to every compile step
#   - Each .cpp file gets compiled into a .o (object) file
#   - The final binary links all .o files together
#   - $< means "the first dependency" (the .cpp file)
#   - $@ means "the target" (the .o or binary name)
#
# Quick reference:
#   Compile only:  $(CXX) $(CXXFLAGS) -c source.cpp -o source.o
#   Link all:      $(CXX) $(CXXFLAGS) -o binaryname obj1.o obj2.o ...
# ============================================

# --- Step 1: Set the compiler and flags ---
CXX = ???
CXXFLAGS = ???

# --- Step 2: Name your output binary ---
TARGET = ???

# --- Step 3: List ALL your .cpp source files ---
# (one from main, some from src/, some from utils/)
SRCS = main.cpp \
       ???

# --- Step 4: Generate object file names from source files ---
# This magic line turns every .cpp in SRCS into a .o
# Example: main.cpp -> main.o, src/Matrix.cpp -> src/Matrix.o
OBJS = $(SRCS:.cpp=.o)

# ============================================
# RULES
# ============================================

# --- Step 5: The default target ---
# "all" depends on $(TARGET), which is your binary
all: $(TARGET)

# --- Step 6: Link all .o files into the final binary ---
# Fill in: how to combine all OBJS into TARGET
$(TARGET): $(OBJS)
	???

# --- Step 7: Pattern rule to compile any .cpp into .o ---
# The % wildcard matches any filename
# $< = the .cpp file, $@ = the .o file
%.o: %.cpp
	???

# --- Step 8: Clean up generated files ---
clean:
	???

# This tells make that "all" and "clean" aren't real files
.PHONY: all clean

# ============================================
# ANSWER KEY (don't peek until you've tried!)
# ============================================
# Uncomment below to check your answers:
#
# CXX = g++
# CXXFLAGS = -std=c++11 -Wall
# TARGET = matrix_program
#
# SRCS = main.cpp \
#        src/Matrix.cpp \
#        src/MatrixException.cpp \
#        src/GaussianElimination.cpp \
#        src/LUDecomposition.cpp \
#        src/Doolittle.cpp \
#        src/Crout.cpp \
#        src/Cholesky.cpp \
#        utils/Input.cpp \
#        utils/Display.cpp
#
# OBJS = $(SRCS:.cpp=.o)
#
# all: $(TARGET)
#
# $(TARGET): $(OBJS)
# 	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)
#
# %.o: %.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@
#
# clean:
# 	rm -f $(OBJS) $(TARGET)
#
# .PHONY: all clean
