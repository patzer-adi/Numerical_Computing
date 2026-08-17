#include "../include/CentralDifference.hpp"

CentralDifference::CentralDifference() : Differentiation() {}
CentralDifference::~CentralDifference() {}

// central difference: (f(x+h) - f(x-h)) / (2h)
double CentralDifference::computeDerivative(MathFunction f, double x, double h) const {
    return (f(x + h) - f(x - h)) / (2.0 * h);
}

string CentralDifference::getMethodName() const {
    return "Central Difference";
}
