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

// Differentiation — abstract base class for numerical differentiation
//
// inherits from Matrix: the inherited data[][] stores the results table
// each row = one (function, h) pair
// 5 columns: func_index, h, exact, approx, error
//
// each derived class (ForwardDifference, BackwardDifference, etc.)
// overrides computeDerivative() with its own formula
//
// follows the same pattern as SystemOfLinearEquationSolver → GaussianElimination

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
    virtual ~Differentiation();

    // register a function with its known derivative
    void addFunction(string name, MathFunction f, MathFunction df);

    // set step sizes to evaluate at
    void setStepSizes(double *h, int n);

    // --- pure virtual: each derived class provides its own formula ---
    virtual double computeDerivative(MathFunction f, double x, double h) const = 0;

    // --- pure virtual: each derived class returns its method name ---
    virtual string getMethodName() const = 0;

    // compute the derivative for all functions at all h values
    // populates the inherited Matrix data[][] with the results table
    void computeAll(double x0);

    // display results with function names and formatted table
    void display() const;

    // save results to file (scientific notation)
    void saveResults(string filename) const;

    // getters for function data (so derived classes or external code can access)
    int getNumFunctions() const;
    int getNumH() const;
    FunctionEntry getFunction(int i) const;
    double getH(int i) const;
};

#endif
