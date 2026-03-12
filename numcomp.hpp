// ============================================
// numcomp.hpp — Unified Numerical Computing Header
// ============================================
// Include this single header to get access to
// all classes in the Numerical Computing library.
//
// Usage:
//   #include "numcomp.hpp"
//
// Author:  Aditya Gowari
// License: MIT
// ============================================

#ifndef NUMCOMP_HPP
#define NUMCOMP_HPP

// ── Matrix Module ───────────────────────────
#include "matrix_class/include/Matrix.hpp"
#include "matrix_class/include/MatrixException.hpp"
#include "matrix_class/include/SystemOfLinearEquationSolver.hpp"
#include "matrix_class/include/GaussianElimination.hpp"
#include "matrix_class/include/LUDecomposition.hpp"
#include "matrix_class/include/GaussJacobi.hpp"

// ── Root-Finding Module ─────────────────────
#include "root_finding_methods/include/RootHunter.hpp"
#include "root_finding_methods/include/Bisection.hpp"
#include "root_finding_methods/include/NewtonRaphson.hpp"
#include "root_finding_methods/include/FixedPoint.hpp"

// ── Complex Number Module ───────────────────
#include "Complex_class_assignment/complexClass_header.hpp"

#endif // NUMCOMP_HPP
