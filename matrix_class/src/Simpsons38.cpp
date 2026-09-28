#include "../include/Simpsons38.hpp"
#include <cmath>

Simpsons38::Simpsons38() : Integration() {}
Simpsons38::~Simpsons38() {}

// composite Simpson's 3/8 rule:
// ∫ₐᵇ f(x)dx ≈ (3h/8) [f(x₀) + 3f(x₁) + 3f(x₂) + 2f(x₃) + 3f(x₄) + 3f(x₅) + 2f(x₆) + ... + f(xₙ)]
// where h = (b - a) / n
// n must be a MULTIPLE OF 3
double Simpsons38::integrate(IntegrandFunction f, double a, double b, int n) const {
    if (n < 3 || n % 3 != 0)
        throw MatrixException("Simpson's 3/8 rule requires n to be a multiple of 3 (n >= 3)");

    double h = (b - a) / n;
    double sum = f(a) + f(b);

    for (int i = 1; i < n; i++) {
        double xi = a + i * h;
        if (i % 3 == 0)
            sum += 2.0 * f(xi);     // every 3rd interior point: coefficient 2
        else
            sum += 3.0 * f(xi);     // other interior points: coefficient 3
    }

    return (3.0 * h / 8.0) * sum;
}

string Simpsons38::getMethodName() const {
    return "Simpson's 3/8 Rule";
}
