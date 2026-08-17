#include "../include/BackwardDifference.hpp"

BackwardDifference::BackwardDifference() : Differentiation() {}
BackwardDifference::~BackwardDifference() {}

// backward difference: (f(x) - f(x-h)) / h
double BackwardDifference::computeDerivative(MathFunction f, double x, double h) const {
    return (f(x) - f(x - h)) / h;
}

string BackwardDifference::getMethodName() const {
    return "Backward Difference";
}
