#include "../include/TrapezoidalRule.hpp"
#include <cmath>

TrapezoidalRule::TrapezoidalRule() : Integration() {}
TrapezoidalRule::~TrapezoidalRule() {}

// composite trapezoidal rule:
// ∫ₐᵇ f(x)dx ≈ (h/2) [f(x₀) + 2·Σᵢ₌₁ⁿ⁻¹ f(xᵢ) + f(xₙ)]
// where h = (b - a) / n
double TrapezoidalRule::integrate(IntegrandFunction f, double a, double b, int n) const {
    if (n < 1)
        throw MatrixException("trapezoidal rule requires n >= 1");

    double h = (b - a) / n;
    double sum = f(a) + f(b);

    for (int i = 1; i < n; i++) {
        double xi = a + i * h;
        sum += 2.0 * f(xi);
    }

    return (h / 2.0) * sum;
}

string TrapezoidalRule::getMethodName() const {
    return "Trapezoidal Rule";
}
