#ifndef CENTRALDIFFERENCE_HPP
#define CENTRALDIFFERENCE_HPP

#include "Differentiation.hpp"

// CentralDifference — f'(x) ≈ (f(x+h) - f(x-h)) / (2h)
//
// Second-order central finite difference.
// Truncation error: O(h²)

class CentralDifference : public Differentiation {
public:
    CentralDifference();
    ~CentralDifference();

    double computeDerivative(MathFunction f, double x, double h) const override;
    string getMethodName() const override;
};

#endif
