#ifndef SIMPSONS38_HPP
#define SIMPSONS38_HPP

#include "Integration.hpp"

// Simpsons38 — composite Simpson's 3/8 rule
//
// ∫ₐᵇ f(x)dx ≈ (3h/8) [f(x₀) + 3f(x₁) + 3f(x₂) + 2f(x₃) + 3f(x₄) + ... + f(xₙ)]
// where h = (b - a) / n
//
// Truncation error: O(h⁴)
// Constraint: n must be a MULTIPLE OF 3 (n ≥ 3)

class Simpsons38 : public Integration {
public:
    Simpsons38();
    ~Simpsons38();

    double integrate(IntegrandFunction f, double a, double b, int n) const override;
    string getMethodName() const override;
};

#endif
