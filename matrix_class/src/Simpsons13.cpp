#include "../include/Simpsons13.hpp"
#include <cmath>

Simpsons13::Simpsons13() : Integration() {}
Simpsons13::~Simpsons13() {}

// composite Simpson's 1/3 rule:
// ∫ₐᵇ f(x)dx ≈ (h/3) [f(x₀) + 4f(x₁) + 2f(x₂) + 4f(x₃) + ... + 4f(xₙ₋₁) + f(xₙ)]
// where h = (b - a) / n
// n must be EVEN
double Simpsons13::integrate(IntegrandFunction f, double a, double b, int n) const {
    if (n < 2 || n % 2 != 0)
        throw MatrixException("Simpson's 1/3 rule requires n to be even (n >= 2)");

    double h = (b - a) / n;
    double sum = f(a) + f(b);

    for (int i = 1; i < n; i++) {
        double xi = a + i * h;
        if (i % 2 == 0)
            sum += 2.0 * f(xi);     // even index: coefficient 2
        else
            sum += 4.0 * f(xi);     // odd index: coefficient 4
    }

    return (h / 3.0) * sum;
}

string Simpsons13::getMethodName() const {
    return "Simpson's 1/3 Rule";
}
