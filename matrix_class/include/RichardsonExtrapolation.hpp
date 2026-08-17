#ifndef RICHARDSONEXTRAPOLATION_HPP
#define RICHARDSONEXTRAPOLATION_HPP

#include "Differentiation.hpp"

// RichardsonExtrapolation — combines two central differences to cancel error
//
// R = (4 * D(h/2) - D(h)) / 3
// where D(h) = central difference at step size h
//
// Truncation error: O(h⁴)

class RichardsonExtrapolation : public Differentiation {
public:
    RichardsonExtrapolation();
    ~RichardsonExtrapolation();

    double computeDerivative(MathFunction f, double x, double h) const override;
    string getMethodName() const override;
};

#endif
