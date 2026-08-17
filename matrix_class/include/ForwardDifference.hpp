#ifndef FORWARDDIFFERENCE_HPP
#define FORWARDDIFFERENCE_HPP

#include "Differentiation.hpp"

// ForwardDifference — f'(x) ≈ (f(x+h) - f(x)) / h
//
// First-order forward finite difference.
// Truncation error: O(h)

class ForwardDifference : public Differentiation {
public:
    ForwardDifference();
    ~ForwardDifference();

    double computeDerivative(MathFunction f, double x, double h) const override;
    string getMethodName() const override;
};

#endif
