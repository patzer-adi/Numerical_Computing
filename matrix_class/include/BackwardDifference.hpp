#ifndef BACKWARDDIFFERENCE_HPP
#define BACKWARDDIFFERENCE_HPP

#include "Differentiation.hpp"

// BackwardDifference — f'(x) ≈ (f(x) - f(x-h)) / h
//
// First-order backward finite difference.
// Truncation error: O(h)

class BackwardDifference : public Differentiation {
public:
    BackwardDifference();
    ~BackwardDifference();

    double computeDerivative(MathFunction f, double x, double h) const override;
    string getMethodName() const override;
};

#endif
