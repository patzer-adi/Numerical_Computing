// differentiation_example.cpp — standalone example of the Differentiation class
//
// Demonstrates:
//   - registering functions with known derivatives
//   - setting step sizes
//   - computing all four differentiation methods
//   - displaying and saving results
//
// Build:
//   g++ -std=c++11 -I. -Imatrix_class differentiation_example.cpp \
//       -Llib -lnumcomp -Wl,-rpath,@executable_path/../lib -o diff_example
//
// Or after `make dylib`:
//   make example  (if added to Makefile)

#include "numcomp.hpp"
#include <cmath>
#include <iostream>
using namespace std;

// --- function definitions ---
double f_exp(double x) { return exp(x); }
double df_exp(double x) { return exp(x); }

double f_sin(double x) { return sin(x); }
double df_sin(double x) { return cos(x); }

double f_cos(double x) { return cos(x); }
double df_cos(double x) { return -sin(x); }

double f_poly(double x) { return x*x*x - 2*x + 1; }
double df_poly(double x) { return 3*x*x - 2; }

int main() {
    cout << "\n=== Numerical Differentiation Example ===" << endl;

    // create the differentiation object
    Differentiation diff;

    // register functions with their known derivatives
    diff.addFunction("e^x", f_exp, df_exp);
    diff.addFunction("sin(x)", f_sin, df_sin);
    diff.addFunction("cos(x)", f_cos, df_cos);
    diff.addFunction("x^3-2x+1", f_poly, df_poly);

    // set step sizes
    double h[] = {0.1, 0.01, 0.001, 0.0001};
    diff.setStepSizes(h, 4);

    // compute all derivatives at x0 = 1.0
    double x0 = 1.0;
    cout << "\nEvaluating at x0 = " << x0 << endl;
    diff.computeAll(x0);

    // display results
    cout << "\n--- Results Table ---" << endl;
    diff.display();

    // save to file
    diff.saveResults("differentiation_output.txt");

    // also demonstrate standalone static method usage
    cout << "\n--- Standalone Usage ---" << endl;
    cout << "Forward  diff of sin(x) at x=1, h=0.01: "
         << Differentiation::forwardDifference(f_sin, 1.0, 0.01) << endl;
    cout << "Central  diff of sin(x) at x=1, h=0.01: "
         << Differentiation::centralDifference(f_sin, 1.0, 0.01) << endl;
    cout << "Exact    derivative:                     "
         << cos(1.0) << endl;

    cout << "\nDone!" << endl;
    return 0;
}
