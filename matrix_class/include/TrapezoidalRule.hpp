#ifndef TRAPEZOIDALRULE_HPP
#define TRAPEZOIDALRULE_HPP

#include "Integration.hpp"

// TrapezoidalRule — composite trapezoidal rule
//
// ∫ₐᵇ f(x)dx ≈ (h/2) [f(x₀) + 2·Σf(xᵢ) + f(xₙ)]
// where h = (b - a) / n
//
// Truncation error: O(h²)
// Constraint: n ≥ 1

class TrapezoidalRule : public Integration {
public:
    TrapezoidalRule();
    ~TrapezoidalRule();

    double integrate(IntegrandFunction f, double a, double b, int n) const override;
    string getMethodName() const override;
};

#endif
