#include "../include/ForwardDifference.hpp"

ForwardDifference::ForwardDifference() : Differentiation() {}
ForwardDifference::~ForwardDifference() {}

// forward difference: (f(x+h) - f(x)) / h
double ForwardDifference::computeDerivative(MathFunction f, double x, double h) const {
    return (f(x + h) - f(x)) / h;
}

string ForwardDifference::getMethodName() const {
    return "Forward Difference";
}
