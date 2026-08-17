#include "../include/RichardsonExtrapolation.hpp"

RichardsonExtrapolation::RichardsonExtrapolation() : Differentiation() {}
RichardsonExtrapolation::~RichardsonExtrapolation() {}

// richardson extrapolation: (4*D(h/2) - D(h)) / 3
// where D(h) = central difference = (f(x+h) - f(x-h)) / (2h)
double RichardsonExtrapolation::computeDerivative(MathFunction f, double x, double h) const {
    // central difference at step h
    double D_h = (f(x + h) - f(x - h)) / (2.0 * h);

    // central difference at step h/2
    double h2 = h / 2.0;
    double D_h2 = (f(x + h2) - f(x - h2)) / (2.0 * h2);

    // Richardson combination
    return (4.0 * D_h2 - D_h) / 3.0;
}

string RichardsonExtrapolation::getMethodName() const {
    return "Richardson Extrapolation";
}
