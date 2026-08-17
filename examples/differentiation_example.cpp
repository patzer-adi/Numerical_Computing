// differentiation_example.cpp — demonstrates the polymorphic Differentiation hierarchy
//
// Hierarchy:
//   Matrix
//     └── Differentiation (abstract base)
//           ├── ForwardDifference
//           ├── BackwardDifference
//           ├── CentralDifference
//           └── RichardsonExtrapolation
//
// Each derived class overrides computeDerivative() with its own formula.
// The base class computeAll() uses the virtual call to compute results.

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

// helper: register all test functions into any Differentiation object
void setupFunctions(Differentiation &d) {
    d.addFunction("e^x", f_exp, df_exp);
    d.addFunction("sin(x)", f_sin, df_sin);
    d.addFunction("cos(x)", f_cos, df_cos);
    d.addFunction("x^3-2x+1", f_poly, df_poly);
}

int main() {
    cout << "\n=== Numerical Differentiation — Polymorphic Design ===" << endl;

    // step sizes
    double h[] = {0.1, 0.01, 0.001, 0.0001};
    int numH = 4;
    double x0 = 1.0;

    // create all four method objects
    ForwardDifference fwd;
    BackwardDifference bwd;
    CentralDifference cen;
    RichardsonExtrapolation rich;

    // polymorphism: iterate over base pointers
    Differentiation *methods[] = {&fwd, &bwd, &cen, &rich};
    int numMethods = 4;

    for (int m = 0; m < numMethods; m++) {
        setupFunctions(*methods[m]);
        methods[m]->setStepSizes(h, numH);
        methods[m]->computeAll(x0);
    }

    // display all results
    cout << "\nEvaluating at x0 = " << x0 << endl;
    for (int m = 0; m < numMethods; m++) {
        methods[m]->display();
    }

    // save each to its own file
    fwd.saveResults("output_forward.txt");
    bwd.saveResults("output_backward.txt");
    cen.saveResults("output_central.txt");
    rich.saveResults("output_richardson.txt");

    cout << "\nDone!" << endl;
    return 0;
}
