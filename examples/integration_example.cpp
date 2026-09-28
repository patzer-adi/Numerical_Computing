// integration_example.cpp — demonstrates the polymorphic Integration hierarchy
//
// Hierarchy:
//   Matrix
//     └── Integration (abstract base)
//           ├── TrapezoidalRule
//           ├── Simpsons13
//           └── Simpsons38
//
// Each derived class overrides integrate() with its own quadrature formula.
// The base class computeAll() uses the virtual call to compute results.

#include "numcomp.hpp"
#include <cmath>
#include <iostream>
using namespace std;

// ═══════════════════════════════════════════════
// Built-in integrand functions + exact integrals
// ═══════════════════════════════════════════════

// 1. e^x
double f_exp_i(double x) { return exp(x); }
double F_exp_i(double a, double b) { return exp(b) - exp(a); }

// 2. sin(x)
double f_sin_i(double x) { return sin(x); }
double F_sin_i(double a, double b) { return -cos(b) + cos(a); }

// 3. x^2
double f_x2(double x) { return x * x; }
double F_x2(double a, double b) { return (b*b*b - a*a*a) / 3.0; }

// 4. x^3 - 2x + 1
double f_poly_i(double x) { return x*x*x - 2*x + 1; }
double F_poly_i(double a, double b) {
    return (pow(b,4)/4.0 - b*b + b) - (pow(a,4)/4.0 - a*a + a);
}

// 5. e^(-x^2)  — Gaussian integrand (no elementary antiderivative)
double f_gauss(double x) { return exp(-x * x); }
double F_gauss(double a, double b) { return sqrt(M_PI) / 2.0 * (erf(b) - erf(a)); }

// 6. 1/(1+x^2) — Lorentzian / arctangent
double f_lorentz(double x) { return 1.0 / (1.0 + x * x); }
double F_lorentz(double a, double b) { return atan(b) - atan(a); }

// 7. ln(x)
double f_ln(double x) { return log(x); }
double F_ln(double a, double b) { return (b*log(b) - b) - (a*log(a) - a); }

// 8. 1/x
double f_inv(double x) { return 1.0 / x; }
double F_inv(double a, double b) { return log(b) - log(a); }

// helper: register all built-in functions into any Integration object
void setupIntegrationFunctions(Integration &integ) {
    integ.addFunction("e^x",         f_exp_i,   F_exp_i);
    integ.addFunction("sin(x)",      f_sin_i,   F_sin_i);
    integ.addFunction("x^2",         f_x2,      F_x2);
    integ.addFunction("x^3-2x+1",   f_poly_i,  F_poly_i);
    integ.addFunction("e^(-x^2)",    f_gauss,   F_gauss);
    integ.addFunction("1/(1+x^2)",   f_lorentz, F_lorentz);
    integ.addFunction("ln(x)",       f_ln,      F_ln);
    integ.addFunction("1/x",         f_inv,     F_inv);
}

int main() {
    cout << "\n=== Numerical Integration — Polymorphic Design ===" << endl;

    // sub-interval counts (chosen to satisfy all rules: multiples of 6)
    int n[] = {6, 12, 24, 48, 96};
    int numN = 5;

    // integration bounds
    double a = 0.0;
    double b = 1.0;

    cout << "\nIntegrating over [" << a << ", " << b << "]" << endl;
    cout << "(Note: ln(x) and 1/x are excluded from [0,1] — they have singularity at x=0)\n" << endl;

    // create all three method objects
    TrapezoidalRule trap;
    Simpsons13 s13;
    Simpsons38 s38;

    // polymorphism: iterate over base pointers
    Integration *methods[] = {&trap, &s13, &s38};
    int numMethods = 3;

    // register functions (skip ln(x) and 1/x for [0,1] bounds)
    for (int m = 0; m < numMethods; m++) {
        methods[m]->addFunction("e^x",         f_exp_i,   F_exp_i);
        methods[m]->addFunction("sin(x)",      f_sin_i,   F_sin_i);
        methods[m]->addFunction("x^2",         f_x2,      F_x2);
        methods[m]->addFunction("x^3-2x+1",   f_poly_i,  F_poly_i);
        methods[m]->addFunction("e^(-x^2)",    f_gauss,   F_gauss);
        methods[m]->addFunction("1/(1+x^2)",   f_lorentz, F_lorentz);

        methods[m]->setSubIntervals(n, numN);
        methods[m]->computeAll(a, b);
    }

    // display all results
    for (int m = 0; m < numMethods; m++) {
        methods[m]->display();
    }

    // save each to its own file
    trap.saveResults("output_trapezoidal.txt");
    s13.saveResults("output_simpsons13.txt");
    s38.saveResults("output_simpsons38.txt");

    // --- bonus: integrate ln(x) and 1/x over [1, 5] ---
    cout << "\n--- Bonus: ln(x) and 1/x over [1, 5] ---\n" << endl;

    TrapezoidalRule trap2;
    Simpsons13 s13_2;
    Simpsons38 s38_2;

    Integration *methods2[] = {&trap2, &s13_2, &s38_2};
    for (int m = 0; m < 3; m++) {
        methods2[m]->addFunction("ln(x)", f_ln, F_ln);
        methods2[m]->addFunction("1/x",   f_inv, F_inv);
        methods2[m]->setSubIntervals(n, numN);
        methods2[m]->computeAll(1.0, 5.0);
        methods2[m]->display();
    }

    cout << "\nDone!" << endl;
    return 0;
}
