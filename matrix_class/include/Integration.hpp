#ifndef INTEGRATION_HPP
#define INTEGRATION_HPP

#include "Matrix.hpp"
#include <string>
using namespace std;

// function pointer types for integration
typedef double (*IntegrandFunction)(double);               // f(x) -> double
typedef double (*ExactIntegralFunction)(double, double);   // F(a, b) -> exact integral

// one registered integrand + its known exact integral
struct IntegrationFunctionEntry {
    string name;                // e.g. "e^x", "sin(x)", "e^(-x^2)"
    IntegrandFunction f;        // the integrand
    ExactIntegralFunction F;    // exact integral over [a, b] (for error computation)
};

// Integration — abstract base class for numerical integration
//
// inherits from Matrix: the inherited data[][] stores the results table
// each row = one (function, n) pair
// 5 columns: func_index, n, exact, approx, error
//
// each derived class (TrapezoidalRule, Simpsons13, Simpsons38)
// overrides integrate() with its own quadrature formula
//
// follows the same pattern as Differentiation → ForwardDifference

class Integration : public Matrix {
protected:
    IntegrationFunctionEntry *functions;   // array of registered integrands
    int numFunctions;                      // how many integrands registered
    int maxFunctions;                      // capacity

    int *subIntervalCounts;                // array of n values (sub-intervals)
    int numN;                              // how many n values

    string *functionNames;                 // stored function names per result row (for display)

public:
    // constructors / destructor
    Integration();
    virtual ~Integration();

    // register an integrand with its known exact integral
    void addFunction(string name, IntegrandFunction f, ExactIntegralFunction F);

    // set sub-interval counts to evaluate at
    void setSubIntervals(int *n, int count);

    // --- pure virtual: each derived class provides its own quadrature formula ---
    virtual double integrate(IntegrandFunction f, double a, double b, int n) const = 0;

    // --- pure virtual: each derived class returns its method name ---
    virtual string getMethodName() const = 0;

    // compute the integral for all functions at all n values over [a, b]
    // populates the inherited Matrix data[][] with the results table
    void computeAll(double a, double b);

    // display results with function names and formatted table
    void display() const;

    // save results to file (scientific notation)
    void saveResults(string filename) const;

    // getters
    int getNumFunctions() const;
    int getNumN() const;
    IntegrationFunctionEntry getFunction(int i) const;
    int getN(int i) const;
};

#endif
