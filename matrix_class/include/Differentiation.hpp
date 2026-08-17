#ifndef DIFFERENTIATION_HPP
#define DIFFERENTIATION_HPP

#include "Matrix.hpp"
#include <string>
using namespace std;

// function pointer type: f(x) -> double
typedef double (*MathFunction)(double);

// one registered function + its known derivative
struct FunctionEntry {
    string name;        // e.g. "e^x", "sin(x)"
    MathFunction f;     // the function
    MathFunction df;    // exact derivative (for error computation)
};

// Differentiation — numerical differentiation methods
//
// inherits from Matrix: the inherited data[][] stores the results table
// each row = one (function, h) pair
// 11 columns: func_index, h, exact, forward, fwd_err, backward, bwd_err,
//             central, cen_err, richardson, rich_err
//
// follows the same inheritance pattern as SystemOfLinearEquationSolver

class Differentiation : public Matrix {
protected:
    FunctionEntry *functions;   // array of registered functions
    int numFunctions;           // how many functions registered
    int maxFunctions;           // capacity

    double *hValues;            // array of step sizes
    int numH;                   // how many step sizes

    string *functionNames;      // stored function names per result row (for display)

public:
    // constructors / destructor
    Differentiation();
    ~Differentiation();

    // register a function with its known derivative
    void addFunction(string name, MathFunction f, MathFunction df);

    // set step sizes to evaluate at
    void setStepSizes(double *h, int n);

    // --- the four numerical differentiation methods ---
    // static so they can be used standalone without an object
    static double forwardDifference(MathFunction f, double x, double h);
    static double backwardDifference(MathFunction f, double x, double h);
    static double centralDifference(MathFunction f, double x, double h);
    static double richardsonExtrapolation(MathFunction f, double x, double h);

    // compute all methods for all functions at all h values
    // populates the inherited Matrix data[][] with the results table
    void computeAll(double x0);

    // display results with function names and formatted table
    void display() const;

    // save results to file (scientific notation, matching Python output format)
    void saveResults(string filename) const;
};

#endif
