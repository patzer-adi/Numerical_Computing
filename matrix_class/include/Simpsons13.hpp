#ifndef SIMPSONS13_HPP
#define SIMPSONS13_HPP

#include "Integration.hpp"

// Simpsons13 — composite Simpson's 1/3 rule
//
// ∫ₐᵇ f(x)dx ≈ (h/3) [f(x₀) + 4f(x₁) + 2f(x₂) + 4f(x₃) + ... + f(xₙ)]
// where h = (b - a) / n
//
// Truncation error: O(h⁴)
// Constraint: n must be EVEN (n ≥ 2)

class Simpsons13 : public Integration {
public:
    Simpsons13();
    ~Simpsons13();

    double integrate(IntegrandFunction f, double a, double b, int n) const override;
    string getMethodName() const override;
};

#endif
